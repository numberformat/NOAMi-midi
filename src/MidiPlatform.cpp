#include "MidiPlatform.h"

#include <windows.h>
#include <mmsystem.h>

#include <algorithm>
#include <memory>
#include <vector>

namespace {

MidiPortTechnology FromWinTechnology(WORD tech) {
    switch (tech) {
        case MOD_MIDIPORT:
            return MidiPortTechnology::MidiPort;
        case MOD_SYNTH:
            return MidiPortTechnology::Synth;
        case MOD_SQSYNTH:
            return MidiPortTechnology::SquareWaveSynth;
        case MOD_FMSYNTH:
            return MidiPortTechnology::FmSynth;
        case MOD_MAPPER:
            return MidiPortTechnology::MidiMapper;
        case 6:
            return MidiPortTechnology::WavetableSynth;
        case 7:
            return MidiPortTechnology::SoftwareSynth;
        default:
            return MidiPortTechnology::Unknown;
    }
}

class WinMidiPlatform final : public MidiPlatform {
public:
    WinMidiPlatform() = default;
    ~WinMidiPlatform() override {
        CloseInput();
        CloseOutput();
    }

    uint32_t GetInputDeviceCount() const override {
        return midiInGetNumDevs();
    }

    uint32_t GetOutputDeviceCount() const override {
        return midiOutGetNumDevs();
    }

    bool GetInputDeviceCaps(uint32_t deviceId, MidiInputCaps &caps) const override {
        MIDIINCAPS incaps;
        if (midiInGetDevCaps(deviceId, &incaps, sizeof(incaps)) != MMSYSERR_NOERROR)
            return false;
        caps.name = incaps.szPname;
        caps.manufacturerId = incaps.wMid;
        caps.productId = incaps.wPid;
        caps.driverVersion = incaps.vDriverVersion;
        return true;
    }

    bool GetOutputDeviceCaps(uint32_t deviceId, MidiOutputCaps &caps) const override {
        MIDIOUTCAPS outcaps;
        if (midiOutGetDevCaps(deviceId, &outcaps, sizeof(outcaps)) != MMSYSERR_NOERROR)
            return false;
        caps.name = outcaps.szPname;
        caps.manufacturerId = outcaps.wMid;
        caps.productId = outcaps.wPid;
        caps.driverVersion = outcaps.vDriverVersion;
        caps.technologyRaw = outcaps.wTechnology;
        caps.technology = FromWinTechnology(outcaps.wTechnology);
        return true;
    }

    bool OpenOutput(uint32_t deviceId) override {
        CloseOutput();
        if (midiOutOpen(&hout_, deviceId, 0, 0, 0) != MMSYSERR_NOERROR) {
            hout_ = nullptr;
            return false;
        }
        return true;
    }

    bool CloseOutput() override {
        if (!hout_)
            return true;
        midiOutReset(hout_);
        const MMRESULT result = midiOutClose(hout_);
        hout_ = nullptr;
        return result == MMSYSERR_NOERROR;
    }

    bool SendShortMessage(uint32_t message) override {
        if (!hout_)
            return false;
        return midiOutShortMsg(hout_, message) == MMSYSERR_NOERROR;
    }

    bool SendLongMessage(const uint8_t *data, size_t size) override {
        if (!hout_ || !data || size == 0)
            return false;

        std::vector<uint8_t> buffer(size);
        std::copy(data, data + size, buffer.begin());

        MIDIHDR header = {};
        header.lpData = reinterpret_cast<LPSTR>(buffer.data());
        header.dwBufferLength = static_cast<DWORD>(buffer.size());
        header.dwBytesRecorded = static_cast<DWORD>(buffer.size());

        if (midiOutPrepareHeader(hout_, &header, sizeof(header)) != MMSYSERR_NOERROR)
            return false;

        const MMRESULT result = midiOutLongMsg(hout_, &header, sizeof(header));
        if (result != MMSYSERR_NOERROR) {
            midiOutUnprepareHeader(hout_, &header, sizeof(header));
            return false;
        }

        MMRESULT unprepare;
        do {
            unprepare = midiOutUnprepareHeader(hout_, &header, sizeof(header));
            if (unprepare == MIDIERR_STILLPLAYING)
                Sleep(1);
        } while (unprepare == MIDIERR_STILLPLAYING);

        return unprepare == MMSYSERR_NOERROR;
    }

    void ResetOutput() override {
        if (hout_)
            midiOutReset(hout_);
    }

    bool OpenInput(uint32_t deviceId, MidiInputCallback callback) override {
        CloseInput();
        callback_ = std::move(callback);
        const MMRESULT result = midiInOpen(&hin_, deviceId, reinterpret_cast<DWORD_PTR>(&WinMidiPlatform::MidiInThunk),
                                           reinterpret_cast<DWORD_PTR>(this), CALLBACK_FUNCTION);
        if (result != MMSYSERR_NOERROR) {
            hin_ = nullptr;
            callback_ = nullptr;
            return false;
        }
        return true;
    }

    bool CloseInput() override {
        if (!hin_)
            return true;
        midiInStop(hin_);
        const MMRESULT result = midiInClose(hin_);
        hin_ = nullptr;
        callback_ = nullptr;
        return result == MMSYSERR_NOERROR;
    }

    bool StartInput() override {
        if (!hin_)
            return false;
        return midiInStart(hin_) == MMSYSERR_NOERROR;
    }

    bool StopInput() override {
        if (!hin_)
            return false;
        return midiInStop(hin_) == MMSYSERR_NOERROR;
    }

private:
    static void CALLBACK MidiInThunk(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
        auto *self = reinterpret_cast<WinMidiPlatform *>(dwInstance);
        if (self)
            self->HandleMidiIn(hMidiIn, wMsg, dwParam1, dwParam2);
    }

    void HandleMidiIn(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
        (void) hMidiIn;
        if (!callback_)
            return;

        MidiInputMessage message;
        switch (wMsg) {
            case MIM_OPEN:
                message.type = MidiInputMessage::Type::Opened;
                callback_(message);
                break;
            case MIM_CLOSE:
                message.type = MidiInputMessage::Type::Closed;
                callback_(message);
                break;
            case MIM_DATA:
                message.type = MidiInputMessage::Type::ShortMessage;
                message.shortMessage = static_cast<uint32_t>(dwParam1);
                callback_(message);
                break;
            case MIM_LONGDATA: {
                message.type = MidiInputMessage::Type::LongData;
                auto *header = reinterpret_cast<LPMIDIHDR>(dwParam1);
                if (header && header->dwBytesRecorded > 0 && header->lpData) {
                    message.longData.assign(reinterpret_cast<uint8_t *>(header->lpData),
                                            reinterpret_cast<uint8_t *>(header->lpData) + header->dwBytesRecorded);
                } else {
                    message.longData.clear();
                }
                callback_(message);
                break;
            }
            default:
                break;
        }
        (void) dwParam2;
    }

    HMIDIOUT hout_ = nullptr;
    HMIDIIN hin_ = nullptr;
    MidiInputCallback callback_;
};

} // namespace

std::unique_ptr<MidiPlatform> CreateMidiPlatform() {
    return std::make_unique<WinMidiPlatform>();
}

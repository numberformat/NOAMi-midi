#include "MidiPlatform.h"

#include <CoreMIDI/CoreMIDI.h>
#include <CoreFoundation/CoreFoundation.h>

#include <algorithm>
#include <chrono>
#include <cstring>
#include <string>
#include <vector>

namespace {

std::string CFStringToStdString(CFStringRef cfstr) {
    if (!cfstr)
        return "";
    char buffer[256];
    if (CFStringGetCString(cfstr, buffer, sizeof(buffer), kCFStringEncodingUTF8))
        return std::string(buffer);
    const CFIndex length = CFStringGetLength(cfstr);
    const CFIndex maxSize = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8) + 1;
    std::string result(static_cast<size_t>(maxSize), '\0');
    if (CFStringGetCString(cfstr, result.data(), maxSize, kCFStringEncodingUTF8)) {
        result.resize(strlen(result.c_str()));
        return result;
    }
    return "";
}

class MacMidiPlatform final : public MidiPlatform {
public:
    MacMidiPlatform() {
        if (MIDIClientCreate(CFSTR("NOAMi MIDI Client"), nullptr, nullptr, &client_) != noErr)
            client_ = 0;
    }

    ~MacMidiPlatform() override {
        CloseInput();
        CloseOutput();
        if (client_)
            MIDIClientDispose(client_);
    }

    uint32_t GetInputDeviceCount() const override {
        return static_cast<uint32_t>(MIDIGetNumberOfSources());
    }

    uint32_t GetOutputDeviceCount() const override {
        return static_cast<uint32_t>(MIDIGetNumberOfDestinations());
    }

    bool GetInputDeviceCaps(uint32_t deviceId, MidiInputCaps &caps) const override {
        if (deviceId >= GetInputDeviceCount())
            return false;
        MIDIEndpointRef source = MIDIGetSource(deviceId);
        if (!source)
            return false;
        caps.name = GetEndpointName(source);
        caps.manufacturerId = 0;
        caps.productId = 0;
        caps.driverVersion = 0;
        return true;
    }

    bool GetOutputDeviceCaps(uint32_t deviceId, MidiOutputCaps &caps) const override {
        if (deviceId >= GetOutputDeviceCount())
            return false;
        MIDIEndpointRef dest = MIDIGetDestination(deviceId);
        if (!dest)
            return false;
        caps.name = GetEndpointName(dest);
        caps.manufacturerId = 0;
        caps.productId = 0;
        caps.driverVersion = 0;
        caps.technologyRaw = 0;
        caps.technology = MidiPortTechnology::Unknown;
        return true;
    }

    bool OpenOutput(uint32_t deviceId) override {
        if (deviceId >= GetOutputDeviceCount() || !client_)
            return false;
        CloseOutput();

        if (!output_port_) {
            if (MIDIOutputPortCreate(client_, CFSTR("NOAMi MIDI Out"), &output_port_) != noErr) {
                output_port_ = 0;
                return false;
            }
        }

        current_destination_ = MIDIGetDestination(deviceId);
        return current_destination_ != 0;
    }

    bool CloseOutput() override {
        current_destination_ = 0;
        if (output_port_) {
            MIDIPortDispose(output_port_);
            output_port_ = 0;
        }
        return true;
    }

    bool SendShortMessage(uint32_t message) override {
        if (!output_port_ || !current_destination_)
            return false;

        uint8_t data[3] = {
            static_cast<uint8_t>(message & 0xFF),
            static_cast<uint8_t>((message >> 8) & 0xFF),
            static_cast<uint8_t>((message >> 16) & 0xFF)
        };
        const size_t length = (data[2] == 0) ? ((data[1] == 0) ? 1 : 2) : 3;
        return SendPacket(data, length);
    }

    bool SendLongMessage(const uint8_t *data, size_t size) override {
        if (!output_port_ || !current_destination_ || !data || size == 0)
            return false;

        const size_t bufferSize = sizeof(MIDIPacketList) + size;
        std::vector<uint8_t> buffer(bufferSize);
        MIDIPacketList *packetList = reinterpret_cast<MIDIPacketList *>(buffer.data());
        MIDIPacket *packet = MIDIPacketListInit(packetList);
        if (!packet)
            return false;
        packet = MIDIPacketListAdd(packetList, bufferSize, packet, 0, size, data);
        if (!packet)
            return false;
        return MIDISend(output_port_, current_destination_, packetList) == noErr;
    }

    void ResetOutput() override {
        // No direct equivalent on macOS; noop.
    }

    bool OpenInput(uint32_t deviceId, MidiInputCallback callback) override {
        if (deviceId >= GetInputDeviceCount() || !client_)
            return false;
        CloseInput();

        input_callback_ = std::move(callback);
        current_source_ = MIDIGetSource(deviceId);
        if (!current_source_)
            return false;

        if (!input_port_) {
            if (MIDIInputPortCreate(client_, CFSTR("NOAMi MIDI In"), MidiReadProc, this, &input_port_) != noErr) {
                input_port_ = 0;
                current_source_ = 0;
                input_callback_ = nullptr;
                return false;
            }
        }
        return true;
    }

    bool CloseInput() override {
        if (listening_ && input_port_ && current_source_)
            MIDIPortDisconnectSource(input_port_, current_source_);
        listening_ = false;
        current_source_ = 0;
        input_callback_ = nullptr;
        if (input_port_) {
            MIDIPortDispose(input_port_);
            input_port_ = 0;
        }
        return true;
    }

    bool StartInput() override {
        if (!input_port_ || !current_source_)
            return false;
        if (listening_)
            return true;
        const OSStatus status = MIDIPortConnectSource(input_port_, current_source_, this);
        listening_ = (status == noErr);
        return listening_;
    }

    bool StopInput() override {
        if (!input_port_ || !current_source_)
            return false;
        if (!listening_)
            return true;
        const OSStatus status = MIDIPortDisconnectSource(input_port_, current_source_);
        if (status == noErr)
            listening_ = false;
        return status == noErr;
    }

private:
    static void MidiReadProc(const MIDIPacketList *pktlist, void *readProcRefCon, void *srcConnRefCon) {
        (void) srcConnRefCon;
        auto *self = static_cast<MacMidiPlatform *>(readProcRefCon);
        if (self)
            self->DispatchPackets(pktlist);
    }

    void DispatchPackets(const MIDIPacketList *pktlist) {
        if (!pktlist || !input_callback_)
            return;

        const MIDIPacket *packet = &pktlist->packet[0];
        for (unsigned int i = 0; i < pktlist->numPackets; ++i) {
            if (packet->length == 0) {
                packet = MIDIPacketNext(const_cast<MIDIPacket *>(packet));
                continue;
            }
            MidiInputMessage message;
            if (packet->length <= 3) {
                message.type = MidiInputMessage::Type::ShortMessage;
                uint32_t packed = 0;
                for (UInt16 b = 0; b < packet->length; ++b)
                    packed |= static_cast<uint32_t>(packet->data[b]) << (8 * b);
                message.shortMessage = packed;
                input_callback_(message);
            } else {
                message.type = MidiInputMessage::Type::LongData;
                message.longData.assign(packet->data, packet->data + packet->length);
                input_callback_(message);
            }
            packet = MIDIPacketNext(const_cast<MIDIPacket *>(packet));
        }
    }

    bool SendPacket(const uint8_t *data, size_t length) {
        if (!data || length == 0 || !output_port_ || !current_destination_)
            return false;
        const size_t bufferSize = sizeof(MIDIPacketList) + length;
        std::vector<uint8_t> buffer(bufferSize);
        MIDIPacketList *packetList = reinterpret_cast<MIDIPacketList *>(buffer.data());
        MIDIPacket *packet = MIDIPacketListInit(packetList);
        if (!packet)
            return false;
        packet = MIDIPacketListAdd(packetList, bufferSize, packet, 0, length, data);
        if (!packet)
            return false;
        return MIDISend(output_port_, current_destination_, packetList) == noErr;
    }

    std::string GetEndpointName(MIDIEndpointRef endpoint) const {
        if (!endpoint)
            return "";
        CFStringRef name = nullptr;
        if (MIDIObjectGetStringProperty(endpoint, kMIDIPropertyDisplayName, &name) != noErr || !name)
            return "";
        std::string result = CFStringToStdString(name);
        CFRelease(name);
        return result;
    }

    MIDIClientRef client_ = 0;
    MIDIPortRef output_port_ = 0;
    MIDIPortRef input_port_ = 0;
    MIDIEndpointRef current_destination_ = 0;
    MIDIEndpointRef current_source_ = 0;
    bool listening_ = false;
    MidiInputCallback input_callback_;
};

} // namespace

std::unique_ptr<MidiPlatform> CreateMidiPlatform() {
    return std::make_unique<MacMidiPlatform>();
}

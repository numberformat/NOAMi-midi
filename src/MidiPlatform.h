#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

enum class MidiPortTechnology {
    Unknown = 0,
    MidiPort,
    Synth,
    SquareWaveSynth,
    FmSynth,
    MidiMapper,
    WavetableSynth,
    SoftwareSynth,
};

struct MidiInputCaps {
    std::string name;
    uint16_t manufacturerId = 0;
    uint16_t productId = 0;
    uint32_t driverVersion = 0;
};

struct MidiOutputCaps {
    std::string name;
    uint16_t manufacturerId = 0;
    uint16_t productId = 0;
    uint32_t driverVersion = 0;
    uint16_t technologyRaw = 0;
    MidiPortTechnology technology = MidiPortTechnology::Unknown;
};

struct MidiInputMessage {
    enum class Type {
        Opened,
        Closed,
        ShortMessage,
        LongData,
    };

    Type type = Type::Opened;
    uint32_t shortMessage = 0;
    std::vector<uint8_t> longData;
};

using MidiInputCallback = std::function<void(const MidiInputMessage &)>;

class MidiPlatform {
public:
    virtual ~MidiPlatform() = default;

    virtual uint32_t GetInputDeviceCount() const = 0;
    virtual uint32_t GetOutputDeviceCount() const = 0;
    virtual bool GetInputDeviceCaps(uint32_t deviceId, MidiInputCaps &caps) const = 0;
    virtual bool GetOutputDeviceCaps(uint32_t deviceId, MidiOutputCaps &caps) const = 0;

    virtual bool OpenOutput(uint32_t deviceId) = 0;
    virtual bool CloseOutput() = 0;
    virtual bool SendShortMessage(uint32_t message) = 0;
    virtual bool SendLongMessage(const uint8_t *data, size_t size) = 0;
    virtual void ResetOutput() = 0;

    virtual bool OpenInput(uint32_t deviceId, MidiInputCallback callback) = 0;
    virtual bool CloseInput() = 0;
    virtual bool StartInput() = 0;
    virtual bool StopInput() = 0;
};

std::unique_ptr<MidiPlatform> CreateMidiPlatform();

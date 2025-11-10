#pragma once

#include <cstddef>
#include <cstdint>

#include "MidiPlatform.h"

class MidiBackend {
public:
    explicit MidiBackend(MidiPlatform &platform);

    bool OpenOutput(int deviceId);
    bool CloseOutput();
    bool HasOutput() const;
    bool SendShortMessage(uint32_t message);
    bool SendLongMessage(const uint8_t *data, size_t size);
    void ResetOutput();

    bool OpenInput(int deviceId, MidiInputCallback callback);
    bool CloseInput();
    bool HasInput() const;
    bool StartInput();
    bool StopInput();

private:
    MidiPlatform &platform_;
    bool output_open_ = false;
    bool input_open_ = false;
    MidiInputCallback input_callback_;
};

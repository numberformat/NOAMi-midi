#include "MidiBackend.h"

#include <utility>

MidiBackend::MidiBackend(MidiPlatform &platform) : platform_(platform) {}

bool MidiBackend::OpenOutput(int deviceId) {
    if (output_open_)
        CloseOutput();
    output_open_ = platform_.OpenOutput(static_cast<uint32_t>(deviceId));
    return output_open_;
}

bool MidiBackend::CloseOutput() {
    if (!output_open_)
        return true;
    const bool closed = platform_.CloseOutput();
    if (closed)
        output_open_ = false;
    return closed;
}

bool MidiBackend::HasOutput() const {
    return output_open_;
}

bool MidiBackend::SendShortMessage(uint32_t message) {
    if (!output_open_)
        return false;
    return platform_.SendShortMessage(message);
}

bool MidiBackend::SendLongMessage(const uint8_t *data, size_t size) {
    if (!output_open_)
        return false;
    return platform_.SendLongMessage(data, size);
}

void MidiBackend::ResetOutput() {
    if (output_open_)
        platform_.ResetOutput();
}

bool MidiBackend::OpenInput(int deviceId, MidiInputCallback callback) {
    CloseInput();
    input_callback_ = std::move(callback);
    input_open_ = platform_.OpenInput(static_cast<uint32_t>(deviceId), input_callback_);
    return input_open_;
}

bool MidiBackend::CloseInput() {
    if (!input_open_)
        return true;
    const bool closed = platform_.CloseInput();
    if (closed) {
        input_open_ = false;
        input_callback_ = nullptr;
    }
    return closed;
}

bool MidiBackend::HasInput() const {
    return input_open_;
}

bool MidiBackend::StartInput() {
    if (!input_open_)
        return false;
    return platform_.StartInput();
}

bool MidiBackend::StopInput() {
    if (!input_open_)
        return false;
    return platform_.StopInput();
}

#include "MidiPlatform.h"
#include "MidiBackend.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

namespace {

void PrintInputDevices(const MidiPlatform &platform) {
    const uint32_t count = platform.GetInputDeviceCount();
    std::cout << "Input devices (" << count << "):\n";
    for (uint32_t i = 0; i < count; ++i) {
        MidiInputCaps caps;
        if (platform.GetInputDeviceCaps(i, caps))
            std::cout << "  [" << i << "] " << caps.name << "\n";
    }
}

void PrintOutputDevices(const MidiPlatform &platform) {
    const uint32_t count = platform.GetOutputDeviceCount();
    std::cout << "Output devices (" << count << "):\n";
    for (uint32_t i = 0; i < count; ++i) {
        MidiOutputCaps caps;
        if (platform.GetOutputDeviceCaps(i, caps))
            std::cout << "  [" << i << "] " << caps.name << "\n";
    }
}

} // namespace

int main() {
    auto platform = CreateMidiPlatform();
    if (!platform) {
        std::cerr << "Failed to create MIDI platform.\n";
        return 1;
    }

    PrintInputDevices(*platform);
    PrintOutputDevices(*platform);

    MidiBackend backend(*platform);

    if (platform->GetOutputDeviceCount() > 0) {
        std::cout << "Opening first output device...\n";
        if (backend.OpenOutput(0)) {
            backend.SendShortMessage(0x007F0090); // Note on (C4)
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            backend.SendShortMessage(0x00000090); // Note off
            backend.CloseOutput();
            std::cout << "Sent test note.\n";
        } else {
            std::cout << "Unable to open output device.\n";
        }
    }

    if (platform->GetInputDeviceCount() > 0) {
        std::cout << "Listening to first input device for 2 seconds...\n";
        bool received = false;
        auto callback = [&received](const MidiInputMessage &msg) {
            if (msg.type == MidiInputMessage::Type::ShortMessage) {
                std::cout << "Received short message: 0x" << std::hex << msg.shortMessage << std::dec << "\n";
            } else if (msg.type == MidiInputMessage::Type::LongData) {
                std::cout << "Received sysex of " << msg.longData.size() << " bytes.\n";
            }
            received = true;
        };
        if (backend.OpenInput(0, callback)) {
            backend.StartInput();
            std::this_thread::sleep_for(std::chrono::seconds(2));
            backend.StopInput();
            backend.CloseInput();
            if (!received)
                std::cout << "No input received.\n";
        } else {
            std::cout << "Unable to open input device.\n";
        }
    }

    return 0;
}

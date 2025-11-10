#pragma once

#include <windows.h>

#include "MidiPlatform.h"

struct midi_device_t;

class MidiDeviceManager {
public:
    explicit MidiDeviceManager(MidiPlatform &platform);
    ~MidiDeviceManager();

    void PopulateCombos(HWND midiInCombo, HWND midiOutCombo, int midiInSelection, int midiOutSelection);
    midi_device_t *Head() const;

private:
    void Clear();
    void AppendDevice(midi_device_t *device);

    MidiPlatform &platform_;
    midi_device_t *head_;
    midi_device_t *tail_;
};

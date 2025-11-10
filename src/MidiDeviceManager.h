#pragma once

#include <windows.h>

struct midi_device_t;

class MidiDeviceManager {
public:
    MidiDeviceManager();
    ~MidiDeviceManager();

    void PopulateCombos(HWND midiInCombo, HWND midiOutCombo, int midiInSelection, int midiOutSelection);
    midi_device_t *Head() const;

private:
    void Clear();
    void AppendDevice(midi_device_t *device);

    midi_device_t *head_;
    midi_device_t *tail_;
};

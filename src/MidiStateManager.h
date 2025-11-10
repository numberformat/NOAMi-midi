#pragma once

#include <windows.h>

#include "tmidi.h"

class MidiStateManager {
public:
    explicit MidiStateManager(midi_state_t &state);

    midi_state_t &State();

    void ResetForNewSong();
    void ResetChannelOverrides();

private:
    midi_state_t &state_;
};

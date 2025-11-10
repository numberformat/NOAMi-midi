#pragma once

#include <windows.h>

#include "tmidi.h"

class MidiStateManager {
public:
    explicit MidiStateManager(midi_state_t &state);

    midi_state_t &State();

    void ResetForNewSong();
    void ResetChannelOverrides();
    void ResetModulation();

    void SetChannelMute(int channel, bool muted);
    void ToggleChannelMute(int channel);
    bool IsChannelMuted(int channel) const;

    void SetModVelocity(int value);

private:
    midi_state_t &state_;
};

#include "MidiStateManager.h"

MidiStateManager::MidiStateManager(midi_state_t &state) : state_(state) {}

midi_state_t &MidiStateManager::State() {
    return state_;
}

void MidiStateManager::ResetForNewSong() {
    state_.uses_percussion = 0;
    state_.highest_pitch_bend = 0;
    state_.perform_analysis = 0;
    ResetChannelOverrides();
}

void MidiStateManager::ResetChannelOverrides() {
    for (int i = 0; i < 16; ++i) {
        state_.channels[i].program_overridden = 0;
        state_.channels[i].muted = 0;
    }
}

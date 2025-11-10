#include "MidiStateManager.h"

MidiStateManager::MidiStateManager(midi_state_t &state) : state_(state) {}

midi_state_t &MidiStateManager::State() {
    return state_;
}

void MidiStateManager::ResetForNewSong() {
    state_.uses_percussion = 0;
    state_.highest_pitch_bend = 0;
    ResetChannelOverrides();
    ResetModulation();
}

void MidiStateManager::ResetChannelOverrides() {
    for (int i = 0; i < 16; ++i) {
        state_.channels[i].program_overridden = 0;
        state_.channels[i].muted = 0;
    }
}

void MidiStateManager::ResetModulation() {
    state_.mod_velocity = 0;
    state_.mod_pitch = 0;
}

void MidiStateManager::SetChannelMute(int channel, bool muted) {
    if (channel < 0 || channel >= 16)
        return;
    state_.channels[channel].muted = muted ? 1 : 0;
}

void MidiStateManager::ToggleChannelMute(int channel) {
    if (channel < 0 || channel >= 16)
        return;
    state_.channels[channel].muted = !state_.channels[channel].muted;
}

bool MidiStateManager::IsChannelMuted(int channel) const {
    if (channel < 0 || channel >= 16)
        return false;
    return state_.channels[channel].muted != 0;
}

void MidiStateManager::SetModVelocity(int value) {
    state_.mod_velocity = value;
}

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

void MidiStateManager::ResetModVelocity() {
    state_.mod_velocity = 0;
}

void MidiStateManager::SetChannelLock(int channel, bool locked) {
    if (channel < 0 || channel >= 16)
        return;
    state_.channels[channel].lock_controller = locked ? 1 : 0;
}

bool MidiStateManager::IsChannelLocked(int channel) const {
    if (channel < 0 || channel >= 16)
        return false;
    return state_.channels[channel].lock_controller != 0;
}

void MidiStateManager::SetChannelDisplayedController(int channel, int controller) {
    if (channel < 0 || channel >= 16)
        return;
    state_.channels[channel].displayed_controller = controller;
}

int MidiStateManager::ChannelDisplayedController(int channel) const {
    if (channel < 0 || channel >= 16)
        return -1;
    return state_.channels[channel].displayed_controller;
}

void MidiStateManager::MarkChannelDrawn(int channel) {
    if (channel < 0 || channel >= 16)
        return;
    state_.channels[channel].drawn = 1;
}

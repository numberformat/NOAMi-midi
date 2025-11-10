#include "MidiStateManager.h"

#include <cstring>

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

void MidiStateManager::SetControllerValue(int channel, int controller, int value) {
    if (channel < 0 || channel >= 16)
        return;
    if (controller < 0 || controller >= 128)
        return;
    state_.channels[channel].controllers[controller] = value;
}

int MidiStateManager::ControllerValue(int channel, int controller) const {
    if (channel < 0 || channel >= 16)
        return -1;
    if (controller < 0 || controller >= 128)
        return -1;
    return state_.channels[channel].controllers[controller];
}

void MidiStateManager::SetPlaying(bool playing) {
    state_.playing = playing ? 1 : 0;
}

bool MidiStateManager::IsPlaying() const {
    return state_.playing != 0;
}

void MidiStateManager::SetPaused(bool paused) {
    state_.paused = paused ? 1 : 0;
}

bool MidiStateManager::IsPaused() const {
    return state_.paused != 0;
}

void MidiStateManager::RequestStop() {
    state_.stop_requested = 1;
}

void MidiStateManager::ClearStopRequest() {
    state_.stop_requested = 0;
}

bool MidiStateManager::StopRequested() const {
    return state_.stop_requested != 0;
}

void MidiStateManager::SetFinishedNaturally(bool finished) {
    state_.finished_naturally = finished ? 1 : 0;
}

bool MidiStateManager::FinishedNaturally() const {
    return state_.finished_naturally != 0;
}

void MidiStateManager::SetAnalyzing(bool analyzing) {
    state_.analyzing = analyzing ? 1 : 0;
}

bool MidiStateManager::IsAnalyzing() const {
    return state_.analyzing != 0;
}

void MidiStateManager::SetLastNote(int channel, unsigned char pitch, unsigned char velocity) {
    if (channel < 0 || channel >= 16)
        return;
    state_.channels[channel].last_note_pitch = static_cast<signed char>(pitch);
    state_.channels[channel].last_note_velocity = static_cast<signed char>(velocity);
}

void MidiStateManager::UpdateNoteVolume(int channel, int note, unsigned char volume) {
    if (channel < 0 || channel >= 16)
        return;
    if (note < 0 || note >= 128)
        return;
    unsigned char &slot = state_.channels[channel].notes[note];
    if (volume)
    {
        if (!slot)
            state_.channels[channel].note_count++;
    }
    else
    {
        if (slot)
            state_.channels[channel].note_count--;
    }
    slot = volume;
}

void MidiStateManager::ResetChannelNotes(int channel) {
    if (channel < 0 || channel >= 16)
        return;
    memset(state_.channels[channel].notes, 0, sizeof(state_.channels[channel].notes));
    state_.channels[channel].note_count = 0;
}

void MidiStateManager::ResetAllChannelNotes() {
    for (int channel = 0; channel < 16; ++channel)
        ResetChannelNotes(channel);
}

void MidiStateManager::SetSeekSliding(bool sliding) {
    state_.seek_sliding = sliding ? 1 : 0;
}

bool MidiStateManager::SeekSliding() const {
    return state_.seek_sliding != 0;
}

void MidiStateManager::SetSeeking(bool seeking) {
    state_.seeking = seeking ? 1 : 0;
}

bool MidiStateManager::Seeking() const {
    return state_.seeking != 0;
}

void MidiStateManager::SetSeekTarget(double milliseconds) {
    state_.seek_to = milliseconds;
}

double MidiStateManager::SeekTarget() const {
    return state_.seek_to;
}

void MidiStateManager::SetLoopCount(int count) {
    state_.loop_count = count;
}

int MidiStateManager::LoopCount() const {
    return state_.loop_count;
}

bool MidiStateManager::ConsumeLoopIteration() {
    if (state_.loop_count > 0) {
        state_.loop_count--;
        return true;
    }
    return false;
}

void MidiStateManager::IncrementLoopCount() {
    state_.loop_count++;
}

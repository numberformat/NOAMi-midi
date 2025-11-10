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
    void ResetModVelocity();

    void SetChannelLock(int channel, bool locked);
    bool IsChannelLocked(int channel) const;
    void SetChannelDisplayedController(int channel, int controller);
    int  ChannelDisplayedController(int channel) const;
    void MarkChannelDrawn(int channel);

    void SetControllerValue(int channel, int controller, int value);
    int ControllerValue(int channel, int controller) const;

    void SetPlaying(bool playing);
    bool IsPlaying() const;
    void SetPaused(bool paused);
    bool IsPaused() const;
    void RequestStop();
    void ClearStopRequest();
    bool StopRequested() const;
    void SetFinishedNaturally(bool finished);
    bool FinishedNaturally() const;
    void SetAnalyzing(bool analyzing);
    bool IsAnalyzing() const;
    void SetSeekSliding(bool sliding);
    bool SeekSliding() const;
    void SetSeeking(bool seeking);
    bool Seeking() const;
    void SetSeekTarget(double milliseconds);
    double SeekTarget() const;
    void SetLoopCount(int count);
    int LoopCount() const;

    void SetLastNote(int channel, unsigned char pitch, unsigned char velocity);
    void UpdateNoteVolume(int channel, int note, unsigned char volume);
    void ResetChannelNotes(int channel);
    void ResetAllChannelNotes();

private:
    midi_state_t &state_;
};

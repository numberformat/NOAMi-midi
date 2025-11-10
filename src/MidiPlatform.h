#pragma once

#include <windows.h>
#include <mmsystem.h>

namespace MidiPlatform {

UINT GetInputDeviceCount();
UINT GetOutputDeviceCount();
MMRESULT GetInputDeviceCaps(UINT deviceId, MIDIINCAPS *caps);
MMRESULT GetOutputDeviceCaps(UINT deviceId, MIDIOUTCAPS *caps);

MMRESULT OpenInput(HMIDIIN *handle, UINT deviceId, DWORD_PTR callback, DWORD_PTR instance);
void CloseInput(HMIDIIN handle);
MMRESULT StartInput(HMIDIIN handle);
MMRESULT StopInput(HMIDIIN handle);

MMRESULT OpenOutput(HMIDIOUT *handle, UINT deviceId);
MMRESULT CloseOutput(HMIDIOUT handle);
void ResetOutput(HMIDIOUT handle);

MMRESULT SendShortMessage(HMIDIOUT handle, DWORD message);
MMRESULT PrepareLongMessage(HMIDIOUT handle, MIDIHDR *header);
MMRESULT SendLongMessage(HMIDIOUT handle, MIDIHDR *header);
MMRESULT UnprepareLongMessage(HMIDIOUT handle, MIDIHDR *header);

} // namespace MidiPlatform

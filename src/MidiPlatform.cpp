#include "MidiPlatform.h"

namespace MidiPlatform {

UINT GetInputDeviceCount() {
    return midiInGetNumDevs();
}

UINT GetOutputDeviceCount() {
    return midiOutGetNumDevs();
}

MMRESULT GetInputDeviceCaps(UINT deviceId, MIDIINCAPS *caps) {
    return midiInGetDevCaps(deviceId, caps, sizeof(MIDIINCAPS));
}

MMRESULT GetOutputDeviceCaps(UINT deviceId, MIDIOUTCAPS *caps) {
    return midiOutGetDevCaps(deviceId, caps, sizeof(MIDIOUTCAPS));
}

MMRESULT OpenInput(HMIDIIN *handle, UINT deviceId, DWORD_PTR callback, DWORD_PTR instance) {
    return midiInOpen(handle, deviceId, callback, instance, CALLBACK_FUNCTION);
}

void CloseInput(HMIDIIN handle) {
    if (handle)
        midiInClose(handle);
}

MMRESULT StartInput(HMIDIIN handle) {
    return midiInStart(handle);
}

MMRESULT StopInput(HMIDIIN handle) {
    return midiInStop(handle);
}

MMRESULT OpenOutput(HMIDIOUT *handle, UINT deviceId) {
    return midiOutOpen(handle, deviceId, 0, 0, 0);
}

MMRESULT CloseOutput(HMIDIOUT handle) {
    if (!handle)
        return MMSYSERR_NOERROR;
    return midiOutClose(handle);
}

void ResetOutput(HMIDIOUT handle) {
    if (handle)
        midiOutReset(handle);
}

MMRESULT SendShortMessage(HMIDIOUT handle, DWORD message) {
    return midiOutShortMsg(handle, message);
}

MMRESULT PrepareLongMessage(HMIDIOUT handle, MIDIHDR *header) {
    return midiOutPrepareHeader(handle, header, sizeof(MIDIHDR));
}

MMRESULT SendLongMessage(HMIDIOUT handle, MIDIHDR *header) {
    return midiOutLongMsg(handle, header, sizeof(MIDIHDR));
}

MMRESULT UnprepareLongMessage(HMIDIOUT handle, MIDIHDR *header) {
    return midiOutUnprepareHeader(handle, header, sizeof(MIDIHDR));
}

} // namespace MidiPlatform

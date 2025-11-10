#include "MidiDeviceManager.h"

#include <mmsystem.h>
#include <stdlib.h>
#include <string.h>

#include "tmidi.h"

MidiDeviceManager::MidiDeviceManager() : head_(NULL), tail_(NULL) {}

MidiDeviceManager::~MidiDeviceManager() {
    Clear();
}

void MidiDeviceManager::PopulateCombos(HWND midiInCombo, HWND midiOutCombo, int midiInSelection, int midiOutSelection) {
    Clear();

    if (midiInCombo) {
        SendMessage(midiInCombo, CB_RESETCONTENT, 0, 0);
        SendMessage(midiInCombo, CB_ADDSTRING, 0, (LPARAM) "[None]");
    }

    const int indevs = midiInGetNumDevs();
    for (int i = 0; i < indevs; i++) {
        MIDIINCAPS incaps;
        if (midiInGetDevCaps(i, &incaps, sizeof(incaps)) != MMSYSERR_NOERROR)
            continue;

        midi_device_t *dev = (midi_device_t *) calloc(1, sizeof(midi_device_t));
        if (!dev)
            continue;

        dev->user_device_name = strdup(incaps.szPname);
        if (!dev->user_device_name) {
            free(dev);
            continue;
        }
        dev->input_device = 1;
        memcpy(&dev->incaps, &incaps, sizeof(MIDIINCAPS));
        dev->usable = 1;
        dev->standards = MIDI_STANDARD_GM;
        dev->next = NULL;

        AppendDevice(dev);

        if (midiInCombo)
            SendMessage(midiInCombo, CB_ADDSTRING, 0, (LPARAM) incaps.szPname);
    }

    if (midiInCombo)
        SendMessage(midiInCombo, CB_SETCURSEL, (WPARAM) midiInSelection, 0);

    if (midiOutCombo) {
        SendMessage(midiOutCombo, CB_RESETCONTENT, 0, 0);
        SendMessage(midiOutCombo, CB_ADDSTRING, 0, (LPARAM) "[None]");
    }

    const int outdevs = midiOutGetNumDevs();
    for (int i = 0; i < outdevs; i++) {
        MIDIOUTCAPS outcaps;
        if (midiOutGetDevCaps(i, &outcaps, sizeof(outcaps)) != MMSYSERR_NOERROR)
            continue;

        midi_device_t *dev = (midi_device_t *) calloc(1, sizeof(midi_device_t));
        if (!dev)
            continue;

        dev->user_device_name = strdup(outcaps.szPname);
        if (!dev->user_device_name) {
            free(dev);
            continue;
        }
        dev->input_device = 0;
        memcpy(&dev->outcaps, &outcaps, sizeof(MIDIOUTCAPS));
        dev->usable = 1;
        dev->standards = MIDI_STANDARD_GM;
        dev->next = NULL;

        AppendDevice(dev);

        if (midiOutCombo)
            SendMessage(midiOutCombo, CB_ADDSTRING, 0, (LPARAM) outcaps.szPname);
    }

    if (midiOutCombo)
        SendMessage(midiOutCombo, CB_SETCURSEL, (WPARAM) midiOutSelection, 0);
}

midi_device_t *MidiDeviceManager::Head() const {
    return head_;
}

void MidiDeviceManager::Clear() {
    midi_device_t *current = head_;
    while (current) {
        midi_device_t *next = current->next;
        if (current->user_device_name)
            free(current->user_device_name);
        free(current);
        current = next;
    }
    head_ = tail_ = NULL;
}

void MidiDeviceManager::AppendDevice(midi_device_t *device) {
    if (!head_) {
        head_ = tail_ = device;
        return;
    }
    tail_->next = device;
    tail_ = device;
}

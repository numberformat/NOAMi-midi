#include "MidiDeviceManager.h"
#include "MidiPlatform.h"

#include <cstdint>
#include <stdlib.h>
#include <string.h>

#include "tmidi.h"

MidiDeviceManager::MidiDeviceManager(MidiPlatform &platform) : platform_(platform), head_(NULL), tail_(NULL) {}

MidiDeviceManager::~MidiDeviceManager() {
    Clear();
}

void MidiDeviceManager::PopulateCombos(HWND midiInCombo, HWND midiOutCombo, int midiInSelection, int midiOutSelection) {
    Clear();

    if (midiInCombo) {
        SendMessage(midiInCombo, CB_RESETCONTENT, 0, 0);
        SendMessage(midiInCombo, CB_ADDSTRING, 0, (LPARAM) "[None]");
    }

    const uint32_t indevs = platform_.GetInputDeviceCount();
    for (uint32_t i = 0; i < indevs; i++) {
        MidiInputCaps incaps;
        if (!platform_.GetInputDeviceCaps(i, incaps))
            continue;

        midi_device_t *dev = (midi_device_t *) calloc(1, sizeof(midi_device_t));
        if (!dev)
            continue;

        dev->user_device_name = strdup(incaps.name.c_str());
        if (!dev->user_device_name) {
            free(dev);
            continue;
        }
        dev->input_device = 1;
        dev->usable = 1;
        dev->standards = MIDI_STANDARD_GM;
        dev->next = NULL;
        dev->manufacturer_id = incaps.manufacturerId;
        dev->product_id = incaps.productId;
        dev->driver_version = incaps.driverVersion;
        dev->technology_raw = 0;
        dev->technology = MidiPortTechnology::Unknown;

        AppendDevice(dev);

        if (midiInCombo)
            SendMessage(midiInCombo, CB_ADDSTRING, 0, (LPARAM) incaps.name.c_str());
    }

    if (midiInCombo)
        SendMessage(midiInCombo, CB_SETCURSEL, (WPARAM) midiInSelection, 0);

    if (midiOutCombo) {
        SendMessage(midiOutCombo, CB_RESETCONTENT, 0, 0);
        SendMessage(midiOutCombo, CB_ADDSTRING, 0, (LPARAM) "[None]");
    }

    const uint32_t outdevs = platform_.GetOutputDeviceCount();
    for (uint32_t i = 0; i < outdevs; i++) {
        MidiOutputCaps outcaps;
        if (!platform_.GetOutputDeviceCaps(i, outcaps))
            continue;

        midi_device_t *dev = (midi_device_t *) calloc(1, sizeof(midi_device_t));
        if (!dev)
            continue;

        dev->user_device_name = strdup(outcaps.name.c_str());
        if (!dev->user_device_name) {
            free(dev);
            continue;
        }
        dev->input_device = 0;
        dev->usable = 1;
        dev->standards = MIDI_STANDARD_GM;
        dev->next = NULL;
        dev->manufacturer_id = outcaps.manufacturerId;
        dev->product_id = outcaps.productId;
        dev->driver_version = outcaps.driverVersion;
        dev->technology_raw = outcaps.technologyRaw;
        dev->technology = outcaps.technology;

        AppendDevice(dev);

        if (midiOutCombo)
            SendMessage(midiOutCombo, CB_ADDSTRING, 0, (LPARAM) outcaps.name.c_str());
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

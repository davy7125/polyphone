/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2026 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program. If not, see http://www.gnu.org/licenses/.    **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef MIDIPROCESSOR_H
#define MIDIPROCESSOR_H

#include <cstddef>
#include "imidivalues.h"
#include "imidilistener.h"

// State of the MIDI channels and processing of the MIDI events before they reach a listener:
// sustain and sostenuto pedals, (N)RPN, bank, relative controllers. The values are read by the modulators.
// The channel -1 is the one of the editor: an event of another channel that is not consumed is repeated on it.
class MidiProcessor: public IMidiValues
{
public:
    MidiProcessor();

    // Receiver of the processed events (possibly null)
    void setListener(IMidiListener * listener) { _listener = listener; }

    // Raw MIDI message: status byte and its data. The controllers of a device (external) can send relative values,
    // detected and converted; the ones of a host or of a file are always absolute.
    void processMidiMessage(const unsigned char * message, size_t length, bool external = true);

    // MIDI events, external being true for the controllers of a device (whose relative values are detected)
    void processKeyOn(int channel, int key, int vel);
    void processKeyOff(int channel, int key);
    void processPolyPressureChanged(int channel, int key, int pressure);
    void processMonoPressureChanged(int channel, int value);
    void processControllerChanged(bool external, int channel, int numController, int value);
    void processBendChanged(int channel, float value);
    void processRPNChanged(int channel, int parameter, int value, bool isRegistered, int trigger);
    void processProgramChanged(int channel, uint8_t preset);

    // Release all keys of the channel -1, including the ones held by the pedals
    void stopAll();

    // Initial state of all channels, without notifying the listener: controllers back to their default values, no key
    // and no pedal held
    void resetChannels();

    // Default value of a controller used as the source of a modulator, if it has not been received
    void setDefaultControllerValue(int controllerNumber, bool isBipolar, bool isDescending);

    // Default values of all controllers back to the ones of a processor just created, before setting the ones of
    // another soundfont (the controllers received keep their value)
    void resetDefaultControllerValues();

    // Default values of all controllers copied from another processor
    void copyDefaultControllerValues(MidiProcessor &other);

    // Value of a controller as long as it has not been received
    int getDefaultControllerValue(int channel, int controllerNumber);

    // Bend sensitivity in semitones (and hundredths)
    void setBendSensitivityValue(int channel, float value);

    // MPE (MIDI Polyphonic Expression), disabled by default: each note is played on its own channel (member channel of a
    // zone) with its own pitch bend, pressure and controllers. The master channel of a zone (first channel for the lower
    // zone, last one for the upper zone) sends the messages for all its notes: its pitch bend is added to the one of each
    // member channel, its controllers are repeated on them (except the timbre, CC 74). Once enabled: a lower zone of 15
    // member channels, changed by the MPE configuration messages (RPN 6). The pitch bend range is 48 semitones on the
    // member channels and 2 on the master ones, changed by the RPN 0 (a member channel changing all of them).
    // The channels go back to their initial state when it is enabled or disabled, without notifying the listener.
    void setMpeEnabled(bool isEnabled);
    bool isMpeEnabled() { return _isMpeEnabled; }

    // IMidiValues implementation
    int getControllerValue(int channel, int controllerNumber) override;
    float getBendValue(int channel) override;
    float getBendSensitivityValue(int channel) override;
    int getMonoPressure(int channel) override;
    int getPolyPressure(int channel, int key) override;

private:
    // MPE zone: lower (master channel 0) or upper (master channel 15), ranges in hundredths of semitone
    struct MpeZone
    {
        int memberCount;
        int masterRange;
        int memberRange;
    };

    int getMpeZone(int channel, bool &isMaster);
    void resetMpeZone(int zone);
    void updateMpeChannel(int channel, bool notify); // Bend and sensitivity of a channel
    void updateMpeZone(int zone, bool notify);

    struct MIDI_State
    {
        // Configuration of a channel, the values being written and read by different threads
        volatile int _controllerValues[128];
        volatile int _controllerDefaultValues[128];
        volatile bool _controllerValueSpecified[128];
        volatile bool _controllerValueIsRelative[128];
        volatile uint8_t _controllerValueIsRelativeCounter[128];
        volatile float _bendValue;
        volatile float _bendSensitivityValue;
        volatile int _monoPressureValue;
        volatile int _polyPressureValues[128];

        // RPN memory
        unsigned char _rpnMSBparameter;
        unsigned char _rpnLSBparameter;
        unsigned char _rpnMSBvalue;
        unsigned char _rpnLSBvalue;
        bool _rpnIsRegistered;
    };

    struct Sustain_State
    {
        bool _currentKeys[128];
        bool _sustainedKeys[128];
        bool _sostenutoMemoryKeys[128];
        bool _isSustainOn;
        bool _isSostenutoOn;
    };

    IMidiListener * _listener;

    // Last values, first is channel -1 (for the editor) then channel 1 to 16
    MIDI_State _midiStates[17];

    // Sustain / Sostenuto pedals
    Sustain_State _sustainStates[17];

    // MPE
    bool _isMpeEnabled;
    MpeZone _mpeZones[2];
    float _mpeBends[16]; // Pitch bend received on each channel
};

#endif // MIDIPROCESSOR_H

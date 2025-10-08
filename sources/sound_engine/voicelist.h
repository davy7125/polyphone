/***************************************************************************
**                                                                        **
**  PVP: Phoenix Voicing Program                                          **
**  Copyright (C) 2013-2024 Davy Triponney                                **
**                2014      Andrea Celani                                 **
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

#ifndef VOICELIST_H
#define VOICELIST_H

#include "basetypes.h"
class Synth;
class Voice;
class VoiceParam;

class VoiceList
{
public:
    VoiceList();
    ~VoiceList();
    void initialize();
    void setThreadCount(int threadCount) { _threadCount = threadCount; }
    void setMaxPolyphony(int maxPolyphony);

    // Prepare multiple voice and then add them, run by the MIDI or GUI thread
    Voice * getVoiceToPrepare();
    void addPreparedVoices();

    // Return all voices currently running and prepared to be run
    void getVoices(Voice ** &voices1, int &count1, Voice ** &voices2, int &count2);
    int getVoiceCount();

    void endComputation();
    void prepareComputation();
    Voice * getNextVoiceToCompute();

private:
    struct ExclusiveClassCloseCommand
    {
        short channel;
        short sf2Id;
        short numPreset;
        short key;
        int exclusiveClass;
        Voice * voice;
    };

    /// Variables shared by different threads

    Voice * _voices[MAX_NUMBER_OF_VOICES];
    VoiceParam * _voiceParameters[MAX_NUMBER_OF_VOICES];
    int _threadCount;

    // Indexes
    volatile int _preparationIndex;  // When voices are being added
    volatile int _additionIndex;     // When a set of voices is complete
    QAtomicInt _currentIndex;        // Index of the last voice being computed
    volatile int _firstRunningIndex; // Index of the first voice to compute
    volatile int _lastRunningIndex;  // Index of the last voice to compute

    /// Variables used by the audio thread only

    volatile int _maxPolyphony; // Also set by the main thread
    int _uncomputedVoiceCount;
    ExclusiveClassCloseCommand _closeCommands[MAX_NUMBER_OF_VOICES];
    unsigned short _closeCommandNumber;
};

#endif // VOICELIST_H

/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
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
    void initialize(Synth * synth, int threadCount);

    // Prepare multiple voice and then add them
    Voice * getVoiceToPrepare();
    void addPreparedVoices();

    // Return all voices currently running and prepared to be run
    void getVoices(Voice ** &voices1, int &count1, Voice ** &voices2, int &count2);

    //
    void endComputation();
    void prepareComputation();
    Voice * getNextVoiceToCompute();

private:
    Voice * _voices[MAX_NUMBER_OF_VOICES];
    VoiceParam * _voiceParameters[MAX_NUMBER_OF_VOICES];
    int _threadCount;

    // Indexes
    QAtomicInt _preparationIndex; // When voices are being added
    QAtomicInt _additionIndex; // When a set of voices is complete
    QAtomicInt _currentIndex;
    QAtomicInt _firstRunningIndex;
    QAtomicInt _lastRunningIndex;
    int _maxPossibleVoicesToCompute;
    int _uncomputedVoiceCount;

    // Exclusive class management
    struct ExclusiveClassCloseCommand
    {
        short channel;
        unsigned short numPreset;
        int exclusiveClass;
    };
    ExclusiveClassCloseCommand _closeCommands[MAX_NUMBER_OF_VOICES];
    unsigned short _closeCommandNumber;
};

#endif // VOICELIST_H

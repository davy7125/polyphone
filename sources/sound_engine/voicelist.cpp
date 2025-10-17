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

#include "voicelist.h"
#include "voice.h"
#include "voiceparam.h"
#include "synth.h"

VoiceList::VoiceList() :
    _threadCount(1),
    _preparationIndex(0),
    _additionIndex(0),
    _currentIndex(0),
    _firstRunningIndex(0),
    _lastRunningIndex(0),
    _maxPolyphony(DEFAULT_POLYPHONY),
    _uncomputedVoiceCount(0)
{
    memset(_voices, 0, MAX_NUMBER_OF_VOICES * sizeof(Voice *));
    memset(_voiceParameters, 0, MAX_NUMBER_OF_VOICES * sizeof(VoiceParam *));
}

VoiceList::~VoiceList()
{
    for (int i = 0; i < MAX_NUMBER_OF_VOICES; ++i)
        delete _voices[i];
    for (int i = 0; i < MAX_NUMBER_OF_VOICES; ++i)
        delete _voiceParameters[i];
}

void VoiceList::initialize(Synth * synth)
{
    for (int i = 0; i < MAX_NUMBER_OF_VOICES; ++i)
    {
        _voiceParameters[i] = new VoiceParam();
        _voices[i] = new Voice(_voiceParameters[i]);
        QObject::connect(_voices[i], SIGNAL(currentPosChanged(quint32)), synth, SIGNAL(currentPosChanged(quint32)));
        QObject::connect(_voices[i], SIGNAL(readFinished(int)), synth, SIGNAL(readFinished(int)));
    }
}

void VoiceList::setMaxPolyphony(int maxPolyphony)
{
    if (maxPolyphony <= 0)
        maxPolyphony = DEFAULT_POLYPHONY;
    else if (maxPolyphony < MIN_POLYPHONY)
        maxPolyphony = MIN_POLYPHONY;
    else if (maxPolyphony > MAX_POLYPHONY)
        maxPolyphony = MAX_POLYPHONY;
    _maxPolyphony = maxPolyphony;
}

Voice * VoiceList::getVoiceToPrepare()
{
    // Current voice range
    int first = (_firstRunningIndex - _threadCount - 1) & VOICE_INDEX_MASK;
    int last = _additionIndex;
    int currentPreparationIndex = _preparationIndex & VOICE_INDEX_MASK;

    // Check the index is available
    if (first <= last)
    {
        if (currentPreparationIndex >= first && currentPreparationIndex < last)
            return nullptr;
    }
    else
    {
        if (currentPreparationIndex >= first || currentPreparationIndex < last)
            return nullptr;
    }

    // Index for preparing a voice
    int index = _preparationIndex & VOICE_INDEX_MASK;
    _preparationIndex += 1;
    return _voices[index];
}

void VoiceList::addPreparedVoices()
{
    _additionIndex = (_preparationIndex & VOICE_INDEX_MASK);
}

void VoiceList::getVoices(Voice ** &voices1, int &count1, Voice ** &voices2, int &count2)
{
    // Return all running and prepared voices
    int first = _firstRunningIndex;
    int last = _additionIndex;
    if (first <= last)
    {
        voices1 = &_voices[first];
        count1 = last - first;
        voices2 = nullptr;
        count2 = 0;
    }
    else
    {
        voices1 = &_voices[first];
        count1 = MAX_NUMBER_OF_VOICES - first;
        voices2 = _voices;
        count2 = last;
    }
}

int VoiceList::getVoiceCount()
{
    return (_lastRunningIndex - _firstRunningIndex) & VOICE_INDEX_MASK;
}

void VoiceList::endComputation()
{
    int first = _firstRunningIndex;
    int last = _lastRunningIndex;
    int currentIndex = _currentIndex.fetchAndStoreOrdered(last) & VOICE_INDEX_MASK;

    // Number of voices that have not been computed
    if (first <= last)
        _uncomputedVoiceCount = (currentIndex < last && currentIndex >= first) ? last - currentIndex : 0;
    else
        _uncomputedVoiceCount = (currentIndex < last || currentIndex >= first) ? (last - currentIndex) & VOICE_INDEX_MASK : 0;
}

void VoiceList::prepareComputation()
{
    int first = _firstRunningIndex;
    int last = _lastRunningIndex;
    int additionIndex = _additionIndex;

    // Minimum number of voices to close
    int numberOfVoicesToClose = ((additionIndex - first) & VOICE_INDEX_MASK) - _maxPolyphony;
    if (_uncomputedVoiceCount > 0)
        numberOfVoicesToClose += (_maxPolyphony >> 3); // The maximum becomes 87.5% of _maxPolyphony

    // Gather the exclusive class close commands
    _closeCommandNumber = 0;
    Voice * voice;
    VoiceParam * voiceParam;
    int exclusiveClass;
    for (int index = last; index != additionIndex; index = (index + 1) & VOICE_INDEX_MASK)
    {
        Voice * voice = _voices[index];
        voiceParam = voice->getParam();
        if (voiceParam->getType() != 0)
            continue;

        exclusiveClass = voiceParam->getInteger(champ_exclusiveClass);
        if (exclusiveClass != 0)
        {
            for (int i = 0; i < _closeCommandNumber; i++)
            {
                if (_closeCommands[i].channel == voiceParam->getChannel() &&
                    _closeCommands[i].sf2Id == voiceParam->getSf2Id() &&
                    _closeCommands[i].numPreset == voiceParam->getPresetId() &&
                    _closeCommands[i].key != voiceParam->getKey() && // The key must be different for the new voices
                    _closeCommands[i].exclusiveClass == exclusiveClass)
                {
                    _closeCommands[i].voice->finish();
                    _closeCommands[i].voice = voice;
                    exclusiveClass = 0; // Cancel the following lines
                    break;
                }
            }

            if (exclusiveClass != 0)
            {
                _closeCommands[_closeCommandNumber].channel = voiceParam->getChannel();
                _closeCommands[_closeCommandNumber].sf2Id = voiceParam->getSf2Id();
                _closeCommands[_closeCommandNumber].numPreset = voiceParam->getPresetId();
                _closeCommands[_closeCommandNumber].key = voiceParam->getKey();
                _closeCommands[_closeCommandNumber].exclusiveClass = exclusiveClass;
                _closeCommands[_closeCommandNumber].voice = voice;
                _closeCommandNumber++;
            }
        }
    }

    // End of voices
    bool quickRelease, close;
    for (int index = first; index != last; index = (index + 1) & VOICE_INDEX_MASK)
    {
        voice = _voices[index];
        voiceParam = voice->getParam();

        close = voice->isFinished() || (numberOfVoicesToClose > 0 && voice->isInRelease());
        quickRelease = false;

        if (!close)
        {
            // Maybe a quick release?
            for (int i = 0; i < _closeCommandNumber; i++)
            {
                if (_closeCommands[i].channel == voiceParam->getChannel() &&
                    _closeCommands[i].sf2Id == voiceParam->getSf2Id() &&
                    _closeCommands[i].numPreset == voiceParam->getPresetId() &&
                    _closeCommands[i].exclusiveClass == voiceParam->getInteger(champ_exclusiveClass))
                {
                    close = numberOfVoicesToClose > 0;
                    quickRelease = true;
                    break;
                }
            }
        }

        if (close)
        {
            voice->finish();
            _voices[index] = _voices[first];
            _voices[first] = voice;

            first = (first + 1) & VOICE_INDEX_MASK;

            --numberOfVoicesToClose;
        }
        else if (quickRelease)
            voice->release(true);
    }

    // If more voices are to be closed, stop the already uncomputed voices
    if (numberOfVoicesToClose > 0 && _uncomputedVoiceCount > 0)
    {
        int index = numberOfVoicesToClose < _uncomputedVoiceCount ?
                        (last - numberOfVoicesToClose) & VOICE_INDEX_MASK :
                        (last - _uncomputedVoiceCount) & VOICE_INDEX_MASK;
        do
        {
            voice = _voices[index];
            voice->finish();
            _voices[index] = _voices[first];
            _voices[first] = voice;

            --numberOfVoicesToClose;
            first = (first + 1) & VOICE_INDEX_MASK;
            index = (index + 1) & VOICE_INDEX_MASK;
        }
        while (index != last);
    }

    // Remove first voices if more of them must be closed
    if (numberOfVoicesToClose > 0)
    {
        last = (first + numberOfVoicesToClose) & VOICE_INDEX_MASK;
        for (; first != last; first = (first + 1) & VOICE_INDEX_MASK)
            _voices[first]->finish();
    }

    // Include the added voices
    _lastRunningIndex = additionIndex;

    // Update the first index
    _firstRunningIndex = first;

    // Reset the running index
    _currentIndex.storeRelease(first);
}

Voice * VoiceList::getNextVoiceToCompute()
{
    int first = _firstRunningIndex;
    int last = _lastRunningIndex;
    int voiceIndex = _currentIndex.fetchAndAddOrdered(1) & VOICE_INDEX_MASK;

    // No more voices to compute?
    if (first <= last)
    {
        if (voiceIndex >= last || voiceIndex < first)
            return nullptr;
    }
    else
    {
        if (voiceIndex >= last && voiceIndex < first)
            return nullptr;
    }

    return _voices[voiceIndex];
}

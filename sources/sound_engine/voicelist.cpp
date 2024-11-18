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
    _maxPossibleVoicesToCompute(2),
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
    int index = _preparationIndex.fetchAndAddRelaxed(1) & VOICE_INDEX_MASK;
    return _voices[index];
}

void VoiceList::addPreparedVoices()
{
    _additionIndex.storeRelaxed(_preparationIndex & VOICE_INDEX_MASK);
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

void VoiceList::endComputation()
{
    int first = _firstRunningIndex;
    int last = _lastRunningIndex;
    int currentIndex = _currentIndex.fetchAndStoreRelaxed(_lastRunningIndex) & VOICE_INDEX_MASK;

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

    // Update the reference of the maximum number of voices to compute
    int voiceCount = (last - first) & VOICE_INDEX_MASK;
    int computedVoiceCount = voiceCount - _uncomputedVoiceCount;
    if (computedVoiceCount > _maxPossibleVoicesToCompute)
        _maxPossibleVoicesToCompute = computedVoiceCount;
    else if (_maxPossibleVoicesToCompute > 4)
        _maxPossibleVoicesToCompute -= 2;

    // Minimum number of voices to close
    int numberOfVoicesToClose = _uncomputedVoiceCount > 0 ? _uncomputedVoiceCount + (_additionIndex - _lastRunningIndex) & VOICE_INDEX_MASK : 0;
    if (voiceCount - numberOfVoicesToClose < _maxPossibleVoicesToCompute)
        numberOfVoicesToClose = voiceCount - _maxPossibleVoicesToCompute;

    // Gather the exclusive class close commands
    _closeCommandNumber = 0;
    VoiceParam * voiceParam;
    int exclusiveClass;
    for (int index = _lastRunningIndex; index != _additionIndex; index = (index + 1) & VOICE_INDEX_MASK)
    {
        voiceParam = _voiceParameters[index];
        if (voiceParam->getType() != 0)
            continue;

        exclusiveClass = voiceParam->getInteger(champ_exclusiveClass);
        if (exclusiveClass != 0)
        {
            _closeCommands[_closeCommandNumber].channel = voiceParam->getChannel();
            _closeCommands[_closeCommandNumber].exclusiveClass = exclusiveClass;
            _closeCommands[_closeCommandNumber].numPreset = voiceParam->getPresetId();
            _closeCommandNumber++;
        }
    }

    // End of voices
    Voice * voice;
    bool quickRelease, close;
    for (int index = first; index != last; index = (index + 1) & VOICE_INDEX_MASK)
    {
        voice = _voices[index];
        voiceParam = _voiceParameters[index];

        close = voice->isFinished() || (numberOfVoicesToClose > 0 && voice->isInRelease());
        quickRelease = false;

        if (!close)
        {
            // Maybe a quick release?
            for (int  i = 0; i < _closeCommandNumber; i++)
            {
                if (_closeCommands[i].channel == voiceParam->getChannel() &&
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
            // Signal emitted for the sample player (voice -1)
            if (voiceParam->getKey() == -1)
                voice->triggerReadFinishedSignal();

            Voice * voiceTmp = _voices[index];
            _voices[index] = _voices[first];
            _voices[first] = voiceTmp;
            first = (first + 1) & VOICE_INDEX_MASK;

            --numberOfVoicesToClose;
        }
        else if (quickRelease)
            voice->release(true);
    }

    // Include the added voices
    _lastRunningIndex = _additionIndex;

    // Remove first voices if more of them must be closed
    if (numberOfVoicesToClose > 0)
        first = (first + numberOfVoicesToClose) & VOICE_INDEX_MASK;
    _firstRunningIndex = first;

    // Reset the running index
    _currentIndex = first;
}

Voice * VoiceList::getNextVoiceToCompute()
{
    int first = _firstRunningIndex;
    int last = _lastRunningIndex;
    int voiceIndex = _currentIndex.fetchAndAddRelaxed(1) & VOICE_INDEX_MASK;

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

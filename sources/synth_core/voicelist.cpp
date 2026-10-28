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
#include <algorithm>
#include <limits>
#include "voice.h"
#include "voiceparam.h"

VoiceList::VoiceList() :
    _listener(nullptr),
    _context(nullptr),
    _reservedChunkCount(0),
    _capacity(0),
    _indexMask(0),
    _threadCount(1),
    _preparationIndex(0),
    _additionIndex(0),
    _currentIndex(0),
    _firstRunningIndex(0),
    _lastRunningIndex(0),
    _maxPolyphony(DEFAULT_POLYPHONY),
    _isRealTime(true),
    _cpuBudget(0.01f * DEFAULT_CPU_BUDGET),
    _uncomputedVoiceCount(0),
    _closeCommandNumber(0),
    _chunkDuration(COMPUTATION_CHUNK / 44100.0),
    _voiceChunkCost(0),
    _otherChunkCost(0),
    _costMeasureCount(0)
{}

VoiceList::~VoiceList()
{
    deleteVoices();
}

void VoiceList::initialize(ISynthListener * listener, const VoiceContext * context)
{
    _listener = listener;
    _context = context;
    createVoices();
}

void VoiceList::createVoices()
{
    // Room for the voices fading out, and for the voices prepared while the others are computed
    int capacity = 1;
    while (capacity < _maxPolyphony + _maxPolyphony / 4 + 64)
        capacity <<= 1;

    deleteVoices();
    _capacity = capacity;
    _indexMask = capacity - 1;
    _voices.resize(capacity);
    _voiceParameters.resize(capacity);
    for (int i = 0; i < capacity; ++i)
    {
        _voiceParameters[i] = new VoiceParam(_context);
        _voices[i] = new Voice(_voiceParameters[i], _listener);
        _voices[i]->reserve(_reservedChunkCount);
    }

    // Arrays used by the audio thread
    _closeCommands.resize(capacity);
    _voicePriorities.resize(capacity);
    _notePriorities.resize(capacity);
    _sortedVoices.resize(capacity);

    // No voices
    _preparationIndex = 0;
    _additionIndex = 0;
    _currentIndex.store(0);
    _firstRunningIndex = 0;
    _lastRunningIndex = 0;
    _uncomputedVoiceCount = 0;
}

void VoiceList::deleteVoices()
{
    for (size_t i = 0; i < _voices.size(); ++i)
        delete _voices[i];
    for (size_t i = 0; i < _voiceParameters.size(); ++i)
        delete _voiceParameters[i];
    _voices.clear();
    _voiceParameters.clear();
}

void VoiceList::setMaxPolyphony(int maxPolyphony)
{
    if (maxPolyphony <= 0)
        maxPolyphony = DEFAULT_POLYPHONY;
    else if (maxPolyphony < MIN_POLYPHONY)
        maxPolyphony = MIN_POLYPHONY;
    else if (maxPolyphony > MAX_POLYPHONY)
        maxPolyphony = MAX_POLYPHONY;
    if (maxPolyphony == _maxPolyphony)
        return;

    _maxPolyphony = maxPolyphony;
    if (_context != nullptr)
        createVoices();
}

void VoiceList::reserve(uint32_t chunkCount)
{
    if (chunkCount > _reservedChunkCount)
        _reservedChunkCount = chunkCount;
    for (int i = 0; i < _capacity; ++i)
        _voices[i]->reserve(chunkCount);
}

void VoiceList::setCpuBudget(int percent)
{
    if (percent < 1)
        percent = 1;
    else if (percent > 100)
        percent = 100;
    _cpuBudget.store(0.01f * static_cast<float>(percent), std::memory_order_relaxed);
}

void VoiceList::setSampleRate(uint32_t sampleRate)
{
    // The previous measures are not valid anymore
    _chunkDuration = static_cast<double>(COMPUTATION_CHUNK) / static_cast<double>(sampleRate);
    _voiceChunkCost = 0;
    _otherChunkCost = 0;
    _costMeasureCount = 0;
}

void VoiceList::addComputationTime(double voiceSeconds, uint32_t voiceChunkCount, double otherSeconds, uint32_t chunkCount)
{
    // Rest of the computation in the same thread (effects...)
    if (chunkCount > 0)
        _otherChunkCost += 0.1 * (otherSeconds / static_cast<double>(chunkCount) - _otherChunkCost);

    // Voices, not significant if they are too few
    if (voiceChunkCount < 32)
        return;
    double cost = voiceSeconds / static_cast<double>(voiceChunkCount);
    if (_costMeasureCount < 8)
    {
        // Average of the first measures
        _voiceChunkCost += (cost - _voiceChunkCost) / static_cast<double>(_costMeasureCount + 1);
        _costMeasureCount++;
    }
    else
    {
        // A slower computation is taken into account faster, the peaks (thread interrupted) being limited
        if (cost > 2.0 * _voiceChunkCost)
            cost = 2.0 * _voiceChunkCost;
        _voiceChunkCost += (cost > _voiceChunkCost ? 0.3 : 0.05) * (cost - _voiceChunkCost);
    }
}

int VoiceList::getProcessorLimit()
{
    // Not enough measures yet
    if (_costMeasureCount < 8 || _voiceChunkCost <= 0)
        return std::numeric_limits<int>::max();

    // Voices that can be computed by all threads during a chunk, with a few voices at least (the rest might be too long
    // by itself)
    double availableTime = _cpuBudget.load(std::memory_order_relaxed) * _chunkDuration - _otherChunkCost;
    double limit = _threadCount * availableTime / _voiceChunkCost;
    if (limit < MIN_POLYPHONY)
        return MIN_POLYPHONY;
    return limit > MAX_POLYPHONY ? std::numeric_limits<int>::max() : static_cast<int>(limit);
}

Voice * VoiceList::getVoiceToPrepare()
{
    // Current voice range
    int first = (_firstRunningIndex - _threadCount - 1) & _indexMask;
    int last = _additionIndex;
    int currentPreparationIndex = _preparationIndex & _indexMask;

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
    int index = _preparationIndex & _indexMask;
    _preparationIndex += 1;
    return _voices[index];
}

void VoiceList::addPreparedVoices()
{
    _additionIndex = (_preparationIndex & _indexMask);
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
        count1 = _capacity - first;
        voices2 = _voices.data();
        count2 = last;
    }
}

int VoiceList::getVoiceCount()
{
    return (_lastRunningIndex - _firstRunningIndex) & _indexMask;
}

void VoiceList::endComputation()
{
    int first = _firstRunningIndex;
    int last = _lastRunningIndex;
    int currentIndex = _currentIndex.exchange(last) & _indexMask;

    // Number of voices that have not been computed
    if (first <= last)
        _uncomputedVoiceCount = (currentIndex < last && currentIndex >= first) ? last - currentIndex : 0;
    else
        _uncomputedVoiceCount = (currentIndex < last || currentIndex >= first) ? (last - currentIndex) & _indexMask : 0;
}

void VoiceList::prepareComputation()
{
    int first = _firstRunningIndex;
    int last = _lastRunningIndex;
    int additionIndex = _additionIndex;

    // Gather the exclusive class close commands
    _closeCommandNumber = 0;
    Voice * voice;
    VoiceParam * voiceParam;
    int exclusiveClass;
    for (int index = last; index != additionIndex; index = (index + 1) & _indexMask)
    {
        voice = _voices[index];
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

    // Remove the finished voices, and quickly release the ones having the exclusive class of a new voice
    for (int index = first; index != last; index = (index + 1) & _indexMask)
    {
        voice = _voices[index];
        if (voice->isFinished())
        {
            voice->finish();
            _voices[index] = _voices[first];
            _voices[first] = voice;
            first = (first + 1) & _indexMask;
            continue;
        }

        voiceParam = voice->getParam();
        for (int i = 0; i < _closeCommandNumber; i++)
        {
            if (_closeCommands[i].channel == voiceParam->getChannel() &&
                _closeCommands[i].sf2Id == voiceParam->getSf2Id() &&
                _closeCommands[i].numPreset == voiceParam->getPresetId() &&
                _closeCommands[i].exclusiveClass == voiceParam->getInteger(champ_exclusiveClass))
            {
                voice->release(true);
                break;
            }
        }
    }

    // Include the added voices
    last = additionIndex;

    // Too many voices for the polyphony or, in real time, for the processor (the voices fading out being still
    // computed): notes fade out
    int activeCount = 0;
    int fadingCount = 0;
    for (int index = first; index != last; index = (index + 1) & _indexMask)
    {
        if (_voices[index]->isFadingOut())
            fadingCount++;
        else
            activeCount++;
    }
    int excess = activeCount - _maxPolyphony;
    if (_isRealTime.load(std::memory_order_relaxed))
    {
        int limit = getProcessorLimit();
        if (limit != std::numeric_limits<int>::max() && activeCount + fadingCount - limit > excess)
            excess = activeCount + fadingCount - limit;

        // Voices that the sound engines could not compute in time
        if (_uncomputedVoiceCount > excess)
            excess = _uncomputedVoiceCount;
    }
    if (excess > 0)
        fadeOutNotes(first, last, excess);
    _uncomputedVoiceCount = 0;

    // Update the indexes
    _lastRunningIndex = last;
    _firstRunningIndex = first;
    _currentIndex.store(first, std::memory_order_release);
}

void VoiceList::fadeOutNotes(int first, int last, int voiceCount)
{
    // Lowest and highest keys held on each channel (-1 for the editor, then 0 to 15)
    int lowestKeys[17], highestKeys[17];
    for (int i = 0; i < 17; i++)
    {
        lowestKeys[i] = 1000;
        highestKeys[i] = -1000;
    }
    Voice * voice;
    VoiceParam * voiceParam;
    int channel, key;
    for (int index = first; index != last; index = (index + 1) & _indexMask)
    {
        voice = _voices[index];
        if (voice->isFadingOut() || voice->isInRelease())
            continue;
        voiceParam = voice->getParam();
        channel = voiceParam->getChannel() + 1;
        if (channel < 0 || channel > 16)
            continue;
        key = voiceParam->getKey();
        if (key < lowestKeys[channel])
            lowestKeys[channel] = key;
        if (key > highestKeys[channel])
            highestKeys[channel] = key;
    }

    // Priority of the voices that are not fading out
    int priorityCount = 0;
    for (int index = first; index != last; index = (index + 1) & _indexMask)
    {
        voice = _voices[index];
        if (voice->isFadingOut())
            continue;
        VoicePriority &priority = _voicePriorities[priorityCount++];
        priority.noteId = voice->getNoteId();
        priority.amplitude = voice->getAmplitude();
        priority.index = index;
        if (voice->isYoung())
            priority.level = 2;
        else if (voice->isInRelease())
            priority.level = 0;
        else
        {
            voiceParam = voice->getParam();
            channel = voiceParam->getChannel() + 1;
            key = voiceParam->getKey();
            bool isExtremeKey = channel >= 0 && channel <= 16 && (key == lowestKeys[channel] || key == highestKeys[channel]);
            priority.level = isExtremeKey ? 2 : 1;
        }
    }

    // Priority of the notes, from their voices: the highest level and amplitude
    std::sort(_voicePriorities.begin(), _voicePriorities.begin() + priorityCount,
              [](const VoicePriority &a, const VoicePriority &b) { return a.noteId < b.noteId; });
    int noteCount = 0;
    for (int i = 0; i < priorityCount; )
    {
        NotePriority &note = _notePriorities[noteCount++];
        note.level = _voicePriorities[i].level;
        note.amplitude = _voicePriorities[i].amplitude;
        note.firstVoicePriority = i;
        int j = i + 1;
        for (; j < priorityCount && _voicePriorities[j].noteId == _voicePriorities[i].noteId; j++)
        {
            if (_voicePriorities[j].level > note.level)
                note.level = _voicePriorities[j].level;
            if (_voicePriorities[j].amplitude > note.amplitude)
                note.amplitude = _voicePriorities[j].amplitude;
        }
        note.voiceCount = j - i;
        i = j;
    }
    std::sort(_notePriorities.begin(), _notePriorities.begin() + noteCount,
              [](const NotePriority &a, const NotePriority &b) {
                  return a.level != b.level ? a.level < b.level : a.amplitude < b.amplitude;
              });

    // Fade out the notes with the lowest priority
    int stoppedNoteCount = 0;
    for (; stoppedNoteCount < noteCount && voiceCount > 0; stoppedNoteCount++)
    {
        NotePriority &note = _notePriorities[stoppedNoteCount];
        for (int i = note.firstVoicePriority; i < note.firstVoicePriority + note.voiceCount; i++)
            _voices[_voicePriorities[i].index]->fadeOut();
        voiceCount -= note.voiceCount;
    }

    // If the sound engines could not compute all voices in time, the notes with the highest priority are computed first,
    // then the ones fading out
    if (_uncomputedVoiceCount > 0)
    {
        int count = 0;
        for (int n = noteCount - 1; n >= stoppedNoteCount; n--)
        {
            NotePriority &note = _notePriorities[n];
            for (int i = note.firstVoicePriority; i < note.firstVoicePriority + note.voiceCount; i++)
                _sortedVoices[count++] = _voices[_voicePriorities[i].index];
        }
        for (int index = first; index != last; index = (index + 1) & _indexMask)
            if (_voices[index]->isFadingOut())
                _sortedVoices[count++] = _voices[index];
        for (int i = 0, index = first; i < count; i++, index = (index + 1) & _indexMask)
            _voices[index] = _sortedVoices[i];
    }
}

Voice * VoiceList::getNextVoiceToCompute()
{
    int first = _firstRunningIndex;
    int last = _lastRunningIndex;
    int voiceIndex = _currentIndex.fetch_add(1) & _indexMask;

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

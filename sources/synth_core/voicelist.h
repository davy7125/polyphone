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

#include <atomic>
#include <vector>
#include "attributetypes.h"
#include "synthconstants.h"
class Voice;
class VoiceParam;
class ISynthListener;
class VoiceContext;

// Voices prepared by the MIDI or GUI thread, then computed by the audio thread (or the sound engines of the editor).
// When there are more notes than the polyphony or than the processor can compute in real time, notes fade out
// (FADE_OUT_DURATION): first the released ones, then the quietest ones. The notes that just started and, on each
// channel, the highest and the lowest keys held are kept as long as possible. All voices of a note (stereo parts,
// layers) are kept or stopped together.
class VoiceList
{
public:
    VoiceList();
    ~VoiceList();
    void initialize(ISynthListener * listener, const VoiceContext * context);
    void setThreadCount(int threadCount) { _threadCount = threadCount; }

    // Maximum number of voices that are not fading out (DEFAULT_POLYPHONY by default). The voices are allocated for it:
    // if the number changes they are all stopped, no voice must be prepared or computed meanwhile.
    void setMaxPolyphony(int maxPolyphony);
    int getMaxPolyphony() { return _maxPolyphony; }

    // Allocate the voices for computing up to chunkCount chunks at once (not in the audio thread)
    void reserve(uint32_t chunkCount);

    // In real time (default), the quietest notes fade out if the voices take more than a share (from 1 to 100%, default
    // DEFAULT_CPU_BUDGET) of the duration of the audio computed. Offline, the computation time is not limited.
    void setRealTime(bool isRealTime) { _isRealTime.store(isRealTime, std::memory_order_relaxed); }
    void setCpuBudget(int percent);
    void setSampleRate(uint32_t sampleRate); // Not while computing

    // Time measured by the audio thread: computation of voiceChunkCount chunks of voices (number of voices times the
    // number of chunks, possibly by several threads) and of the rest (effects) for chunkCount chunks in the same thread
    void addComputationTime(double voiceSeconds, uint32_t voiceChunkCount, double otherSeconds, uint32_t chunkCount);

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

    // Priority of a voice, and of a note (a set of voices) for stopping the voices with the lowest priority
    struct VoicePriority
    {
        int noteId;
        int level; // 0: released, 1: held, 2: starting or highest / lowest key of its channel
        float amplitude;
        int index;
    };
    struct NotePriority
    {
        int level;
        float amplitude;
        int firstVoicePriority;
        int voiceCount;
    };

    void createVoices();
    void deleteVoices();
    int getProcessorLimit();
    void fadeOutNotes(int first, int last, int voiceCount);

    ISynthListener * _listener;
    const VoiceContext * _context;
    uint32_t _reservedChunkCount;

    /// Variables shared by different threads

    std::vector<Voice *> _voices;
    std::vector<VoiceParam *> _voiceParameters;
    int _capacity; // Power of 2
    int _indexMask;
    int _threadCount;

    // Indexes, published with a release store and read with an acquire load: the voices written before by a thread
    // are then complete for the others
    int _preparationIndex;                // When voices are being added (MIDI or GUI thread only)
    std::atomic<int> _additionIndex;      // When a set of voices is complete
    std::atomic<int> _currentIndex;       // Index of the last voice being computed
    std::atomic<int> _firstRunningIndex;  // Index of the first voice to compute
    std::atomic<int> _lastRunningIndex;   // Index of the last voice to compute

    // Limits
    volatile int _maxPolyphony;
    std::atomic<bool> _isRealTime;
    std::atomic<float> _cpuBudget; // From 0 to 1

    /// Variables used by the audio thread only

    int _uncomputedVoiceCount;
    std::vector<ExclusiveClassCloseCommand> _closeCommands;
    unsigned short _closeCommandNumber;
    std::vector<VoicePriority> _voicePriorities;
    std::vector<NotePriority> _notePriorities;
    std::vector<Voice *> _sortedVoices;

    // Estimation of the computation time (seconds) of a voice and of the rest during a chunk
    double _chunkDuration;
    double _voiceChunkCost, _otherChunkCost;
    int _costMeasureCount;
};

#endif // VOICELIST_H

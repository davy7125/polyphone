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

#ifndef SYNTHBUSES_H
#define SYNTHBUSES_H

#include <cstdint>
class Voice;

// Stereo buffers in which the voices are mixed according to their effect sends,
// before the chorus and the reverb are applied
class SynthBuses
{
public:
    // Channels that can have their own buffers (rendering at the frame)
    static const int CHANNEL_BUS_COUNT = 16;

    SynthBuses();
    ~SynthBuses();

    // Allocate the buffers for len frames rounded up to whole chunks, if they are smaller (not in the audio thread)
    // With channel buses, the buffers of the channels are allocated too
    // The buffers are aligned for FastMaths
    void reserve(uint32_t len, bool withChannelBuses = false);
    uint32_t capacity() { return _capacity; } // Whole chunks
    bool hasChannelBuses() { return _hasChannelBuses; }

    // Set the first len frames to 0 (and the ones of the channels having their own buffers)
    void clear(uint32_t len);

    // Compute len frames of a voice and mix them, the first voice of a side overwriting its buffers
    // The buffers must have been cleared: those whose coefficient is 0 are not written, nor the silent voices
    void addVoice(Voice * voice, uint32_t len, bool &leftInitialized, bool &rightInitialized);

    // Rendering at the frame: mix len frames (any number, up to the capacity) of a voice, starting with the frames of
    // its last chunk not mixed yet. The voice computes whole chunks: the frames not asked are mixed by the next call,
    // with the coefficients of their chunk. Return the number of chunks computed.
    uint32_t addVoiceFrames(Voice * voice, uint32_t len, bool &leftInitialized, bool &rightInitialized);

    // Rendering at the frame: the part of the voices without effect goes to the buffers of their channel instead of
    // dataL and dataR (the channel buses must have been reserved)
    void setChannelBus(int channel, bool hasOwnBus);
    bool hasChannelBus(int channel) { return channel >= 0 && channel < CHANNEL_BUS_COUNT && _isChannelBusUsed[channel]; }

    // Copy or add the first len frames into other buses
    void copyTo(SynthBuses &destination, uint32_t len);
    void addTo(SynthBuses &destination, uint32_t len);

    // Without effect, with the reverb, with the chorus, with both
    float * dataL, * dataR;
    float * dataRevL, * dataRevR;
    float * dataChoL, * dataChoR;
    float * dataChoRevL, * dataChoRevR;

    // Without effect, for the channels having their own buffers
    float * channelL[CHANNEL_BUS_COUNT], * channelR[CHANNEL_BUS_COUNT];

private:
    void deleteBuffers();
    void mixFrames(const float * voiceData, uint32_t position, uint32_t len, const float coefficients[8], int channel,
                   bool overwriteLeft, bool overwriteRight, bool withLeft, bool withRight);

    float * _voiceData;
    uint32_t _capacity;
    bool _hasChannelBuses;
    bool _isChannelBusUsed[CHANNEL_BUS_COUNT];
};

#endif // SYNTHBUSES_H

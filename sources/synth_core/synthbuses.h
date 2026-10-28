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
    SynthBuses();
    ~SynthBuses();

    // Allocate the buffers for len frames rounded up to whole chunks, if they are smaller (not in the audio thread)
    // The buffers are aligned for FastMaths
    void reserve(uint32_t len);
    uint32_t capacity() { return _capacity; } // Whole chunks

    // Set the first len frames to 0
    void clear(uint32_t len);

    // Compute len frames of a voice and mix them, the first voice of a side overwriting its buffers
    // The buffers must have been cleared: those whose coefficient is 0 are not written, nor the silent voices
    void addVoice(Voice * voice, uint32_t len, bool &leftInitialized, bool &rightInitialized);

    // Copy or add the first len frames into other buses
    void copyTo(SynthBuses &destination, uint32_t len);
    void addTo(SynthBuses &destination, uint32_t len);

    // Without effect, with the reverb, with the chorus, with both
    float * dataL, * dataR;
    float * dataRevL, * dataRevR;
    float * dataChoL, * dataChoR;
    float * dataChoRevL, * dataChoRevR;

private:
    void deleteBuffers();

    float * _voiceData;
    uint32_t _capacity;
};

#endif // SYNTHBUSES_H

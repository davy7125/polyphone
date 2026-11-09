/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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


#ifndef CLAP_INSTANCE_H
#define CLAP_INSTANCE_H

#include <cstdint>
#include <string>
#include <vector>

// Minimal host of the CLAP format for the tests: the first plugin of a .clap file, stereo output, MIDI messages
class ClapInstance
{
public:
    struct MidiEvent
    {
        uint32_t position;
        uint8_t data[3];
    };

    ClapInstance();
    ~ClapInstance();

    bool load(const std::string &path, std::string &error);
    bool setState(const std::vector<uint8_t> &state);
    bool getState(std::vector<uint8_t> &state);

    // Activation for a sample rate and a maximum block size, possibly offline (rendering to a file)
    bool activate(double sampleRate, uint32_t maxFrameCount, bool isOffline);
    void deactivate();

    // Compute frames (not more than the maximum block size), the MIDI messages being at their position
    void process(float * left, float * right, uint32_t frameCount, const std::vector<MidiEvent> &events);

    // Data of the instance, used by the callbacks of the host
    struct Private;

private:
    Private * _private;
};

#endif // CLAP_INSTANCE_H

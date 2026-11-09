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


#ifndef DLSDEFINITIONS_H
#define DLSDEFINITIONS_H

#include <cstdint>

// Constants of the DLS format (levels 1 and 2), shared by the import (editor and plugin) and the export

namespace Dls
{
    // Sources and controls of a connection block
    enum Source
    {
        SRC_NONE = 0x0000,
        SRC_LFO = 0x0001,
        SRC_KEYONVELOCITY = 0x0002,
        SRC_KEYNUMBER = 0x0003,
        SRC_EG1 = 0x0004,
        SRC_EG2 = 0x0005,
        SRC_PITCHWHEEL = 0x0006,
        SRC_POLYPRESSURE = 0x0007,
        SRC_CHANNELPRESSURE = 0x0008,
        SRC_VIBRATO = 0x0009,
        SRC_CC_FIRST = 0x0080, // 0x0080 + controller number
        SRC_CC1 = 0x0081,
        SRC_CC7 = 0x0087,
        SRC_CC10 = 0x008A,
        SRC_CC11 = 0x008B,
        SRC_CC91 = 0x00DB,
        SRC_CC93 = 0x00DD,
        SRC_RPN0 = 0x0100, // Pitch bend range
        SRC_RPN1 = 0x0101, // Fine tune
        SRC_RPN2 = 0x0102  // Coarse tune
    };

    // Destinations of a connection block
    enum Destination
    {
        DST_NONE = 0x0000,
        DST_GAIN = 0x0001, // "Attenuation" in DLS level 1, with the same unit
        DST_PITCH = 0x0003,
        DST_PAN = 0x0004,
        DST_KEYNUMBER = 0x0005,
        DST_CHORUS = 0x0080,
        DST_REVERB = 0x0081,
        DST_LFO_FREQUENCY = 0x0104,
        DST_LFO_STARTDELAY = 0x0105,
        DST_VIB_FREQUENCY = 0x0114,
        DST_VIB_STARTDELAY = 0x0115,
        DST_EG1_ATTACKTIME = 0x0206,
        DST_EG1_DECAYTIME = 0x0207,
        DST_EG1_RELEASETIME = 0x0209,
        DST_EG1_SUSTAINLEVEL = 0x020A,
        DST_EG1_DELAYTIME = 0x020B,
        DST_EG1_HOLDTIME = 0x020C,
        DST_EG1_SHUTDOWNTIME = 0x020D,
        DST_EG2_ATTACKTIME = 0x030A,
        DST_EG2_DECAYTIME = 0x030B,
        DST_EG2_RELEASETIME = 0x030D,
        DST_EG2_SUSTAINLEVEL = 0x030E,
        DST_EG2_DELAYTIME = 0x030F,
        DST_EG2_HOLDTIME = 0x0310,
        DST_FILTER_CUTOFF = 0x0500,
        DST_FILTER_Q = 0x0501
    };

    // Curves of the transforms (output, source and control)
    enum Transform
    {
        TRN_NONE = 0,
        TRN_CONCAVE = 1,
        TRN_CONVEX = 2,
        TRN_SWITCH = 3
    };

    // Values with a special meaning
    const int32_t ABSOLUTE_ZERO_TIME = static_cast<int32_t>(0x80000000); // Time of 0 second that cannot be modulated
    const int32_t FILTER_DISABLED = 0x7FFFFFFF; // Initial cutoff frequency when no filter is applied

    // Flags
    const uint32_t F_INSTRUMENT_DRUMS = 0x80000000;
    const uint16_t F_RGN_OPTION_SELFNONEXCLUSIVE = 0x0001;
    const uint16_t F_WAVELINK_PHASE_MASTER = 0x0001;
    const uint16_t F_WAVELINK_MULTICHANNEL = 0x0002;
    const uint32_t WAVELINK_CHANNEL_LEFT = 0x0001;
    const uint32_t WAVELINK_CHANNEL_RIGHT = 0x0002;
    const uint32_t F_WSMP_NO_TRUNCATION = 0x0001;
    const uint32_t F_WSMP_NO_COMPRESSION = 0x0002;
    const uint32_t WLOOP_TYPE_FORWARD = 0x0000;
    const uint32_t WLOOP_TYPE_RELEASE = 0x0001;

    // Wave formats (not "WAVE_FORMAT_PCM", already a macro in the Windows headers)
    const uint16_t FORMAT_TAG_PCM = 0x0001;

    // Gains are in 1/655360 dB, times in 1/65536 timecents, pitches in 1/65536 cents and percentages in 1/65536 of 0.1%
    const double UNIT_SCALE = 65536.0;

    // Attenuation in dB of the volume envelope (EG1) when its level is 0%
    const double EG1_DYNAMIC_RANGE = 96.0;

    // A connection block (art1 or art2 chunk)
    struct Connection
    {
        uint16_t source;
        uint16_t control;
        uint16_t destination;
        uint16_t transform;
        int32_t scale;

        // Accessors to the bitfields of the transform (only the output transform is used in DLS level 1)
        uint16_t outputTransform() const { return transform & 0x000F; }
        uint16_t controlTransform() const { return (transform >> 4) & 0x000F; }
        bool isControlBipolar() const { return (transform & 0x0100) != 0; }
        bool isControlInverted() const { return (transform & 0x0200) != 0; }
        uint16_t sourceTransform() const { return (transform >> 10) & 0x000F; }
        bool isSourceBipolar() const { return (transform & 0x4000) != 0; }
        bool isSourceInverted() const { return (transform & 0x8000) != 0; }

        static uint16_t buildTransform(uint16_t sourceTransform, bool sourceBipolar, bool sourceInverted,
                                      uint16_t controlTransform, bool controlBipolar, bool controlInverted,
                                      uint16_t outputTransform)
        {
            return static_cast<uint16_t>((sourceInverted ? 0x8000 : 0) | (sourceBipolar ? 0x4000 : 0) |
                                        ((sourceTransform & 0x000F) << 10) |
                                        (controlInverted ? 0x0200 : 0) | (controlBipolar ? 0x0100 : 0) |
                                        ((controlTransform & 0x000F) << 4) |
                                        (outputTransform & 0x000F));
        }
    };
}

#endif // DLSDEFINITIONS_H

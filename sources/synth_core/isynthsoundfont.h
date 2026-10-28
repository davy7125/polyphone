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

#ifndef ISYNTHSOUNDFONT_H
#define ISYNTHSOUNDFONT_H

#include "attributetypes.h"

// What the synth reads in the soundfonts. These interfaces are implemented by the model of the editor
// (Soundfonts, Soundfont, InstPrst, Division, Smpl), so that the synth does not depend on it.

class ISynthSample
{
public:
    virtual ~ISynthSample() {}

    /// Read the sample data if needed, before getData
    virtual void loadData() = 0;

    /// Sample data: 16 bits, plus the 8 least significant bits of a 24-bit sample (otherwise null)
    /// If getCopy is true, the arrays are copies that the caller deletes
    virtual void getData(uint32_t &length, int16_t * &data16, uint8_t * &data24, bool getCopy) = 0;

    virtual uint32_t getSampleRate() = 0;
    virtual uint32_t getLength() = 0;
    virtual uint32_t getLoopStart() = 0;
    virtual uint32_t getLoopEnd() = 0;
    virtual uint32_t getRootKey() = 0;
    virtual int32_t getPitchCorrection() = 0;
    virtual SFSampleLink getSampleType() = 0;
    virtual int getLinkedSampleIndex() = 0;
};

class ISynthZone
{
public:
    virtual ~ISynthZone() {}

    /// False if the zone must not be played (deleted or muted)
    virtual bool isPlayable() = 0;

    /// Attributes indexed by AttributeType, until champ_endOper: whether they are set, and their values
    virtual bool * getAttributeSet() = 0;
    virtual AttributeValue * getAttributeValues() = 0;

    /// Modulators by position, null for a modulator that must be ignored (deleted)
    virtual int getModulatorCount() = 0;
    virtual ModulatorData * getModulatorData(int position) = 0;
};

// An instrument or a preset
class ISynthInstPrst
{
public:
    virtual ~ISynthInstPrst() {}

    /// Index of the element in its soundfont
    virtual int getIndex() = 0;

    /// Preset and bank numbers (for presets only)
    virtual int getPresetNumber() = 0;
    virtual int getBankNumber() = 0;

    /// True if the element is played with any other element of the same kind (editor)
    virtual bool isAlwaysPlayed() = 0;

    /// Global zone and zones by position
    virtual ISynthZone * getGlobalZone() = 0;
    virtual int getZoneCount() = 0;
    virtual ISynthZone * getZone(int position) = 0;
};

class ISynthSoundfont
{
public:
    virtual ~ISynthSoundfont() {}

    /// Elements by index, null if the index is not used
    virtual ISynthSample * getSynthSample(int index) = 0;
    virtual ISynthInstPrst * getSynthInstrument(int index) = 0;
    virtual ISynthInstPrst * getSynthPreset(int index) = 0;

    /// Instruments and presets by position, for browsing all of them
    virtual int getInstrumentCount() = 0;
    virtual ISynthInstPrst * getInstrumentAt(int position) = 0;
    virtual int getPresetCount() = 0;
    virtual ISynthInstPrst * getPresetAt(int position) = 0;
};

class ISynthSoundfonts
{
public:
    virtual ~ISynthSoundfonts() {}

    /// Soundfont by index, null if the index is not used
    virtual ISynthSoundfont * getSynthSoundfont(int index) = 0;
};

#endif // ISYNTHSOUNDFONT_H

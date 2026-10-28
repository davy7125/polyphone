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

#ifndef ATTRIBUTETYPES_H
#define ATTRIBUTETYPES_H

#include <cstdint>
#include <cstring>
#include "modulatordata.h"

// Types of the soundfont format, shared by the synth and the editor

typedef enum
{
    champ_unknown = -1,
    champ_startAddrsOffset = 0,         // Start of generator fields
    champ_endAddrsOffset = 1,
    champ_startloopAddrsOffset = 2,
    champ_endloopAddrsOffset = 3,
    champ_startAddrsCoarseOffset = 4,
    champ_modLfoToPitch = 5,
    champ_vibLfoToPitch = 6,
    champ_modEnvToPitch = 7,
    champ_initialFilterFc = 8,
    champ_initialFilterQ = 9,
    champ_modLfoToFilterFc = 10,
    champ_modEnvToFilterFc = 11,
    champ_endAddrsCoarseOffset = 12,
    champ_modLfoToVolume = 13,
    champ_unused1 = 14,
    champ_chorusEffectsSend = 15,
    champ_reverbEffectsSend = 16,
    champ_pan = 17,
    champ_unused2 = 18,
    champ_unused3 = 19,
    champ_unused4 = 20,
    champ_delayModLFO = 21,
    champ_freqModLFO = 22,
    champ_delayVibLFO = 23,
    champ_freqVibLFO = 24,
    champ_delayModEnv = 25,
    champ_attackModEnv = 26,
    champ_holdModEnv = 27,
    champ_decayModEnv = 28,
    champ_sustainModEnv = 29,
    champ_releaseModEnv = 30,
    champ_keynumToModEnvHold = 31,
    champ_keynumToModEnvDecay = 32,
    champ_delayVolEnv = 33,
    champ_attackVolEnv = 34,
    champ_holdVolEnv = 35,
    champ_decayVolEnv = 36,
    champ_sustainVolEnv = 37,
    champ_releaseVolEnv = 38,
    champ_keynumToVolEnvHold = 39,
    champ_keynumToVolEnvDecay = 40,
    champ_instrument = 41,
    champ_reserved1 = 42,
    champ_keyRange = 43,
    champ_velRange = 44,
    champ_startloopAddrsCoarseOffset = 45,
    champ_keynum = 46,
    champ_velocity = 47,
    champ_initialAttenuation = 48,
    champ_reserved2 = 49,
    champ_endloopAddrsCoarseOffset = 50,
    champ_coarseTune = 51,
    champ_fineTune = 52,
    champ_sampleID = 53,
    champ_sampleModes = 54,
    champ_reserved3 = 55,
    champ_scaleTuning = 56,
    champ_exclusiveClass = 57,
    champ_overridingRootKey = 58,
    champ_unused5 = 59,
    champ_endOper = 60,

    champ_IFIL = 128,                   // Start of SF2 fields
    champ_IVER = 129,

    champ_bpsFile = 130,                // Start of sample fields
    champ_dwStart16 = 131,
    champ_dwStart24 = 132,
    champ_dwLength = 133,
    champ_dwStartLoop = 134,
    champ_dwEndLoop = 135,
    champ_dwSampleRate = 136,
    champ_byOriginalPitch = 137,
    champ_chPitchCorrection = 138,
    champ_wSampleLink = 139,
    champ_sfSampleType = 140,

    champ_wPreset = 141,                // Start of preset fields
    champ_wBank = 142,
    champ_dwLibrary = 143,
    champ_dwGenre = 144,
    champ_dwMorphology = 145,

    champ_sfModSrcOper = 146,           // Start of modulator fields
    champ_sfModDestOper = 147,
    champ_modAmount = 148,
    champ_sfModAmtSrcOper = 149,
    champ_sfModTransOper = 150,

    champ_indexMod = 152,               // Misc
    champ_son = 156,
    champ_wBpsInit = 157,
    champ_wBpsSave = 158,
    champ_wChannel = 159,   // current channel
    champ_wChannels = 160,  // nb canaux total

    champ_filenameInitial = 163,   // QString (sf2)
    champ_filenameForData = 164,
    champ_ISNG = 165,
    champ_IROM = 166,
    champ_ICRD = 167,
    champ_IENG = 168,
    champ_IPRD = 169,
    champ_ICOP = 170,
    champ_ICMT = 171,
    champ_ISFT = 172,
    champ_name = 173,                   // (sf2, smpl, inst et prst)
    champ_nameSort = 174,

    champ_sampleData = 200,             // QVector<float>
    champ_sampleRawData = 201,

    champ_pitchDefined = 300, // if pitch defined in a sample (wav)
    champ_mute = 301, // Divisions can be muted for testing purposes
    champ_alwaysPlayed = 302, // Instruments and presets can be played along for testing purposes
    champ_rawDataAvailable = 303 // Raw data (for example compressed data) is written in the source file
} AttributeType;

typedef enum
{
    linkInvalid = 0,
    monoSample = 1,
    rightSample = 2,
    leftSample = 4,
    linkedSample = 8,
    RomMonoSample = 0x8001,
    RomRightSample = 0x8002,
    RomLeftSample = 0x8004,
    RomLinkedSample = 0x8008
} SFSampleLink;

typedef struct
{
    uint16_t wMajor;
    uint16_t wMinor;
} SfVersionTag;

typedef struct
{
    uint8_t byLo;
    uint8_t byHi;
} RangesType;

union AttributeValue
{
    // Unsigned values
    uint8_t bValue;
    uint16_t wValue; // SFModulator data is also represented here
    uint32_t dwValue;

    // Signed values
    int8_t cValue;
    int16_t shValue;

    // Complex data
    RangesType rValue;
    SFModulator sfModValue;
    SfVersionTag sfVerValue;
    SFSampleLink sfLinkValue;
    SFTransform sfTransValue;

    AttributeValue() { memset(static_cast<void *>(this), 0, sizeof(AttributeValue)); }
};

#endif // ATTRIBUTETYPES_H

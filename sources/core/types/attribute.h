/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include "modulatordata.h"

typedef enum
{
    champ_unknown = -1,
    champ_startAddrsOffset = 0,         // Début champs des générateurs
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
    champ_IFIL = 128,                   // Début champs SF2, valeur
    champ_IVER = 129,
    champ_bpsFile = 130,                // Début champs samples
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
    champ_wPreset = 141,                // Début champs preset
    champ_wBank = 142,
    champ_dwLibrary = 143,
    champ_dwGenre = 144,
    champ_dwMorphology = 145,
    champ_sfModSrcOper = 146,           // Début champs mod
    champ_sfModDestOper = 147,
    champ_modAmount = 148,
    champ_sfModAmtSrcOper = 149,
    champ_sfModTransOper = 150,

    champ_indexMod = 152,              // Misc
    champ_son = 156,
    champ_wBpsInit = 157,
    champ_wBpsSave = 158,
    champ_wChannel = 159,   // canal utilisé
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

    champ_sampleData16 = 200,           // QByteArray
    champ_sampleData24 = 201, // Only the extra 8 bits
    champ_sampleDataFull24 = 202,
    champ_sampleData32 = 203,

    champ_pitchDefined = 300, // if pitch defined in a sample (wav)
    champ_mute = 301 // Divisions can be muted for testing purposes
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
    quint16 wMajor;
    quint16 wMinor;
} SfVersionTag;

typedef struct
{
    quint8 byLo;
    quint8 byHi;
} RangesType;

union AttributeValue
{
    // Unsigned values
    quint8 bValue;
    quint16 wValue; // SFModulator data is also represented here
    quint32 dwValue;

    // Signed values
    qint8 cValue;
    qint16 shValue;

    // Complex data
    RangesType rValue;
    SFModulator sfModValue;
    SfVersionTag sfVerValue;
    SFSampleLink sfLinkValue;
    SFTransform sfTransValue;

    AttributeValue() { memset(this, 0, sizeof(AttributeValue)); }
};

class Attribute: QObject
{
    Q_OBJECT

public:
    // Creation of a parameter
    // Champ: type of the parameter
    // isPrst: true if this parameter is for a preset
    // value: real value
    Attribute(AttributeType champ, bool isPrst);
    Attribute(AttributeType champ, bool isPrst, double value);
    Attribute(AttributeType champ, bool isPrst, AttributeValue storedValue);

    // Get / Set the real value
    // The stored value will be adapted accordingly
    void setRealValue(double realValue);
    double getRealValue() { return _realValue; }

    // Get / Set the stored value (from the binary file)
    // The real value will be adapted accordingly
    void setStoredValue(AttributeValue storedValue);
    void setStoredValue(qint16 storedValue);
    void setStoredValue(quint16 storedValue);
    void setStoredValue(quint8 lower, quint8 upper);
    AttributeValue getStoredValue() { return _storedValue; }

    // Default values
    static double getDefaultRealValue(AttributeType champ, bool isPrst);
    static AttributeValue getDefaultStoredValue(AttributeType champ, bool isPrst);

    // Conversions
    static double toRealValue(AttributeType champ, bool isPrst, AttributeValue storedValue);
    static AttributeValue fromRealValue(AttributeType champ, bool isPrst, double realValue);
    static QString toString(AttributeType champ, bool isPrst, AttributeValue storedValue);
    static AttributeValue fromString(AttributeType champ, bool isPrst, QString strValue, bool &ok);

    // Limit
    static AttributeValue limit(AttributeType champ, AttributeValue value, bool isPrst);

    // Textual description
    static QString getDescription(AttributeType champ, bool isPrst);

private:
    static int limit(int value, int min, int max);
    static double limit(double value, double min, double max);

    AttributeType _champ;
    bool _isPrst;
    double _realValue;
    AttributeValue _storedValue;
};

#endif // ATTRIBUTE_H

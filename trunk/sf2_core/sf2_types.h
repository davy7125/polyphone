/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef SF2_TYPES_H
#define SF2_TYPES_H

#include <QString>
#include <stdio.h>

#define DB_SF2_TO_SFZ 0.397


// Déclaration anticipée
typedef struct infoSound InfoSound;

typedef enum
{
    elementUnknown = -1,
    elementSf2 = 0,
    elementSmpl = 1,
    elementInst = 2,
    elementPrst = 3,
    elementInstSmpl = 4,
    elementPrstInst = 5,
    elementRootSmpl = 6,
    elementRootInst = 7,
    elementRootPrst = 8,
    elementInstMod = 9,
    elementPrstMod = 10,
    elementInstSmplMod = 11,
    elementPrstInstMod = 12,
    elementInstGen = 13,
    elementPrstGen = 14,
    elementInstSmplGen = 15,
    elementPrstInstGen = 16
}ElementType;

class EltID
{
public:
    EltID(ElementType elementType, int indexSf2, int indexElt, int indexElt2, int indexMod)
    {
        this->typeElement = elementType;
        this->indexSf2    = indexSf2;
        this->indexElt    = indexElt;
        this->indexElt2   = indexElt2;
        this->indexMod    = indexMod;
    }
    EltID()
    {
        this->typeElement = elementUnknown;
        this->indexSf2 = 0;
        this->indexElt = 0;
        this->indexElt2 = 0;
        this->indexMod = 0;
    }

    bool operator==(const EltID &other)
    {
        bool ok = true;
        if (this->typeElement != other.typeElement || this->indexSf2 != other.indexSf2)
            ok = false;
        else
        {
            switch (typeElement)
            {
            case elementUnknown:
                ok = false;
                break;
            case elementInstMod: case elementPrstMod:
            case elementInstGen: case elementPrstGen: // Pas besoin de vérifier sur indexElt2
                ok = (this->indexElt == other.indexElt) &&
                        (this->indexMod == other.indexMod);
                break;
            case elementInstSmplMod: case elementPrstInstMod:
            case elementInstSmplGen: case elementPrstInstGen:
                ok = (this->indexMod == other.indexMod);
            case elementInstSmpl: case elementPrstInst:
                ok = ok && (this->indexElt2 == other.indexElt2);
            case elementSmpl: case elementInst: case elementPrst:
                ok = ok && (this->indexElt == other.indexElt);
            default:
                break;
            }
        }
        return ok;
    }

    QString toString()
    {
        QString type;
        switch (typeElement)
        {
        case elementSf2: type = "SF2"; break;
        case elementSmpl: type = "SMPL"; break;
        case elementInst: type = "INST"; break;
        case elementPrst: type = "PRST"; break;
        case elementInstSmpl: type = "InstSmpl"; break;
        case elementPrstInst: type = "PrstInst"; break;
        case elementInstMod: type = "InstMod"; break;
        case elementPrstMod: type = "PrstMod"; break;
        case elementInstSmplMod: type = "InstSmplMod"; break;
        case elementPrstInstMod: type = "PrstInstMod"; break;
        case elementInstGen: type = "InstGen"; break;
        case elementPrstGen: type = "PrstGen"; break;
        case elementInstSmplGen: type = "InstSmplGen"; break;
        case elementPrstInstGen: type = "PrstInstGen"; break;
        case elementUnknown: type = "Unknown"; break;
        default: type = "erreur"; break;
        }
        return "ID\ntype " + type + "\nsf2  " + QString::number(indexSf2) +
                "\nElt  " + QString::number(indexElt) + "\nElt2 " + QString::number(indexElt2) +
                "\nMod  " + QString::number(indexMod);
    }

    ElementType typeElement;
    int indexSf2;
    int indexElt;
    int indexElt2;
    int indexMod;
};

typedef struct sfVersionTag
{
    quint16 wMajor;
    quint16 wMinor;
}SfVersionTag;

typedef enum
{
    SFGenerator_value = 0,
    sfModSrcOper = 1
} SFSource;

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
    champ_hidden = 151,                 // Divers (hidden means that the element is deleted but undo is possible)
    champ_indexMod = 152,
    champ_sfGenOper = 153,
    champ_sfGenAmount = 154,
    champ_ram = 155,
    champ_son = 156,
    champ_wBpsInit = 157,
    champ_wBpsSave = 158,
    champ_wChannel = 159,   // canal utilisé
    champ_wChannels = 160,  // nb canaux total

    champ_filename = 164,               // QString (sf2)
    champ_ISNG = 165,
    champ_IROM = 166,
    champ_ICRD = 167,
    champ_IENG = 168,
    champ_IPRD = 169,
    champ_ICOP = 170,
    champ_ICMT = 171,
    champ_ISFT = 172,
    champ_name = 173,                   // (sf2, smpl, inst et prst)

    champ_sampleData16 = 200,           // QByteArray
    champ_sampleData24 = 201,
    champ_sampleDataFull24 = 202,
    champ_sampleData32
} Champ;

typedef struct sfmodulator
{
    quint16
        Type  : 6, // 6 bits pour le type
        P     : 1, // Polarité
        D     : 1, // Direction
        CC    : 1,
        Index : 7;
} SFModulator;

typedef enum
{
    linear = 0,
    absolute_value = 2
} SFTransform;

typedef struct
{
    quint8 byLo;
    quint8 byHi;
} rangesType;

typedef union
{
    rangesType ranges;
    short shAmount;
    quint16 wAmount;
} genAmountType;

typedef union
{
    // Valeurs non signées
    quint8 bValue;
    quint16 wValue;
    quint32 dwValue;

    // Valeurs signées
    char cValue;
    short shValue;
    long lValue;
    double dValue;

    // Tableaux
    rangesType rValue;
    SFModulator sfModValue;
    SfVersionTag sfVerValue;
    genAmountType genValue;
    SFSampleLink sfLinkValue;
    Champ sfGenValue;
    SFTransform sfTransValue;

    // Pointeur
    char *ptCharValue;
}Valeur;

// Définition des méthodes

// LECTURE
quint32 freadSize(QDataStream *stream);
quint32 readDWORD(const char *chaine, int pos);
quint16 readWORD(const char *chaine, int pos);
quint8 readQUINT8(const char *chaine, int pos);
short readSHORT(const char *chaine, int pos);
SFSampleLink readSFSL(const char *chaine, int pos);
Champ readSFGenerator(const char *chaine, int pos);
SFModulator readSFModulator(const char *chaine, int pos);
SFTransform readSFTransform(const char *chaine, int pos);
genAmountType readGenAmountType(const char *chaine, int pos);
SfVersionTag readSFVersionTag(const char *chaine, int pos);
void readbloc(char *bloc, const char * bloc_data, int pos);
char * readdata(char *data, const char *bloc_data, int pos, int length);

// CREATION D'ELEMENTS
SfVersionTag create_Version(quint16 wMajor, quint16 wMinor);

// DIVERS
QString decrementerQstr(QString chaine);


#endif // SF2_TYPES_H

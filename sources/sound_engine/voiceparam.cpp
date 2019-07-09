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

#include "voiceparam.h"
#include "qmath.h"

// Chargement des paramètres
VoiceParam::VoiceParam(SoundfontManager *sf2, EltID id, VoiceParam *voiceParamTmp) :
    _sm(sf2)
{
    // Default values
    if (voiceParamTmp && id.typeElement == elementSmpl)
        this->init(elementPrst);
    else
        this->init(id.typeElement, id.indexElt);

    // Initialize the parameters according to id
    if (id.typeElement == elementSmpl)
        this->readSample(id);
    else
        this->read(id);

    // Possibly merge with the parameters coming from upper levels (instrument / preset)
    if (voiceParamTmp)
        this->add(voiceParamTmp);

    // At the end, limit the params
    if (id.typeElement == elementSmpl)
        this->limit();
}

void VoiceParam::init(ElementType type, int numPreset)
{
    // TON
    this->fineTune              = 0;
    this->coarseTune            = 0;
    this->fixedKey              = -1;
    this->rootkey               = -1;
    // FILTRE
    this->filterQ               = 0;
    // BOUCLES ET OFFSETS
    this->loopStartOffset       = 0;
    this->loopEndOffset         = 0;
    this->sampleStartOffset     = 0;
    this->sampleEndOffset       = 0;
    this->loopStartCoarseOffset = 0;
    this->loopEndCoarseOffset   = 0;
    this->sampleStartCoarseOffset = 0;
    this->sampleEndCoarseOffset = 0;
    this->length = 0;
    this->startLoop = 0;
    this->endLoop = 0;
    // ATTENUATION, BALANCE
    this->attenuation           = 0;
    this->pan                   = 0;
    this->fixedVelocity         = -1;
    // ENVELOPPE VOLUME
    this->volKeynumToHold       = 0;
    this->volKeynumToDecay      = 0;
    this->volSustainLevel       = 0;
    // ENVELOPPE MODULATION
    this->modKeynumToHold       = 0;
    this->modKeynumToDecay      = 0;
    this->modEnvToPitch         = 0;
    this->modEnvToFilterFc      = 0;
    this->modSustainLevel       = 0;
    // LFOs
    this->modLfoToPitch         = 0;
    this->modLfoToFilterFreq    = 0;
    this->vibLfoToPitch         = 0;
    this->modLfoToVolume        = 0;
    // EFFETS
    this->reverb                = 0;
    this->chorus                = 0;
    // DIVERS
    this->exclusiveClass        = 0;

    // Initialisation spécifiques
    if (type == elementSmpl)
    {
        // Initialisation sample
        this->filterFreq            = 19914;
        this->volDelayTime          = 0.001;
        this->volAttackTime         = 0.001;
        this->volHoldTime           = 0.001;
        this->volDecayTime          = 0.001;
        this->volReleaseTime        = 0.1;
        this->modDelayTime          = 0.001;
        this->modAttackTime         = 0.001;
        this->modHoldTime           = 0.001;
        this->modDecayTime          = 0.001;
        this->modReleaseTime        = 0.001;
        this->modLfoDelay           = 0.001;
        this->modLfoFreq            = 8.176;
        this->vibLfoDelay           = 0.001;
        this->vibLfoFreq            = 8.176;
        this->scaleTune             = 100;
        this->loopMode              = -1;
        this->numPreset             = -1;
    }
    else if (type == elementInstSmpl)
    {
        this->filterFreq            = 19914;
        this->volDelayTime          = 0.001;
        this->volAttackTime         = 0.001;
        this->volHoldTime           = 0.001;
        this->volDecayTime          = 0.001;
        this->volReleaseTime        = 0.001;
        this->modDelayTime          = 0.001;
        this->modAttackTime         = 0.001;
        this->modHoldTime           = 0.001;
        this->modDecayTime          = 0.001;
        this->modReleaseTime        = 0.001;
        this->modLfoDelay           = 0.001;
        this->modLfoFreq            = 8.176;
        this->vibLfoDelay           = 0.001;
        this->vibLfoFreq            = 8.176;
        this->scaleTune             = 100;
        this->loopMode              = 0;
        this->numPreset             = -1;
    }
    else
    {
        this->filterFreq            = 1;
        this->volDelayTime          = 1;
        this->volAttackTime         = 1;
        this->volHoldTime           = 1;
        this->volDecayTime          = 1;
        this->volReleaseTime        = 1;
        this->modDelayTime          = 1;
        this->modAttackTime         = 1;
        this->modHoldTime           = 1;
        this->modDecayTime          = 1;
        this->modReleaseTime        = 1;
        this->modLfoDelay           = 1;
        this->modLfoFreq            = 1;
        this->vibLfoDelay           = 1;
        this->vibLfoFreq            = 1;
        this->scaleTune             = 0;
        this->loopMode              = -1;
        this->numPreset             = numPreset;
    }
}

void VoiceParam::prepareForSmpl(int key, SFSampleLink link)
{
    // Calling a second time the same sample mute the first one
    this->exclusiveClass = key;

    // Default release
    this->volReleaseTime = 0.2;

    // Pan
    switch (link)
    {
    case leftSample: case RomLeftSample:
        pan = -50;
        break;
    case rightSample: case RomRightSample:
        pan = 50;
        break;
    default:
        pan = 0;
        break;
    }
}

void VoiceParam::readSample(EltID id)
{
    // Lecture d'un sample
    this->fineTune = _sm->get(id, champ_chPitchCorrection).cValue;
    this->rootkey = _sm->get(id, champ_byOriginalPitch).bValue;
    this->length = _sm->get(id, champ_dwLength).dwValue;
    this->startLoop = _sm->get(id, champ_dwStartLoop).dwValue;
    this->endLoop = _sm->get(id, champ_dwEndLoop).dwValue;

    // Type de boucle
    this->loopMode = 0;
}

void VoiceParam::limit()
{
    this->fineTune           = limitI(this->fineTune,             -99,    99      );
    this->coarseTune         = limitI(this->coarseTune,           -120,   120     );
    this->pan                = limitD(this->pan,                  -50.,   50.     );
    this->attenuation        = limitD(this->attenuation,          0.,     144.    );
    this->scaleTune          = limitI(this->scaleTune,            0,      1200    );
    this->filterFreq         = limitD(this->filterFreq,           18.,    19914.  );
    this->filterQ            = limitD(this->filterQ,              0.,     96.     );
    this->volDelayTime       = limitD(this->volDelayTime,         0.001,  18.     );
    this->volAttackTime      = limitD(this->volAttackTime,        0.001,  101.6   );
    this->volHoldTime        = limitD(this->volHoldTime,          0.001,  18.     );
    this->volDecayTime       = limitD(this->volDecayTime,         0.001,  101.6   );
    this->volSustainLevel    = limitD(this->volSustainLevel,      0.,     144.    );
    this->volReleaseTime     = limitD(this->volReleaseTime,       0.001,  101.6   );
    this->volKeynumToHold    = limitI(this->volKeynumToHold,      -1200,  1200    );
    this->volKeynumToDecay   = limitI(this->volKeynumToDecay,     -1200,  1200    );
    this->modDelayTime       = limitD(this->modDelayTime,         0.001,  18.     );
    this->modAttackTime      = limitD(this->modAttackTime,        0.001,  101.6   );
    this->modHoldTime        = limitD(this->modHoldTime,          0.001,  18.     );
    this->modDecayTime       = limitD(this->modDecayTime,         0.001,  101.6   );
    this->modSustainLevel    = limitD(this->modSustainLevel,      0.,     144.    );
    this->modReleaseTime     = limitD(this->modReleaseTime,       0.001,  101.6   );
    this->modKeynumToHold    = limitI(this->modKeynumToHold,      -1200,  1200    );
    this->modKeynumToDecay   = limitI(this->modKeynumToDecay,     -1200,  1200    );
    this->modEnvToPitch      = limitI(this->modEnvToPitch,        -12000, 12000   );
    this->modEnvToFilterFc   = limitI(this->modEnvToFilterFc,     -12000, 12000   );
    this->modLfoDelay        = limitD(this->modLfoDelay,          0.001,  20.     );
    this->modLfoFreq         = limitD(this->modLfoFreq,           0.001,  100.    );
    this->vibLfoDelay        = limitD(this->vibLfoDelay,          0.001,  20.     );
    this->vibLfoFreq         = limitD(this->vibLfoFreq,           0.001,  100.    );
    this->modLfoToPitch      = limitI(this->modLfoToPitch,        -12000, 12000   );
    this->modLfoToFilterFreq = limitI(this->modLfoToFilterFreq,   -12000, 12000   );
    this->modLfoToVolume     = limitD(this->modLfoToVolume,       -96.,   96.     );
    this->vibLfoToPitch      = limitI(this->vibLfoToPitch,        -12000, 12000   );
    this->reverb             = limitD(this->reverb,               0.,     100.    );
    this->chorus             = limitD(this->chorus,               0.,     100.    );
    this->startLoop          = limitUI(this->startLoop, 0, this->length);
    this->endLoop            = limitUI(this->endLoop, 0, this->length);
}

void VoiceParam::read(EltID id)
{
    // Chargement des bags globaux
    if (id.typeElement == elementInstSmpl)
        id.typeElement = elementInst;
    else
        id.typeElement = elementPrst;
    QList<AttributeType> listChamps;
    QList<AttributeValue> listValeurs;
    _sm->getListeBags(id, listChamps, listValeurs);

    // Chargement des bags des éléments
    if (id.typeElement == elementInst)
        id.typeElement = elementInstSmpl;
    else
        id.typeElement = elementPrstInst;
    QList<AttributeType> listChamps2;
    QList<AttributeValue> listValeurs2;
    _sm->getListeBags(id, listChamps2, listValeurs2);
    for (int i = 0; i < listChamps2.count(); i++)
    {
        listChamps << listChamps2.at(i);
        listValeurs << listValeurs2.at(i);
    }

    AttributeType champTmp;
    AttributeValue valTmp;
    for (int i = 0; i < listChamps.size(); i++)
    {
        champTmp = listChamps.at(i);
        valTmp = listValeurs.at(i);
        switch (champTmp)
        {
        case champ_fineTune:                    this->fineTune           = valTmp.shValue;               break;
        case champ_coarseTune:                  this->coarseTune         = valTmp.shValue;               break;
        case champ_pan:                         this->pan                = 0.1 * valTmp.shValue;         break;
        case champ_initialAttenuation:          this->attenuation        = 0.1 * valTmp.shValue;         break;
        case champ_initialFilterQ:              this->filterQ            = 0.1 * valTmp.shValue;         break;
        case champ_sustainVolEnv:               this->volSustainLevel    = 0.1 * valTmp.shValue;         break;
        case champ_keynumToVolEnvHold:          this->volKeynumToHold    = valTmp.shValue;               break;
        case champ_keynumToVolEnvDecay:         this->volKeynumToDecay   = valTmp.shValue;               break;
        case champ_sustainModEnv:               this->modSustainLevel    = 0.1 * valTmp.shValue;         break;
        case champ_keynumToModEnvHold:          this->modKeynumToHold    = valTmp.shValue;               break;
        case champ_keynumToModEnvDecay:         this->modKeynumToDecay   = valTmp.shValue;               break;
        case champ_modEnvToPitch:               this->modEnvToPitch      = valTmp.shValue;               break;
        case champ_modEnvToFilterFc:            this->modEnvToFilterFc   = valTmp.shValue;               break;
        case champ_modLfoToPitch:               this->modLfoToPitch      = valTmp.shValue;               break;
        case champ_modLfoToFilterFc:            this->modLfoToFilterFreq = valTmp.shValue;               break;
        case champ_modLfoToVolume:              this->modLfoToVolume     = 0.1 * valTmp.shValue;         break;
        case champ_vibLfoToPitch:               this->vibLfoToPitch      = valTmp.shValue;               break;
        case champ_reverbEffectsSend:           this->reverb             = 0.1 * valTmp.shValue;         break;
        case champ_chorusEffectsSend:           this->chorus             = 0.1 * valTmp.shValue;         break;
        case champ_delayVolEnv:                 this->volDelayTime       = d1200e2(valTmp.shValue);      break;
        case champ_attackVolEnv:                this->volAttackTime      = d1200e2(valTmp.shValue);      break;
        case champ_holdVolEnv:                  this->volHoldTime        = d1200e2(valTmp.shValue);      break;
        case champ_decayVolEnv:                 this->volDecayTime       = d1200e2(valTmp.shValue);      break;
        case champ_releaseVolEnv:               this->volReleaseTime     = d1200e2(valTmp.shValue);      break;
        case champ_delayModEnv:                 this->modDelayTime       = d1200e2(valTmp.shValue);      break;
        case champ_attackModEnv:                this->modAttackTime      = d1200e2(valTmp.shValue);      break;
        case champ_holdModEnv:                  this->modHoldTime        = d1200e2(valTmp.shValue);      break;
        case champ_decayModEnv:                 this->modDecayTime       = d1200e2(valTmp.shValue);      break;
        case champ_releaseModEnv:               this->modReleaseTime     = d1200e2(valTmp.shValue);      break;
        case champ_delayModLFO:                 this->modLfoDelay        = d1200e2(valTmp.shValue);      break;
        case champ_delayVibLFO:                 this->vibLfoDelay        = d1200e2(valTmp.shValue);      break;
        case champ_scaleTuning:                 this->scaleTune          = valTmp.shValue;               break;
        case champ_overridingRootKey:           this->rootkey            = valTmp.wValue;                break;
        case champ_keynum:                      this->fixedKey           = valTmp.wValue;                break;
        case champ_velocity:                    this->fixedVelocity      = valTmp.wValue;                break;
        case champ_sampleModes:                 this->loopMode           = valTmp.wValue;                break;
        case champ_exclusiveClass:              this->exclusiveClass     = valTmp.wValue;                break;
        case champ_startloopAddrsOffset:        this->loopStartOffset    = valTmp.shValue;               break;
        case champ_startloopAddrsCoarseOffset:  this->loopStartCoarseOffset = valTmp.shValue;            break;
        case champ_endloopAddrsOffset:          this->loopEndOffset      = valTmp.shValue;               break;
        case champ_endloopAddrsCoarseOffset:    this->loopEndCoarseOffset = valTmp.shValue;              break;
        case champ_startAddrsOffset:            this->sampleStartOffset  = valTmp.shValue;               break;
        case champ_startAddrsCoarseOffset:      this->sampleStartCoarseOffset = valTmp.shValue;          break;
        case champ_endAddrsOffset:              this->sampleEndOffset    = valTmp.shValue;               break;
        case champ_endAddrsCoarseOffset:        this->sampleEndCoarseOffset = valTmp.shValue;            break;
        case champ_initialFilterFc:
            if (id.typeElement == elementInstSmpl)
                this->filterFreq = d1200e2(valTmp.shValue) * 8.176;
            else
                this->filterFreq = d1200e2(valTmp.shValue);
            break;
        case champ_freqModLFO:
            if (id.typeElement == elementInstSmpl)
                this->modLfoFreq = d1200e2(valTmp.shValue) * 8.176;
            else
                this->modLfoFreq = d1200e2(valTmp.shValue);
            break;
        case champ_freqVibLFO:
            if (id.typeElement == elementInstSmpl)
                this->vibLfoFreq = d1200e2(valTmp.shValue) * 8.176;
            else
                this->vibLfoFreq = d1200e2(valTmp.shValue);
            break;
        default:
            break;
        }
    }
}

void VoiceParam::add(VoiceParam * voiceParamTmp)
{
    this->fineTune           += voiceParamTmp->fineTune;
    this->coarseTune         += voiceParamTmp->coarseTune;
    this->pan                += voiceParamTmp->pan;
    this->attenuation        += voiceParamTmp->attenuation;
    this->scaleTune          += voiceParamTmp->scaleTune;
    this->filterFreq         *= voiceParamTmp->filterFreq;
    this->filterQ            += voiceParamTmp->filterQ;

    // Volume envelop
    this->volDelayTime       *= voiceParamTmp->volDelayTime;
    this->volAttackTime      *= voiceParamTmp->volAttackTime;
    this->volHoldTime        *= voiceParamTmp->volHoldTime;
    this->volDecayTime       *= voiceParamTmp->volDecayTime;
    this->volSustainLevel    += voiceParamTmp->volSustainLevel;
    this->volReleaseTime     *= voiceParamTmp->volReleaseTime;
    this->volKeynumToHold    += voiceParamTmp->volKeynumToHold;
    this->volKeynumToDecay   += voiceParamTmp->volKeynumToDecay;

    // Modulation envelop
    this->modDelayTime       *= voiceParamTmp->modDelayTime;
    this->modAttackTime      *= voiceParamTmp->modAttackTime;
    this->modHoldTime        *= voiceParamTmp->modHoldTime;
    this->modDecayTime       *= voiceParamTmp->modDecayTime;
    this->modSustainLevel    += voiceParamTmp->modSustainLevel;
    this->modReleaseTime     *= voiceParamTmp->modReleaseTime;
    this->modKeynumToHold    += voiceParamTmp->modKeynumToHold;
    this->modKeynumToDecay   += voiceParamTmp->modKeynumToDecay;
    this->modEnvToPitch      += voiceParamTmp->modEnvToPitch;
    this->modEnvToFilterFc   += voiceParamTmp->modEnvToFilterFc;

    // LFOs
    this->modLfoDelay        *= voiceParamTmp->modLfoDelay;
    this->modLfoFreq         *= voiceParamTmp->modLfoFreq;
    this->vibLfoDelay        *= voiceParamTmp->vibLfoDelay;
    this->vibLfoFreq         *= voiceParamTmp->vibLfoFreq;
    this->modLfoToPitch      += voiceParamTmp->modLfoToPitch;
    this->modLfoToFilterFreq += voiceParamTmp->modLfoToFilterFreq;
    this->modLfoToVolume     += voiceParamTmp->modLfoToVolume;
    this->vibLfoToPitch      += voiceParamTmp->vibLfoToPitch;

    // Effects
    this->reverb             += voiceParamTmp->reverb;
    this->chorus             += voiceParamTmp->chorus;

    // Offsets
    this->loopStartOffset     += voiceParamTmp->loopStartOffset;
    this->loopEndOffset       += voiceParamTmp->loopEndOffset;
    this->sampleStartOffset   += voiceParamTmp->sampleStartOffset;
    this->sampleEndOffset     += voiceParamTmp->sampleEndOffset;
    this->loopStartCoarseOffset += voiceParamTmp->loopStartCoarseOffset;
    this->loopEndCoarseOffset += voiceParamTmp->loopEndCoarseOffset;
    this->sampleStartCoarseOffset += voiceParamTmp->sampleStartCoarseOffset;
    this->sampleEndCoarseOffset += voiceParamTmp->sampleEndCoarseOffset;

    // Misc
    if (voiceParamTmp->loopMode != -1)
        this->loopMode        = voiceParamTmp->loopMode;
    if (voiceParamTmp->rootkey >= 0)
        this->rootkey         = voiceParamTmp->rootkey;
    if (voiceParamTmp->fixedKey != -1)
        this->fixedKey        = voiceParamTmp->fixedKey;
    if (voiceParamTmp->fixedVelocity != -1)
        this->fixedVelocity   = voiceParamTmp->fixedVelocity;
    if (voiceParamTmp->exclusiveClass != 0)
        this->exclusiveClass  = voiceParamTmp->exclusiveClass;
    if (voiceParamTmp->numPreset != -1)
        this->numPreset       = voiceParamTmp->numPreset;
}

double VoiceParam::d1200e2(qint32 val)
{
    return qPow(2., 0.000833333 * val); // Division by 1200
}

double VoiceParam::limitD(double val, double min, double max)
{
    if (val < min)
        return min;
    else if (val > max)
        return max;
    return val;
}

qint32 VoiceParam::limitI(qint32 val, qint32 min, qint32 max)
{
    if (val < min)
        return min;
    else if (val > max)
        return max;
    return val;
}

quint32 VoiceParam::limitUI(quint32 val, quint32 min, quint32 max)
{
    if (val < min)
        return min;
    else if (val > max)
        return max;
    return val;
}

double VoiceParam::getDouble(AttributeType type)
{
    double result = 0;

    switch (type)
    {
    case champ_chorusEffectsSend:
        result = chorus;
        break;
    case champ_reverbEffectsSend:
        result = reverb;
        break;
    case champ_initialFilterFc:
        result = filterFreq;
        break;
    case champ_initialFilterQ:
        result = filterQ;
        break;
    case champ_pan:
        result = pan;
        break;
    case champ_initialAttenuation:
        result = attenuation;
        break;
    case champ_modLfoToVolume:
        result = modLfoToVolume;
        break;
    case champ_delayModLFO:
        result = modLfoDelay;
        break;
    case champ_freqModLFO:
        result = modLfoFreq;
        break;
    case champ_delayVibLFO:
        result = vibLfoDelay;
        break;
    case champ_freqVibLFO:
        result = vibLfoFreq;
        break;
    case champ_delayModEnv:
        result = modDelayTime;
        break;
    case champ_attackModEnv:
        result = modAttackTime;
        break;
    case champ_holdModEnv:
        result = modHoldTime;
        break;
    case champ_decayModEnv:
        result = modDecayTime;
        break;
    case champ_sustainModEnv:
        result = modSustainLevel;
        break;
    case champ_releaseModEnv:
        result = modReleaseTime;
        break;
    case champ_delayVolEnv:
        result = volDelayTime;
        break;
    case champ_attackVolEnv:
        result = volAttackTime;
        break;
    case champ_holdVolEnv:
        result = volHoldTime;
        break;
    case champ_decayVolEnv:
        result = volDecayTime;
        break;
    case champ_sustainVolEnv:
        result = volSustainLevel;
        break;
    case champ_releaseVolEnv:
        result = volReleaseTime;
        break;
    default:
        qDebug() << "VoiceParam: type" << type << "is not readable as a double";
    }

    return result;
}

qint32 VoiceParam::getInteger(AttributeType type)
{
    qint32 result = 0;

    switch (type)
    {
    case champ_startAddrsOffset:
        result = sampleStartOffset;
        break;
    case champ_endAddrsOffset:
        result = sampleEndOffset;
        break;
    case champ_startloopAddrsOffset:
        result = loopStartOffset;
        break;
    case champ_endloopAddrsOffset:
        result = loopEndOffset;
        break;
    case champ_startAddrsCoarseOffset:
        result = sampleStartCoarseOffset;
        break;
    case champ_endAddrsCoarseOffset:
        result = sampleEndCoarseOffset;
        break;
    case champ_startloopAddrsCoarseOffset:
        result = loopStartCoarseOffset;
        break;
    case champ_endloopAddrsCoarseOffset:
        result = loopEndCoarseOffset;
        break;
    case champ_exclusiveClass:
        result = exclusiveClass;
        break;
    case champ_wPreset:
        result = numPreset;
        break;
    case champ_coarseTune:
        result = coarseTune;
        break;
    case champ_fineTune:
        result = fineTune;
        break;
    case champ_scaleTuning:
        result = scaleTune;
        break;
    case champ_keynum:
        result = fixedKey;
        break;
    case champ_velocity:
        result = fixedVelocity;
        break;
    case champ_overridingRootKey:
        result = rootkey;
        break;
    case champ_sampleModes:
        result = loopMode;
        break;
    case champ_modLfoToPitch:
        result = modLfoToPitch;
        break;
    case champ_vibLfoToPitch:
        result = vibLfoToPitch;
        break;
    case champ_modEnvToPitch:
        result = modEnvToPitch;
        break;
    case champ_modLfoToFilterFc:
        result = modLfoToFilterFreq;
        break;
    case champ_modEnvToFilterFc:
        result = modEnvToFilterFc;
        break;
    case champ_keynumToModEnvHold:
        result = modKeynumToHold;
        break;
    case champ_keynumToModEnvDecay:
        result = modKeynumToDecay;
        break;
    case champ_keynumToVolEnvHold:
        result = volKeynumToHold;
        break;
    case champ_keynumToVolEnvDecay:
        result = volKeynumToDecay;
        break;
    default:
        qDebug() << "VoiceParam: type" << type << "is not readable as an integer";
    }

    return result;
}

quint32 VoiceParam::getUnsigned(AttributeType type)
{
    quint32 result = 0;

    switch (type)
    {
    case champ_dwStart16: { // Used here for computing the beginning
        qint32 offset = 32768 * sampleStartCoarseOffset + sampleStartOffset;
        if (offset < 0)
            result = 0;
        else
            result = static_cast<quint32>(offset);
        quint32 currentLength = this->getUnsigned(champ_dwLength);
        if (result > currentLength)
            result = currentLength;
    } break;
    case champ_dwLength: {
        qint32 offset = 32768 * sampleEndCoarseOffset + sampleEndOffset;
        if (static_cast<qint32>(length) + offset < 0)
            result = 0;
        else if (offset > 0)
            result = length;
        else
            result = static_cast<quint32>(static_cast<qint32>(length) + offset);
    } break;
    case champ_dwStartLoop: {
        qint32 offset = 32768 * loopStartCoarseOffset + loopStartOffset;
        if (static_cast<qint32>(startLoop) + offset < 0)
            result = 0;
        else
            result = static_cast<quint32>(static_cast<qint32>(startLoop) + offset);
        quint32 currentLength = this->getUnsigned(champ_dwLength);
        if (result > currentLength)
            result = currentLength;
    } break;
    case champ_dwEndLoop: {
        qint32 offset = 32768 * loopEndCoarseOffset + loopEndOffset;
        if (static_cast<qint32>(endLoop) + offset < 0)
            result = 0;
        else
            result = static_cast<quint32>(static_cast<qint32>(endLoop) + offset);
        quint32 currentLength = this->getUnsigned(champ_dwLength);
        if (result > currentLength)
            result = currentLength;
    } break;
    default:
        qDebug() << "VoiceParam: type" << type << "is not readable as an unsigned integer";
    }

    return result;
}

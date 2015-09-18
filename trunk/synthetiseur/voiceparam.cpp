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

#include "voiceparam.h"
#include "qmath.h"

// Chargement des paramètres
VoiceParam::VoiceParam(Pile_sf2 *sf2, EltID id, VoiceParam *voiceParamTmp) :
    m_sf2(sf2)
{
    // Initialisation
    if (voiceParamTmp && id.typeElement == elementSmpl)
        this->init(elementPrst);
    else
        this->init(id.typeElement, id.indexElt);

    // Lecture
    if (sf2)
    {
        if (id.typeElement == elementSmpl)
            this->readSample(id);
        else
            this->read(id);

        // Prise en compte des paramètres dans l'instance
        if (voiceParamTmp)
            this->add(voiceParamTmp);
    }

    if (id.typeElement == elementSmpl)
        this->limit(id);
}

void VoiceParam::init(ElementType type, int numPreset)
{
    // TON
    this->fineTune              = 0;
    this->coarseTune            = 0;
    this->keynum                = -1;
    this->rootkey               = -1;
    // FILTRE
    this->filterQ               = 0;
    // BOUCLES ET OFFSETS
    this->loopStart             = 0;
    this->loopEnd               = 0;
    this->sampleStart           = 0;
    this->sampleEnd             = 0;
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

void VoiceParam::readSample(EltID id)
{
    // Lecture d'un sample
    this->fineTune          = m_sf2->get(id, champ_chPitchCorrection).cValue;
    this->rootkey           = m_sf2->get(id, champ_byOriginalPitch).bValue;
    // Début et fin du sample
    this->sampleStart       = 0;
    this->sampleEnd         = m_sf2->get(id, champ_dwLength).dwValue;
    // Début et fin de la boucle
    this->loopStart         = m_sf2->get(id, champ_dwStartLoop).dwValue;
    this->loopEnd           = m_sf2->get(id, champ_dwEndLoop).dwValue;
    // Type de boucle
    this->loopMode          = 0;
}

void VoiceParam::limit(EltID id)
{
    // Limites
    this->fineTune           = limit(this->fineTune,             -99,    99      );
    this->coarseTune         = limit(this->coarseTune,           -120,   120     );
    this->pan                = limit(this->pan,                  -50.,   50.     );
    this->attenuation        = limit(this->attenuation,          0.,     144.    );
    this->scaleTune          = limit(this->scaleTune,            0,      1200    );
    this->filterFreq         = limit(this->filterFreq,           18.,    19914.  );
    this->filterQ            = limit(this->filterQ,              0.,     96.     );
    this->volDelayTime       = limit(this->volDelayTime,         0.001,  18.     );
    this->volAttackTime      = limit(this->volAttackTime,        0.001,  101.6   );
    this->volHoldTime        = limit(this->volHoldTime,          0.001,  18.     );
    this->volDecayTime       = limit(this->volDecayTime,         0.001,  101.6   );
    this->volSustainLevel    = limit(this->volSustainLevel,      0.,     144.    );
    this->volReleaseTime     = limit(this->volReleaseTime,       0.001,  101.6   );
    this->volKeynumToHold    = limit(this->volKeynumToHold,      -1200,  1200    );
    this->volKeynumToDecay   = limit(this->volKeynumToDecay,     -1200,  1200    );
    this->modDelayTime       = limit(this->modDelayTime,         0.001,  18.     );
    this->modAttackTime      = limit(this->modAttackTime,        0.001,  101.6   );
    this->modHoldTime        = limit(this->modHoldTime,          0.001,  18.     );
    this->modDecayTime       = limit(this->modDecayTime,         0.001,  101.6   );
    this->modSustainLevel    = limit(this->modSustainLevel,      0.,     144.    );
    this->modReleaseTime     = limit(this->modReleaseTime,       0.001,  101.6   );
    this->modKeynumToHold    = limit(this->modKeynumToHold,      -1200,  1200    );
    this->modKeynumToDecay   = limit(this->modKeynumToDecay,     -1200,  1200    );
    this->modEnvToPitch      = limit(this->modEnvToPitch,        -12000, 12000   );
    this->modEnvToFilterFc   = limit(this->modEnvToFilterFc,     -12000, 12000   );
    this->modLfoDelay        = limit(this->modLfoDelay,          0.001,  20.     );
    this->modLfoFreq         = limit(this->modLfoFreq,           0.001,  100.    );
    this->vibLfoDelay        = limit(this->vibLfoDelay,          0.001,  20.     );
    this->vibLfoFreq         = limit(this->vibLfoFreq,           0.001,  100.    );
    this->modLfoToPitch      = limit(this->modLfoToPitch,        -12000, 12000   );
    this->modLfoToFilterFreq = limit(this->modLfoToFilterFreq,   -12000, 12000   );
    this->modLfoToVolume     = limit(this->modLfoToVolume,       -96.,   96.     );
    this->vibLfoToPitch      = limit(this->vibLfoToPitch,        -12000, 12000   );
    this->reverb             = limit(this->reverb,               0.,     100.    );
    this->chorus             = limit(this->chorus,               0.,     100.    );

    // Positions de début et fin
    int length = m_sf2->get(id, champ_dwLength).dwValue;
    this->sampleStart        = limit(this->sampleStart, 0, length);
    this->sampleEnd          = limit(this->sampleEnd, this->sampleStart, length);
    this->loopStart          = limit(this->loopStart, this->sampleStart, this->sampleEnd);
    this->loopEnd            = limit(this->loopEnd, this->loopStart, this->sampleEnd);
}

void VoiceParam::read(EltID id)
{
    // Chargement des bags globaux
    if (id.typeElement == elementInstSmpl)
        id.typeElement = elementInst;
    else
        id.typeElement = elementPrst;
    QList<Champ> listChamps;
    QList<genAmountType> listValeurs;
    m_sf2->getListeBags(id, listChamps, listValeurs);

    // Chargement des bags des éléments
    if (id.typeElement == elementInst)
        id.typeElement = elementInstSmpl;
    else
        id.typeElement = elementPrstInst;
    m_sf2->getListeBags(id, listChamps, listValeurs);

    // Doublons à enlever
    QList<Champ> listChampsNonDoublon;
    listChampsNonDoublon << champ_startloopAddrsOffset
                         << champ_startloopAddrsCoarseOffset
                         << champ_endloopAddrsOffset
                         << champ_endloopAddrsCoarseOffset
                         << champ_startAddrsOffset
                         << champ_startAddrsCoarseOffset
                         << champ_endAddrsOffset
                         << champ_endAddrsCoarseOffset;
    for (int i = 0; i < listChampsNonDoublon.size(); i++)
    {
        Champ champTmp = listChampsNonDoublon.at(i);
        if (listChamps.count(champTmp) > 1)
        {
            int firstIndex = listChamps.indexOf(champTmp);
            listChamps.removeAt(firstIndex);
            listValeurs.removeAt(firstIndex);
        }
    }

    Champ champTmp;
    genAmountType valTmp;
    for (int i = 0; i < listChamps.size(); i++)
    {
        champTmp = listChamps.at(i);
        valTmp = listValeurs.at(i);
        switch (champTmp)
        {
        case champ_fineTune:                    this->fineTune           = valTmp.shAmount;               break;
        case champ_coarseTune:                  this->coarseTune         = valTmp.shAmount;               break;
        case champ_pan:                         this->pan                = (double)valTmp.shAmount / 10.; break;
        case champ_initialAttenuation:          this->attenuation        = (double)valTmp.shAmount / 10.; break;
        case champ_initialFilterQ:              this->filterQ            = (double)valTmp.shAmount / 10.; break;
        case champ_sustainVolEnv:               this->volSustainLevel    = (double)valTmp.shAmount / 10.; break;
        case champ_keynumToVolEnvHold:          this->volKeynumToHold    = valTmp.shAmount;               break;
        case champ_keynumToVolEnvDecay:         this->volKeynumToDecay   = valTmp.shAmount;               break;
        case champ_sustainModEnv:               this->modSustainLevel    = (double)valTmp.shAmount / 10.; break;
        case champ_keynumToModEnvHold:          this->modKeynumToHold    = valTmp.shAmount;               break;
        case champ_keynumToModEnvDecay:         this->modKeynumToDecay   = valTmp.shAmount;               break;
        case champ_modEnvToPitch:               this->modEnvToPitch      = valTmp.shAmount;               break;
        case champ_modEnvToFilterFc:            this->modEnvToFilterFc   = valTmp.shAmount;               break;
        case champ_modLfoToPitch:               this->modLfoToPitch      = valTmp.shAmount;               break;
        case champ_modLfoToFilterFc:            this->modLfoToFilterFreq = valTmp.shAmount;               break;
        case champ_modLfoToVolume:              this->modLfoToVolume     = (double)valTmp.shAmount / 10.; break;
        case champ_vibLfoToPitch:               this->vibLfoToPitch      = valTmp.shAmount;               break;
        case champ_reverbEffectsSend:           this->reverb             = (double)valTmp.shAmount / 10.; break;
        case champ_chorusEffectsSend:           this->chorus             = (double)valTmp.shAmount / 10.; break;
        case champ_delayVolEnv:                 this->volDelayTime       = d1200e2(valTmp.shAmount);      break;
        case champ_attackVolEnv:                this->volAttackTime      = d1200e2(valTmp.shAmount);      break;
        case champ_holdVolEnv:                  this->volHoldTime        = d1200e2(valTmp.shAmount);      break;
        case champ_decayVolEnv:                 this->volDecayTime       = d1200e2(valTmp.shAmount);      break;
        case champ_releaseVolEnv:               this->volReleaseTime     = d1200e2(valTmp.shAmount);      break;
        case champ_delayModEnv:                 this->modDelayTime       = d1200e2(valTmp.shAmount);      break;
        case champ_attackModEnv:                this->modAttackTime      = d1200e2(valTmp.shAmount);      break;
        case champ_holdModEnv:                  this->modHoldTime        = d1200e2(valTmp.shAmount);      break;
        case champ_decayModEnv:                 this->modDecayTime       = d1200e2(valTmp.shAmount);      break;
        case champ_releaseModEnv:               this->modReleaseTime     = d1200e2(valTmp.shAmount);      break;
        case champ_delayModLFO:                 this->modLfoDelay        = d1200e2(valTmp.shAmount);      break;
        case champ_delayVibLFO:                 this->vibLfoDelay        = d1200e2(valTmp.shAmount);      break;
        case champ_scaleTuning:                 this->scaleTune          = valTmp.shAmount;               break;
        case champ_overridingRootKey:           this->rootkey            = valTmp.wAmount;                break;
        case champ_keynum:                      this->keynum             = valTmp.wAmount;                break;
        case champ_velocity:                    this->fixedVelocity      = valTmp.wAmount;                break;
        case champ_sampleModes:                 this->loopMode           = valTmp.wAmount;                break;
        case champ_exclusiveClass:              this->exclusiveClass     = valTmp.wAmount;                break;
        case champ_startloopAddrsOffset:        this->loopStart         += valTmp.shAmount;               break;
        case champ_startloopAddrsCoarseOffset:  this->loopStart         += valTmp.shAmount * 32768;       break;
        case champ_endloopAddrsOffset:          this->loopEnd           += valTmp.shAmount;               break;
        case champ_endloopAddrsCoarseOffset:    this->loopEnd           += valTmp.shAmount * 32768;       break;
        case champ_startAddrsOffset:            this->sampleStart       += valTmp.shAmount;               break;
        case champ_startAddrsCoarseOffset:      this->sampleStart       += valTmp.shAmount * 32768;       break;
        case champ_endAddrsOffset:              this->sampleEnd         += valTmp.shAmount;               break;
        case champ_endAddrsCoarseOffset:        this->sampleEnd         += valTmp.shAmount * 32768;       break;
        case champ_initialFilterFc:
            if (id.typeElement == elementInstSmpl)
                this->filterFreq = d1200e2(valTmp.shAmount) * 8.176;
            else
                this->filterFreq = d1200e2(valTmp.shAmount);
            break;
        case champ_freqModLFO:
            if (id.typeElement == elementInstSmpl)
                this->modLfoFreq = d1200e2(valTmp.shAmount) * 8.176;
            else
                this->modLfoFreq = d1200e2(valTmp.shAmount);
            break;
        case champ_freqVibLFO:
            if (id.typeElement == elementInstSmpl)
                this->vibLfoFreq = d1200e2(valTmp.shAmount) * 8.176;
            else
                this->vibLfoFreq = d1200e2(valTmp.shAmount);
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
    // Enveloppe volume
    this->volDelayTime       *= voiceParamTmp->volDelayTime;
    this->volAttackTime      *= voiceParamTmp->volAttackTime;
    this->volHoldTime        *= voiceParamTmp->volHoldTime;
    this->volDecayTime       *= voiceParamTmp->volDecayTime;
    this->volSustainLevel    += voiceParamTmp->volSustainLevel;
    this->volReleaseTime     *= voiceParamTmp->volReleaseTime;
    this->volKeynumToHold    += voiceParamTmp->volKeynumToHold;
    this->volKeynumToDecay   += voiceParamTmp->volKeynumToDecay;
    // Enveloppe modulation
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
    // Effets
    this->reverb             += voiceParamTmp->reverb;
    this->chorus             += voiceParamTmp->chorus;
    // Autres
    if (voiceParamTmp->loopMode != -1)
        this->loopMode        = voiceParamTmp->loopMode;
    if (voiceParamTmp->rootkey != -1)
        this->rootkey         = voiceParamTmp->rootkey;
    if (voiceParamTmp->keynum != -1)
        this->keynum          = voiceParamTmp->keynum;
    if (voiceParamTmp->fixedVelocity != -1)
        this->fixedVelocity   = voiceParamTmp->fixedVelocity;
    this->loopStart          += voiceParamTmp->loopStart;
    this->loopEnd            += voiceParamTmp->loopEnd;
    this->sampleStart        += voiceParamTmp->sampleStart;
    this->sampleEnd          += voiceParamTmp->sampleEnd;
    if (voiceParamTmp->exclusiveClass != 0)
        this->exclusiveClass  = voiceParamTmp->exclusiveClass;
    if (voiceParamTmp->numPreset != -1)
        this->numPreset       = voiceParamTmp->numPreset;
}

double VoiceParam::getPitchDifference(int note)
{
    double noteJouee = note;
    if (this->keynum > -1)
        noteJouee = this->keynum;
    return (noteJouee - this->rootkey) * ((double)scaleTune / 100.)
           + (double)(fineTune) / 100 + coarseTune;
}
double VoiceParam::d1200e2(qint32 val)
{
    return qPow(2., (double)val / 1200);
}
double VoiceParam::limit(double val, double min, double max)
{
    if (val < min)
        return min;
    else if (val > max)
        return max;
    return val;
}
qint32 VoiceParam::limit(qint32 val, qint32 min, qint32 max)
{
    if (val < min)
        return min;
    else if (val > max)
        return max;
    return val;
}

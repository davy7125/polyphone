/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013 Davy Triponney                                     **
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

// Chargement des paramètres
VoiceParam::VoiceParam(Pile_sf2 *sf2, EltID id, VoiceParam *voiceParamTmp) :
    m_sf2(sf2)
{
    if (voiceParamTmp)
    {
        // Recopie
        this->copy(voiceParamTmp);
    }
    else
    {
        // Initialisation
        this->init(id);
    }

    // Lecture
    if (sf2)
    {
        if (id.typeElement == elementSmpl)
            this->readSample(id);
        else
            this->read(id);
    }
}

void VoiceParam::init(EltID id)
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
    if (id.typeElement == elementSmpl)
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
    else if (id.typeElement == elementInstSmpl)
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
        this->numPreset             = id.indexElt;
    }
}
void VoiceParam::copy(VoiceParam * voiceParamTmp)
{
    // TON
    this->fineTune              = voiceParamTmp->fineTune;
    this->coarseTune            = voiceParamTmp->coarseTune;
    this->keynum                = voiceParamTmp->keynum;
    this->scaleTune             = voiceParamTmp->scaleTune;
    this->rootkey               = voiceParamTmp->rootkey;
    // FILTRE
    this->filterFreq            = voiceParamTmp->filterFreq;
    this->filterQ               = voiceParamTmp->filterQ;
    // BOUCLES ET OFFSETS
    this->loopMode              = voiceParamTmp->loopMode;
    this->loopStart             = voiceParamTmp->loopStart;
    this->loopEnd               = voiceParamTmp->loopEnd;
    this->sampleStart           = voiceParamTmp->sampleStart;
    this->sampleEnd             = voiceParamTmp->sampleEnd;
    // ATTENUATION, BALANCE
    this->attenuation           = voiceParamTmp->attenuation;
    this->pan                   = voiceParamTmp->pan;
    this->fixedVelocity         = voiceParamTmp->fixedVelocity;
    // ENVELOPPE VOLUME
    this->volDelayTime          = voiceParamTmp->volDelayTime;
    this->volAttackTime         = voiceParamTmp->volAttackTime;
    this->volHoldTime           = voiceParamTmp->volHoldTime;
    this->volDecayTime          = voiceParamTmp->volDecayTime;
    this->volReleaseTime        = voiceParamTmp->volReleaseTime;
    this->volKeynumToHold       = voiceParamTmp->volKeynumToHold;
    this->volKeynumToDecay      = voiceParamTmp->volKeynumToDecay;
    this->volSustainLevel       = voiceParamTmp->volSustainLevel;
    // ENVELOPPE MODULATION
    this->modDelayTime          = voiceParamTmp->modDelayTime;
    this->modAttackTime         = voiceParamTmp->modAttackTime;
    this->modHoldTime           = voiceParamTmp->modHoldTime;
    this->modDecayTime          = voiceParamTmp->modDecayTime;
    this->modReleaseTime        = voiceParamTmp->modReleaseTime;
    this->modKeynumToHold       = voiceParamTmp->modKeynumToHold;
    this->modKeynumToDecay      = voiceParamTmp->modKeynumToDecay;
    this->modEnvToPitch         = voiceParamTmp->modEnvToPitch;
    this->modEnvToFilterFc      = voiceParamTmp->modEnvToFilterFc;
    this->modSustainLevel       = voiceParamTmp->modSustainLevel;
    // LFOs
    this->modLfoDelay           = voiceParamTmp->modLfoDelay;
    this->modLfoFreq            = voiceParamTmp->modLfoFreq;
    this->vibLfoDelay           = voiceParamTmp->vibLfoDelay;
    this->vibLfoFreq            = voiceParamTmp->vibLfoFreq;
    this->modLfoToPitch         = voiceParamTmp->modLfoToPitch;
    this->modLfoToFilterFreq    = voiceParamTmp->modLfoToFilterFreq;
    this->vibLfoToPitch         = voiceParamTmp->vibLfoToPitch;
    this->modLfoToVolume        = voiceParamTmp->modLfoToVolume;
    // EFFETS
    this->reverb                = voiceParamTmp->reverb;
    this->chorus                = voiceParamTmp->chorus;
    // DIVERS
    this->exclusiveClass        = voiceParamTmp->exclusiveClass;
    this->numPreset             = voiceParamTmp->numPreset;
}

void VoiceParam::readSample(EltID id)
{
    // Lecture d'un sample
    this->fineTune          += m_sf2->get(id, champ_chPitchCorrection).cValue;
    if (this->rootkey == -1)
        this->rootkey       = m_sf2->get(id, champ_byOriginalPitch).bValue;
    this->loopStart         += m_sf2->get(id, champ_dwStartLoop).dwValue;
    this->loopEnd           += m_sf2->get(id, champ_dwEndLoop).dwValue;
    if (this->loopMode == -1)
        this->loopMode = 0;

    // Limites
    this->fineTune          = limit(this->fineTune,             -99,    99);
    this->coarseTune        = limit(this->coarseTune,           -120,   120);
    this->pan               = limit(this->pan,                  -50.,   50.);
    this->attenuation       = limit(this->attenuation,          0.,     144.);
    this->scaleTune         = limit(this->scaleTune,            0,      1200);
    this->filterFreq        = limit(this->filterFreq,           18.,    19914.);
    this->filterQ           = limit(this->filterQ,              0.,     96.);
    this->volDelayTime      = limit(this->volDelayTime,         0.001,  18.);
    this->volAttackTime     = limit(this->volAttackTime,        0.001,  101.6);
    this->volHoldTime       = limit(this->volHoldTime,          0.001,  18.);
    this->volDecayTime      = limit(this->volDecayTime,         0.001,  101.6);
    this->volSustainLevel   = limit(this->volSustainLevel,      0.,     144.);
    this->volReleaseTime    = limit(this->volReleaseTime,       0.001,  101.6);
    this->volKeynumToHold   = limit(this->volKeynumToHold,      -1200,  1200);
    this->volKeynumToDecay  = limit(this->volKeynumToDecay,     -1200,  1200);
    this->modDelayTime      = limit(this->modDelayTime,         0.001,  18.);
    this->modAttackTime     = limit(this->modAttackTime,        0.001,  101.6);
    this->modHoldTime       = limit(this->modHoldTime,          0.001,  18.);
    this->modDecayTime      = limit(this->modDecayTime,         0.001,  101.6);
    this->modSustainLevel   = limit(this->modSustainLevel,      0.,     144.);
    this->modReleaseTime    = limit(this->modReleaseTime,       0.001,  101.6);
    this->modKeynumToHold   = limit(this->modKeynumToHold,      -1200,  1200);
    this->modKeynumToDecay  = limit(this->modKeynumToDecay,     -1200,  1200);
    this->modEnvToPitch     = limit(this->modEnvToPitch,        -12000, 12000);
    this->modEnvToFilterFc  = limit(this->modEnvToFilterFc,     -12000, 12000);
    this->modLfoDelay       = limit(this->modLfoDelay,          0.001,  20.);
    this->modLfoFreq        = limit(this->modLfoFreq,           0.001,  100.);
    this->vibLfoDelay       = limit(this->vibLfoDelay,          0.001,  20.);
    this->vibLfoFreq        = limit(this->vibLfoFreq,           0.001,  100.);
    this->modLfoToPitch     = limit(this->modLfoToPitch,        -12000, 12000);
    this->modLfoToFilterFreq= limit(this->modLfoToFilterFreq,   -12000, 12000);
    this->modLfoToVolume    = limit(this->modLfoToVolume,       -96.,   96.);
    this->vibLfoToPitch     = limit(this->vibLfoToPitch,        -12000, 12000);
    this->reverb            = limit(this->reverb,               0.,     100.);
    this->chorus            = limit(this->chorus,               0.,     100.);
}

void VoiceParam::read(EltID id)
{
    // Chargement des bags globaux
    if (id.typeElement == elementInstSmpl)
        id.typeElement = elementInst;
    else
        id.typeElement = elementPrst;
    QList<Champ> listChampsGlobaux;
    QList<Valeur> listValeursGlobales;
    m_sf2->getListeBags(id, listChampsGlobaux, listValeursGlobales);

    // Chargement des bags des éléments
    QList<Champ> listChampsElements;
    QList<Valeur> listValeursElements;
    if (id.typeElement == elementInst)
        id.typeElement = elementInstSmpl;
    else
        id.typeElement = elementPrstInst;
    m_sf2->getListeBags(id, listChampsElements, listValeursElements);

    // Fusion
    listChampsGlobaux.append(listChampsElements);
    listValeursGlobales.append(listValeursElements);

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
        if (listChampsGlobaux.count(champTmp) > 1)
        {
            int lastIndex = listChampsGlobaux.lastIndexOf(champTmp);
            listChampsGlobaux.removeAt(lastIndex);
            listValeursGlobales.removeAt(lastIndex);
        }
    }

    // Initialisation paramètres temporaires
    VoiceParam voiceParamTmp(NULL, id);

    // Chargement des données des bags
    if (id.typeElement == elementInstSmpl)
        id.typeElement = elementInstGen;
    else
        id.typeElement = elementPrstGen;
    this->updateParam(id, voiceParamTmp, listChampsGlobaux, listValeursGlobales);

    // Prise en compte des paramètres dans l'instance
    this->add(&voiceParamTmp);
}

void VoiceParam::updateParam(EltID id, VoiceParam &voiceParamTmp, QList<Champ> listeChamps, QList<Valeur> listeValeurs)
{
    Champ champTmp;
    Valeur valTmp;
    for (int i = 0; i < listeChamps.size(); i++)
    {
        champTmp = listeChamps.at(i);
        valTmp = listeValeurs.at(i);
        switch (champTmp)
        {
        case champ_fineTune:                    voiceParamTmp.fineTune          = valTmp.shValue;               break;
        case champ_coarseTune:                  voiceParamTmp.coarseTune        = valTmp.shValue;               break;
        case champ_pan:                         voiceParamTmp.pan               = (double)valTmp.shValue / 10.; break;
        case champ_initialAttenuation:          voiceParamTmp.attenuation       = (double)valTmp.shValue / 10.; break;
        case champ_initialFilterQ:              voiceParamTmp.filterQ           = (double)valTmp.shValue / 10.; break;
        case champ_sustainVolEnv:               voiceParamTmp.volSustainLevel   = (double)valTmp.shValue / 10.; break;
        case champ_keynumToVolEnvHold:          voiceParamTmp.volKeynumToHold   = valTmp.shValue;               break;
        case champ_keynumToVolEnvDecay:         voiceParamTmp.volKeynumToDecay  = valTmp.shValue;               break;
        case champ_sustainModEnv:               voiceParamTmp.modSustainLevel   = (double)valTmp.shValue / 10.; break;
        case champ_keynumToModEnvHold:          voiceParamTmp.modKeynumToHold   = valTmp.shValue;               break;
        case champ_keynumToModEnvDecay:         voiceParamTmp.modKeynumToDecay  = valTmp.shValue;               break;
        case champ_modEnvToPitch:               voiceParamTmp.modEnvToPitch     = valTmp.shValue;               break;
        case champ_modEnvToFilterFc:            voiceParamTmp.modEnvToFilterFc  = valTmp.shValue;               break;
        case champ_modLfoToPitch:               voiceParamTmp.modLfoToPitch     = valTmp.shValue;               break;
        case champ_modLfoToFilterFc:            voiceParamTmp.modLfoToFilterFreq= valTmp.shValue;               break;
        case champ_modLfoToVolume:              voiceParamTmp.modLfoToVolume    = (double)valTmp.shValue / 10.; break;
        case champ_vibLfoToPitch:               voiceParamTmp.vibLfoToPitch     = valTmp.shValue;               break;
        case champ_reverbEffectsSend:           voiceParamTmp.reverb            = (double)valTmp.shValue / 10.; break;
        case champ_chorusEffectsSend:           voiceParamTmp.chorus            = (double)valTmp.shValue / 10.; break;
        case champ_delayVolEnv:                 voiceParamTmp.volDelayTime      = d1200e2(valTmp.shValue);      break;
        case champ_attackVolEnv:                voiceParamTmp.volAttackTime     = d1200e2(valTmp.shValue);      break;
        case champ_holdVolEnv:                  voiceParamTmp.volHoldTime       = d1200e2(valTmp.shValue);      break;
        case champ_decayVolEnv:                 voiceParamTmp.volDecayTime      = d1200e2(valTmp.shValue);      break;
        case champ_releaseVolEnv:               voiceParamTmp.volReleaseTime    = d1200e2(valTmp.shValue);      break;
        case champ_delayModEnv:                 voiceParamTmp.modDelayTime      = d1200e2(valTmp.shValue);      break;
        case champ_attackModEnv:                voiceParamTmp.modAttackTime     = d1200e2(valTmp.shValue);      break;
        case champ_holdModEnv:                  voiceParamTmp.modHoldTime       = d1200e2(valTmp.shValue);      break;
        case champ_decayModEnv:                 voiceParamTmp.modDecayTime      = d1200e2(valTmp.shValue);      break;
        case champ_releaseModEnv:               voiceParamTmp.modReleaseTime    = d1200e2(valTmp.shValue);      break;
        case champ_delayModLFO:                 voiceParamTmp.modLfoDelay       = d1200e2(valTmp.shValue);      break;
        case champ_delayVibLFO:                 voiceParamTmp.vibLfoDelay       = d1200e2(valTmp.shValue);      break;
        case champ_scaleTuning:                 voiceParamTmp.scaleTune         = valTmp.shValue;               break;
        case champ_overridingRootKey:           voiceParamTmp.rootkey           = valTmp.wValue;                break;
        case champ_keynum:                      voiceParamTmp.keynum            = valTmp.wValue;                break;
        case champ_velocity:                    voiceParamTmp.fixedVelocity     = valTmp.wValue;                break;
        case champ_sampleModes:                 voiceParamTmp.loopMode          = valTmp.wValue;                break;
        case champ_exclusiveClass:              voiceParamTmp.exclusiveClass    = valTmp.wValue;                break;
        case champ_startloopAddrsOffset:        voiceParamTmp.loopStart        += valTmp.shValue;               break;
        case champ_startloopAddrsCoarseOffset:  voiceParamTmp.loopStart        += valTmp.shValue * 32768;       break;
        case champ_endloopAddrsOffset:          voiceParamTmp.loopEnd          += valTmp.shValue;               break;
        case champ_endloopAddrsCoarseOffset:    voiceParamTmp.loopEnd          += valTmp.shValue * 32768;       break;
        case champ_startAddrsOffset:            voiceParamTmp.sampleStart      += valTmp.shValue;               break;
        case champ_startAddrsCoarseOffset:      voiceParamTmp.sampleStart      += valTmp.shValue * 32768;       break;
        case champ_endAddrsOffset:              voiceParamTmp.sampleEnd        += valTmp.shValue;               break;
        case champ_endAddrsCoarseOffset:        voiceParamTmp.sampleEnd        += valTmp.shValue * 32768;       break;
        case champ_initialFilterFc:
            if (id.typeElement == elementInstSmpl)
                voiceParamTmp.filterFreq = d1200e2(valTmp.shValue) * 8.176;
            else
                voiceParamTmp.filterFreq = d1200e2(valTmp.shValue);
            break;
        case champ_freqModLFO:
            if (id.typeElement == elementInstSmpl)
                voiceParamTmp.modLfoFreq = d1200e2(valTmp.shValue) * 8.176;
            else
                voiceParamTmp.modLfoFreq = d1200e2(valTmp.shValue);
            break;
        case champ_freqVibLFO:
            if (id.typeElement == elementInstSmpl)
                voiceParamTmp.vibLfoFreq = d1200e2(valTmp.shValue) * 8.176;
            else
                voiceParamTmp.vibLfoFreq = d1200e2(valTmp.shValue);
            break;
        default:
            break;
        }
    }
}

void VoiceParam::add(VoiceParam * voiceParamTmp)
{
    this->fineTune          += voiceParamTmp->fineTune;
    this->coarseTune        += voiceParamTmp->coarseTune;
    this->pan               += voiceParamTmp->pan;
    this->attenuation       += voiceParamTmp->attenuation;
    this->scaleTune         += voiceParamTmp->scaleTune;
    this->filterFreq        *= voiceParamTmp->filterFreq;
    this->filterQ           += voiceParamTmp->filterQ;
    // Enveloppe volume
    this->volDelayTime      *= voiceParamTmp->volDelayTime;
    this->volAttackTime     *= voiceParamTmp->volAttackTime;
    this->volHoldTime       *= voiceParamTmp->volHoldTime;
    this->volDecayTime      *= voiceParamTmp->volDecayTime;
    this->volSustainLevel   += voiceParamTmp->volSustainLevel;
    this->volReleaseTime    *= voiceParamTmp->volReleaseTime;
    this->volKeynumToHold   += voiceParamTmp->volKeynumToHold;
    this->volKeynumToDecay  += voiceParamTmp->volKeynumToDecay;
    // Enveloppe modulation
    this->modDelayTime      *= voiceParamTmp->modDelayTime;
    this->modAttackTime     *= voiceParamTmp->modAttackTime;
    this->modHoldTime       *= voiceParamTmp->modHoldTime;
    this->modDecayTime      *= voiceParamTmp->modDecayTime;
    this->modSustainLevel   += voiceParamTmp->modSustainLevel;
    this->modReleaseTime    *= voiceParamTmp->modReleaseTime;
    this->modKeynumToHold   += voiceParamTmp->modKeynumToHold;
    this->modKeynumToDecay  += voiceParamTmp->modKeynumToDecay;
    this->modEnvToPitch     += voiceParamTmp->modEnvToPitch;
    this->modEnvToFilterFc  += voiceParamTmp->modEnvToFilterFc;
    // LFOs
    this->modLfoDelay       *= voiceParamTmp->modLfoDelay;
    this->modLfoFreq        *= voiceParamTmp->modLfoFreq;
    this->vibLfoDelay       *= voiceParamTmp->vibLfoDelay;
    this->vibLfoFreq        *= voiceParamTmp->vibLfoFreq;
    this->modLfoToPitch     += voiceParamTmp->modLfoToPitch;
    this->modLfoToFilterFreq+= voiceParamTmp->modLfoToFilterFreq;
    this->modLfoToVolume    += voiceParamTmp->modLfoToVolume;
    this->vibLfoToPitch     += voiceParamTmp->vibLfoToPitch;
    // Effets
    this->reverb            += voiceParamTmp->reverb;
    this->chorus            += voiceParamTmp->chorus;
    // Autres
    if (voiceParamTmp->loopMode != -1)
        this->loopMode = voiceParamTmp->loopMode;
    if (voiceParamTmp->rootkey != -1)
        this->rootkey = voiceParamTmp->rootkey;
    if (voiceParamTmp->keynum != -1)
        this->keynum = voiceParamTmp->keynum;
    if (voiceParamTmp->fixedVelocity != -1)
        this->fixedVelocity = voiceParamTmp->fixedVelocity;
    this->loopStart         += voiceParamTmp->loopStart;
    this->loopEnd           += voiceParamTmp->loopEnd;
    this->sampleStart       += voiceParamTmp->sampleStart;
    this->sampleEnd         += voiceParamTmp->sampleEnd;
    if (voiceParamTmp->exclusiveClass != 0)
        this->exclusiveClass = voiceParamTmp->exclusiveClass;
    if (voiceParamTmp->numPreset != -1)
        this->numPreset = voiceParamTmp->numPreset;
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
    return pow(2., ((double)val / 1200));
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

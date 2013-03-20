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
VoiceParam::VoiceParam(Pile_sf2 *sf2, EltID id, VoiceParam *voiceParamTmp, bool smplMode) : m_sf2(sf2)
{
    // Chargement des données
    if (id.typeElement == elementSmpl)
    {
        m_id = id;
        m_idGlobal = id;

        this->pan               = 0;
        this->attenuation       = 0;
        this->fixedVelocity     = -1;
        if (smplMode || !voiceParamTmp)
            this->filterFreq    = 19914;
        else
            this->filterFreq    = 1;
        this->filterQ           = 0;
        // Enveloppe volume
        if (smplMode || !voiceParamTmp)
        {
            this->volDelayTime      = 0.001;
            this->volAttackTime     = 0.001;
            this->volHoldTime       = 0.001;
            this->volDecayTime      = 0.001;
            this->volReleaseTime    = 0.1;
        }
        else
        {
            this->volDelayTime      = 1;
            this->volAttackTime     = 1;
            this->volHoldTime       = 1;
            this->volDecayTime      = 1;
            this->volReleaseTime    = 1;
        }
        this->volSustainLevel       = 0;
        this->volKeynumToHold       = 0;
        this->volKeynumToDecay      = 0;
        // Enveloppe modulation
        if (smplMode || !voiceParamTmp)
        {
            this->modDelayTime      = 0.001;
            this->modAttackTime     = 0.001;
            this->modHoldTime       = 0.001;
            this->modDecayTime      = 0.001;
            this->modReleaseTime    = 0.001;
        }
        else
        {
            this->modDelayTime      = 1;
            this->modAttackTime     = 1;
            this->modHoldTime       = 1;
            this->modDecayTime      = 1;
            this->modReleaseTime    = 1;
        }
        this->modSustainLevel   = 0;
        this->modKeynumToHold   = 0;
        this->modKeynumToDecay  = 0;
        this->modEnvToPitch     = 0;
        this->modEnvToFilterFc  = 0;
        // Note et boucle
        if (smplMode || !voiceParamTmp)
            this->scaleTune = 100;
        else
            this->scaleTune         = 0;
        this->fineTune          = m_sf2->get(m_id, champ_chPitchCorrection).cValue;
        this->coarseTune        = 0;
        this->keynum            = -1;
        this->rootkey           = m_sf2->get(m_id, champ_byOriginalPitch).bValue;
        this->loopStart         = m_sf2->get(m_id, champ_dwStartLoop).dwValue;
        this->loopEnd           = m_sf2->get(m_id, champ_dwEndLoop).dwValue;
        this->sampleStart       = 0;
        this->sampleEnd         = 0;
        this->loopMode          = 0;
        // LFOs
        if (smplMode || !voiceParamTmp)
        {
            this->modLfoDelay       = 0.001;
            this->modLfoFreq        = 8.176;
            this->vibLfoDelay       = 0.001;
            this->vibLfoFreq        = 8.176;
        }
        else
        {
            this->modLfoDelay       = 1;
            this->modLfoFreq        = 1;
            this->vibLfoDelay       = 1;
            this->vibLfoFreq        = 1;
        }
        this->modLfoToPitch = 0;
        this->modLfoToFilterFreq = 0;
        this->modLfoToVolume = 0;
        this->vibLfoToPitch = 0;
        // Effets
        this->reverb = 0;
        this->chorus = 0;
    }
    else
    {
        m_id = id;
        m_idGlobal = id;

        // Identifiant de l'instrument ou du preset
        if (id.typeElement == elementInstSmpl)
            m_idGlobal.typeElement = elementInst;
        else
            m_idGlobal.typeElement = elementPrst;

        // Accordage, position, attenuation
        this->fineTune          = this->get(champ_fineTune, readShort, 0);
        this->coarseTune        = this->get(champ_coarseTune, readShort, 0);
        this->pan               = (double)this->get(champ_pan, readShort, 0) / 10;
        this->attenuation       = (double)this->get(champ_initialAttenuation, readShort, 0) / 10;
        this->filterQ           = (double)this->get(champ_initialFilterQ, readShort, 0) / 10;

        // Enveloppe volume
        this->volSustainLevel   = (double)this->get(champ_sustainVolEnv, readShort, 0) / 10;
        this->volKeynumToHold   = this->get(champ_keynumToVolEnvHold, readShort, 0);
        this->volKeynumToDecay  = this->get(champ_keynumToVolEnvDecay, readShort, 0);

        // Enveloppe modulation
        this->modSustainLevel   = (double)this->get(champ_sustainModEnv, readShort, 0) / 10;
        this->modKeynumToHold   = this->get(champ_keynumToModEnvHold, readShort, 0);
        this->modKeynumToDecay  = this->get(champ_keynumToModEnvDecay, readShort, 0);
        this->modEnvToPitch     = this->get(champ_modEnvToPitch, readShort, 0);
        this->modEnvToFilterFc  = this->get(champ_modEnvToFilterFc, readShort, 0);

        // LFOs
        this->modLfoToPitch     = this->get(champ_modLfoToPitch, readShort, 0);
        this->modLfoToFilterFreq= this->get(champ_modLfoToFilterFc, readShort, 0);
        this->modLfoToVolume    = (double)this->get(champ_modLfoToVolume, readShort, 0) / 10;
        this->vibLfoToPitch     = this->get(champ_vibLfoToPitch, readShort, 0);

        // Effets
        this->reverb            = (double)this->get(champ_reverbEffectsSend, readShort, 0) / 10;
        this->chorus            = (double)this->get(champ_chorusEffectsSend, readShort, 0) / 10;

        // Paramétrage spécifique
        if (id.typeElement == elementInstSmpl)
        {
            // Filtre
            this->filterFreq        = d1200e2(this->get(champ_initialFilterFc, readShort, 13500)) * 8.176;

            // Enveloppe volume
            this->volDelayTime      = d1200e2(this->get(champ_delayVolEnv, readShort, -12000));
            this->volAttackTime     = d1200e2(this->get(champ_attackVolEnv, readShort, -12000));
            this->volHoldTime       = d1200e2(this->get(champ_holdVolEnv, readShort, -12000));
            this->volDecayTime      = d1200e2(this->get(champ_decayVolEnv, readShort, -12000));
            this->volReleaseTime    = d1200e2(this->get(champ_releaseVolEnv, readShort, -12000));
            this->volKeynumToHold   = this->get(champ_keynumToVolEnvHold, readShort, 0);
            this->volKeynumToDecay  = this->get(champ_keynumToVolEnvDecay, readShort, 0);

            // Enveloppe modulation
            this->modDelayTime      = d1200e2(this->get(champ_delayModEnv, readShort, -12000));
            this->modAttackTime     = d1200e2(this->get(champ_attackModEnv, readShort, -12000));
            this->modHoldTime       = d1200e2(this->get(champ_holdModEnv, readShort, -12000));
            this->modDecayTime      = d1200e2(this->get(champ_decayModEnv, readShort, -12000));
            this->modReleaseTime    = d1200e2(this->get(champ_releaseModEnv, readShort, -12000));

            // Valeurs fixes, boucle
            this->rootkey           = this->get(champ_overridingRootKey, readWord, -1);
            this->keynum            = this->get(champ_keynum, readWord, -1);
            this->scaleTune         = this->get(champ_scaleTuning, readShort, 100);
            this->fixedVelocity     = this->get(champ_velocity, readWord, -1);
            this->loopStart         = this->get(champ_startloopAddrsOffset, readShort, 0) +
                    this->get(champ_startloopAddrsCoarseOffset, readShort, 0) * 32768;
            this->loopEnd           = this->get(champ_endloopAddrsOffset, readShort, 0) +
                    this->get(champ_endloopAddrsCoarseOffset, readShort, 0) * 32768;
            this->sampleStart         = this->get(champ_startAddrsOffset, readShort, 0) +
                    this->get(champ_startAddrsCoarseOffset, readShort, 0) * 32768;
            this->sampleEnd           = this->get(champ_endAddrsOffset, readShort, 0) +
                    this->get(champ_endAddrsCoarseOffset, readShort, 0) * 32768;
            this->loopMode          = this->get(champ_sampleModes, readWord, 0);

            // LFOs
            this->modLfoDelay       = d1200e2(this->get(champ_delayModLFO, readShort, -12000));
            this->modLfoFreq        = d1200e2(this->get(champ_freqModLFO, readShort, 0)) * 8.176;
            this->vibLfoDelay       = d1200e2(this->get(champ_delayVibLFO, readShort, -12000));
            this->vibLfoFreq        = d1200e2(this->get(champ_freqVibLFO, readShort, 0)) * 8.176;
        }
        else
        {
            // Filtre
            this->filterFreq        = d1200e2(this->get(champ_initialFilterFc, readShort, 0));

            // Enveloppe volume
            this->volDelayTime      = d1200e2(this->get(champ_delayVolEnv, readShort, 0));
            this->volAttackTime     = d1200e2(this->get(champ_attackVolEnv, readShort, 0));
            this->volHoldTime       = d1200e2(this->get(champ_holdVolEnv, readShort, 0));
            this->volDecayTime      = d1200e2(this->get(champ_decayVolEnv, readShort, 0));
            this->volReleaseTime    = d1200e2(this->get(champ_releaseVolEnv, readShort, 0));

            // Enveloppe modulation
            this->modDelayTime      = d1200e2(this->get(champ_delayModEnv, readShort, 0));
            this->modAttackTime     = d1200e2(this->get(champ_attackModEnv, readShort, 0));
            this->modHoldTime       = d1200e2(this->get(champ_holdModEnv, readShort, 0));
            this->modDecayTime      = d1200e2(this->get(champ_decayModEnv, readShort, 0));
            this->modReleaseTime    = d1200e2(this->get(champ_releaseModEnv, readShort, 0));

            // Valeurs fixes, boucle
            this->rootkey           = -1;
            this->loopStart         = 0;
            this->loopEnd           = 0;
            this->sampleStart       = 0;
            this->sampleEnd         = 0;
            this->keynum            = -1;
            this->scaleTune         = this->get(champ_scaleTuning, readShort, 0);
            this->fixedVelocity     = -1;
            this->loopMode          = -1;

            // LFOs
            this->modLfoDelay       = d1200e2(this->get(champ_delayModLFO, readShort, 0));
            this->modLfoFreq        = d1200e2(this->get(champ_freqModLFO, readShort, 0));
            this->vibLfoDelay       = d1200e2(this->get(champ_delayVibLFO, readShort, 0));
            this->vibLfoFreq        = d1200e2(this->get(champ_freqVibLFO, readShort, 0));
        }
    }

    // Addition avec voiceParamTmp si présent
    if (voiceParamTmp)
        this->add(voiceParamTmp);

    // Limites
    if (id.typeElement == elementSmpl)
    {
        this->fineTune          = limit(this->fineTune,     -99,    99);
        this->coarseTune        = limit(this->coarseTune,   -120,   120);
        this->pan               = limit(this->pan,          -50.,   50.);
        this->attenuation       = limit(this->attenuation,  0.,     144.);
        this->scaleTune         = limit(this->scaleTune,    0,      1200);
        this->filterFreq        = limit(this->filterFreq,   18.,    19914.);
        this->filterQ           = limit(this->filterQ,      0.,     96.);
        this->volDelayTime      = limit(this->volDelayTime, 0.001, 18.);
        this->volAttackTime     = limit(this->volAttackTime, 0.001, 101.6);
        this->volHoldTime       = limit(this->volHoldTime, 0.001, 18.);
        this->volDecayTime      = limit(this->volDecayTime, 0.001, 101.6);
        this->volSustainLevel   = limit(this->volSustainLevel, 0., 144.);
        this->volReleaseTime    = limit(this->volReleaseTime, 0.001, 101.6);
        this->volKeynumToHold   = limit(this->volKeynumToHold, -1200, 1200);
        this->volKeynumToDecay  = limit(this->volKeynumToDecay, -1200, 1200);
        this->modDelayTime      = limit(this->modDelayTime, 0.001, 18.);
        this->modAttackTime     = limit(this->modAttackTime, 0.001, 101.6);
        this->modHoldTime       = limit(this->modHoldTime, 0.001, 18.);
        this->modDecayTime      = limit(this->modDecayTime, 0.001, 101.6);
        this->modSustainLevel   = limit(this->modSustainLevel, 0., 144.);
        this->modReleaseTime    = limit(this->modReleaseTime, 0.001, 101.6);
        this->modKeynumToHold   = limit(this->modKeynumToHold, -1200, 1200);
        this->modKeynumToDecay  = limit(this->modKeynumToDecay, -1200, 1200);
        this->modEnvToPitch     = limit(this->modEnvToPitch, -12000, 12000);
        this->modEnvToFilterFc  = limit(this->modEnvToFilterFc, -12000, 12000);
        this->modLfoDelay       = limit(this->modLfoDelay, 0.001, 20.);
        this->modLfoFreq        = limit(this->modLfoFreq, 0.001, 100.);
        this->vibLfoDelay       = limit(this->vibLfoDelay, 0.001, 20.);
        this->vibLfoFreq        = limit(this->vibLfoFreq, 0.001, 100.);
        this->modLfoToPitch     = limit(this->modLfoToPitch, -12000, 12000);
        this->modLfoToFilterFreq= limit(this->modLfoToFilterFreq, -12000, 12000);
        this->modLfoToVolume    = limit(this->modLfoToVolume, -96., 96.);
        this->vibLfoToPitch     = limit(this->vibLfoToPitch, -12000, 12000);
        this->reverb            = limit(this->reverb, 0., 100.);
        this->chorus            = limit(this->chorus, 0., 100.);
    }
}

// Remplissage des paramètres
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
}

qint32 VoiceParam::get(Champ champ, ReadMethod readMethod, qint32 defaultValue)
{
    EltID id;
    if (m_sf2->isSet(m_id, champ))
        id = m_id;
    else if (m_sf2->isSet(m_idGlobal, champ))
        id = m_idGlobal;
    else
        return defaultValue;
    switch (readMethod)
    {
    case readChar:  return m_sf2->get(id, champ).cValue;
    case readByte:  return m_sf2->get(id, champ).bValue;
    case readShort: return m_sf2->get(id, champ).shValue;
    case readWord:  return m_sf2->get(id, champ).wValue;
    }
    return 0;
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

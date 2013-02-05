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
VoiceParam::VoiceParam(Pile_sf2 *sf2, EltID id, VoiceParam *voiceParamTmp) : m_sf2(sf2)
{
    // Chargement des données
    if (id.typeElement == elementSmpl)
    {
        m_id = id;
        m_idGlobal = id;

        this->pan               = 0;
        this->attenuation       = 0;
        this->filterFreq        = 0;
        this->filterQ           = 0;
        // Enveloppe volume
        this->volDelayTime      = 0;
        this->volAttackTime     = 0;
        this->volHoldTime       = 0;
        this->volDecayTime      = 0;
        this->volSustainLevel   = 0;
        this->volReleaseTime    = 0;
        this->volKeynumToHold   = 0;
        this->volKeynumToDecay  = 0;
        // Enveloppe modulation
        this->modDelayTime      = 0;
        this->modAttackTime     = 0;
        this->modHoldTime       = 0;
        this->modDecayTime      = 0;
        this->modSustainLevel   = 0;
        this->modReleaseTime    = 0;
        this->modKeynumToHold   = 0;
        this->modKeynumToDecay  = 0;
        this->modEnvToPitch     = 0;
        this->modEnvToFilterFc  = 0;
        // Note et boucle
        this->fineTune          = m_sf2->get(m_id, champ_chPitchCorrection).cValue;
        this->coarseTune        = 0;
        this->scaleTune         = 0;
        this->keynum            = -1;
        this->rootkey           = m_sf2->get(m_id, champ_byOriginalPitch).bValue;
        this->loopStart         = m_sf2->get(m_id, champ_dwStartLoop).dwValue;
        this->loopEnd           = m_sf2->get(m_id, champ_dwEndLoop).dwValue;
        this->loopMode          = 0;
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

        // Paramètres communs instruments et presets
        this->fineTune          = this->get(champ_fineTune, readShort, 0);
        this->coarseTune        = this->get(champ_coarseTune, readShort, 0);
        this->scaleTune         = this->get(champ_scaleTuning, readWord, 0);
        this->pan               = (double)this->get(champ_pan, readShort, 0) / 10;
        this->attenuation       = (double)this->get(champ_initialAttenuation, readWord, 0) / 10;
        this->filterFreq        = this->get(champ_initialFilterFc, readWord, 13500);
        this->filterQ           = this->get(champ_initialFilterQ, readWord, 0);
        // Enveloppe volume
        this->volDelayTime      = d1200e2(this->get(champ_delayVolEnv, readShort, -12000));
        this->volAttackTime     = d1200e2(this->get(champ_attackVolEnv, readShort, -12000));
        this->volHoldTime       = d1200e2(this->get(champ_holdVolEnv, readShort, -12000));
        this->volDecayTime      = d1200e2(this->get(champ_decayVolEnv, readShort, -12000));
        this->volSustainLevel   = (double)this->get(champ_sustainVolEnv, readWord, 0) / 10;
        this->volReleaseTime    = d1200e2(this->get(champ_releaseVolEnv, readShort, -12000));
        this->volKeynumToHold   = this->get(champ_keynumToVolEnvHold, readShort, 0);
        this->volKeynumToDecay  = this->get(champ_keynumToVolEnvDecay, readShort, 0);
        this->modEnvToPitch     = this->get(champ_modEnvToPitch, readShort, 0);
        this->modEnvToFilterFc  = this->get(champ_modEnvToFilterFc, readShort, 0);
        // Enveloppe modulation
        this->modDelayTime      = d1200e2(this->get(champ_delayModEnv, readShort, -12000));
        this->modAttackTime     = d1200e2(this->get(champ_attackModEnv, readShort, -12000));
        this->modHoldTime       = d1200e2(this->get(champ_holdModEnv, readShort, -12000));
        this->modDecayTime      = d1200e2(this->get(champ_decayModEnv, readShort, -12000));
        this->modSustainLevel   = (double)this->get(champ_sustainModEnv, readWord, 0) / 10;
        this->modReleaseTime    = d1200e2(this->get(champ_releaseModEnv, readShort, -12000));
        this->modKeynumToHold   = this->get(champ_keynumToModEnvHold, readShort, 0);
        this->modKeynumToDecay  = this->get(champ_keynumToModEnvDecay, readShort, 0);
        // Paramétrage spécifique
        if (id.typeElement == elementInstSmpl)
        {
            this->rootkey = this->get(champ_overridingRootKey, readWord, -1);
            this->keynum = this->get(champ_keynum, readWord, -1);
            this->loopStart = this->get(champ_startloopAddrsOffset, readShort, 0) +
                    this->get(champ_startloopAddrsCoarseOffset, readShort, 0) * 32768;
            this->loopEnd = this->get(champ_endloopAddrsOffset, readShort, 0) +
                    this->get(champ_endloopAddrsCoarseOffset, readShort, 0) * 32768;
            this->loopMode = this->get(champ_sampleModes, readWord, 0);
        }
        else
        {
            this->rootkey = -1;
            this->loopStart = 0;
            this->loopEnd = 0;
            this->keynum = -1;
            this->loopMode = -1;
        }
    }

    // Addition avec voiceParamTmp si présent
    if (voiceParamTmp)
        this->add(voiceParamTmp);
}

// Remplissage des paramètres
void VoiceParam::add(VoiceParam * voiceParamTmp)
{
    this->fineTune          += voiceParamTmp->fineTune;
    this->coarseTune        += voiceParamTmp->coarseTune;
    this->pan               += voiceParamTmp->pan;
    this->attenuation       += voiceParamTmp->attenuation;
    this->scaleTune         += voiceParamTmp->scaleTune;
    this->filterFreq        += voiceParamTmp->filterFreq;
    this->filterQ           += voiceParamTmp->filterQ;
    // Enveloppe volume
    this->volDelayTime      += voiceParamTmp->volDelayTime;
    this->volAttackTime     += voiceParamTmp->volAttackTime;
    this->volHoldTime       += voiceParamTmp->volHoldTime;
    this->volDecayTime      += voiceParamTmp->volDecayTime;
    this->volSustainLevel   += voiceParamTmp->volSustainLevel;
    this->volReleaseTime    += voiceParamTmp->volReleaseTime;
    this->volKeynumToHold   += voiceParamTmp->volKeynumToHold;
    this->volKeynumToDecay  += voiceParamTmp->volKeynumToDecay;
    // Enveloppe modulation
    this->modDelayTime      += voiceParamTmp->modDelayTime;
    this->modAttackTime     += voiceParamTmp->modAttackTime;
    this->modHoldTime       += voiceParamTmp->modHoldTime;
    this->modDecayTime      += voiceParamTmp->modDecayTime;
    this->modSustainLevel   += voiceParamTmp->modSustainLevel;
    this->modReleaseTime    += voiceParamTmp->modReleaseTime;
    this->modKeynumToHold   += voiceParamTmp->modKeynumToHold;
    this->modKeynumToDecay  += voiceParamTmp->modKeynumToDecay;
    this->modEnvToPitch     += voiceParamTmp->modEnvToPitch;
    this->modEnvToFilterFc  += voiceParamTmp->modEnvToFilterFc;
    //
    if (voiceParamTmp->loopMode != -1)
        this->loopMode = voiceParamTmp->loopMode;
    if (voiceParamTmp->rootkey != -1)
        this->rootkey = voiceParamTmp->rootkey;
    if (voiceParamTmp->keynum != -1)
        this->keynum = voiceParamTmp->keynum;
    this->loopStart         += voiceParamTmp->loopStart;
    this->loopEnd           += voiceParamTmp->loopEnd;
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
    noteJouee += (double)(fineTune) / 100 + coarseTune;
    return noteJouee - this->rootkey;
}

double VoiceParam::d1200e2(qint32 val)
{
    return pow(2, ((double)val / 1200));
}

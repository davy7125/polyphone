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

#ifndef VOICEPARAM_H
#define VOICEPARAM_H

#include "basetypes.h"
#include "soundfontmanager.h"

// Classe regroupant la liste des paramètres de lecture
class VoiceParam
{
public:
    VoiceParam(SoundfontManager * sf2, EltID id, VoiceParam * voiceParamTmp = nullptr);
    ~VoiceParam() {}
    double getPitchDifference(int note);
    // Liste des paramètres de lecture (libre accès)
    // PITCH
    qint32 fineTune, coarseTune, keynum, scaleTune;
    double rootkey;
    // FILTRE
    double filterFreq, filterQ;
    // BOUCLES ET OFFSETS
    int loopMode;
    quint32 loopStart, loopEnd, sampleStart, sampleEnd;
    // ATTENUATION, BALANCE
    double attenuation, pan;
    int fixedVelocity;
    // ENVELOPPE VOLUME
    double volDelayTime, volAttackTime, volHoldTime, volDecayTime, volReleaseTime;
    qint32 volKeynumToHold, volKeynumToDecay;
    double volSustainLevel;
    // ENVELOPPE MODULATION
    double modDelayTime, modAttackTime, modHoldTime, modDecayTime, modReleaseTime;
    qint32 modKeynumToHold, modKeynumToDecay;
    qint32 modEnvToPitch, modEnvToFilterFc;
    double modSustainLevel;
    // LFOs
    double modLfoDelay, modLfoFreq, vibLfoDelay, vibLfoFreq;
    qint32 modLfoToPitch, modLfoToFilterFreq, vibLfoToPitch;
    double modLfoToVolume;
    // EFFETS
    float reverb, chorus;
    // DIVERS
    int exclusiveClass, numPreset;

private:
    SoundfontManager * m_sf2;

    void add(VoiceParam *voiceParamTmp);
    static double d1200e2(qint32 val);

    // Initialisation, lecture
    void init(ElementType type, int numPreset = -1);
    void readSample(EltID id);
    void read(EltID id);

    // Limites
    void limit(EltID id);
    static double limitD(double val, double min, double max);
    static float limitF(float val, float min, float max);
    static qint32 limitI(qint32 val, qint32 min, qint32 max);
    static quint32 limitUI(quint32 val, quint32 min, quint32 max);
};


#endif // VOICEPARAM_H

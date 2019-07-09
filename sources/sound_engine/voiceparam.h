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

// Class gathering all parameters useful to create a sound
// Parameters can evolve in real-time depending on the modulators
class VoiceParam
{
public:
    // Initialize a set of parameters
    VoiceParam(SoundfontManager * sf2, EltID id, VoiceParam * voiceParamTmp = nullptr);

    // Sample reading
    void prepareForSmpl(int key, SFSampleLink link);
    void setPan(double val) { pan = val; }
    void setLoopMode(int val) { loopMode = val; }
    void setLoopStart(quint32 val) { startLoop = val; }
    void setLoopEnd(quint32 val) { endLoop = val; }
    void setFineTune(qint32 val) { fineTune = val; }

    // Destructor
    ~VoiceParam() {}

    // Update parameters before reading them (modulators)
    void updateParameters() {}

    // Get a param
    double getDouble(AttributeType type);
    qint32 getInteger(AttributeType type);
    quint32 getUnsigned(AttributeType type);

private:
    SoundfontManager * _sm;

    // All parameters
    quint32 length;
    quint32 startLoop;
    quint32 endLoop;
    qint32 loopStartOffset, loopEndOffset, sampleStartOffset, sampleEndOffset;
    qint32 loopStartCoarseOffset, loopEndCoarseOffset, sampleStartCoarseOffset, sampleEndCoarseOffset;
    qint32 exclusiveClass, numPreset;
    qint32 fineTune, coarseTune, fixedKey, scaleTune;
    qint32 loopMode;
    qint32 fixedVelocity;
    qint32 volKeynumToHold, volKeynumToDecay;
    qint32 modKeynumToHold, modKeynumToDecay;
    qint32 modEnvToPitch, modEnvToFilterFc;
    qint32 modLfoToPitch, modLfoToFilterFreq, vibLfoToPitch;
    qint32 rootkey;
    double filterFreq, filterQ;
    double attenuation, pan;
    double volSustainLevel, volDelayTime, volAttackTime, volHoldTime, volDecayTime, volReleaseTime;
    double modSustainLevel, modDelayTime, modAttackTime, modHoldTime, modDecayTime, modReleaseTime;
    double modLfoDelay, modLfoFreq, vibLfoDelay, vibLfoFreq;
    double modLfoToVolume;
    double reverb, chorus;

    void add(VoiceParam *voiceParamTmp);
    static double d1200e2(qint32 val);

    // Initialisation, lecture
    void init(ElementType type, int numPreset = -1);
    void readSample(EltID id);
    void read(EltID id);

    // Limites
    void limit();
    static double limitD(double val, double min, double max);
    static qint32 limitI(qint32 val, qint32 min, qint32 max);
    static quint32 limitUI(quint32 val, quint32 min, quint32 max);
};

#endif // VOICEPARAM_H

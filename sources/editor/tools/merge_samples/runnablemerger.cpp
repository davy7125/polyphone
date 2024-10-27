/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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

#include "runnablemerger.h"
#include "toolmergesamples.h"
#include "soundfontmanager.h"
#include "sampleutils.h"
#include "soundfonts.h"
#include "soundfont.h"
#include "instprst.h"
#include "voice.h"

quint32 RunnableMerger::SAMPLE_RATE = 48000;
quint32 RunnableMerger::BUFFER_LENGTH = 1024;


RunnableMerger::RunnableMerger(ToolMergeSamples * tool, EltID idPrst, quint32 key, quint32 minKey, bool loop, bool stereo, quint32 sustainDuration, quint32 releaseDuration) : QRunnable(),
    _tool(tool),
    _idPrst(idPrst),
    _key(key),
    _minKey(minKey),
    _loop(loop),
    _stereo(stereo),
    _sustainDuration(sustainDuration),
    _releaseDuration(releaseDuration)
{}

RunnableMerger::~RunnableMerger()
{

}

void RunnableMerger::run()
{
    SoundfontManager * sm = SoundfontManager::getInstance();

    // Vectors that will contain the sound
    int sampleLength = (_sustainDuration + _releaseDuration) * SAMPLE_RATE;
    QVector<float> dataR(sampleLength, 0.0f);
    QVector<float> dataL(sampleLength, 0.0f);

    // Browse the preset
    Soundfont * soundfont = sm->getSoundfonts()->getSoundfont(_idPrst.indexSf2);
    InstPrst * prst = soundfont != nullptr ? soundfont->getPreset(_idPrst.indexElt) : nullptr;
    if (prst != nullptr)
        playPrst(dataR.data(), dataL.data(), soundfont, prst);

    // Loop sample if needed
    quint32 loopStart = 0;
    quint32 loopEnd = 0;
    if (_loop)
    {
        quint32 crossfadeLength;
        bool result = SampleUtils::loopStep1(dataR, SAMPLE_RATE, loopStart, loopEnd, crossfadeLength);
        if (result)
        {
            dataR = SampleUtils::loopStep2(dataR, loopStart, loopEnd, crossfadeLength);
            dataL = SampleUtils::loopStep2(dataL, loopStart, loopEnd, crossfadeLength);
        }
    }

    // Create a new sample
    EltID idSmplR(elementSmpl, _idPrst.indexSf2);
    idSmplR.indexElt = sm->add(idSmplR);

    // Add data
    sm->set(idSmplR, dataR);

    // Configuration
    AttributeValue value;
    value.dwValue = static_cast<quint32>(dataR.length());
    sm->set(idSmplR, champ_dwLength, value);
    value.dwValue = SAMPLE_RATE;
    sm->set(idSmplR, champ_dwSampleRate, value);
    value.wValue = static_cast<quint16>(_key);
    sm->set(idSmplR, champ_byOriginalPitch, value);
    value.cValue = 0;
    sm->set(idSmplR, champ_chPitchCorrection, value);
    value.dwValue = loopStart;
    sm->set(idSmplR, champ_dwStartLoop, value);
    value.dwValue = loopEnd;
    sm->set(idSmplR, champ_dwEndLoop, value);
    value.sfLinkValue = _stereo ? rightSample : monoSample;
    sm->set(idSmplR, champ_sfSampleType, value);

    // Left part?
    EltID idSmplL(elementUnknown, _idPrst.indexSf2);
    if (_stereo)
    {
        idSmplL.typeElement = elementSmpl;
        idSmplL.indexElt = sm->add(idSmplL);

        // Add data
        sm->set(idSmplL, dataL);

        // Configuration
        AttributeValue value;
        value.dwValue = static_cast<quint32>(dataL.length());
        sm->set(idSmplL, champ_dwLength, value);
        value.dwValue = SAMPLE_RATE;
        sm->set(idSmplL, champ_dwSampleRate, value);
        value.wValue = static_cast<quint16>(_key);
        sm->set(idSmplL, champ_byOriginalPitch, value);
        value.cValue = 0;
        sm->set(idSmplL, champ_chPitchCorrection, value);
        value.dwValue = loopStart;
        sm->set(idSmplL, champ_dwStartLoop, value);
        value.dwValue = loopEnd;
        sm->set(idSmplL, champ_dwEndLoop, value);
        value.sfLinkValue = leftSample;
        sm->set(idSmplL, champ_sfSampleType, value);
    }

    emit _tool->elementProcessed(idSmplR, idSmplL, _key, _minKey);
}

void RunnableMerger::playPrst(float * dataR, float * dataL, Soundfont * soundfont, InstPrst * prst)
{
    // Default preset range
    RangesType defaultKeyRange;
    if (prst->getGlobalDivision()->isSet(champ_keyRange))
        defaultKeyRange = prst->getGlobalDivision()->getGen(champ_keyRange).rValue;
    else
    {
        defaultKeyRange.byLo = 0;
        defaultKeyRange.byHi = 127;
    }

    // Browse the ranges of all linked instruments
    QVector<Division *> divisions = prst->getDivisions().values();
    for (int i = 0; i < divisions.count(); ++i)
    {
        // Skip hidden or muted divisions
        Division * prstDiv = divisions[i];
        if (prstDiv->isHidden() || prstDiv->isMute())
            continue;

        // Check that {key} is in the division and go inside the instruments
        RangesType currentKeyRange = prstDiv->isSet(champ_keyRange) ? prstDiv->getGen(champ_keyRange).rValue : defaultKeyRange;
        if (currentKeyRange.byLo <= _key && _key <= currentKeyRange.byHi)
        {
            InstPrst * inst = soundfont->getInstrument(prstDiv->getGen(champ_instrument).wValue);
            if (inst != nullptr)
                playInst(dataR, dataL, soundfont, prst, prstDiv, inst);
        }
    }
}

void RunnableMerger::playInst(float * dataR, float * dataL, Soundfont * soundfont, InstPrst * prst, Division * prstDiv, InstPrst * inst)
{
    // Default instrument range
    RangesType defaultKeyRange;
    if (inst->getGlobalDivision()->isSet(champ_keyRange))
        defaultKeyRange = inst->getGlobalDivision()->getGen(champ_keyRange).rValue;
    else
    {
        defaultKeyRange.byLo = 0;
        defaultKeyRange.byHi = 127;
    }

    // Browse the range of all linked samples
    QVector<Division *> divisions = inst->getDivisions().values();
    for (int i = 0; i < divisions.count(); ++i)
    {
        // Skip hidden or muted divisions
        Division * instDiv = divisions[i];
        if (instDiv->isHidden() || instDiv->isMute())
            continue;

        // Check that {key} is in the division and go inside the samples
        RangesType currentKeyRange = instDiv->isSet(champ_keyRange) ? instDiv->getGen(champ_keyRange).rValue : defaultKeyRange;
        if (currentKeyRange.byLo <= _key && _key <= currentKeyRange.byHi)
        {
            Smpl * smpl = soundfont->getSample(instDiv->getGen(champ_sampleID).wValue);
            if (smpl != nullptr)
                this->playSmpl(dataR, dataL, prst, prstDiv, inst, instDiv, smpl);
        }
    }
}

void RunnableMerger::playSmpl(float * dataR, float * dataL, InstPrst * prst, Division * prstDiv, InstPrst * inst, Division * instDiv, Smpl * smpl)
{
    // Initialize a voice
    VoiceInitializer voiceInitializer;
    voiceInitializer.prst = prst;
    voiceInitializer.prstDiv = prstDiv;
    voiceInitializer.inst = inst;
    voiceInitializer.instDiv = instDiv;
    voiceInitializer.smpl = smpl;
    voiceInitializer.channel = 0;
    voiceInitializer.key = _key;
    voiceInitializer.vel = 127;
    voiceInitializer.audioSmplRate = SAMPLE_RATE;
    voiceInitializer.token = 0;
    voiceInitializer.type = 0;

    Voice * voice = new Voice();
    voice->initialize(&voiceInitializer);

    // Buffers
    float * bufferR = new float[BUFFER_LENGTH];
    float * bufferL = new float[BUFFER_LENGTH];

    // Get the sustained data
    quint32 sustainLength = _sustainDuration * SAMPLE_RATE;
    quint32 computedLength = 0;
    quint32 len;
    while (computedLength < sustainLength)
    {
        // Length to compute
        len = sustainLength - computedLength;
        if (len > BUFFER_LENGTH)
            len = BUFFER_LENGTH;

        // Compute data in the sustain phase
        voice->generateData(bufferL, bufferR, len);

        // Merge
        for (quint32 i = 0; i < len; i++)
        {
            dataR[computedLength + i] += bufferR[i];
            dataL[computedLength + i] += bufferL[i];
        }

        computedLength += len;
    }

    // Get the released data
    voice->release();
    quint32 releaseLength = _releaseDuration * SAMPLE_RATE;
    computedLength = 0;
    while (computedLength < releaseLength)
    {
        // Length to compute
        len = releaseLength - computedLength;
        if (len > BUFFER_LENGTH)
            len = BUFFER_LENGTH;

        // Compute data in the release phase
        voice->generateData(bufferL, bufferR, len);

        // Merge
        for (quint32 i = 0; i < len; i++)
        {
            dataR[sustainLength + computedLength + i] += bufferR[i];
            dataL[sustainLength + computedLength + i] += bufferL[i];
        }

        computedLength += len;
    }

    delete [] bufferR;
    delete [] bufferL;
    delete voice;
}

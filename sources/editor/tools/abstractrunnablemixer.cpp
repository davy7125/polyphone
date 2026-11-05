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

#include "abstractrunnablemixer.h"
#include "soundfontmanager.h"
#include "sampleutils.h"

const double AbstractRunnableMixer::SAMPLE_DURATION = 7.0;
const quint32 AbstractRunnableMixer::SAMPLE_RATE = 48000;

AbstractRunnableMixer::AbstractRunnableMixer(EltID idInst, bool loop, bool stereo, int side) : QRunnable(),
    _idInst(idInst),
    _loop(loop),
    _stereo(stereo),
    _side(side)
{}

double AbstractRunnableMixer::getMinimumAttenuation(const QList<double> &keys)
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    double attMini = 1000000;
    foreach (double key, keys)
    {
        double ecart;
        EltID idInstSmpl;
        closestSample(_idInst, key, ecart, _side, idInstSmpl);
        double attenuation = 0;
        if (sm->isSet(idInstSmpl, champ_initialAttenuation))
            attenuation = 0.04 * sm->get(idInstSmpl, champ_initialAttenuation).shValue;
        if (attenuation < attMini)
            attMini = attenuation;
    }
    return attMini;
}

void AbstractRunnableMixer::addKey(QVector<float> &data, double key, double minAttenuation, double extraAttenuation, double divisor)
{
    // Closest sample and difference of pitch
    SoundfontManager * sm = SoundfontManager::getInstance();
    double ecart = 0;
    EltID idInstSmpl;
    EltID idSmpl = closestSample(_idInst, key, ecart, _side, idInstSmpl);

    // Fictitious initial sample rate (tuning)
    double initialSampleRate = pow(2, ecart / 12.0) * sm->get(idSmpl, champ_dwSampleRate).dwValue;

    // Sound, the loop being repeated
    QVector<float> vData = SampleUtils::readWithLoop(sm->getDataFloat(idSmpl), sm->get(idSmpl, champ_dwStartLoop).dwValue,
                                                     sm->get(idSmpl, champ_dwEndLoop).dwValue,
                                                     static_cast<quint32>(initialSampleRate * SAMPLE_DURATION));

    // Attenuation in dB
    double gain = 1;
    if (sm->isSet(idInstSmpl, champ_initialAttenuation))
    {
        gain = 0.04 * sm->get(idInstSmpl, champ_initialAttenuation).shValue - minAttenuation + extraAttenuation;
        gain = pow(10, -gain / 20.0);
    }
    gain /= divisor;

    // Resampling and mix
    vData = SampleUtils::resampleMono(vData, initialSampleRate, SAMPLE_RATE);
    SampleUtils::add(data, vData, gain);
}

EltID AbstractRunnableMixer::createSample(QVector<float> data, int rootKey)
{
    // Loop sample if needed
    quint32 loopStart = 0;
    quint32 loopEnd = 0;
    if (_loop)
    {
        quint32 crossfadeLength;
        bool result = SampleUtils::loopStep1(data, SAMPLE_RATE, loopStart, loopEnd, crossfadeLength);
        if (result)
            data = SampleUtils::loopStep2(std::move(data), loopStart, loopEnd, crossfadeLength, true);
    }

    // New sample
    SoundfontManager * sm = SoundfontManager::getInstance();
    EltID idSmpl(elementSmpl, _idInst.indexSf2);
    idSmpl.indexElt = sm->add(idSmpl);
    sm->setData(idSmpl, data);

    // Configuration
    AttributeValue value;
    value.dwValue = static_cast<quint32>(data.length());
    sm->set(idSmpl, champ_dwLength, value);
    value.dwValue = SAMPLE_RATE;
    sm->set(idSmpl, champ_dwSampleRate, value);
    value.wValue = static_cast<quint16>(rootKey);
    sm->set(idSmpl, champ_byOriginalPitch, value);
    value.cValue = 0;
    sm->set(idSmpl, champ_chPitchCorrection, value);
    value.dwValue = loopStart;
    sm->set(idSmpl, champ_dwStartLoop, value);
    value.dwValue = loopEnd;
    sm->set(idSmpl, champ_dwEndLoop, value);

    // Link
    if (_stereo)
    {
        if (_side == 0)
            value.sfLinkValue = rightSample;
        else
            value.sfLinkValue = leftSample;
    }
    else
        value.sfLinkValue = monoSample;
    sm->set(idSmpl, champ_sfSampleType, value);

    return idSmpl;
}

EltID AbstractRunnableMixer::closestSample(EltID idInst, double pitch, double &ecart, int cote, EltID &idInstSmpl)
{
    // Recherche du sample le plus proche de pitch dans l'instrument idInst
    SoundfontManager * sm = SoundfontManager::getInstance();
    double ecart_min_abs = 1000;
    EltID idInstSmplTmp = idInst;
    idInstSmplTmp.typeElement = elementInstSmpl;
    EltID idSmpl = idInst;
    idSmpl.indexElt = -1;
    idSmpl.typeElement = elementSmpl;
    EltID idSmplRet = idSmpl;
    foreach (int i, sm->getSiblings(idInstSmplTmp))
    {
        idInstSmplTmp.indexElt2 = i;

        // Hauteur du sample
        idSmpl.indexElt = sm->get(idInstSmplTmp, champ_sampleID).wValue;
        double pitchSmpl = sm->get(idSmpl, champ_byOriginalPitch).bValue
                - 0.01 * sm->get(idSmpl, champ_chPitchCorrection).cValue;

        // Mesure de l'écart
        double ecartTmp = pitchSmpl - pitch;
        double absEcart;
        if (ecartTmp < 0) absEcart = -3 * ecartTmp;
        else absEcart = ecartTmp;
        if (absEcart < ecart_min_abs)
        {
            ecart_min_abs = absEcart;
            ecart = -ecartTmp;
            idSmplRet = idSmpl;
            idInstSmpl = idInstSmplTmp;
        }
    }

    // Type de sample
    int indexEltBase = idSmplRet.indexElt;
    SFSampleLink type = sm->get(idSmplRet, champ_sfSampleType).sfLinkValue;
    if (!(type == RomMonoSample || type == monoSample ||
          ((type == RomRightSample || type == rightSample || type == RomLinkedSample || type == linkedSample) && cote == 0) ||
          ((type == RomLeftSample || type == leftSample) && cote == 1)))
        idSmplRet.indexElt = sm->get(idSmplRet, champ_wSampleLink).wValue;
    double ecartMin = 1000;
    double ecartTmp;
    int rootKeySmpl = sm->get(idSmplRet, champ_byOriginalPitch).bValue;

    // Recherche de l'instSmpl le plus proche de pitch, ayant comme sample_ID idSmplRet
    foreach (int i, sm->getSiblings(idInstSmplTmp))
    {
        idInstSmplTmp.indexElt2 = i;

        if (sm->get(idInstSmplTmp, champ_sampleID).wValue == idSmplRet.indexElt)
        {
            // Notes min et max pour lesquels le sample est joué
            int noteMin = sm->get(idInstSmplTmp, champ_keyRange).rValue.byLo;
            int noteMax = sm->get(idInstSmplTmp, champ_keyRange).rValue.byHi;
            // Ajustement
            int rootKeyInstSmpl = rootKeySmpl;
            if (sm->isSet(idInstSmplTmp, champ_overridingRootKey))
                rootKeyInstSmpl = sm->get(idInstSmplTmp, champ_overridingRootKey).wValue;
            noteMin += rootKeySmpl - rootKeyInstSmpl;
            noteMax += rootKeySmpl - rootKeyInstSmpl;
            // Mesure de l'écart
            if (pitch < noteMin)
                ecartTmp = noteMin - pitch;
            else if (pitch > noteMax)
                ecartTmp = pitch - noteMax;
            else
                ecartTmp = 0;
            if (ecartTmp < ecartMin)
            {
                ecartMin = ecartTmp;
                idInstSmpl = idInstSmplTmp;
            }
        }
    }

    if (ecartMin > 900 && idSmplRet.indexElt != indexEltBase)
    {
        // Le sample associé n'a pas été trouvé, retour sur le sample de base
        idSmplRet.indexElt = indexEltBase;
        rootKeySmpl = sm->get(idSmplRet, champ_byOriginalPitch).bValue;
        foreach (int i, sm->getSiblings(idInstSmplTmp))
        {
            idInstSmplTmp.indexElt2 = i;

            if (sm->get(idInstSmplTmp, champ_sampleID).wValue == idSmplRet.indexElt)
            {
                // Notes min et max pour lesquels le sample est joué
                int noteMin = sm->get(idInstSmplTmp, champ_keyRange).rValue.byLo;
                int noteMax = sm->get(idInstSmplTmp, champ_keyRange).rValue.byHi;

                // Ajustement
                int rootKeyInstSmpl = rootKeySmpl;
                if (sm->isSet(idInstSmplTmp, champ_overridingRootKey))
                    rootKeyInstSmpl = sm->get(idInstSmplTmp, champ_overridingRootKey).wValue;
                noteMin += rootKeySmpl - rootKeyInstSmpl;
                noteMax += rootKeySmpl - rootKeyInstSmpl;

                // Mesure de l'écart
                if (pitch < noteMin)
                    ecartTmp = noteMin - pitch;
                else if (pitch > noteMax)
                    ecartTmp = pitch - noteMax;
                else
                    ecartTmp = 0;
                if (ecartTmp < ecartMin)
                {
                    ecartMin = ecartTmp;
                    idInstSmpl = idInstSmplTmp;
                }
            }
        }
    }

    return idSmplRet;
}

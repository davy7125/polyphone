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

#include "runnablesamplecreator.h"
#include "toolmixturecreation.h"
#include "soundfontmanager.h"
#include "sampleutils.h"

double RunnableSampleCreator::SAMPLE_DURATION = 7.0;
quint32 RunnableSampleCreator::SAMPLE_RATE = 48000;

RunnableSampleCreator::RunnableSampleCreator(ToolMixtureCreation * tool, EltID idInst, DivisionInfo di, int key, int minKey, bool loop, bool stereo, int side) : QRunnable(),
    _tool(tool),
    _idInst(idInst),
    _di(di),
    _key(key),
    _minKey(minKey),
    _loop(loop),
    _stereo(stereo),
    _side(side)
{}

RunnableSampleCreator::~RunnableSampleCreator()
{
}

void RunnableSampleCreator::run()
{
    // Data initialization
    SoundfontManager * sm = SoundfontManager::getInstance();
    QVector<float> vData(SAMPLE_DURATION * SAMPLE_RATE, 0.0f);

    // Minimum attenuation for all ranks
    double attMini = 1000000;
    foreach (RankInfo ri, _di.getRanks())
    {
        double noteTmp = static_cast<double>(_key) + ri.getOffset();
        double ecart;
        EltID idInstSmplTmp;
        closestSample(_idInst, noteTmp, ecart, _side, idInstSmplTmp);
        double attenuation = 0;
        if (sm->isSet(idInstSmplTmp, champ_initialAttenuation))
            attenuation = 0.04 * sm->get(idInstSmplTmp, champ_initialAttenuation).shValue;
        if (attenuation < attMini)
            attMini = attenuation;
    }

    // For each rank
    foreach (RankInfo ri, _di.getRanks())
    {
        // Calcul de la note à ajouter à la mixture
        double noteTmp = static_cast<double>(_key) + ri.getOffset();
        if (noteTmp <= 120)
        {
            // Sample le plus proche et écart associé
            double ecart;
            EltID idInstSmplTmp;
            EltID idSmpl = closestSample(_idInst, noteTmp, ecart, _side, idInstSmplTmp);

            // Fréquence d'échantillonnage initiale fictive (pour accordage)
            double fEchInit = pow(2, ecart / 12.0) * sm->get(idSmpl, champ_dwSampleRate).dwValue;

            // Récupération du son
            QVector<float> vDataTmp = getSampleData(idSmpl, static_cast<quint32>(SAMPLE_DURATION * fEchInit));

            // Prise en compte atténuation en dB
            double attenuation = 1;
            if (sm->isSet(idInstSmplTmp, champ_initialAttenuation))
            {
                attenuation = 0.04 * sm->get(idInstSmplTmp, champ_initialAttenuation).shValue - attMini;
                attenuation = pow(10, -attenuation / 20.0);
            }

            // Rééchantillonnage
            vDataTmp = SampleUtils::resampleMono(vDataTmp, fEchInit, SAMPLE_RATE);

            // Ajout du son
            addSampleData(vData, vDataTmp, attenuation);
        }
    }

    // Loop sample if needed
    quint32 loopStart = 0;
    quint32 loopEnd = 0;
    if (_loop)
    {
        quint32 crossfadeLength;
        bool result = SampleUtils::loopStep1(vData, SAMPLE_RATE, loopStart, loopEnd, crossfadeLength);
        if (result)
            vData = SampleUtils::loopStep2(vData, loopStart, loopEnd, crossfadeLength, true);
    }

    // Création d'un nouveau sample
    EltID idSmpl(elementSmpl, _idInst.indexSf2);
    idSmpl.indexElt = sm->add(idSmpl);

    // Ajout des données
    sm->set(idSmpl, vData);

    // Configuration
    AttributeValue value;
    value.dwValue = static_cast<quint32>(vData.length());
    sm->set(idSmpl, champ_dwLength, value);
    value.dwValue = SAMPLE_RATE;
    sm->set(idSmpl, champ_dwSampleRate, value);
    value.wValue = static_cast<quint16>(_key);
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

    _tool->elementProcessed(idSmpl, _key, _minKey, attMini);
}

EltID RunnableSampleCreator::closestSample(EltID idInst, double pitch, double &ecart, int cote, EltID &idInstSmpl)
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

QVector<float> RunnableSampleCreator::getSampleData(EltID idSmpl, quint32 nbRead)
{
    // Récupération de données provenant d'un sample, en prenant en compte la boucle
    SoundfontManager * sm = SoundfontManager::getInstance();
    QVector<float> vData = sm->getData(idSmpl);
    quint32 loopStart = sm->get(idSmpl, champ_dwStartLoop).dwValue;
    quint32 loopEnd = sm->get(idSmpl, champ_dwEndLoop).dwValue;
    QVector<float> vDataRet(nbRead);
    quint32 posInit = 0;
    const float * data = vData.constData();
    float * dataRet = vDataRet.data();
    if (loopStart != loopEnd)
    {
        // Boucle
        quint32 total = 0;
        while (total < nbRead)
        {
            const quint32 chunk = qMin(loopEnd - posInit, nbRead - total);
            memcpy(&dataRet[total], &data[posInit], chunk * sizeof(float));
            posInit += chunk;
            if (posInit >= loopEnd)
                posInit = loopStart;
            total += chunk;
        }
    }
    else
    {
        // No loop
        if (static_cast<quint32>(vData.size()) < nbRead)
        {
            vDataRet.fill(0);
            memcpy(dataRet, data, vData.size() * sizeof(float));
        }
        else
            memcpy(dataRet, data, nbRead * sizeof(float));
    }
    return vDataRet;
}

void RunnableSampleCreator::addSampleData(QVector<float> &vData1, QVector<float> &vData2, double mult)
{
    // Add vData2 multiplied by mult in vData1
    float * data1 = vData1.data();
    const float * data2 = vData2.constData();
    for (int i = 0; i < qMin(vData1.size(), vData2.size()); i++)
        data1[i] += mult * data2[i];
}

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

#include "runnablechordcreator.h"
#include "toolchords.h"
#include "soundfontmanager.h"
#include "sampleutils.h"

double RunnableChordCreator::SAMPLE_DURATION = 7.0;
int RunnableChordCreator::SAMPLE_RATE = 48000;

RunnableChordCreator::RunnableChordCreator(ToolChords * tool, EltID idInst, ChordInfo ci, int key, int minKey, bool loop, bool stereo, int side) : QRunnable(),
    _tool(tool),
    _idInst(idInst),
    _ci(ci),
    _key(key),
    _minKey(minKey),
    _loop(loop),
    _stereo(stereo),
    _side(side)
{}

RunnableChordCreator::~RunnableChordCreator()
{

}

void RunnableChordCreator::run()
{
    // Data initialization
    SoundfontManager * sm = SoundfontManager::getInstance();
    QByteArray baData;
    baData.resize(SAMPLE_DURATION * SAMPLE_RATE * 4);
    baData.fill(0);

    // Get the different pitches with the corresponding attenuation
    QMap<int, int> pitches = getChordKeys(_key, _ci);


    // Minimum attenuation for all ranks
    double attMini = 1000000;
    foreach (int pitch, pitches.keys())
    {
        double ecart;
        EltID idInstSmplTmp;
        closestSample(_idInst, pitch, ecart, _side, idInstSmplTmp);
        double attenuation = 0;
        if (sm->isSet(idInstSmplTmp, champ_initialAttenuation))
            attenuation = (double)sm->get(idInstSmplTmp, champ_initialAttenuation).shValue / 10.0;
        if (attenuation < attMini)
            attMini = attenuation;
    }

    // For each part
    foreach (int pitch, pitches.keys())
    {
        if (pitch <= 120)
        {
            // Sample le plus proche et écart associé
            double ecart;
            EltID idInstSmplTmp;
            EltID idSmpl = closestSample(_idInst, pitch, ecart, _side, idInstSmplTmp);

            // Fréquence d'échantillonnage initiale fictive (pour accordage)
            double fEchInit = (double)sm->get(idSmpl, champ_dwSampleRate).dwValue * pow(2, ecart / 12.0);

            // Récupération du son
            QByteArray baDataTmp = getSampleData(idSmpl, SAMPLE_DURATION * fEchInit);

            // Prise en compte atténuation en dB
            double attenuation = 1;
            if (sm->isSet(idInstSmplTmp, champ_initialAttenuation))
            {
                attenuation = (double)sm->get(idInstSmplTmp, champ_initialAttenuation).shValue / 10.0 - attMini + (double)pitches[pitch] / 10.;
                attenuation = pow(10, -attenuation / 20.0);
            }
            attenuation /= pitches.count();

            // Rééchantillonnage
            baDataTmp = SampleUtils::resampleMono(baDataTmp, fEchInit, SAMPLE_RATE, 32);

            // Ajout du son
            addSampleData(baData, baDataTmp, attenuation);
        }
    }

    // Loop sample if needed
    qint32 loopStart = 0;
    qint32 loopEnd = 0;
    if (_loop)
    {
        QByteArray baData2 = SampleUtils::bouclage(baData, SAMPLE_RATE, loopStart, loopEnd, 32);
        if (!baData2.isEmpty())
            baData = baData2;
    }

    // Création d'un nouveau sample
    EltID idSmpl(elementSmpl, _idInst.indexSf2);
    idSmpl.indexElt = sm->add(idSmpl);

    // Ajout des données
    sm->set(idSmpl, champ_sampleData16, SampleUtils::bpsConversion(baData, 32, 16));
    EltID idSf2 = idSmpl;
    idSf2.typeElement = elementSf2;
    if (sm->get(idSf2, champ_wBpsSave).wValue == 24)
        sm->set(idSmpl, champ_sampleData24, SampleUtils::bpsConversion(baData, 32, 824));

    // Configuration
    AttributeValue value;
    value.dwValue = baData.length() / 4;
    sm->set(idSmpl, champ_dwLength, value);
    value.dwValue = SAMPLE_RATE;
    sm->set(idSmpl, champ_dwSampleRate, value);
    value.wValue = _key;
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

EltID RunnableChordCreator::closestSample(EltID idInst, double pitch, double &ecart, int cote, EltID &idInstSmpl)
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
                - (double)sm->get(idSmpl, champ_chPitchCorrection).cValue / 100.0;

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

QByteArray RunnableChordCreator::getSampleData(EltID idSmpl, qint32 nbRead)
{
    // Récupération de données provenant d'un sample, en prenant en compte la boucle
    SoundfontManager * sm = SoundfontManager::getInstance();
    QByteArray baData = sm->getData(idSmpl, champ_sampleData32);
    qint64 loopStart = sm->get(idSmpl, champ_dwStartLoop).dwValue;
    qint64 loopEnd = sm->get(idSmpl, champ_dwEndLoop).dwValue;
    QByteArray baDataRet;
    baDataRet.resize(nbRead * 4);
    qint64 posInit = 0;
    const char * data = baData.constData();
    char * dataRet = baDataRet.data();
    if (loopStart != loopEnd)
    {
        // Boucle
        qint64 total = 0;
        while (nbRead - total > 0)
        {
            const qint64 chunk = qMin(loopEnd - posInit, nbRead - total);
            memcpy(dataRet + 4 * total, data + 4 * posInit, 4 * chunk);
            posInit += chunk;
            if (posInit >= loopEnd) posInit = loopStart;
            total += chunk;
        }
    }
    else
    {
        // Pas de boucle
        if (baData.size() / 4 < nbRead)
        {
            baDataRet.fill(0);
            memcpy(dataRet, data, baData.size());
        }
        else
            memcpy(dataRet, data, 4 * nbRead);
    }
    return baDataRet;
}

void RunnableChordCreator::addSampleData(QByteArray &baData1, QByteArray &baData2, double mult)
{
    // Ajout de baData2 multiplié par mult dans baData1
    qint32 * data1 = (qint32 *)baData1.data();
    qint32 * data2 = (qint32 *)baData2.data();
    for (int i = 0; i < qMin(baData1.size(), baData2.size()) / 4; i++)
        data1[i] += mult * data2[i];
}

// List of all possible keys with their corresponding attenuation
QMap<int, int> RunnableChordCreator::getChordKeys(int key, ChordInfo& chordInfo)
{
    QMap<int, int> chordKeys;
    int shift = 0;

    // Root key (not impacted by the octave or inversion)
    if (chordInfo.chordType1 == 1)
        chordKeys[0] = chordInfo.chordType1Attenuation;

    // Third
    switch (chordInfo.chordType3)
    {
    case 1: shift = 4; break; // Major
    case 2: shift = 3; break; // Minor
    default: shift = 0; break;
    }
    if (shift != 0)
        chordKeys[key + shift + 12 * (chordInfo.octave - (chordInfo.chordType3Inversion ? 1 : 0))] = chordInfo.chordType3Attenuation;

    // Fifth
    switch (chordInfo.chordType5)
    {
    case 1: shift = 7; break; // Perfect
    case 2: shift = 6; break; // Diminished
    case 3: shift = 8; break; // Augmented
    default: shift = 0; break;
    }
    if (shift != 0)
        chordKeys[key + shift + 12 * (chordInfo.octave - (chordInfo.chordType5Inversion ? 1 : 0))] = chordInfo.chordType5Attenuation;

    // Seventh
    switch (chordInfo.chordType7)
    {
    case 1: shift = 10; break; // Minor
    case 2: shift = 11; break; // Major
    case 3: shift = 9; break; // Diminished
    default: shift = 0; break;
    }
    if (shift != 0)
        chordKeys[key + shift + 12 * (chordInfo.octave - (chordInfo.chordType7Inversion ? 1 : 0))] = chordInfo.chordType7Attenuation;

    // Ninth
    switch (chordInfo.chordType9)
    {
    case 1: shift = 14; break; // Major
    case 2: shift = 13; break; // Minor
    default: shift = 0; break;
    }
    if (shift != 0)
        chordKeys[key + shift + 12 * (chordInfo.octave - (chordInfo.chordType9Inversion ? 1 : 0))] = chordInfo.chordType9Attenuation;

    return chordKeys;
}

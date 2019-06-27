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

#include "sfzparametergroup.h"
#include "soundfontmanager.h"

QList<int> SfzParameterGroup::getSampleIndex(SoundfontManager *sf2, EltID idElt, QString pathSfz) const
{
    QList<int> sampleIndex;

    int indexOpSample = -1;
    for (int i = 0; i < _listeParam.size(); i++)
        if (_listeParam.at(i).getOpCode() == SfzParameter::op_sample)
            indexOpSample = i;

    if (indexOpSample == -1)
        return sampleIndex;

    // Reconstitution adresse du fichier
    QString filePath =  _listeParam.at(indexOpSample).getStringValue();
    QString fileName = pathSfz + "/" + filePath;
    if (!QFile(fileName).exists())
    {
        QStringList list = getFullPath(pathSfz, filePath.split("/", QString::SkipEmptyParts));
        if (!list.isEmpty())
            fileName = list.first();
        else
            return sampleIndex;
    }

    // Sample déjà chargé ?
    idElt.typeElement = elementSmpl;
    QStringList names;
    foreach (int i, sf2->getSiblings(idElt))
    {
        idElt.indexElt = i;
        if (sf2->getQstr(idElt, champ_filenameForData) == fileName)
            sampleIndex << i;
        names << sf2->getQstr(idElt, champ_name);
    }
    if (!sampleIndex.isEmpty())
        return sampleIndex;

    // Récupération des informations d'un sample
    Sound son(fileName, false);
    int nChannels = son.getUInt32(champ_wChannels);
    QString nom = QFileInfo(fileName).completeBaseName();
    QString nom2 = nom;

    // Adaptation du nom, éventuellement
    int suffixNumber = 0;
    if (nChannels == 2)
    {
        while ((names.contains(getName(nom, 20, suffixNumber, "L"), Qt::CaseInsensitive) ||
                names.contains(getName(nom, 20, suffixNumber, "R"), Qt::CaseInsensitive)) &&
               suffixNumber < 100)
        {
            suffixNumber++;
        }
        nom2 = getName(nom, 20, suffixNumber, "L");
        nom = getName(nom, 20, suffixNumber, "R");
    }
    else
    {
        while (names.contains(getName(nom, 20, suffixNumber), Qt::CaseInsensitive) && suffixNumber < 100)
        {
            suffixNumber++;
        }
        nom = getName(nom, 20, suffixNumber);
    }

    // Création d'un nouveau sample
    AttributeValue val;
    for (int numChannel = 0; numChannel < nChannels; numChannel++)
    {
        idElt.indexElt = sf2->add(idElt);
        sampleIndex << idElt.indexElt;
        if (nChannels == 2)
        {
            if (numChannel == 0)
            {
                // Gauche
                sf2->set(idElt, champ_name, nom2);
                val.wValue = idElt.indexElt + 1;
                sf2->set(idElt, champ_wSampleLink, val);
                val.sfLinkValue = leftSample;
                sf2->set(idElt, champ_sfSampleType, val);
            }
            else
            {
                // Droite
                sf2->set(idElt, champ_name, nom);
                val.wValue = idElt.indexElt - 1;
                sf2->set(idElt, champ_wSampleLink, val);
                val.sfLinkValue = rightSample;
                sf2->set(idElt, champ_sfSampleType, val);
            }
        }
        else
        {
            sf2->set(idElt, champ_name, nom);
            val.wValue = 0;
            sf2->set(idElt, champ_wSampleLink, val);
            val.sfLinkValue = monoSample;
            sf2->set(idElt, champ_sfSampleType, val);
        }
        sf2->set(idElt, champ_filenameForData, fileName);
        val.dwValue = son.getUInt32(champ_dwStart16);
        sf2->set(idElt, champ_dwStart16, val);
        val.dwValue = son.getUInt32(champ_dwStart24);
        sf2->set(idElt, champ_dwStart24, val);
        val.wValue = numChannel;
        sf2->set(idElt, champ_wChannel, val);
        val.dwValue = son.getUInt32(champ_dwLength);
        sf2->set(idElt, champ_dwLength, val);
        val.dwValue = son.getUInt32(champ_dwSampleRate);
        sf2->set(idElt, champ_dwSampleRate, val);
        val.dwValue = son.getUInt32(champ_dwStartLoop);
        sf2->set(idElt, champ_dwStartLoop, val);
        val.dwValue = son.getUInt32(champ_dwEndLoop);
        sf2->set(idElt, champ_dwEndLoop, val);
        val.bValue = (quint8)son.getUInt32(champ_byOriginalPitch);
        sf2->set(idElt, champ_byOriginalPitch, val);
        val.cValue = (char)son.getInt32(champ_chPitchCorrection);
        sf2->set(idElt, champ_chPitchCorrection, val);
    }

    return sampleIndex;
}

void SfzParameterGroup::adaptOffsets(int startLoop, int endLoop, int length)
{
    for (int i = 0; i < _listeParam.size(); i++)
    {
        if (_listeParam.at(i).getOpCode() == SfzParameter::op_loop_start)
            _listeParam[i].setIntValue(_listeParam.at(i).getIntValue() - startLoop);
        else if (_listeParam.at(i).getOpCode() == SfzParameter::op_loop_end)
            _listeParam[i].setIntValue(_listeParam.at(i).getIntValue() - endLoop);
        else if (_listeParam.at(i).getOpCode() == SfzParameter::op_end)
            _listeParam[i].setIntValue(_listeParam.at(i).getIntValue() - length);
    }
    if (startLoop != endLoop && !isDefined(SfzParameter::op_loop_mode) && (startLoop != 0 || endLoop != 1))
        _listeParam << SfzParameter("loop_mode", "loop_continuous");
}

void SfzParameterGroup::adjustStereoVolumeAndCorrection(QString path, int defaultCorrection)
{
    QString sample = getStrValue(SfzParameter::op_sample);
    if (!sample.isEmpty())
    {
        Sound son(path + "/" + sample);
        if (son.getUInt32(champ_wChannels) == 2)
            adjustVolume(3.);
        int correctionSample = son.getInt32(champ_chPitchCorrection);
        if (correctionSample != 0)
            adjustCorrection(correctionSample, defaultCorrection);
    }
}

bool SfzParameterGroup::sampleValid(QString path)
{
    bool bRet = false;
    if (this->isDefined(SfzParameter::op_sample))
    {
        QString sample = getStrValue(SfzParameter::op_sample);
        bRet = QFile(path + "/" + sample).exists();
    }
    return bRet;
}

void SfzParameterGroup::checkFilter()
{
    bool removeFilter = false;
    if (isDefined(SfzParameter::op_filterType))
    {
        QString type = getStrValue(SfzParameter::op_filterType);
        if (type != "lpf_2p" && type != "lpf_1p") // 1 pôle : acceptable
            removeFilter = true;
    }

    if (removeFilter)
    {
        for (int i = _listeParam.size() - 1; i >= 0; i--)
        {
            if (_listeParam.at(i).getOpCode() == SfzParameter::op_filterFreq ||
                    _listeParam.at(i).getOpCode() == SfzParameter::op_filterType ||
                    _listeParam.at(i).getOpCode() == SfzParameter::op_filterQ)
                _listeParam.removeAt(i);
        }
    }
}

void SfzParameterGroup::adjustVolume(double offset)
{
    bool isDefined = false;
    for (int i = 0; i < _listeParam.size(); i++)
    {
        if (_listeParam.at(i).getOpCode() == SfzParameter::op_volume)
        {
            _listeParam[i].setDoubleValue(_listeParam.at(i).getDoubleValue() - offset);
            isDefined = true;
        }
    }
    if (!isDefined)
        _listeParam << SfzParameter(SfzParameter::op_volume, -offset);
}

void SfzParameterGroup::adjustCorrection(int offset, int defaultCorrection)
{
    bool isDefined = false;
    for (int i = 0; i < _listeParam.size(); i++)
    {
        if (_listeParam.at(i).getOpCode() == SfzParameter::op_tuningFine)
        {
            _listeParam[i].setIntValue(_listeParam.at(i).getIntValue() - offset);
            isDefined = true;
        }
    }
    if (!isDefined)
        _listeParam << SfzParameter(SfzParameter::op_tuningFine, defaultCorrection - offset);
}

QStringList SfzParameterGroup::getFullPath(QString base, QStringList directories)
{
    QStringList listRet;
    if (directories.isEmpty())
        listRet << base;
    else
    {
        QDir dir(base);
        dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
        QStringList childs = dir.entryList();
        QString nextDirectory = directories.takeFirst();
        for (int i = 0; i < childs.size(); i++)
            if (childs.at(i).compare(nextDirectory, Qt::CaseInsensitive) == 0)
                listRet << getFullPath(base + "/" + childs.at(i), directories);
    }
    return listRet;
}

void SfzParameterGroup::decode(SoundfontManager * sf2, EltID idElt) const
{
    AttributeValue val;
    double dTmp;
    for (int i = 0; i < _listeParam.size(); i++)
    {
        switch (_listeParam.at(i).getOpCode())
        {
        case SfzParameter::op_key:
            val.rValue.byLo = _listeParam.at(i).getIntValue();
            val.rValue.byHi = val.rValue.byLo;
            sf2->set(idElt, champ_keyRange, val);
            val.wValue = _listeParam.at(i).getIntValue();
            sf2->set(idElt, champ_overridingRootKey, val);
            break;
        case SfzParameter::op_keyMin:
            if (sf2->isSet(idElt, champ_keyRange))
                val = sf2->get(idElt, champ_keyRange);
            else
                val.rValue.byHi = 127;
            val.rValue.byLo = _listeParam.at(i).getIntValue();
            sf2->set(idElt, champ_keyRange, val);
            break;
        case SfzParameter::op_keyMax:
            if (sf2->isSet(idElt, champ_keyRange))
                val = sf2->get(idElt, champ_keyRange);
            else
                val.rValue.byLo = 0;
            val.rValue.byHi = _listeParam.at(i).getIntValue();
            sf2->set(idElt, champ_keyRange, val);
            break;
        case SfzParameter::op_velMin:
            if (sf2->isSet(idElt, champ_velRange))
                val = sf2->get(idElt, champ_velRange);
            else
                val.rValue.byHi = 127;
            val.rValue.byLo = _listeParam.at(i).getIntValue();
            sf2->set(idElt, champ_velRange, val);
            break;
        case SfzParameter::op_velMax:
            if (sf2->isSet(idElt, champ_velRange))
                val = sf2->get(idElt, champ_velRange);
            else
                val.rValue.byLo = 0;
            val.rValue.byHi = _listeParam.at(i).getIntValue();
            sf2->set(idElt, champ_velRange, val);
            break;
        case SfzParameter::op_rootKey:
            if (!isDefined(SfzParameter::op_key))
            {
                val.wValue = _listeParam.at(i).getIntValue();
                sf2->set(idElt, champ_overridingRootKey, val);
            }
            break;
        case SfzParameter::op_exclusiveClass:
            val.wValue = _listeParam.at(i).getIntValue();
            if (isDefined(SfzParameter::op_off_by) && getIntValue(SfzParameter::op_off_by) == val.wValue)
                sf2->set(idElt, champ_exclusiveClass, val);
            break;
        case SfzParameter::op_tuningFine:
            val.wValue = _listeParam.at(i).getIntValue();
            sf2->set(idElt, champ_fineTune, val);
            break;
        case SfzParameter::op_tuningCoarse:
            val.wValue = _listeParam.at(i).getIntValue();
            sf2->set(idElt, champ_coarseTune, val);
            break;
        case SfzParameter::op_delay:
            dTmp = _listeParam.at(i).getDoubleValue();
            addSeconds(dTmp, champ_delayModEnv, sf2, idElt);
            addSeconds(dTmp, champ_delayModLFO, sf2, idElt);
            addSeconds(dTmp, champ_delayVibLFO, sf2, idElt);
            addSeconds(dTmp, champ_delayVolEnv, sf2, idElt);
            break;
        case SfzParameter::op_offset:
            if (idElt.typeElement == elementInstSmpl)
            {
                val.shValue = _listeParam.at(i).getIntValue() % 32768;
                sf2->set(idElt, champ_startAddrsOffset, val);
                val.shValue = _listeParam.at(i).getIntValue() / 32768;
                sf2->set(idElt, champ_startAddrsCoarseOffset, val);
            }
            break;
        case SfzParameter::op_end:
            if (idElt.typeElement == elementInstSmpl)
            {
                val.shValue = _listeParam.at(i).getIntValue() % 32768;
                sf2->set(idElt, champ_endAddrsOffset, val);
                val.shValue = _listeParam.at(i).getIntValue() / 32768;
                sf2->set(idElt, champ_endAddrsCoarseOffset, val);
            }
            break;
        case SfzParameter::op_loop_start:
            if (idElt.typeElement == elementInstSmpl)
            {
                val.shValue = _listeParam.at(i).getIntValue() % 32768;
                sf2->set(idElt, champ_startloopAddrsOffset, val);
                val.shValue = _listeParam.at(i).getIntValue() / 32768;
                sf2->set(idElt, champ_startloopAddrsCoarseOffset, val);
            }
            break;
        case SfzParameter::op_loop_end:
            if (idElt.typeElement == elementInstSmpl)
            {
                val.shValue = _listeParam.at(i).getIntValue() % 32768;
                sf2->set(idElt, champ_endloopAddrsOffset, val);
                val.shValue = _listeParam.at(i).getIntValue() / 32768;
                sf2->set(idElt, champ_endloopAddrsCoarseOffset, val);
            }
            break;
        case SfzParameter::op_loop_mode:
            if (_listeParam.at(i).getStringValue() == "no_loop")
                val.wValue = 0;
            else if (_listeParam.at(i).getStringValue() == "one_shot")
            {
                val.wValue = 0;
                addSeconds(100, champ_releaseVolEnv, sf2, idElt);
                addSeconds(100, champ_releaseModEnv, sf2, idElt);
            }
            else if (_listeParam.at(i).getStringValue() == "loop_continuous")
                val.wValue = 1;
            else if (_listeParam.at(i).getStringValue() == "loop_sustain")
                val.wValue = 3;
            sf2->set(idElt, champ_sampleModes, val);
            break;
        case SfzParameter::op_volume:
            dTmp = -_listeParam.at(i).getDoubleValue() / DB_SF2_TO_SFZ;
            if (sf2->isSet(idElt, champ_initialAttenuation))
                dTmp += (double)sf2->get(idElt, champ_initialAttenuation).shValue / 10.;
            val.shValue = qRound(10. * dTmp);
            sf2->set(idElt, champ_initialAttenuation, val);
            break;
        case SfzParameter::op_tuningScale:
            val.shValue = _listeParam.at(i).getIntValue();
            sf2->set(idElt, champ_scaleTuning, val);
            break;
        case SfzParameter::op_ampeg_delay:
            addSeconds(_listeParam.at(i).getDoubleValue(), champ_delayVolEnv, sf2, idElt);
            break;
        case SfzParameter::op_ampeg_attack:
            val.shValue = qRound(log2m1200(_listeParam.at(i).getDoubleValue()));
            sf2->set(idElt, champ_attackVolEnv, val);
            break;
        case SfzParameter::op_ampeg_hold:
            if (!isDefined(SfzParameter::op_noteToVolEnvHold))
            {
                val.shValue = qRound(log2m1200(_listeParam.at(i).getDoubleValue()));
                sf2->set(idElt, champ_holdVolEnv, val);
            }
            break;
        case SfzParameter::op_ampeg_decay:
            if (!isDefined(SfzParameter::op_noteToVolEnvDecay))
            {
                val.shValue = qRound(log2m1200(_listeParam.at(i).getDoubleValue()));
                sf2->set(idElt, champ_decayVolEnv, val);
            }
            break;
        case SfzParameter::op_ampeg_sustain:
            dTmp = _listeParam.at(i).getDoubleValue();
            if (dTmp >= 0.1)
                val.shValue = qRound(-10. * percentToDB(dTmp));
            else
                val.shValue = 1440;
            sf2->set(idElt, champ_sustainVolEnv, val);
            break;
        case SfzParameter::op_ampeg_release:
            val.shValue = qRound(log2m1200(_listeParam.at(i).getDoubleValue()));
            sf2->set(idElt, champ_releaseVolEnv, val);
            break;
        case SfzParameter::op_noteToVolEnvHold:
        {
            double baseValue = 0.001;
            int keynum = 0;
            getKeynumValues(baseValue, keynum, SfzParameter::op_noteToVolEnvHold, SfzParameter::op_ampeg_hold);
            val.shValue = keynum;
            sf2->set(idElt, champ_keynumToVolEnvHold, val);
            val.shValue = qRound(log2m1200(baseValue));
            sf2->set(idElt, champ_holdVolEnv, val);
        }break;
        case SfzParameter::op_noteToVolEnvDecay:
        {
            double baseValue = 0.001;
            int keynum = 0;
            getKeynumValues(baseValue, keynum, SfzParameter::op_noteToVolEnvDecay, SfzParameter::op_ampeg_decay);
            val.shValue = keynum;
            sf2->set(idElt, champ_keynumToVolEnvDecay, val);
            val.shValue = qRound(log2m1200(baseValue));
            sf2->set(idElt, champ_decayVolEnv, val);
        }break;
        case SfzParameter::op_reverb:
            val.wValue = qRound(10. * _listeParam.at(i).getDoubleValue());
            sf2->set(idElt, champ_reverbEffectsSend, val);
            break;
        case SfzParameter::op_chorus:
            val.wValue = qRound(10. * _listeParam.at(i).getDoubleValue());
            sf2->set(idElt, champ_chorusEffectsSend, val);
            break;
        case SfzParameter::op_filterFreq:
            val.shValue = qRound(log2m1200(_listeParam.at(i).getDoubleValue() / 8.176));
            sf2->set(idElt, champ_initialFilterFc, val);
            break;
        case SfzParameter::op_filterQ:
            val.shValue = qRound(10. * _listeParam.at(i).getDoubleValue());
            sf2->set(idElt, champ_initialFilterQ, val);
            break;
        case SfzParameter::op_vibLFOdelay:
            if (isDefined(SfzParameter::op_vibLFOtoTon))
                addSeconds(_listeParam.at(i).getDoubleValue(), champ_delayVibLFO, sf2, idElt);
            break;
        case SfzParameter::op_vibLFOfreq:
            val.shValue = qRound(log2m1200(_listeParam.at(i).getDoubleValue() / 8.176));
            sf2->set(idElt, champ_freqVibLFO, val);
            break;
        case SfzParameter::op_vibLFOtoTon:
            val.shValue = _listeParam.at(i).getIntValue();
            sf2->set(idElt, champ_vibLfoToPitch, val);
            break;
        case SfzParameter::op_pitcheg_delay:
            if (isDefined(SfzParameter::op_modEnvToTon))
                addSeconds(_listeParam.at(i).getDoubleValue(), champ_delayModEnv, sf2, idElt);
            break;
        case SfzParameter::op_pitcheg_attack:
            val.shValue = qRound(log2m1200(_listeParam.at(i).getDoubleValue()));
            sf2->set(idElt, champ_attackModEnv, val);
            break;
        case SfzParameter::op_pitcheg_hold:
            if (!isDefined(SfzParameter::op_noteToModEnvHold))
            {
                val.shValue = qRound(log2m1200(_listeParam.at(i).getDoubleValue()));
                sf2->set(idElt, champ_holdModEnv, val);
            }
            break;
        case SfzParameter::op_pitcheg_decay:
            if (!isDefined(SfzParameter::op_noteToModEnvDecay))
            {
                val.shValue = qRound(log2m1200(_listeParam.at(i).getDoubleValue()));
                sf2->set(idElt, champ_decayModEnv, val);
            }
            break;
        case SfzParameter::op_pitcheg_sustain:
            val.shValue = qRound(1000. - 10. * _listeParam.at(i).getDoubleValue());
            sf2->set(idElt, champ_sustainModEnv, val);
            break;
        case SfzParameter::op_pitcheg_release:
            val.shValue = qRound(log2m1200(_listeParam.at(i).getDoubleValue()));
            sf2->set(idElt, champ_releaseModEnv, val);
            break;
        case SfzParameter::op_modEnvToTon:
            val.shValue = _listeParam.at(i).getIntValue();
            sf2->set(idElt, champ_modEnvToPitch, val);
            break;
        case SfzParameter::op_noteToModEnvHold:
        {
            double baseValue = 0.001;
            int keynum = 0;
            getKeynumValues(baseValue, keynum, SfzParameter::op_noteToModEnvHold, SfzParameter::op_pitcheg_hold);
            val.shValue = keynum;
            sf2->set(idElt, champ_keynumToModEnvHold, val);
            val.shValue = qRound(log2m1200(baseValue));
            sf2->set(idElt, champ_holdModEnv, val);
        }break;
        case SfzParameter::op_noteToModEnvDecay:
        {
            double baseValue = 0.001;
            int keynum = 0;
            getKeynumValues(baseValue, keynum, SfzParameter::op_noteToModEnvDecay, SfzParameter::op_pitcheg_decay);
            val.shValue = keynum;
            sf2->set(idElt, champ_keynumToModEnvDecay, val);
            val.shValue = qRound(log2m1200(baseValue));
            sf2->set(idElt, champ_decayModEnv, val);
        }break;
        case SfzParameter::op_modLFOdelay:
            addSeconds(_listeParam.at(i).getDoubleValue(), champ_delayModLFO, sf2, idElt);
            break;
        case SfzParameter::op_modLFOfreq:
            val.shValue = qRound(log2m1200(_listeParam.at(i).getDoubleValue() / 8.176));
            sf2->set(idElt, champ_freqModLFO, val);
            break;
        case SfzParameter::op_modLFOtoVolume:
            val.shValue = qRound(10. * _listeParam.at(i).getDoubleValue());
            sf2->set(idElt, champ_modLfoToVolume, val);
            break;
        default:
            break;
        }
    }

    // Vélocité fixe
    if (isDefined(SfzParameter::op_amp_velcurve_1) && isDefined(SfzParameter::op_amp_velcurve_127))
    {
        int velMin = qRound(127. * getDoubleValue(SfzParameter::op_amp_velcurve_1));
        int velMax = qRound(127. * getDoubleValue(SfzParameter::op_amp_velcurve_127));
        if (velMin == velMax)
        {
            val.wValue = velMin;
            sf2->set(idElt, champ_velocity, val);
        }
    }

    // Enveloppe pour le filtre
    if (isDefined(SfzParameter::op_modEnvToFilter))
    {
        if (isDefined(SfzParameter::op_modEnvToTon))
        {
            // Mêmes paramètres ?
            if (getDoubleValue(SfzParameter::op_fileg_delay) == getDoubleValue(SfzParameter::op_pitcheg_delay) &&
                getDoubleValue(SfzParameter::op_fileg_attack) == getDoubleValue(SfzParameter::op_pitcheg_attack) &&
                getDoubleValue(SfzParameter::op_fileg_hold) == getDoubleValue(SfzParameter::op_pitcheg_hold) &&
                getDoubleValue(SfzParameter::op_fileg_decay) == getDoubleValue(SfzParameter::op_pitcheg_decay) &&
                getDoubleValue(SfzParameter::op_fileg_sustain) == getDoubleValue(SfzParameter::op_pitcheg_sustain) &&
                getDoubleValue(SfzParameter::op_fileg_release) == getDoubleValue(SfzParameter::op_pitcheg_release) &&
                getDoubleValue(SfzParameter::op_fileg_holdcc133) == getDoubleValue(SfzParameter::op_noteToModEnvHold) &&
                getDoubleValue(SfzParameter::op_fileg_decaycc133) == getDoubleValue(SfzParameter::op_noteToModEnvDecay))
            {
                val.shValue = getIntValue(SfzParameter::op_modEnvToFilter);
                sf2->set(idElt, champ_modEnvToFilterFc, val);
            }
        }
        else
        {
            if (isDefined(SfzParameter::op_fileg_delay))
                addSeconds(getDoubleValue(SfzParameter::op_fileg_delay), champ_delayModEnv, sf2, idElt);
            if (isDefined(SfzParameter::op_fileg_attack))
            {
                val.shValue = qRound(log2m1200(getDoubleValue(SfzParameter::op_fileg_attack)));
                sf2->set(idElt, champ_attackModEnv, val);
            }
            if (isDefined(SfzParameter::op_fileg_hold) && !isDefined(SfzParameter::op_fileg_holdcc133))
            {
                val.shValue = qRound(log2m1200(getDoubleValue(SfzParameter::op_fileg_hold)));
                sf2->set(idElt, champ_holdModEnv, val);
            }
            if (isDefined(SfzParameter::op_fileg_decay) && !isDefined(SfzParameter::op_fileg_decaycc133))
            {
                val.shValue = qRound(log2m1200(getDoubleValue(SfzParameter::op_fileg_decay)));
                sf2->set(idElt, champ_decayModEnv, val);
            }
            if (isDefined(SfzParameter::op_fileg_sustain))
            {
                val.shValue = qRound(1000. - 10. * getDoubleValue(SfzParameter::op_fileg_sustain));
                sf2->set(idElt, champ_sustainModEnv, val);
            }
            if (isDefined(SfzParameter::op_fileg_release))
            {
                val.shValue = qRound(log2m1200(getDoubleValue(SfzParameter::op_fileg_release)));
                sf2->set(idElt, champ_releaseModEnv, val);
            }
            if (isDefined(SfzParameter::op_fileg_holdcc133))
            {
                double baseValue = 0.001;
                int keynum = 0;
                getKeynumValues(baseValue, keynum, SfzParameter::op_fileg_holdcc133, SfzParameter::op_fileg_hold);
                val.shValue = keynum;
                sf2->set(idElt, champ_keynumToModEnvHold, val);
                val.shValue = qRound(log2m1200(baseValue));
                sf2->set(idElt, champ_holdModEnv, val);
            }
            if (isDefined(SfzParameter::op_fileg_decaycc133))
            {
                double baseValue = 0.001;
                int keynum = 0;
                getKeynumValues(baseValue, keynum, SfzParameter::op_fileg_decaycc133, SfzParameter::op_fileg_decay);
                val.shValue = keynum;
                sf2->set(idElt, champ_keynumToModEnvDecay, val);
                val.shValue = qRound(log2m1200(baseValue));
                sf2->set(idElt, champ_decayModEnv, val);
            }

            val.shValue = getIntValue(SfzParameter::op_modEnvToFilter);
            sf2->set(idElt, champ_modEnvToFilterFc, val);
        }
    }

    // LFO filtre
    if (isDefined(SfzParameter::op_modLFOtoFilter))
    {
        if (isDefined(SfzParameter::op_modLFOtoVolume))
        {
            // Mêmes paramètres ?
            if (getDoubleValue(SfzParameter::op_filLFOdelay) == getDoubleValue(SfzParameter::op_modLFOdelay) &&
                getDoubleValue(SfzParameter::op_filLFOfreq) == getDoubleValue(SfzParameter::op_modLFOfreq))
            {
                val.shValue = getIntValue(SfzParameter::op_modLFOtoFilter);
                sf2->set(idElt, champ_modLfoToFilterFc, val);
            }
        }
        else
        {
            if (isDefined(SfzParameter::op_filLFOdelay))
                addSeconds(getDoubleValue(SfzParameter::op_filLFOdelay), champ_delayModLFO, sf2, idElt);
            if (isDefined(SfzParameter::op_filLFOfreq))
            {
                val.shValue = qRound(log2m1200(getDoubleValue(SfzParameter::op_filLFOfreq) / 8.176));
                sf2->set(idElt, champ_freqModLFO, val);
            }
            val.shValue = getIntValue(SfzParameter::op_modLFOtoFilter);
            sf2->set(idElt, champ_modLfoToFilterFc, val);
        }
    }
}

void SfzParameterGroup::getKeynumValues(double &baseValue, int &keynum,
                                       SfzParameter::OpCode opCodeKeynum, SfzParameter::OpCode opCodeBase) const
{
    // Etendue de la division
    int noteMin = 0;
    int noteMax = 127;
    if (isDefined(SfzParameter::op_key))
        noteMin = noteMax = getIntValue(SfzParameter::op_key);
    if (isDefined(SfzParameter::op_keyMin))
        noteMin = getIntValue(SfzParameter::op_keyMin);
    if (isDefined(SfzParameter::op_keyMax))
        noteMax = getIntValue(SfzParameter::op_keyMax);

    // Valeur de base du paramètre
    double valBase = 0.001;
    if (isDefined(opCodeBase))
        valBase = getDoubleValue(opCodeBase);

    // Valeur aux positions min et max
    double valKeynum = getDoubleValue(opCodeKeynum);
    double valMin = valBase + (double)noteMin / 127. * valKeynum;
    double valMax = valBase + (double)noteMax / 127. * valKeynum;

    if (noteMin == noteMax || valMax <= 0 || valMin <= 0)
    {
        baseValue = valMin;
        keynum = 0;
    }
    else
    {
        keynum = qRound(1200. * log2(valMin / valMax) / (noteMax - noteMin));
        baseValue = valMin * qPow(valMin / valMax, (double)(noteMin - 60) / (noteMax - noteMin));
    }
}

void SfzParameterGroup::addSeconds(double value, AttributeType champ, SoundfontManager * sf2, EltID id)
{
    double dTmp;
    AttributeValue val;
    if (sf2->isSet(id, champ))
        dTmp = d1200e2(sf2->get(id, champ).shValue);
    else
        dTmp = 0;
    val.shValue = qRound(log2m1200(dTmp + value));
    sf2->set(id, champ, val);
}

QString SfzParameterGroup::getName(QString name, int maxCharacters, int suffixNumber, QString suffix)
{
    int suffixSize = suffix.size();

    // Cas où la taille du suffix est supérieure au nombre de caractères max
    if (suffixSize > maxCharacters)
        return name.left(maxCharacters);

    // Cas où un numéro n'est pas nécessaire
    if (suffixNumber == 0)
        return name.left(maxCharacters - suffixSize) + suffix;

    QString suffixNum = QString::number(suffixNumber);
    int suffixNumSize = suffixNum.length() + 1;

    // Cas où le suffix numérique est trop grand
    if (suffixNumSize > 3 || maxCharacters - suffixSize < suffixNumSize)
        return name.left(maxCharacters - suffixSize) + suffix;

    return name.left(maxCharacters - suffixNumSize - suffixSize) + suffix + "-" + suffixNum;
}

void SfzParameterGroup::mergeIfNotDefined(SfzParameterGroup &groupToMerge)
{
    foreach (SfzParameter param, groupToMerge._listeParam)
        if (!this->isDefined(param.getOpCode()))
            _listeParam << param;
}

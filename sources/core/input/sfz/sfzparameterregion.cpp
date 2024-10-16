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

#include "sfzparameterregion.h"
#include "soundfontmanager.h"

QList<int> SfzParameterRegion::getSampleIndex(SoundfontManager *sf2, EltID idElt, QString pathSfz) const
{
    QList<int> sampleIndex;

    int indexOpSample = -1;
    for (int i = 0; i < _listeParam.size(); i++)
        if (_listeParam.at(i).getOpCode() == SfzParameter::op_sample)
            indexOpSample = i;

    if (indexOpSample == -1)
        return sampleIndex;

    // Build the file path
    QString filePath =  _listeParam.at(indexOpSample).getStringValue();
    QString fileName = pathSfz + "/" + filePath;
    if (!QFile(fileName).exists())
    {
        QStringList list = getFullPath(pathSfz, filePath.split("/", Qt::SkipEmptyParts));
        if (!list.isEmpty())
            fileName = list.first();
        else
            return sampleIndex;
    }

    // Sample already loaded?
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

    // Gather sample information
    Sound son;
    if (!son.setFileName(fileName, false))
    {
        return sampleIndex;
    }
    int nChannels = son.getUInt32(champ_wChannels);
    QString nom = QFileInfo(fileName).completeBaseName();
    QString nom2 = nom;

    // Possibly adapt the name
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

    // Create a new sample
    AttributeValue val;
    for (int numChannel = 0; numChannel < nChannels; numChannel++)
    {
        idElt.indexElt = sf2->add(idElt);
        sampleIndex << idElt.indexElt;
        if (nChannels == 2)
        {
            if (numChannel == 0)
            {
                // Left
                sf2->set(idElt, champ_name, nom2);
                val.wValue = idElt.indexElt + 1;
                sf2->set(idElt, champ_wSampleLink, val);
                val.sfLinkValue = leftSample;
                sf2->set(idElt, champ_sfSampleType, val);
            }
            else
            {
                // Right
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
        val.wValue = son.getUInt32(champ_bpsFile);
        sf2->set(idElt, champ_bpsFile, val);
    }

    return sampleIndex;
}

void SfzParameterRegion::adaptLoop(int startLoop, int endLoop, int length)
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

    if (isDefined(SfzParameter::op_trigger) && getStrValue(SfzParameter::op_trigger).startsWith("release"))
    {
        removeOpCode(SfzParameter::op_loop_mode);
        _listeParam << SfzParameter("loop_mode", "one_shot");
    }
}

void SfzParameterRegion::adjustCorrection(QString path, int defaultCorrection)
{
    QString sample = getStrValue(SfzParameter::op_sample);
    if (!sample.isEmpty())
    {
        Sound son;
        son.setFileName(path + "/" + sample);
        int correctionSample = son.getInt32(champ_chPitchCorrection);
        if (correctionSample != 0)
            adjustCorrection(correctionSample, defaultCorrection);
    }
}

bool SfzParameterRegion::sampleValid(QString path)
{
    bool bRet = false;
    if (this->isDefined(SfzParameter::op_sample))
    {
        QString sample = getStrValue(SfzParameter::op_sample);
        bRet = QFile(path + "/" + sample).exists();
    }
    return bRet;
}

void SfzParameterRegion::checkFilter()
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
                _listeParam.at(i).getOpCode() == SfzParameter::op_filterQ ||
                _listeParam.at(i).getOpCode() == SfzParameter::op_fil_veltrack)
                _listeParam.removeAt(i);
        }
    }
}

void SfzParameterRegion::checkKeyTrackedFilter(bool remove)
{
    if (remove)
    {
        // Not the global division: remove op_fil_keytrack and op_fil_keycenter
        removeOpCode(SfzParameter::op_fil_keytrack);
        removeOpCode(SfzParameter::op_fil_keycenter);
        return;
    }

    if (isDefined(SfzParameter::op_fil_keytrack) && isDefined(SfzParameter::op_filterFreq))
    {
        // Key center specified?
        int keyCenter = 60; // Default value
        if (isDefined(SfzParameter::op_fil_keycenter))
            keyCenter = getIntValue(SfzParameter::op_fil_keycenter);

        // The value of the filter is set for key "keyCenter" but we want the value for key 64
        int diff = 64 - keyCenter;
        int cents = getIntValue(SfzParameter::op_fil_keytrack);

        for (int i = 0; i < _listeParam.size(); i++)
        {
            if (_listeParam.at(i).getOpCode() == SfzParameter::op_filterFreq)
            {
                int currentFreq = _listeParam.at(i).getDoubleValue();
                _listeParam[i].setDoubleValue(d1200e2(diff * cents) * currentFreq);
                break;
            }
        }
    }
}

void SfzParameterRegion::adjustVolume(double offset)
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

void SfzParameterRegion::adjustCorrection(int offset, int defaultCorrection)
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

QStringList SfzParameterRegion::getFullPath(QString base, QStringList directories)
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

void SfzParameterRegion::decode(SoundfontManager * sf2, EltID idElt)
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
            if (!isDefined(SfzParameter::op_rootKey))
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
            val.wValue = _listeParam.at(i).getIntValue();
            sf2->set(idElt, champ_overridingRootKey, val);
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
            if (idElt.typeElement == elementInstSmpl && _listeParam.at(i).getIntValue() != 0)
            {
                val.shValue = _listeParam.at(i).getIntValue() % 32768;
                sf2->set(idElt, champ_startAddrsOffset, val);
                val.shValue = _listeParam.at(i).getIntValue() / 32768;
                sf2->set(idElt, champ_startAddrsCoarseOffset, val);
            }
            break;
        case SfzParameter::op_end:
            if (idElt.typeElement == elementInstSmpl && _listeParam.at(i).getIntValue() != 0)
            {
                val.shValue = _listeParam.at(i).getIntValue() % 32768;
                sf2->set(idElt, champ_endAddrsOffset, val);
                val.shValue = _listeParam.at(i).getIntValue() / 32768;
                sf2->set(idElt, champ_endAddrsCoarseOffset, val);
            }
            break;
        case SfzParameter::op_loop_start:
            if (idElt.typeElement == elementInstSmpl && _listeParam.at(i).getIntValue() != 0)
            {
                val.shValue = _listeParam.at(i).getIntValue() % 32768;
                sf2->set(idElt, champ_startloopAddrsOffset, val);
                val.shValue = _listeParam.at(i).getIntValue() / 32768;
                sf2->set(idElt, champ_startloopAddrsCoarseOffset, val);
            }
            break;
        case SfzParameter::op_loop_end:
            if (idElt.typeElement == elementInstSmpl && _listeParam.at(i).getIntValue() != 0)
            {
                val.shValue = _listeParam.at(i).getIntValue() % 32768;
                sf2->set(idElt, champ_endloopAddrsOffset, val);
                val.shValue = _listeParam.at(i).getIntValue() / 32768;
                sf2->set(idElt, champ_endloopAddrsCoarseOffset, val);
            }
            break;
        case SfzParameter::op_loop_mode:
            if (this->isDefined(SfzParameter::op_trigger) && getStrValue(SfzParameter::op_trigger).startsWith("release"))
            {
                val.wValue = 2;
                addSeconds(100, champ_releaseVolEnv, sf2, idElt);
                addSeconds(100, champ_releaseModEnv, sf2, idElt);
            }
            else if (_listeParam.at(i).getStringValue() == "no_loop")
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
        case SfzParameter::op_trigger:
            // Nothing here, processed in op_loop_mode
            break;
        case SfzParameter::op_volume:
            dTmp = -_listeParam.at(i).getDoubleValue() / DB_SF2_TO_REAL_DB;
            if (sf2->isSet(idElt, champ_initialAttenuation))
                dTmp += (double)sf2->get(idElt, champ_initialAttenuation).shValue / 10.;
            val.shValue = qRound(10. * dTmp);
            sf2->set(idElt, champ_initialAttenuation, val);
            break;
        case SfzParameter::op_tuningScale:
            val.shValue = _listeParam.at(i).getIntValue();
            sf2->set(idElt, champ_scaleTuning, val);
            break;
        case SfzParameter::op_amp_veltrack: {
            // Create a modulator modifying the attenuation based on the velocity
            ModulatorData modData;
            modData.srcOper = SFModulator(GeneralController::GC_noteOnVelocity, ModType::typeConcave, true, false);
            modData.amtSrcOper = SFModulator(GeneralController::GC_noController, ModType::typeLinear, false, false);
            modData.amount = 9.6 * _listeParam.at(i).getIntValue();
            modData.transOper = SFTransform::linear;
            modData.destOper = champ_initialAttenuation;
            addModulator(modData);
        } break;
        case SfzParameter::op_ampeg_delay:
            addSeconds(_listeParam.at(i).getDoubleValue(), champ_delayVolEnv, sf2, idElt);
            break;
        case SfzParameter::op_ampeg_attack:
            if (_listeParam.at(i).getDoubleValue() == 0.001)
                val.shValue = -12000;
            else
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
                val.shValue = qRound(-10. * gainToDB(dTmp / 100.));
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
        case SfzParameter::op_filterFreq: {
            int modifier = this->getIntValue(SfzParameter::op_fil_veltrack);
            val.shValue = qRound(log2m1200(_listeParam.at(i).getDoubleValue() / 8.176) + modifier);
            sf2->set(idElt, champ_initialFilterFc, val);
        } break;
        case SfzParameter::op_filterQ:
            val.shValue = qRound(10. * _listeParam.at(i).getDoubleValue());
            sf2->set(idElt, champ_initialFilterQ, val);
            break;
        case SfzParameter::op_fil_veltrack: {
            // Create a modulator modifying the filter frequency based on the velocity
            ModulatorData modData;
            if (this->isDefined(SfzParameter::op_filterFreq))
            {
                modData.srcOper = SFModulator(GeneralController::GC_noteOnVelocity, ModType::typeLinear, true, false);
                modData.amount = -_listeParam.at(i).getIntValue();
            }
            else
            {
                modData.srcOper = SFModulator(GeneralController::GC_noteOnVelocity, ModType::typeLinear, false, false);
                modData.amount = _listeParam.at(i).getIntValue();
            }
            modData.amtSrcOper = SFModulator(GeneralController::GC_noController, ModType::typeLinear, false, false);
            modData.transOper = SFTransform::linear;
            modData.destOper = champ_initialFilterFc;
            addModulator(modData);
        } break;
        case SfzParameter::op_fil_keycenter:
            // Nothing here, already processed for changing the value of the filter if op_fil_keytrack is set
            break;
        case SfzParameter::op_fil_keytrack: {
            // Create a modulator modifying the filter frequency based on the key (center has been changed to 64)
            ModulatorData modData;
            modData.srcOper = SFModulator(GeneralController::GC_noteOnKeyNumber, ModType::typeLinear, false, true);
            modData.amtSrcOper = SFModulator(GeneralController::GC_noController, ModType::typeLinear, false, false);
            modData.amount = 64 * _listeParam.at(i).getIntValue();
            modData.transOper = SFTransform::linear;
            modData.destOper = champ_initialFilterFc;
            addModulator(modData);
        } break;
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

    // Fixed velocity
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

    // Filter envelop
    if (isDefined(SfzParameter::op_modEnvToFilter))
    {
        if (isDefined(SfzParameter::op_modEnvToTon))
        {
            // Same parameters?
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

    // Filter LFO
    if (isDefined(SfzParameter::op_modLFOtoFilter))
    {
        if (isDefined(SfzParameter::op_modLFOtoVolume))
        {
            // Same parameters?
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

    // Disable the default modulators if they are not already defined and
    if (idElt.typeElement == elementInst)
        disableDefaultModulators();

    // Save the modulator list in the soundfont
    foreach (ModulatorData modData, _decodedModulators)
        saveModulator(sf2, idElt, modData);
}

void SfzParameterRegion::getKeynumValues(double &baseValue, int &keynum,
                                        SfzParameter::OpCode opCodeKeynum, SfzParameter::OpCode opCodeBase) const
{
    // Division range
    int noteMin = 0;
    int noteMax = 127;
    if (isDefined(SfzParameter::op_key))
        noteMin = noteMax = getIntValue(SfzParameter::op_key);
    if (isDefined(SfzParameter::op_keyMin))
        noteMin = getIntValue(SfzParameter::op_keyMin);
    if (isDefined(SfzParameter::op_keyMax))
        noteMax = getIntValue(SfzParameter::op_keyMax);

    // Base value of the parameter
    double valBase = 0.001;
    if (isDefined(opCodeBase))
        valBase = getDoubleValue(opCodeBase);

    // Value at min and max positions
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

void SfzParameterRegion::addSeconds(double value, AttributeType champ, SoundfontManager * sf2, EltID id)
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

QString SfzParameterRegion::getName(QString name, int maxCharacters, int suffixNumber, QString suffix)
{
    int suffixSize = suffix.size();

    // Case when the suffix size is higher than the maximum number of characters
    if (suffixSize > maxCharacters)
        return name.left(maxCharacters);

    // Case when a number is not necessary
    if (suffixNumber == 0)
        return name.left(maxCharacters - suffixSize) + suffix;

    QString suffixNum = QString::number(suffixNumber);
    int suffixNumSize = suffixNum.length() + 1;

    // Case when the numerical suffix is too high
    if (suffixNumSize > 3 || maxCharacters - suffixSize < suffixNumSize)
        return name.left(maxCharacters - suffixSize) + suffix;

    return name.left(maxCharacters - suffixNumSize - suffixSize) + suffix + "-" + suffixNum;
}

void SfzParameterRegion::mergeIfNotDefined(SfzParameterRegion &groupToMerge)
{
    foreach (SfzParameter param, groupToMerge._listeParam)
    if (!this->isDefined(param.getOpCode()))
        _listeParam << param;
}

void SfzParameterRegion::addModulator(ModulatorData modData)
{
    // Store the modulator only if it's not already defined
    foreach (ModulatorData existingModData, _decodedModulators)
        if (existingModData == modData)
            return;
    _decodedModulators << modData;
}

void SfzParameterRegion::saveModulator(SoundfontManager * sf2, EltID idElt, ModulatorData &modData)
{
    // If we are about to add a modulator for the global division, first check that it's not the definition of a default modulator
    if (idElt.typeElement == elementInst)
    {
        int defaultModulatorCount;
        ModulatorData * defaultModulators = ModulatorData::getDefaultModulators(defaultModulatorCount);
        for (int i = 0; i < defaultModulatorCount; i++)
        {
            if (defaultModulators[i] == modData && defaultModulators[i].amount == modData.amount)
                return;
        }
    }

    // Add a new modulator
    EltID idMod(idElt.typeElement == elementInst ? elementInstMod : elementInstSmplMod, idElt.indexSf2, idElt.indexElt, idElt.indexElt2);
    idMod.indexMod = sf2->add(idMod);

    // And configure it
    AttributeValue val;
    val.sfModValue = modData.srcOper;
    sf2->set(idMod, champ_sfModSrcOper, val);
    val.wValue = modData.destOper;
    sf2->set(idMod, champ_sfModDestOper, val);
    val.wValue = modData.amount;
    sf2->set(idMod, champ_modAmount, val);
    val.sfModValue = modData.amtSrcOper;
    sf2->set(idMod, champ_sfModAmtSrcOper, val);
    val.sfTransValue = modData.transOper;
    sf2->set(idMod, champ_sfModTransOper, val);
}

void SfzParameterRegion::disableDefaultModulators()
{
    // Disable "MIDI Note-On Velocity to Initial Attenuation"
    ModulatorData modData;
    modData.srcOper = SFModulator(GeneralController::GC_noteOnVelocity, ModType::typeConcave, true, false);
    modData.amtSrcOper = SFModulator(GeneralController::GC_noController, ModType::typeLinear, false, false);
    modData.amount = 0;
    modData.transOper = SFTransform::linear;
    modData.destOper = champ_initialAttenuation;
    addModulator(modData);

    // Disable "MIDI Note-On Velocity to Filter Cutoff"
    modData.srcOper = SFModulator(GeneralController::GC_noteOnVelocity, ModType::typeLinear, true, false);
    modData.destOper = champ_initialFilterFc;
    addModulator(modData);
}

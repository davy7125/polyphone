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

#include "conversion_sfz.h"
#include "soundfontmanager.h"
#include "sampleutils.h"
#include "samplewriterwav.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDate>
#include "contextmanager.h"
#include "attribute.h"
#include "sfzparamlist.h"
#include "balanceparameters.h"
#include "sfzwriter.h"


ConversionSfz::ConversionSfz() : QObject(),
    _sf2(SoundfontManager::getInstance()),
    _sfzWriter(nullptr)
{}

QString ConversionSfz::convert(QString dirPath, EltID idSf2, bool presetPrefix, bool bankDir, bool gmSort)
{
    // Create the base directory
    if (QDir(dirPath).exists())
    {
        int i = 1;
        while (QDir(dirPath + "-" + QString::number(i)).exists())
            i++;
        dirPath += "-" + QString::number(i);
    }
    if (!QDir().mkdir(dirPath))
        return tr("Cannot create directory \"%1\"").arg(dirPath);

    // More than one bank in use?
    int uniqueBankNumber = -1;
    _bankSortEnabled = false;
    EltID presetId(elementPrst, idSf2.indexSf2);
    foreach (int presetNumber, _sf2->getSiblings(presetId))
    {
        presetId.indexElt = presetNumber;
        if (uniqueBankNumber == -1)
            uniqueBankNumber = _sf2->get(presetId, champ_wBank).wValue;
        else if (uniqueBankNumber != _sf2->get(presetId, champ_wBank).wValue)
        {
            _bankSortEnabled = true;
            uniqueBankNumber = -1;
            break;
        }
    }

    _bankSortEnabled &= bankDir;
    _gmSortEnabled = gmSort;

    // Sample directory
    _dirSamples = dirPath + "/samples";
    QDir().mkdir(_dirSamples);

    // For each preset in the soundfont
    foreach (int presetNumber, _sf2->getSiblings(presetId))
    {
        presetId.indexElt = presetNumber;

        // Directory that will contain the sfz file
        QString sourceDir = dirPath;

        int numBank = _sf2->get(presetId, champ_wBank).wValue;
        int numPreset = _sf2->get(presetId, champ_wPreset).wValue;

        if (_bankSortEnabled)
        {
            QString numText;
            numText.asprintf("%.3u", numBank);
            sourceDir += "/" + numText;
            if (!QDir(sourceDir).exists())
                QDir(sourceDir).mkdir(sourceDir);
        }
        if (_gmSortEnabled)
        {
            if (numBank == 128)
            {
                if (_bankSortEnabled || uniqueBankNumber == 128)
                    sourceDir += "/" + getDrumCategory(numPreset);
                else
                    sourceDir += "/" + getDirectoryName(128);
            }
            else
                sourceDir += "/" + getDirectoryName(numPreset);
            if (!QDir(sourceDir).exists())
                QDir(sourceDir).mkdir(sourceDir);
        }

        exportPrst(sourceDir, presetId, presetPrefix);
    }

    return "";
}

void ConversionSfz::exportPrst(QString dir, EltID id, bool presetPrefix)
{
    QString numText;
    if (presetPrefix)
        numText.asprintf("%.3u_", _sf2->get(id, champ_wPreset).wValue);
    int numBank = _sf2->get(id, champ_wBank).wValue;

    _sfzWriter = new SfzWriter(getPathSfz(dir, numText + _sf2->getQstr(id, champ_name)) + ".sfz");
    writeEntete(id);
    id.typeElement = elementPrstInst;
    foreach (int i, _sf2->getSiblings(id))
    {
        id.indexElt2 = i;
        QList<EltID> listProcessedInstSmpl;

        // Preset parameters
        SfzParamList * paramPrst = new SfzParamList(_sf2, id);

        // ID of the linked instrument
        EltID idInst = EltID(elementInst, id.indexSf2, _sf2->get(id, champ_instrument).wValue);

        // Instrument parameters
        SfzParamList * paramInst = new SfzParamList(_sf2, paramPrst, idInst);
        writeGroup(paramInst, numBank == 128);
        delete paramInst;

        // Write each division of the instrument, ordered by keyRange.byLo
        QList<EltID> listInstSmpl;
        {
            EltID idInstSmpl(elementInstSmpl, idInst.indexSf2, idInst.indexElt);
            QMultiMap<int, EltID> map;
            foreach (int j, _sf2->getSiblings(idInstSmpl))
            {
                idInstSmpl.indexElt2 = j;
                map.insert(_sf2->get(idInstSmpl, champ_keyRange).rValue.byLo, idInstSmpl);
            }
            listInstSmpl = map.values();
        }

        for (int j = 0; j < listInstSmpl.count(); j++)
        {
            // Already processed or not covered by the preset range?
            EltID idInstSmpl = listInstSmpl.at(j);
            if (listProcessedInstSmpl.contains(idInstSmpl) || !isIncluded(paramPrst, idInstSmpl))
                continue;

            // Get the sample path
            int sampleChannel, linkedSampleId;
            QString samplePath = getSamplePath(EltID(elementSmpl, idInstSmpl.indexSf2, _sf2->get(idInstSmpl, champ_sampleID).wValue),
                                               sampleChannel, linkedSampleId);

            // Balance parameters
            BalanceParameters balance(sampleChannel == 0);
            balance.getAttenuationAndPan(_sf2, idInstSmpl, sampleChannel);

            // If the sample is stereo, search another instSmpl that exactly matches the other channel
            if (sampleChannel != 0)
            {
                // Parameters of the current InstSmpl
                QMap<AttributeType, AttributeValue> map = getInstSmplParameters(idInstSmpl);

                int index = -1;
                for (int k = j + 1; k < listInstSmpl.count(); k++)
                {
                    if (_sf2->get(listInstSmpl.at(k), champ_sampleID).wValue != linkedSampleId)
                        continue;

                    // Parameters of the other InstSmpl
                    QMap<AttributeType, AttributeValue> mapLinked = getInstSmplParameters(listInstSmpl.at(k));

                    // Compare parameters
                    if (map.keys().count() == mapLinked.keys().count())
                    {
                        bool isEqual = true;
                        QList<AttributeType> attributes = map.keys();
                        foreach (AttributeType attribute, attributes)
                        {
                            if (attribute != champ_sampleID && (!mapLinked.contains(attribute) || mapLinked[attribute].shValue != map[attribute].shValue))
                            {
                                isEqual = false;
                                break;
                            }
                        }
                        if (!isEqual)
                            continue;
                    }
                    else
                        continue;

                    index = k;
                    break;
                }

                if (index != -1)
                {
                    listProcessedInstSmpl << listInstSmpl[index];
                    balance.getAttenuationAndPan(_sf2, listInstSmpl[index], -sampleChannel);
                }
            }

            balance.computeSfzParameters();
            SfzParamList * paramInstSmpl = new SfzParamList(_sf2, paramPrst, idInstSmpl);
            writeRegion(paramInstSmpl, samplePath, &balance);
            delete paramInstSmpl;
            listProcessedInstSmpl << idInstSmpl;
        }
        delete paramPrst;
    }

    _sfzWriter->write();

    // Delete the writer so that a new one can be created for the next preset
    delete _sfzWriter;
    _sfzWriter = nullptr;
}

QString ConversionSfz::getPathSfz(QString dir, QString name)
{
    if (name.isEmpty())
        name = tr("untitled");
    name = escapeStr(name);
    QFile file(dir + "/" + name + ".sfz");
    QDir dossier(dir + "/" + name);
    if (file.exists() || dossier.exists())
    {
        int i = 1;
        while (QFile(dir + "/" + name + "-" + QString::number(i) + ".sfz").exists() ||
               QDir(dir + "/" + name + "-" + QString::number(i)).exists())
            i++;
        name = name + "-" + QString::number(i);
    }

    return dir + "/" + name;
}

QMap<AttributeType, AttributeValue> ConversionSfz::getInstSmplParameters(EltID idInstSmpl)
{
    EltID idInstSmplGen(elementInstSmplGen, idInstSmpl.indexSf2, idInstSmpl.indexElt, idInstSmpl.indexElt2);
    QMap<AttributeType, AttributeValue> map;
    foreach (int champ, _sf2->getSiblings(idInstSmplGen))
    {
        if (champ == champ_pan || champ == champ_initialAttenuation)
            continue; // Skip these attributes
        map[(AttributeType)champ] = _sf2->get(idInstSmpl, (AttributeType)champ);
    }
    return map;
}

void ConversionSfz::writeEntete(EltID id)
{
    // Write header
    id.typeElement = elementSf2;
    _sfzWriter->addLine("// Sfz exported from a sf2 file with Polyphone");
    _sfzWriter->addLine("// Name      : " + _sf2->getQstr(id, champ_name).replace(QRegularExpression("[\r\n]"), " "));
    _sfzWriter->addLine("// Author    : " + _sf2->getQstr(id, champ_IENG).replace(QRegularExpression("[\r\n]"), " "));
    _sfzWriter->addLine("// Copyright : " + _sf2->getQstr(id, champ_ICOP).replace(QRegularExpression("[\r\n]"), " "));
    _sfzWriter->addLine("// Date      : " + QDate::currentDate().toString("yyyy/MM/dd"));
    _sfzWriter->addLine("// Comment   : " + _sf2->getQstr(id, champ_ICMT).replace(QRegularExpression("[\r\n]"), " "));
}

void ConversionSfz::writeGroup(SfzParamList * listeParam, bool isPercKit)
{
    // Write the parameters in common for different regions
    _sfzWriter->newGroup();
    if (isPercKit)
    {
        _sfzWriter->addLine("lochan", 10);
        _sfzWriter->addLine("hichan", 10);
    }
    for (int i = 0; i < listeParam->attributeCount(); i++)
    {
        if (listeParam->getAttribute(i) != champ_pan && listeParam->getAttribute(i) != champ_initialAttenuation)
            writeElement(listeParam->getAttribute(i), listeParam->getValue(i));
    }

    // Associated modulators
    for (int i = 0; i < listeParam->modulatorCount(); i++)
        writeModulator(listeParam->getModulator(i));
}

void ConversionSfz::writeRegion(SfzParamList * listeParam, QString pathSample, BalanceParameters *balance)
{
    // Write the parameters specific to a region
    _sfzWriter->newRegion();
    _sfzWriter->addLine("sample", pathSample.replace("/", "\\"));
    if (!balance->isMono() && balance->getWidth() != 100)
    {
        _sfzWriter->addLine("width", balance->getWidth());
        if (balance->getPosition() != 0)
            _sfzWriter->addLine("position", balance->getPosition());
    }

    // Sample pan and attenuation
    _sfzWriter->addLine("volume", balance->getVolume());
    _sfzWriter->addLine("pan", balance->getPan());

    for (int i = 0; i < listeParam->attributeCount(); i++)
    {
        if (listeParam->getAttribute(i) == champ_initialAttenuation || listeParam->getAttribute(i) == champ_pan)
            continue; // Already processed with the balance
        writeElement(listeParam->getAttribute(i), listeParam->getValue(i));
    }

    // Write the modulators
    for (int i = 0; i < listeParam->modulatorCount(); i++)
        writeModulator(listeParam->getModulator(i));
}

void ConversionSfz::writeElement(AttributeType champ, double value)
{
    switch (champ)
    {
    case champ_fineTune:                _sfzWriter->addLine("tune", qRound(value));                    break;
    case champ_coarseTune:              _sfzWriter->addLine("transpose", qRound(value));               break;
    case champ_scaleTuning:             _sfzWriter->addLine("pitch_keytrack", qRound(value));          break;
    case champ_startloopAddrsOffset:    _sfzWriter->addLine("loop_start", qRound(value));              break;
    case champ_startAddrsOffset:        _sfzWriter->addLine("offset", qRound(value));                  break;
    case champ_endloopAddrsOffset:      _sfzWriter->addLine("loop_end", qRound(value) - 1);            break;
    case champ_endAddrsOffset:          _sfzWriter->addLine("end", qRound(value) - 1);                 break;
    case champ_pan:                     _sfzWriter->addLine("pan", 2 * value);                         break;
    case champ_initialAttenuation:      _sfzWriter->addLine("volume", -value * DB_SF2_TO_REAL_DB);     break;
    case champ_initialFilterQ:          _sfzWriter->addLine("resonance", value);                       break;
    case champ_sustainModEnv:
        _sfzWriter->addLine("fileg_sustain", 100. - value);
        _sfzWriter->addLine("pitcheg_sustain", 100. - value);
        break;
    case champ_delayModEnv:
        _sfzWriter->addLine("pitcheg_delay", value);
        _sfzWriter->addLine("fileg_delay", value);
        break;
    case champ_attackModEnv:
        _sfzWriter->addLine("pitcheg_attack", value);
        _sfzWriter->addLine("fileg_attack", value);
        break;
    case champ_holdModEnv:
        _sfzWriter->addLine("pitcheg_hold", value);
        _sfzWriter->addLine("fileg_hold", value);
        break;
    case champ_decayModEnv:
        _sfzWriter->addLine("pitcheg_decay", value);
        _sfzWriter->addLine("fileg_decay", value);
        break;
    case champ_releaseModEnv:
        _sfzWriter->addLine("pitcheg_release", value);
        _sfzWriter->addLine("fileg_release", value);
        break;
    case champ_modEnvToPitch:           _sfzWriter->addLine("pitcheg_depth", qRound(value));           break;
    case champ_modEnvToFilterFc:        _sfzWriter->addLine("fileg_depth", qRound(value));             break;
    case champ_keynumToModEnvHold:
        _sfzWriter->addLine("pitcheg_holdcc133", value);
        _sfzWriter->addLine("fileg_holdcc133", value);
        break;
    case champ_keynumToModEnvDecay:
        _sfzWriter->addLine("pitcheg_decaycc133", value);
        _sfzWriter->addLine("fileg_decaycc133", value);
        break;
    case champ_delayModLFO:
        _sfzWriter->addLine("amplfo_delay", value);
        _sfzWriter->addLine("fillfo_delay", value);
        break;
    case champ_freqModLFO:
        _sfzWriter->addLine("amplfo_freq", value);
        _sfzWriter->addLine("fillfo_freq", value);
        break;
    case champ_modLfoToVolume:          _sfzWriter->addLine("amplfo_depth", value);                    break;
    case champ_modLfoToFilterFc:        _sfzWriter->addLine("fillfo_depth", value);                    break;
    case champ_modLfoToPitch:           /* IMPOSSIBLE !!! */                                           break;
    case champ_keynum:
        _sfzWriter->addLine("pitch_keycenter", ContextManager::keyName()->getKeyName(qRound(value), false, false, true));
        _sfzWriter->addLine("pitch_keytrack", 0);
        break;
    case champ_reverbEffectsSend:       _sfzWriter->addLine("effect1", value);                         break;
    case champ_chorusEffectsSend:       _sfzWriter->addLine("effect2", value);                         break;
    case champ_delayVolEnv:             _sfzWriter->addLine("ampeg_delay", value);                     break;
    case champ_attackVolEnv:            _sfzWriter->addLine("ampeg_attack", value);                    break;
    case champ_sustainVolEnv:           _sfzWriter->addLine("ampeg_sustain", dbToPercent(value));      break;
    case champ_holdVolEnv:              _sfzWriter->addLine("ampeg_hold", value);                      break;
    case champ_decayVolEnv:             _sfzWriter->addLine("ampeg_decay", value);                     break;
    case champ_keynumToVolEnvHold:      _sfzWriter->addLine("ampeg_holdcc133", value);                 break;
    case champ_keynumToVolEnvDecay:     _sfzWriter->addLine("ampeg_decaycc133", value);                break;
    case champ_releaseVolEnv:           _sfzWriter->addLine("ampeg_release", value);                   break;
    case champ_overridingRootKey:
        _sfzWriter->addLine("pitch_keycenter", ContextManager::keyName()->getKeyName(qRound(value), false, false, true));
        break;
    case champ_delayVibLFO:             _sfzWriter->addLine("pitchlfo_delay", value);                  break;
    case champ_freqVibLFO:              _sfzWriter->addLine("pitchlfo_freq", value);                   break;
    case champ_vibLfoToPitch:           _sfzWriter->addLine("pitchlfo_depth", qRound(value));          break;
    case champ_velocity:
        _sfzWriter->addLine("amp_velcurve_1", value / 127.);
        _sfzWriter->addLine("amp_velcurve_127", value / 127.);
        break;
    case champ_exclusiveClass:
        if (value != 0)
        {
            _sfzWriter->addLine("group", qRound(value));
            _sfzWriter->addLine("off_by", qRound(value));
        }
        break;
    case champ_initialFilterFc:
        _sfzWriter->addLine("fil_type", "lpf_2p");
        _sfzWriter->addLine("cutoff", qRound(value));
        break;
    case champ_keyRange:{
        int lokey = qRound(value / 1000.);
        int hikey = qRound(value - 1000. * qRound(value / 1000.));
        if (lokey != hikey)
        {
            _sfzWriter->addLine("lokey", ContextManager::keyName()->getKeyName(lokey, false, false, true));
            _sfzWriter->addLine("hikey", ContextManager::keyName()->getKeyName(hikey, false, false, true));
        }
        else
            _sfzWriter->addLine("key", ContextManager::keyName()->getKeyName(lokey, false, false, true));
    }break;
    case champ_velRange:{
        int lovel = qRound(value / 1000.);
        int hivel = qRound(value - 1000. * lovel);
        _sfzWriter->addLine("lovel", lovel);
        _sfzWriter->addLine("hivel", hivel);
    }break;
    case champ_sampleModes:
        if (value == 0.)
            _sfzWriter->addLine("loop_mode", "no_loop");
        else if (value == 1.)
            _sfzWriter->addLine("loop_mode", "loop_continuous");
        else if (value == 3.)
            _sfzWriter->addLine("loop_mode", "loop_sustain");
        break;
    default:
        break;
    }
}

void ConversionSfz::writeModulator(ModulatorData modData)
{
    // If the second source of the modulator is not set
    if (!modData.amtSrcOper.CC && modData.amtSrcOper.Index == GC_noController)
    {
        // If the first source is the key velocity
        if (!modData.srcOper.CC && modData.srcOper.Index == GC_noteOnVelocity)
        {
            // Filter cutoff modulated by the velocity
            if (modData.destOper == champ_initialFilterFc)
            {
                // Further checks
                if (modData.srcOper.Type == typeSwitch || modData.srcOper.isBipolar || modData.srcOper.isDescending)
                    return;

                _sfzWriter->addLine("fil_veltrack", modData.amount);
            }

            // Attenuation modulated by the velocity
            if (modData.destOper == champ_initialAttenuation)
            {
                // Further checks
                if (modData.srcOper.Type == typeSwitch || modData.srcOper.isBipolar)
                    return;

                int value = static_cast<int>(modData.amount / 9.6 + 0.5);
                if (!modData.srcOper.isDescending)
                    value = -value;

                _sfzWriter->addLine("amp_veltrack", value);
            }
        }

        // If the first source is the key number
        if (!modData.srcOper.CC && modData.srcOper.Index == GC_noteOnKeyNumber)
        {
            // Filter cutoff modulated by the key number
            if (modData.destOper == champ_initialFilterFc)
            {
                // Further checks
                if (modData.srcOper.Type == typeSwitch)
                    return;

                // Key center and amount
                int amount = modData.amount;
                int keyCenter = 64;
                if (modData.srcOper.isBipolar)
                    amount = modData.srcOper.isDescending ? -amount : amount;
                else
                    keyCenter = modData.srcOper.isDescending ? 128  : 0;

                _sfzWriter->addLine("fil_keytrack", qRound(static_cast<double>(modData.amount) / (modData.srcOper.isBipolar ? 64 : 128)));
                _sfzWriter->addLine("fil_keycenter", keyCenter);
            }
        }
    }
}

QString ConversionSfz::getSamplePath(EltID idSmpl, int &sampleChannel, int &linkedSampleId)
{
    // Sample already processed?
    if (_mapSamples.contains(idSmpl.indexElt))
    {
        linkedSampleId = _stereoIds[idSmpl.indexElt];
        sampleChannel = _mapSamples[idSmpl.indexElt].first;
        return _mapSamples[idSmpl.indexElt].second;
    }

    // Stereo sample?
    SFSampleLink typeSample = _sf2->get(idSmpl, champ_sfSampleType).sfLinkValue;

    // Channel
    sampleChannel = 0;
    if (typeSample == leftSample || typeSample == RomLeftSample || typeSample == linkedSample || typeSample == RomLinkedSample)
        sampleChannel = -1;
    else if (typeSample == rightSample || typeSample == RomRightSample)
        sampleChannel = 1;

    // Find the possible other channel
    linkedSampleId = -1;
    if (sampleChannel != 0)
    {
        linkedSampleId = findOtherChannel(idSmpl);
        if (linkedSampleId == -1)
            sampleChannel = 0;
    }
    _stereoIds[idSmpl.indexElt] = linkedSampleId;
    if (linkedSampleId != -1)
        _stereoIds[linkedSampleId] = idSmpl.indexElt;
    EltID idSmplLinked(elementSmpl, idSmpl.indexSf2, linkedSampleId);

    // Find a unique sample name
    QString name;
    if (linkedSampleId != -1)
    {
        // File name
        QString nom1 = _sf2->getQstr(idSmpl, champ_name);
        QString nom2 = _sf2->getQstr(idSmplLinked, champ_name);
        int nb = SampleUtils::lastLettersToRemove(nom1, nom2);
        name = nom1.left(nom1.size() - nb);
    }
    else
        name = _sf2->getQstr(idSmpl, champ_name);

    name = escapeStr(name);
    QFile file(_dirSamples + "/" + name + ".wav");
    if (file.exists())
    {
        int i = 1;
        while (QFile(_dirSamples + "/" + name + "-" + QString::number(i) + ".wav").exists())
            i++;
        name = name + "-" + QString::number(i);
    }

    // Path of the file
    QString path = QDir(_dirSamples).dirName() + "/" + name + ".wav";
    if (_bankSortEnabled)
        path.prepend("../");
    if (_gmSortEnabled)
        path.prepend("../");

    // Export and save
    SampleWriterWav writer(_dirSamples + "/" + name + ".wav");
    if (linkedSampleId == -1)
    {
        // Mono sample
        writer.write(_sf2->getSound(idSmpl));
        _mapSamples[idSmpl.indexElt] = QPair<int, QString>(0, path);
    }
    else
    {
        // Stereo, left channel first
        if (sampleChannel == 1)
            writer.write(_sf2->getSound(idSmplLinked), _sf2->getSound(idSmpl));
        else
            writer.write(_sf2->getSound(idSmpl), _sf2->getSound(idSmplLinked));

        _mapSamples[idSmpl.indexElt] = QPair<int, QString>(sampleChannel, path);
        _mapSamples[idSmplLinked.indexElt] = QPair<int, QString>(-sampleChannel, path);
    }

    return path;
}

int ConversionSfz::findOtherChannel(EltID idSmpl)
{
    // Linked sample
    EltID idSmplLinked(elementSmpl, idSmpl.indexSf2, _sf2->get(idSmpl, champ_wSampleLink).wValue);

    // Check that the parameters linked to the samples are the same
    if (_sf2->get(idSmpl, champ_dwStartLoop).dwValue == _sf2->get(idSmplLinked, champ_dwStartLoop).dwValue &&
        _sf2->get(idSmpl, champ_dwEndLoop).dwValue == _sf2->get(idSmplLinked, champ_dwEndLoop).dwValue &&
        _sf2->get(idSmpl, champ_dwLength).dwValue == _sf2->get(idSmplLinked, champ_dwLength).dwValue &&
        _sf2->get(idSmpl, champ_chPitchCorrection).cValue == _sf2->get(idSmplLinked, champ_chPitchCorrection).cValue &&
        _sf2->get(idSmpl, champ_byOriginalPitch).bValue == _sf2->get(idSmplLinked, champ_byOriginalPitch).bValue &&
        _sf2->get(idSmpl, champ_dwSampleRate).dwValue == _sf2->get(idSmplLinked, champ_dwSampleRate).dwValue)
        return idSmplLinked.indexElt;

    // Otherwise keep the two channels separate
    return -1;
}

QString ConversionSfz::escapeStr(QString str)
{
    return str.replace(QRegularExpression(QString::fromUtf8("[`~*|:<>«»?/{}\"\\\\]")), "_");
}

int ConversionSfz::lastLettersToRemove(QString str1, QString str2)
{
    str1 = str1.toLower();
    str2 = str2.toLower();
    int nbLetters = 0;

    int size = 0;
    if (str1.size() == str2.size())
        size = str1.size();
    else return 0;

    if (str1.left(size - 2).compare(str2.left(size - 2)) != 0)
        return 0;

    QString fin1_3 = str1.right(3);
    QString fin2_3 = str2.right(3);
    QString fin1_2 = str1.right(2).left(1);
    QString fin2_2 = str2.right(2).left(1);
    QString fin1_1 = str1.right(1);
    QString fin2_1 = str2.right(1);

    if ((fin1_3.compare("(r)") == 0 && fin2_3.compare("(l)") == 0) ||
        (fin1_3.compare("(l)") == 0 && fin2_3.compare("(r)") == 0))
        nbLetters = 3;
    else if (((fin1_1.compare("r") == 0 && fin2_1.compare("l") == 0) ||
              (fin1_1.compare("l") == 0 && fin2_1.compare("r") == 0)) &&
             str1.left(size - 1).compare(str2.left(size - 1)) == 0)
    {
        nbLetters = 1;
        if ((fin1_2.compare("-") == 0 && fin2_2.compare("-") == 0) ||
            (fin1_2.compare("_") == 0 && fin2_2.compare("_") == 0) ||
            (fin1_2.compare(".") == 0 && fin2_2.compare(".") == 0))
            nbLetters = 2;
    }

    return nbLetters;
}

QString ConversionSfz::getDirectoryName(int numPreset)
{
    QString strRet = tr("other");

    if (numPreset < 8)
        strRet = "000-007 " + tr("Piano");
    else if (numPreset < 16)
        strRet = "008-015 " + tr("Chromatic percussion");
    else if (numPreset < 24)
        strRet = "016-023 " + tr("Organ");
    else if (numPreset < 32)
        strRet = "024-031 " + tr("Guitar");
    else if (numPreset < 40)
        strRet = "032-039 " + tr("Bass");
    else if (numPreset < 48)
        strRet = "040-047 " + tr("Strings");
    else if (numPreset < 56)
        strRet = "048-055 " + tr("Ensemble");
    else if (numPreset < 64)
        strRet = "056-063 " + tr("Brass");
    else if (numPreset < 72)
        strRet = "064-071 " + tr("Reed");
    else if (numPreset < 80)
        strRet = "072-079 " + tr("Pipe");
    else if (numPreset < 88)
        strRet = "080-087 " + tr("Synth lead");
    else if (numPreset < 96)
        strRet = "088-095 " + tr("Synth pad");
    else if (numPreset < 104)
        strRet = "096-103 " + tr("Synth effects");
    else if (numPreset < 112)
        strRet = "104-111 " + tr("Ethnic");
    else if (numPreset < 120)
        strRet = "112-119 " + tr("Percussive");
    else if (numPreset < 128)
        strRet = "120-127 " + tr("Sound effects");
    else if (numPreset == 128)
        strRet = tr("Percussion kit");

    return strRet;
}

QString ConversionSfz::getDrumCategory(int numPreset)
{
    QString strRet = tr("other");

    if (numPreset < 8)
        strRet = "000-007 Standard kit";
    else if (numPreset < 16)
        strRet = "008-015 Room kit";
    else if (numPreset < 24)
        strRet = "016-023 Power kit";
    else if (numPreset < 32)
        strRet = "024-031 Electronic kit";
    else if (numPreset < 40)
        strRet = "032-039 Jazz kit";
    else if (numPreset < 48)
        strRet = "040-047 Brush kit";
    else if (numPreset < 56)
        strRet = "048-055 Orchestra kit";
    else if (numPreset < 64)
        strRet = "056-063 Sound FX kit";
    else if (numPreset < 72)
        strRet = "064-071 Additional kit";
    else if (numPreset < 80)
        strRet = "072-079 Additional kit";
    else if (numPreset < 88)
        strRet = "080-087 Additional kit";
    else if (numPreset < 96)
        strRet = "088-095 Additional kit";
    else if (numPreset < 104)
        strRet = "096-103 Additional kit";
    else if (numPreset < 112)
        strRet = "104-111 Additional kit";
    else if (numPreset < 120)
        strRet = "112-119 Additional kit";
    else if (numPreset < 128)
        strRet = "120-127 Additional kit";

    return strRet;
}

bool ConversionSfz::isIncluded(SfzParamList * paramPrst, EltID idInstSmpl)
{
    // Ranges of the preset global division
    int prstMinKey = 0, prstMaxKey = 127, prstMinVel = 0, prstMaxVel = 127;
    int index = paramPrst->findAttribute(champ_keyRange);
    if (index != -1)
    {
        prstMinKey = qRound(paramPrst->getValue(index) / 1000);
        prstMaxKey = qRound(paramPrst->getValue(index) - prstMinKey * 1000);
    }
    index = paramPrst->findAttribute(champ_velRange);
    if (index != -1)
    {
        prstMinVel = qRound(paramPrst->getValue(index) / 1000);
        prstMaxVel = qRound(paramPrst->getValue(index) - prstMinVel * 1000);
    }

    // Ranges of the instrument global division
    int instMinKey = 0, instMaxKey = 127, instMinVel = 0, instMaxVel = 127;
    if (_sf2->isSet(idInstSmpl, champ_keyRange))
    {
        RangesType range = _sf2->get(idInstSmpl, champ_keyRange).rValue;
        instMinKey = range.byLo;
        instMaxKey = range.byHi;
    }
    if (_sf2->isSet(idInstSmpl, champ_velRange))
    {
        RangesType range = _sf2->get(idInstSmpl, champ_velRange).rValue;
        instMinVel = range.byLo;
        instMaxVel = range.byHi;
    }

    return instMinKey <= prstMaxKey && instMaxKey >= prstMinKey &&
           instMinVel <= prstMaxVel && instMaxVel >= prstMinVel;
}

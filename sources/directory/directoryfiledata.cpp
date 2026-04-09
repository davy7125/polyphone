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

#include "directoryfiledata.h"
#include "inputfactory.h"
#include "abstractinputparser.h"
#include "soundfontmanager.h"
#include "soundfonts.h"
#include "soundfont.h"
#include "smpl.h"
#include "instprst.h"
#include "utils.h"

DirectoryFileData::DirectoryFileData(const QFileInfo &fileInfo) :
    _path(fileInfo.absoluteFilePath()),
    _fileSize(fileInfo.size()),
    _lastModified(fileInfo.lastModified()),
    _status(NOT_INITIALIZED)
{
    if (InputFactory::isSuffixSupported(fileInfo.suffix()))
    {
        if (fileInfo.isReadable())
        {
            AbstractInputParser * parser = InputFactory::getInput(_path);
            if (parser->canFastLoad())
                if (scan(parser))
                    _status = OK;
                else
                    _status = CANNOT_SCAN;
            else
                _status = NOT_SCANNABLE;
            SoundfontManager::getInstance()->remove(EltID(elementSf2, parser->getSf2Index()));
            SoundfontManager::getInstance()->revertNewEditing();
            delete parser;
        }
        else
            _status = NOT_READABLE;
    }
    else
        _status = NOT_OPENABLE;
}

bool DirectoryFileData::scan(AbstractInputParser * parser)
{
    parser->process(false);
    if (!parser->isSuccess())
        return false;

    Soundfont * soundfont = SoundfontManager::getInstance()->getSoundfonts()->getSoundfont(parser->getSf2Index());
    QVectorIterator<Smpl*> iteratorSmpl(soundfont->getSamples().values());
    scanSmpl(iteratorSmpl, _samples);
    QVectorIterator<InstPrst*> iteratorInst(soundfont->getInstruments().values());
    scanInstPrst(iteratorInst, _instruments, false);
    QVectorIterator<InstPrst*> iteratorPrst(soundfont->getPresets().values());
    scanInstPrst(iteratorPrst, _presets, true);

    return true;
}


void DirectoryFileData::scanSmpl(QVectorIterator<Smpl*> &i, QList<DirectorySampleData> &list)
{
    while (i.hasNext())
    {
        Smpl * elt = i.next();

        DirectorySampleData data;
        data.id = elt->getId().indexElt;
        data.name = elt->getName();
        data.nameSort = Utils::removeAccents(data.name).toLower();
        data.sampleType = elt->_sfSampleType;

        data.samplingRateHz = elt->_sound.getUInt32(champ_dwSampleRate);
        if (data.samplingRateHz > 0)
        {
            data.totalDurationMilliSec = (1000 * elt->_sound.getUInt32(champ_dwLength)) / data.samplingRateHz;
            quint32 startLoop = elt->_sound.getUInt32(champ_dwStartLoop);
            quint32 endLoop = elt->_sound.getUInt32(champ_dwEndLoop);
            data.loopDurationMilliSec = startLoop < endLoop ? (1000 * (endLoop - startLoop)) / data.samplingRateHz : 0;
        }
        else
        {
            data.totalDurationMilliSec = 0;
            data.loopDurationMilliSec = 0;
        }

        list << data;
    }

    // Sort data
    std::sort(list.begin(), list.end(), [](const DirectorySampleData &a, const DirectorySampleData &b) {
        return Utils::naturalOrder(a.nameSort, b.nameSort) < 0;
    });
}

void DirectoryFileData::scanInstPrst(QVectorIterator<InstPrst*> &i, QList<DirectoryInstrumentPresetData> &list, bool isPrst)
{
    while (i.hasNext())
    {
        InstPrst * elt = i.next();

        // Initialize data
        DirectoryInstrumentPresetData data;
        data.id = elt->getId().indexElt;
        data.name = isPrst ? QString("%1:%2 %3")
                                 .arg(QString::number(elt->getExtraField(champ_wBank)), 3, QChar('0'))
                                 .arg(QString::number(elt->getExtraField(champ_wPreset)), 3, QChar('0'))
                                 .arg(elt->getName())
                           : elt->getName();
        data.nameSort = Utils::removeAccents(data.name).toLower();
        data.numDivisions = 0;
        data.numParameters = 0;
        data.numModulators = 0;
        QMap<int, bool> possibleKeyRanges, possibleVelRanges;

        // Global division
        Division * div = elt->getGlobalDivision();
        bool * sets = div->getAttributeSet();
        for (int pos = 0; pos < champ_endOper; pos++)
        {
            if (sets[pos])
                data.numParameters++;
        }
        data.numModulators += div->getMods().values().count();

        int defaultKeyRange = 127; // 000127
        int defaultVelRange = 127; // 000127
        if (sets[champ_keyRange])
            defaultKeyRange = div->getAttributeValues()[champ_keyRange].wValue;
        if (sets[champ_velRange])
            defaultVelRange = div->getAttributeValues()[champ_velRange].wValue;

        // Divisions linked to another element
        QVectorIterator<Division*> iteratorDiv = elt->getDivisions().values();
        while (iteratorDiv.hasNext())
        {
            div = iteratorDiv.next();
            data.numDivisions++;
            sets = div->getAttributeSet();
            for (int pos = 0; pos < champ_endOper; pos++)
            {
                if (sets[pos])
                    data.numParameters++;
            }
            data.numModulators += div->getMods().values().count();
            possibleKeyRanges[sets[champ_keyRange] ? div->getAttributeValues()[champ_keyRange].wValue : defaultKeyRange] = true;
            possibleVelRanges[sets[champ_velRange] ? div->getAttributeValues()[champ_velRange].wValue : defaultVelRange] = true;
        }
        data.numDistinctKeyRanges = possibleKeyRanges.count();
        data.numDistinctVelocityRanges = possibleVelRanges.count();

        list << data;
    }

    // Sort data
    std::sort(list.begin(), list.end(), [](const DirectoryInstrumentPresetData &a, const DirectoryInstrumentPresetData &b) {
        return Utils::naturalOrder(a.nameSort, b.nameSort) < 0;
    });
}

DirectoryFileData::DetailsData DirectoryFileData::getSampleDetails() const
{
    DetailsData result;
    foreach (DirectorySampleData sample, _samples)
    {
        result.names << sample.name;
        result.details << sample.getDetail();
        result.values << sample.id;
    }
    return result;
}

DirectoryFileData::DetailsData DirectoryFileData::getInstrumentDetails() const
{
    DetailsData result;
    foreach (DirectoryInstrumentPresetData instrument, _instruments)
    {
        result.names << instrument.name;
        result.details << instrument.getDetail();
        result.values << instrument.id;
    }
    return result;
}

DirectoryFileData::DetailsData DirectoryFileData::getPresetDetails() const
{
    DetailsData result;
    foreach (DirectoryInstrumentPresetData preset, _presets)
    {
        result.names << preset.name;
        result.details << preset.getDetail();
        result.values << preset.id;
    }
    return result;
}

QString DirectoryFileData::DirectorySampleData::getDetail()
{
    QString result;
    switch (sampleType)
    {
    case linkInvalid:
        result = QObject::tr("Invalid link");
        break;
    case monoSample: case RomMonoSample:
        result = QObject::tr("Mono", "opposite to stereo");
        break;
    case rightSample: case RomRightSample:
        result = QObject::tr("Stereo right");
        break;
    case leftSample: case RomLeftSample:
        result = QObject::tr("Stereo left");
        break;
    case linkedSample: case RomLinkedSample:
        result = QObject::tr("Stereo non defined");
        break;
    }

    QString secondUnit = QObject::tr("s", "unit for seconds");
    result += ", " + QLocale::system().toString((double)totalDurationMilliSec / 1000, 'f', 3) + " " + secondUnit;
    if (loopDurationMilliSec > 0)
        result += ", " + QObject::tr("loop") + " " + QLocale::system().toString((double)loopDurationMilliSec / 1000, 'f', 3) + " " + secondUnit;

    return result;
}

QString DirectoryFileData::DirectoryInstrumentPresetData::getDetail()
{
    return QObject::tr("%1 division(s): %2 distinct key range(s) and %3 distinct velocity range(s)")
               .arg(numDivisions)
               .arg(numDistinctKeyRanges)
               .arg(numDistinctVelocityRanges) +
           "\n" +
           QObject::tr("%1 parameter(s), %2 modulator(s)").arg(numParameters).arg(numModulators);
}
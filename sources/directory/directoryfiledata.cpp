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
    _lastModified(fileInfo.lastModified())
{
    _isReadable = _isScanned = false;
    _isOpenable = InputFactory::isSuffixSupported(fileInfo.suffix());
    if (_isOpenable)
    {
        _isReadable = fileInfo.isReadable();
        if (_isReadable)
        {
            AbstractInputParser * parser = InputFactory::getInput(_path);
            if (parser->canFastLoad())
                _isScanned = scan(parser);
            SoundfontManager::getInstance()->remove(EltID(elementSf2, parser->getSf2Index()));
            SoundfontManager::getInstance()->revertNewEditing();
            delete parser;
        }
    }
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

DirectoryFileData::DetailsData DirectoryFileData::getSampleDetails(int displayOptions) const
{
    DetailsData result;
    foreach (DirectorySampleData sample, _samples)
    {
        result.texts << sample.getDetails(displayOptions);
        result.values << sample.id;
    }
    return result;
}

DirectoryFileData::DetailsData DirectoryFileData::getInstrumentDetails(int displayOptions) const
{
    DetailsData result;
    foreach (DirectoryInstrumentPresetData instrument, _instruments)
    {
        result.texts << instrument.getDetails(displayOptions, false);
        result.values << instrument.id;
    }
    return result;
}

DirectoryFileData::DetailsData DirectoryFileData::getPresetDetails(int displayOptions) const
{
    DetailsData result;
    foreach (DirectoryInstrumentPresetData preset, _presets)
    {
        result.texts << preset.getDetails(displayOptions, true);
        result.values << preset.id;
    }
    return result;
}

QString DirectoryFileData::DirectorySampleData::getDetails(int displayOptions)
{
    // TODO
    return name;
}

QString DirectoryFileData::DirectoryInstrumentPresetData::getDetails(int displayOptions, bool isPrst)
{
    // TODO
    return name;
}
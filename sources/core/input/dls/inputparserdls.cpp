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

#include "inputparserdls.h"
#include "soundfontmanager.h"
#include "samplereaderdls.h"
#include "dlsarticulation.h"
#include "utils.h"
#include <QFile>
#include <QFileInfo>
#include <QtEndian>

static quint16 readUInt16(const QByteArray &data, int pos)
{
    return pos >= 0 && pos + 2 <= data.size() ? qFromLittleEndian<quint16>(data.constData() + pos) : 0;
}

static quint32 readUInt32(const QByteArray &data, int pos)
{
    return pos >= 0 && pos + 4 <= data.size() ? qFromLittleEndian<quint32>(data.constData() + pos) : 0;
}

InputParserDls::InputParserDls() : AbstractInputParser(),
    _file(nullptr),
    _sm(nullptr)
{}

void InputParserDls::processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int &sf2Index, QString &tempFilePath)
{
    Q_UNUSED(tempFilePath)
    _sm = sm;
    _fileName = fileName;

    // Open and parse the file
    QFile file(fileName);
    if (!file.exists())
    {
        success = false;
        error = tr("Cannot find file \"%1\".").arg(fileName);
        return;
    }
    if (!file.open(QIODevice::ReadOnly))
    {
        success = false;
        error = tr("Access denied for reading file \"%1\".").arg(fileName);
        return;
    }
    _file = &file;
    success = parseFile(error);
    _file = nullptr;
    file.close();
    if (!success)
        return;

    // Create a soundfont
    sf2Index = _sm->add(EltID(elementSf2));
    EltID idSf2(elementSf2, sf2Index);
    QString name = _info.value("INAM");
    if (name.isEmpty())
        name = QFileInfo(fileName).completeBaseName();
    _sm->set(idSf2, champ_name, name);
    _sm->set(idSf2, champ_ICOP, _info.value("ICOP"));
    _sm->set(idSf2, champ_IENG, _info.contains("IENG") ? _info.value("IENG") : _info.value("IART"));
    _sm->set(idSf2, champ_ICRD, _info.value("ICRD"));
    _sm->set(idSf2, champ_IPRD, _info.value("IPRD"));
    _sm->set(idSf2, champ_ISFT, _info.value("ISFT"));
    _sm->set(idSf2, champ_ICMT, _info.contains("ICMT") ? _info.value("ICMT") : QString("Sf2 imported from dls by Polyphone"));

    // Samples
    createSamples(sf2Index);
    linkStereoSamples(sf2Index);

    // Instruments and presets
    foreach (const Instrument &instrument, _instruments)
    {
        QList<Zone> zones;
        foreach (const Region &region, instrument.regions)
            zones << getZones(instrument, region);
        createInstrument(sf2Index, instrument, zones);
    }

    // Possible 24-bit mode
    foreach (const Wave &wave, _waves)
    {
        if (wave.bitsPerSample > 16 && !wave.sampleIds.isEmpty())
        {
            AttributeValue value;
            value.wValue = 24;
            _sm->set(idSf2, champ_wBpsSave, value);
            break;
        }
    }

    success = true;
}

bool InputParserDls::parseFile(QString &error)
{
    char header[12];
    if (_file->read(header, 12) != 12 || memcmp(header, "RIFF", 4) != 0 || memcmp(header + 8, "DLS ", 4) != 0)
    {
        error = tr("This is not a DLS file.");
        return false;
    }
    qint64 end = qMin(static_cast<qint64>(qFromLittleEndian<quint32>(header + 4)) + 8, _file->size());

    // A conditional chunk may exist at the top level: an editor loads the file anyway
    foreach (const Chunk &chunk, readChunks(12, end))
    {
        if (chunk.id == "ptbl")
        {
            QByteArray data = readData(chunk);
            quint32 headerSize = readUInt32(data, 0);
            quint32 count = readUInt32(data, 4);
            for (quint32 i = 0; i < count && headerSize + 4 * i + 4 <= static_cast<quint32>(data.size()); i++)
                _poolTable << readUInt32(data, headerSize + 4 * i);
        }
        else if (chunk.id == "LIST")
        {
            if (chunk.listType == "lins")
            {
                foreach (const Chunk &subChunk, readSubChunks(chunk))
                    if (subChunk.id == "LIST" && subChunk.listType == "ins ")
                        parseInstrument(subChunk);
            }
            else if (chunk.listType == "wvpl")
                parseWavePool(chunk);
            else if (chunk.listType == "INFO")
                parseInfo(chunk, _info);
        }
    }

    return true;
}

QList<InputParserDls::Chunk> InputParserDls::readChunks(qint64 start, qint64 end)
{
    QList<Chunk> chunks;
    qint64 pos = start;
    while (pos + 8 <= end)
    {
        char header[8];
        if (!_file->seek(pos) || _file->read(header, 8) != 8)
            break;

        Chunk chunk;
        chunk.id = QByteArray(header, 4);
        chunk.size = qFromLittleEndian<quint32>(header + 4);
        chunk.dataPosition = pos + 8;
        if (chunk.dataPosition + chunk.size > end)
            chunk.size = static_cast<quint32>(end - chunk.dataPosition); // Truncated chunk
        if (chunk.id == "LIST" || chunk.id == "RIFF")
        {
            if (chunk.size < 4 || _file->read(header, 4) != 4)
                break;
            chunk.listType = QByteArray(header, 4);
        }
        chunks << chunk;

        pos = chunk.dataPosition + chunk.size + (chunk.size % 2);
    }
    return chunks;
}

QList<InputParserDls::Chunk> InputParserDls::readSubChunks(const Chunk &list)
{
    return readChunks(list.dataPosition + 4, list.dataPosition + list.size);
}

QByteArray InputParserDls::readData(const Chunk &chunk)
{
    if (!_file->seek(chunk.dataPosition))
        return QByteArray();
    return _file->read(chunk.size);
}

void InputParserDls::parseInfo(const Chunk &list, QMap<QByteArray, QString> &info)
{
    foreach (const Chunk &chunk, readSubChunks(list))
    {
        if (chunk.id == "LIST")
            continue;

        // Text ending with one or more '\0'
        QByteArray data = readData(chunk);
        int nullIndex = data.indexOf('\0');
        if (nullIndex != -1)
            data.truncate(nullIndex);
        QString text = Utils::isValidUtf8(data) ? QString::fromUtf8(data) : QString::fromLatin1(data);
        info[chunk.id] = text.trimmed();
    }
}

void InputParserDls::parseWavePool(const Chunk &list)
{
    // Offsets of the pool table are relative to the data following "wvpl"
    qint64 base = list.dataPosition + 4;
    foreach (const Chunk &chunk, readSubChunks(list))
        if (chunk.id == "LIST" && chunk.listType == "wave")
            parseWave(chunk, static_cast<quint32>(chunk.dataPosition - 8 - base));
}

void InputParserDls::parseWave(const Chunk &list, quint32 poolOffset)
{
    Wave wave;
    wave.listPosition = static_cast<quint32>(list.dataPosition - 8);
    wave.poolOffset = poolOffset;

    foreach (const Chunk &chunk, readSubChunks(list))
    {
        if (chunk.id == "fmt ")
        {
            QByteArray data = readData(chunk);
            wave.formatTag = readUInt16(data, 0);
            wave.channels = readUInt16(data, 2);
            wave.sampleRate = readUInt32(data, 4);
            wave.blockAlign = readUInt16(data, 12);
            wave.bitsPerSample = readUInt16(data, 14);
        }
        else if (chunk.id == "data")
            wave.dataSize = chunk.size;
        else if (chunk.id == "fact")
        {
            wave.hasFact = true;
            wave.factLength = readUInt32(readData(chunk), 0);
        }
        else if (chunk.id == "wsmp")
            wave.waveSample = parseWaveSample(readData(chunk));
        else if (chunk.id == "LIST" && chunk.listType == "INFO")
        {
            QMap<QByteArray, QString> info;
            parseInfo(chunk, info);
            wave.name = info.value("INAM");
        }
    }

    // Number of frames
    if ((wave.formatTag == 0x0001 || wave.formatTag == 0x0003 || wave.formatTag == 0x0006 ||
         wave.formatTag == 0x0007 || wave.formatTag == 0xFFFE) && wave.blockAlign > 0)
    {
        // PCM, float, A-law, µ-law or extensible
        wave.length = wave.dataSize / wave.blockAlign;
    }
    else if (wave.hasFact)
        wave.length = wave.factLength;
    else
    {
        // Compressed format
        quint16 channels;
        quint32 sampleRate;
        if (SampleReaderDls::readWaveInfo(*_file, wave.listPosition, wave.length, channels, sampleRate) != SampleReader::FILE_OK)
            wave.channels = 0; // Not supported
    }

    _waves << wave;
}

void InputParserDls::parseInstrument(const Chunk &list)
{
    QList<Chunk> chunks = readSubChunks(list);
    if (!isListConditionTrue(chunks))
        return;

    Instrument instrument;
    QList<Dls::Connection> connectionsLevel1, connectionsLevel2;
    foreach (const Chunk &chunk, chunks)
    {
        if (chunk.id == "insh")
        {
            QByteArray data = readData(chunk);
            instrument.bank = readUInt32(data, 4);
            instrument.program = readUInt32(data, 8);
        }
        else if (chunk.id == "LIST")
        {
            if (chunk.listType == "lrgn")
            {
                foreach (const Chunk &subChunk, readSubChunks(chunk))
                    if (subChunk.id == "LIST" && (subChunk.listType == "rgn " || subChunk.listType == "rgn2"))
                        parseRegion(subChunk, instrument);
            }
            else if (chunk.listType == "lart" || chunk.listType == "lar2")
                parseArticulationList(chunk, connectionsLevel1, connectionsLevel2);
            else if (chunk.listType == "INFO")
            {
                QMap<QByteArray, QString> info;
                parseInfo(chunk, info);
                instrument.name = info.value("INAM");
            }
        }
    }

    // The level 2 connections take precedence over the identical level 1 connections
    instrument.connections = connectionsLevel1 + connectionsLevel2;
    _instruments << instrument;
}

void InputParserDls::parseRegion(const Chunk &list, Instrument &instrument)
{
    QList<Chunk> chunks = readSubChunks(list);
    if (!isListConditionTrue(chunks))
        return;

    Region region;
    QList<Dls::Connection> connectionsLevel1, connectionsLevel2;
    foreach (const Chunk &chunk, chunks)
    {
        if (chunk.id == "rgnh")
        {
            QByteArray data = readData(chunk);
            region.keyMin = readUInt16(data, 0);
            region.keyMax = readUInt16(data, 2);
            region.velMin = readUInt16(data, 4);
            region.velMax = readUInt16(data, 6);
            region.keyGroup = readUInt16(data, 10);
        }
        else if (chunk.id == "wsmp")
            region.waveSample = parseWaveSample(readData(chunk));
        else if (chunk.id == "wlnk")
        {
            QByteArray data = readData(chunk);
            region.hasWaveLink = true;
            region.linkOptions = readUInt16(data, 0);
            region.phaseGroup = readUInt16(data, 2);
            region.channel = readUInt32(data, 4);
            region.tableIndex = readUInt32(data, 8);
        }
        else if (chunk.id == "LIST" && (chunk.listType == "lart" || chunk.listType == "lar2"))
        {
            if (parseArticulationList(chunk, connectionsLevel1, connectionsLevel2))
                region.hasArticulation = true;
        }
    }

    if (region.hasWaveLink)
    {
        region.connections = connectionsLevel1 + connectionsLevel2;
        instrument.regions << region;
    }
}

bool InputParserDls::parseArticulationList(const Chunk &list, QList<Dls::Connection> &connectionsLevel1, QList<Dls::Connection> &connectionsLevel2)
{
    QList<Chunk> chunks = readSubChunks(list);
    if (!isListConditionTrue(chunks))
        return false;

    bool found = false;
    foreach (const Chunk &chunk, chunks)
    {
        if (chunk.id == "art1")
        {
            parseArticulation(readData(chunk), true, connectionsLevel1);
            found = true;
        }
        else if (chunk.id == "art2")
        {
            parseArticulation(readData(chunk), false, connectionsLevel2);
            found = true;
        }
    }
    return found;
}

void InputParserDls::parseArticulation(const QByteArray &data, bool isLevel1, QList<Dls::Connection> &connections)
{
    quint32 headerSize = qMax(readUInt32(data, 0), 8u);
    quint32 count = readUInt32(data, 4);
    for (quint32 i = 0; i < count; i++)
    {
        int pos = static_cast<int>(headerSize + 12 * i);
        if (pos + 12 > data.size())
            break;

        Dls::Connection connection;
        connection.source = readUInt16(data, pos);
        connection.control = readUInt16(data, pos + 2);
        connection.destination = readUInt16(data, pos + 4);
        connection.transform = readUInt16(data, pos + 6);
        connection.scale = static_cast<qint32>(readUInt32(data, pos + 8));

        if (isLevel1)
        {
            // Only the output transform exists in DLS level 1, the pitch wheel and the pan being bipolar
            connection.transform &= 0x000F;
            if (connection.source == Dls::SRC_PITCHWHEEL || connection.source == Dls::SRC_CC10)
                connection.transform |= 0x4000;
        }
        connections << connection;
    }
}

InputParserDls::WaveSample InputParserDls::parseWaveSample(const QByteArray &data)
{
    WaveSample waveSample;
    waveSample.isDefined = true;
    quint32 headerSize = qMax(readUInt32(data, 0), 20u);
    waveSample.unityNote = readUInt16(data, 4);
    waveSample.fineTune = static_cast<qint16>(readUInt16(data, 6));
    waveSample.gain = static_cast<qint32>(readUInt32(data, 8));
    quint32 loopCount = readUInt32(data, 16);
    if (loopCount > 0 && headerSize + 16 <= static_cast<quint32>(data.size()))
    {
        // Only the first loop is used
        waveSample.loopType = readUInt32(data, headerSize + 4);
        waveSample.loopStart = readUInt32(data, headerSize + 8);
        waveSample.loopLength = readUInt32(data, headerSize + 12);
        waveSample.hasLoop = waveSample.loopLength > 0;
    }
    return waveSample;
}

bool InputParserDls::isListConditionTrue(const QList<Chunk> &subChunks)
{
    foreach (const Chunk &chunk, subChunks)
        if (chunk.id == "cdl ")
            return isConditionTrue(readData(chunk));
    return true;
}

bool InputParserDls::isConditionTrue(const QByteArray &data)
{
    // Expression in reverse polish notation, evaluated as a DLS level 2 device
    QList<quint32> stack;
    int pos = 0;
    while (pos + 2 <= data.size())
    {
        quint16 opcode = readUInt16(data, pos);
        pos += 2;

        if (opcode == 0x0010) // Constant
        {
            stack << readUInt32(data, pos);
            pos += 4;
        }
        else if (opcode == 0x0011 || opcode == 0x0012) // Query (value or support)
        {
            bool isSupported;
            quint32 value = queryDevice(readUInt32(data, pos), isSupported);
            pos += 16;
            if (opcode == 0x0011)
                stack << (isSupported ? value : 0);
            else
                stack << (isSupported ? 0xFFFFFFFF : 0);
        }
        else if (opcode == 0x000F) // Not
        {
            if (stack.isEmpty())
                return true;
            stack << (stack.takeLast() ? 0 : 0xFFFFFFFF);
        }
        else if (opcode >= 0x0001 && opcode <= 0x000E) // Binary operators
        {
            if (stack.count() < 2)
                return true;
            quint32 x = stack.takeLast();
            quint32 y = stack.takeLast();
            quint32 result = 0;
            switch (opcode)
            {
            case 0x0001: result = x & y; break;
            case 0x0002: result = x | y; break;
            case 0x0003: result = x ^ y; break;
            case 0x0004: result = x + y; break;
            case 0x0005: result = x - y; break;
            case 0x0006: result = x * y; break;
            case 0x0007: result = y == 0 ? 0 : x / y; break;
            case 0x0008: result = (x && y) ? 0xFFFFFFFF : 0; break;
            case 0x0009: result = (x || y) ? 0xFFFFFFFF : 0; break;
            case 0x000A: result = x < y ? 0xFFFFFFFF : 0; break;
            case 0x000B: result = x <= y ? 0xFFFFFFFF : 0; break;
            case 0x000C: result = x > y ? 0xFFFFFFFF : 0; break;
            case 0x000D: result = x >= y ? 0xFFFFFFFF : 0; break;
            case 0x000E: result = x == y ? 0xFFFFFFFF : 0; break;
            default: break;
            }
            stack << result;
        }
        else
            return true; // Unknown opcode: the list is used
    }

    return stack.isEmpty() || stack.last() != 0;
}

quint32 InputParserDls::queryDevice(quint32 queryId, bool &isSupported)
{
    // Queries are identified by the first field of their GUID
    isSupported = true;
    switch (queryId)
    {
    case 0x178f2f27: // Supports DLS level 1
    case 0xf14599e5: // Supports DLS level 2
    case 0x178f2f28: // Sample memory size
        return 0xFFFFFFFF;
    case 0x178f2f24: // GM in hardware
    case 0x178f2f25: // GS in hardware
    case 0x178f2f26: // XG in hardware
    case 0xb03e1181: // Manufacturer ID
    case 0xb03e1182: // Product ID
        return 0;
    case 0x2a91f713: // Sample playback rate
        return 44100;
    default:
        break;
    }
    isSupported = false;
    return 0;
}

int InputParserDls::getWaveIndex(quint32 tableIndex)
{
    if (tableIndex < static_cast<quint32>(_poolTable.count()))
    {
        quint32 offset = _poolTable[tableIndex];
        for (int i = 0; i < _waves.count(); i++)
            if (_waves[i].poolOffset == offset)
                return i;
    }

    // No pool table or wrong offset: the index is the position of the wave
    return tableIndex < static_cast<quint32>(_waves.count()) ? static_cast<int>(tableIndex) : -1;
}

void InputParserDls::createSamples(int sf2Index)
{
    // Pitches and loops used by the regions for each wave, so that the samples have the most frequent ones
    QMap<int, QMap<QPair<int, int>, int> > pitchCounts;
    QMap<int, QMap<QPair<quint32, quint32>, int> > loopCounts;
    foreach (const Instrument &instrument, _instruments)
    {
        foreach (const Region &region, instrument.regions)
        {
            int waveIndex = getWaveIndex(region.tableIndex);
            if (waveIndex == -1)
                continue;
            const WaveSample &waveSample = region.waveSample.isDefined ? region.waveSample : _waves[waveIndex].waveSample;
            pitchCounts[waveIndex][QPair<int, int>(waveSample.unityNote, waveSample.fineTune)]++;
            if (waveSample.hasLoop)
                loopCounts[waveIndex][QPair<quint32, quint32>(waveSample.loopStart, waveSample.loopLength)]++;
        }
    }

    EltID id(elementSmpl, sf2Index);
    AttributeValue value;
    for (int i = 0; i < _waves.count(); i++)
    {
        Wave &wave = _waves[i];
        if (wave.channels == 0)
            continue;

        // Pitch
        QPair<int, int> pitch(wave.waveSample.unityNote, wave.waveSample.fineTune);
        int maxCount = 0;
        foreach (auto key, pitchCounts[i].keys())
        {
            if (pitchCounts[i][key] > maxCount)
            {
                maxCount = pitchCounts[i][key];
                pitch = key;
            }
        }
        wave.rootKey = qBound(0, pitch.first, 127);
        wave.correction = qBound(-99, pitch.second, 99);

        // Loop
        QPair<quint32, quint32> loop(wave.waveSample.loopStart, wave.waveSample.hasLoop ? wave.waveSample.loopLength : 0);
        maxCount = 0;
        foreach (auto key, loopCounts[i].keys())
        {
            if (loopCounts[i][key] > maxCount)
            {
                maxCount = loopCounts[i][key];
                loop = key;
            }
        }
        if (loop.second > 0)
        {
            wave.loopStart = qMin(loop.first, wave.length);
            wave.loopEnd = static_cast<quint32>(qMin(static_cast<qint64>(loop.first) + loop.second, static_cast<qint64>(wave.length)));
        }

        QString name = wave.name.isEmpty() ? QString("wave %1").arg(i + 1) : wave.name;
        for (int channel = 0; channel < wave.channels; channel++)
        {
            id.indexElt = _sm->add(id);
            if (wave.channels == 2)
                _sm->set(id, champ_name, name.left(19) + (channel == 0 ? "L" : "R"));
            else if (wave.channels > 2)
                _sm->set(id, champ_name, name.left(17) + "-" + QString::number(channel + 1));
            else
                _sm->set(id, champ_name, name);
            _sm->set(id, champ_filenameForData, _fileName);

            Sound * sound = _sm->getSound(id);
            value.dwValue = wave.listPosition;
            sound->set(champ_dwStart16, value);
            value.dwValue = 0;
            sound->set(champ_dwStart24, value);
            value.wValue = wave.bitsPerSample > 16 ? 24 : 16;
            sound->set(champ_bpsFile, value);
            value.wValue = wave.channels;
            sound->set(champ_wChannels, value);
            value.wValue = static_cast<quint16>(channel);
            sound->set(champ_wChannel, value);
            value.dwValue = wave.length;
            sound->set(champ_dwLength, value);
            value.dwValue = wave.sampleRate > 0 ? wave.sampleRate : 44100;
            sound->set(champ_dwSampleRate, value);
            value.bValue = static_cast<quint8>(wave.rootKey);
            sound->set(champ_byOriginalPitch, value);
            value.cValue = static_cast<qint8>(wave.correction);
            sound->set(champ_chPitchCorrection, value);
            value.dwValue = wave.loopStart;
            sound->set(champ_dwStartLoop, value);
            value.dwValue = wave.loopEnd;
            sound->set(champ_dwEndLoop, value);
            value.cValue = 0;
            sound->set(champ_rawDataAvailable, value);

            if (wave.channels == 2)
                value.sfLinkValue = (channel == 0 ? leftSample : rightSample);
            else
                value.sfLinkValue = monoSample;
            _sm->set(id, champ_sfSampleType, value);

            wave.sampleIds << id.indexElt;
        }

        // Stereo wave
        if (wave.channels == 2)
        {
            value.wValue = static_cast<quint16>(wave.sampleIds[1]);
            _sm->set(EltID(elementSmpl, sf2Index, wave.sampleIds[0]), champ_wSampleLink, value);
            value.wValue = static_cast<quint16>(wave.sampleIds[0]);
            _sm->set(EltID(elementSmpl, sf2Index, wave.sampleIds[1]), champ_wSampleLink, value);
        }
    }
}

void InputParserDls::linkStereoSamples(int sf2Index)
{
    // Mono waves played together in the left and right channels with the same phase group form a stereo sample
    foreach (const Instrument &instrument, _instruments)
    {
        for (int i = 0; i < instrument.regions.count(); i++)
        {
            const Region &left = instrument.regions[i];
            if (left.phaseGroup == 0 || left.channel != Dls::WAVELINK_CHANNEL_LEFT)
                continue;
            int leftWave = getWaveIndex(left.tableIndex);

            for (int j = 0; j < instrument.regions.count(); j++)
            {
                const Region &right = instrument.regions[j];
                if (right.phaseGroup != left.phaseGroup || right.channel != Dls::WAVELINK_CHANNEL_RIGHT)
                    continue;
                int rightWave = getWaveIndex(right.tableIndex);
                if (leftWave == -1 || rightWave == -1 || leftWave == rightWave ||
                    _waves[leftWave].sampleIds.count() != 1 || _waves[rightWave].sampleIds.count() != 1)
                    continue;

                EltID idLeft(elementSmpl, sf2Index, _waves[leftWave].sampleIds[0]);
                EltID idRight(elementSmpl, sf2Index, _waves[rightWave].sampleIds[0]);
                if (_sm->get(idLeft, champ_sfSampleType).sfLinkValue != monoSample ||
                    _sm->get(idRight, champ_sfSampleType).sfLinkValue != monoSample)
                    continue;

                AttributeValue value;
                value.sfLinkValue = leftSample;
                _sm->set(idLeft, champ_sfSampleType, value);
                value.sfLinkValue = rightSample;
                _sm->set(idRight, champ_sfSampleType, value);
                value.wValue = static_cast<quint16>(idRight.indexElt);
                _sm->set(idLeft, champ_wSampleLink, value);
                value.wValue = static_cast<quint16>(idLeft.indexElt);
                _sm->set(idRight, champ_wSampleLink, value);
                break;
            }
        }
    }
}

QList<InputParserDls::Zone> InputParserDls::getZones(const Instrument &instrument, const Region &region)
{
    QList<Zone> zones;
    int waveIndex = getWaveIndex(region.tableIndex);
    if (waveIndex == -1 || _waves[waveIndex].sampleIds.isEmpty())
        return zones;
    const Wave &wave = _waves[waveIndex];

    // The wave sample of the region replaces the one of the wave, the local articulation replaces the global one
    const WaveSample &waveSample = region.waveSample.isDefined ? region.waveSample : wave.waveSample;
    DlsArticulation articulation(region.hasArticulation ? region.connections : instrument.connections);

    // Channels to play: a multichannel wave is completely played, unless other regions play the other channels
    QList<int> channels;
    bool allChannels = false;
    if (wave.sampleIds.count() == 1)
        channels << 0;
    else
    {
        bool otherChannelPlayed = false;
        foreach (const Region &other, instrument.regions)
        {
            if (&other != &region && getWaveIndex(other.tableIndex) == waveIndex && other.channel != region.channel)
            {
                otherChannelPlayed = true;
                break;
            }
        }
        if (otherChannelPlayed)
            channels << (region.channel == Dls::WAVELINK_CHANNEL_RIGHT ? 1 : 0);
        else
        {
            for (int i = 0; i < wave.sampleIds.count(); i++)
                channels << i;
            allChannels = true;
        }
    }

    AttributeValue value;
    foreach (int channel, channels)
    {
        Zone zone;
        zone.sampleId = wave.sampleIds[channel];

        // Articulation
        QMap<AttributeType, qint16> generators = articulation.getGenerators();
        foreach (AttributeType champ, generators.keys())
        {
            value.shValue = generators[champ];
            zone.generators[champ] = value;
        }
        zone.modulators = articulation.getModulators();

        // Ranges, a velocity range 0-0 meaning that it is not used (DLS level 1)
        value.rValue.byLo = static_cast<quint8>(qMin(region.keyMin, region.keyMax) & 0x7F);
        value.rValue.byHi = static_cast<quint8>(qMin(qMax(region.keyMin, region.keyMax), static_cast<quint16>(127)));
        zone.generators[champ_keyRange] = value;
        if (region.velMax > 0)
        {
            value.rValue.byLo = static_cast<quint8>(qMin(region.velMin, region.velMax) & 0x7F);
            value.rValue.byHi = static_cast<quint8>(qMin(qMax(region.velMin, region.velMax), static_cast<quint16>(127)));
            zone.generators[champ_velRange] = value;
        }

        // Key group
        if (region.keyGroup > 0)
        {
            value.shValue = static_cast<qint16>(qMin(region.keyGroup, static_cast<quint16>(127)));
            zone.generators[champ_exclusiveClass] = value;
        }

        // Root key and tuning
        int unityNote = qBound(0, static_cast<int>(waveSample.unityNote), 127);
        if (unityNote != wave.rootKey)
        {
            value.shValue = static_cast<qint16>(unityNote);
            zone.generators[champ_overridingRootKey] = value;
        }
        int tuning = qRound(waveSample.fineTune + articulation.getTuning()) - wave.correction;
        int coarseTune = qBound(-120, tuning / 100, 120);
        int fineTune = qBound(-99, tuning - 100 * coarseTune, 99);
        if (coarseTune != 0)
        {
            value.shValue = static_cast<qint16>(coarseTune);
            zone.generators[champ_coarseTune] = value;
        }
        if (fineTune != 0)
        {
            value.shValue = static_cast<qint16>(fineTune);
            zone.generators[champ_fineTune] = value;
        }

        // Attenuation in cB (the sf2 attenuations are multiplied by 0.4 in the synthesizers)
        zone.attenuation = -(waveSample.gain / Dls::UNIT_SCALE + articulation.getGain()) / DB_SF2_TO_REAL_DB;

        // Loop
        if (waveSample.hasLoop)
        {
            value.shValue = waveSample.loopType == Dls::WLOOP_TYPE_RELEASE ? 3 : 1;
            zone.generators[champ_sampleModes] = value;
            setOffset(zone, champ_startloopAddrsOffset, champ_startloopAddrsCoarseOffset,
                      static_cast<qint64>(waveSample.loopStart) - wave.loopStart);
            setOffset(zone, champ_endloopAddrsOffset, champ_endloopAddrsCoarseOffset,
                      static_cast<qint64>(waveSample.loopStart) + waveSample.loopLength - wave.loopEnd);
        }

        // Pan of the channels
        if (allChannels && channel < 2)
        {
            value.shValue = (channel == 0 ? -500 : 500);
            zone.generators[champ_pan] = value;
        }
        else if (region.linkOptions & Dls::F_WAVELINK_MULTICHANNEL)
        {
            // The channel replaces the pan of the articulation
            if (region.channel == Dls::WAVELINK_CHANNEL_RIGHT)
            {
                value.shValue = 500;
                zone.generators[champ_pan] = value;
            }
            else if (region.channel == Dls::WAVELINK_CHANNEL_LEFT && region.phaseGroup != 0)
            {
                value.shValue = -500;
                zone.generators[champ_pan] = value;
            }
        }

        zones << zone;
    }

    return zones;
}

void InputParserDls::setOffset(Zone &zone, AttributeType fineChamp, AttributeType coarseChamp, qint64 offset)
{
    AttributeValue value;
    qint64 coarse = offset / 32768;
    qint64 fine = offset - 32768 * coarse;
    if (coarse != 0)
    {
        value.shValue = static_cast<qint16>(qBound(static_cast<qint64>(-32767), coarse, static_cast<qint64>(32767)));
        zone.generators[coarseChamp] = value;
    }
    if (fine != 0)
    {
        value.shValue = static_cast<qint16>(fine);
        zone.generators[fineChamp] = value;
    }
}

void InputParserDls::createInstrument(int sf2Index, const Instrument &instrument, QList<Zone> &zones)
{
    // A positive gain is a negative attenuation, only possible at the preset level
    double minAttenuation = 0;
    foreach (const Zone &zone, zones)
        minAttenuation = qMin(minAttenuation, zone.attenuation);
    int presetAttenuation = qMax(-1440, qRound(minAttenuation));

    // Instrument
    EltID idInst(elementInst, sf2Index);
    int number = _sm->getSiblings(idInst).count() + 1;
    QString name = instrument.name.isEmpty() ? QString("instrument %1").arg(number) : instrument.name;
    idInst.indexElt = _sm->add(idInst);
    _sm->set(idInst, champ_name, name);

    // Modulators common to all divisions are moved in the global division
    QList<ModulatorData> globalModulators;
    if (!zones.isEmpty())
    {
        QList<ModulatorData> firstModulators = zones[0].modulators;
        foreach (const ModulatorData &modulator, firstModulators)
        {
            bool isEverywhere = true;
            for (int i = 1; i < zones.count() && isEverywhere; i++)
            {
                bool found = false;
                foreach (const ModulatorData &other, zones[i].modulators)
                {
                    if (other == modulator && other.amount == modulator.amount)
                    {
                        found = true;
                        break;
                    }
                }
                isEverywhere = found;
            }

            if (isEverywhere)
            {
                globalModulators << modulator;
                for (int i = 0; i < zones.count(); i++)
                {
                    for (int j = 0; j < zones[i].modulators.count(); j++)
                    {
                        if (zones[i].modulators[j] == modulator)
                        {
                            zones[i].modulators.removeAt(j);
                            break;
                        }
                    }
                }
            }
        }
    }

    // Modulators compensating the default modulators of the sf2 format, unless they are already overridden
    foreach (const ModulatorData &modulator, DlsArticulation::getDefaultModulatorCompensation())
        if (!globalModulators.contains(modulator))
            globalModulators << modulator;

    int modulatorIndex = 0;
    writeModulators(_sm, idInst, globalModulators, modulatorIndex);

    // Divisions
    EltID idInstSmpl(elementInstSmpl, sf2Index, idInst.indexElt);
    AttributeValue value;
    foreach (const Zone &zone, zones)
    {
        idInstSmpl.indexElt2 = _sm->add(idInstSmpl);
        value.wValue = static_cast<quint16>(zone.sampleId);
        _sm->set(idInstSmpl, champ_sampleID, value);

        foreach (AttributeType champ, zone.generators.keys())
            if (zone.generators[champ].dwValue != AttributeBase::getDefaultStoredValue(champ, false).dwValue)
                _sm->set(idInstSmpl, champ, zone.generators[champ]);

        value.shValue = static_cast<qint16>(qBound(0, qRound(zone.attenuation) - presetAttenuation, 1440));
        if (value.shValue != 0)
            _sm->set(idInstSmpl, champ_initialAttenuation, value);

        writeModulators(_sm, idInstSmpl, zone.modulators, modulatorIndex);
    }

    // Parameters common to all divisions are moved in the global division
    for (int champ = 0; champ < champ_endOper; champ++)
    {
        switch (champ)
        {
        case champ_keyRange: case champ_velRange: case champ_sampleID: case champ_instrument:
        case champ_unused1: case champ_unused2: case champ_unused3: case champ_unused4: case champ_unused5:
        case champ_reserved1: case champ_reserved2: case champ_reserved3:
            break;
        default:
            _sm->simplify(idInst, static_cast<AttributeType>(champ));
            break;
        }
    }

    // Preset, drums being in the bank 128
    int bank;
    int preset = instrument.program & 0x7F;
    if (instrument.bank & Dls::F_INSTRUMENT_DRUMS)
        bank = 128;
    else
    {
        int msb = (instrument.bank >> 8) & 0x7F;
        int lsb = instrument.bank & 0x7F;
        bank = (msb != 0 ? msb : lsb);
    }
    EltID idSf2(elementSf2, sf2Index);
    if (!_sm->isAvailable(idSf2, static_cast<quint16>(bank), static_cast<quint16>(preset)))
    {
        int availablePreset = _sm->closestAvailablePreset(idSf2, static_cast<quint16>(bank), static_cast<quint16>(preset));
        if (availablePreset != -1)
            preset = availablePreset;
    }

    EltID idPrst(elementPrst, sf2Index);
    idPrst.indexElt = _sm->add(idPrst);
    _sm->set(idPrst, champ_name, name);
    value.wValue = static_cast<quint16>(bank);
    _sm->set(idPrst, champ_wBank, value);
    value.wValue = static_cast<quint16>(preset);
    _sm->set(idPrst, champ_wPreset, value);

    EltID idPrstInst(elementPrstInst, sf2Index, idPrst.indexElt);
    idPrstInst.indexElt2 = _sm->add(idPrstInst);
    value.wValue = static_cast<quint16>(idInst.indexElt);
    _sm->set(idPrstInst, champ_instrument, value);
    if (presetAttenuation < 0)
    {
        value.shValue = static_cast<qint16>(presetAttenuation);
        _sm->set(idPrstInst, champ_initialAttenuation, value);
    }
}

void InputParserDls::writeModulators(SoundfontManager * sm, EltID id, const QList<ModulatorData> &modulators, int &index)
{
    id.typeElement = (id.typeElement == elementInst ? elementInstMod : elementInstSmplMod);
    AttributeValue value;
    foreach (const ModulatorData &modulator, modulators)
    {
        id.indexMod = sm->add(id);
        value.sfModValue = modulator.srcOper;
        sm->set(id, champ_sfModSrcOper, value);
        value.wValue = modulator.destOper;
        sm->set(id, champ_sfModDestOper, value);
        value.shValue = modulator.amount;
        sm->set(id, champ_modAmount, value);
        value.sfModValue = modulator.amtSrcOper;
        sm->set(id, champ_sfModAmtSrcOper, value);
        value.sfTransValue = modulator.transOper;
        sm->set(id, champ_sfModTransOper, value);
        value.wValue = static_cast<quint16>(index++);
        sm->set(id, champ_indexMod, value);
    }
}

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

#include "samplereaderwav.h"
#include "basetypes.h"

SampleReaderWav::SampleReaderWav(QString filename) : SampleReader(filename),
    _info(nullptr),
    _isIeeeFloat(false)
{

}

SampleReaderWav::SampleReaderResult SampleReaderWav::getInfo(QFile &fi, InfoSound &info)
{
    // Keep a track of the info
    _info = &info;

    // Default info
    info.reset();

    // Parse file
    QDataStream in(&fi);

    // "RIFF"
    char char4[5];
    char4[4] = '\0';
    if (in.readRawData(char4, 4) != 4)
        return FILE_CORRUPT;
    if (strcmp("RIFF", char4) != 0)
        return FILE_CORRUPT;

    // Total length of the file
    quint32Reversed val;
    in >> val;
    quint32 fullLength = val.value + 8;
    if (val.value + 8 != fi.size())
        return FILE_CORRUPT;

    // "WAVE"
    if (in.readRawData(char4, 4) != 4)
        return FILE_CORRUPT;
    if (strcmp("WAVE", char4) != 0)
        return FILE_CORRUPT;

    // Read all sections
    quint32 pos = 12; // We already read 12 bytes
    bool smplOk = false;
    bool dataOk = false;
    while (pos < fullLength - 8) // Should be possible to read at least 8 more bytes
    {
        // Section name
        char section[5];
        section[4] = '\0';
        in.readRawData(section, 4);
        pos += 4;

        // Section size
        quint32Reversed sectionSize;
        in >> sectionSize;
        pos += 4;

        if (!strcmp(section, "fmt "))
        {
            // Read format of the audio signal
            if (sectionSize.value < 16 || sectionSize.value > 40)
                return FILE_CORRUPT;

            quint16Reversed tmp16;
            quint32Reversed tmp32;
            in >> tmp16; // (1: PCM, 3: IEEE float)
            _isIeeeFloat = (tmp16.value == 3);
            in >> tmp16;
            info.wChannels = tmp16.value;
            in >> tmp32;
            info.dwSampleRate = tmp32.value;
            in.skipRawData(6); // BytePerSec and BytePerBloc
            in >> tmp16;
            info.wBpsFile = tmp16.value;

            // Skip the rest
            in.skipRawData(static_cast<int>(sectionSize.value) - 16);

            smplOk = true;
        }
        else if (!strcmp(section, "smpl"))
        {
            // Informations about the sample
            if (sectionSize.value >= 36)
            {
                in.skipRawData(12); // Manufacturer, Product, Sample period

                // Root key
                quint32Reversed tmp32;
                in >> tmp32;
                info.dwRootKey = tmp32.value;
                info.pitchDefined = info.dwRootKey > 0 && info.dwRootKey < 128;
                if (info.dwRootKey > 127)
                    info.dwRootKey = 127;
                if (!info.pitchDefined)
                    info.dwRootKey = 60; // back to middle C

                // Tuning
                qint32Reversed tmp32s;
                in >> tmp32s;
                info.iFineTune = qRound(static_cast<double>(tmp32s.value) / 2147483648. * 50.);

                in.skipRawData(16); // SMPTE Format, SMPTE Offset, Num Sample Loops, Sampler Data

                // Loop points defined?
                if (sectionSize.value >= 60)
                {
                    in.skipRawData(8); // Cue Point ID, Type

                    // Loop
                    quint32Reversed tmp32;
                    in >> tmp32;
                    info.dwStartLoop = tmp32.value;
                    in >> tmp32;
                    info.dwEndLoop = tmp32.value + 1;

                    // Skip the rest
                    in.skipRawData(static_cast<int>(sectionSize.value) - 52);
                }
                else
                    in.skipRawData(static_cast<int>(sectionSize.value) - 36);
            }
            else
                in.skipRawData(static_cast<int>(sectionSize.value));
        }
        else if (!strcmp(section, "data"))
        {
            if (sectionSize.value == 0)
                sectionSize.value = fullLength - pos;
            info.dwStart = pos;
            if (info.wBpsFile != 0 && info.wChannels != 0)
                info.dwLength = qMin(sectionSize.value, fullLength - pos) / (info.wBpsFile * info.wChannels / 8);
            dataOk = true;

            // Skip it
            in.skipRawData(static_cast<int>(sectionSize.value));
        }
        else
        {
            // Skip the bloc
            in.skipRawData(static_cast<int>(sectionSize.value));
        }

        // Update position
        pos += sectionSize.value;
    }

    // Check that we have what we need
    if (!dataOk || !smplOk)
        return FILE_CORRUPT;

    return FILE_OK;
}

SampleReaderWav::SampleReaderResult SampleReaderWav::getData16(QFile &fi, QByteArray &smpl)
{
    QByteArray data = loadData(fi);
    quint32 sampleNumber = static_cast<unsigned int>(data.size()) / _info->wChannels / (_info->wBpsFile / 8);
    if (sampleNumber == 0)
        return FILE_CORRUPT;

    // Resize the vector
    smpl.resize(static_cast<int>(sampleNumber) * 2);

    unsigned int bytePerValue = _info->wBpsFile / 8;
    unsigned int bytePerSample = _info->wChannels * bytePerValue;
    unsigned int channelShift =  _info->wChannel * bytePerValue;
    char * dataResult = smpl.data();
    const char * dataSource = data.constData();
    if (bytePerValue == 1)
    {
        for (quint32 i = 0; i < sampleNumber; i++)
        {
            dataResult[2 * i] = 0;
            dataResult[2 * i + 1] = dataSource[bytePerSample * i + channelShift] - 128;
        }
    }
    else
    {
        unsigned int shift = bytePerValue - 2;
        for (quint32 i = 0; i < sampleNumber; i++)
        {
            dataResult[2 * i] = dataSource[shift + bytePerSample * i + channelShift];
            dataResult[2 * i + 1] = dataSource[1 + shift + bytePerSample * i + channelShift];
        }
    }

    return FILE_OK;
}

SampleReaderWav::SampleReaderResult SampleReaderWav::getExtraData24(QFile &fi, QByteArray &sm24)
{
    QByteArray data = loadData(fi);
    quint32 sampleNumber = static_cast<unsigned int>(data.size()) / _info->wChannels / (_info->wBpsFile / 8);
    if (sampleNumber == 0)
        return FILE_CORRUPT;

    // Resize the vector
    sm24.resize(static_cast<int>(sampleNumber));

    unsigned int bytePerValue = _info->wBpsFile / 8;
    unsigned int bytePerSample = _info->wChannels * bytePerValue;
    unsigned int channelShift =  _info->wChannel * bytePerValue;
    char * dataResult = sm24.data();
    const char * dataSource = data.constData();
    if (bytePerValue < 3)
        sm24.fill(0);
    else
    {
        unsigned int shift = bytePerValue - 3;
        for (quint32 i = 0; i < sampleNumber; i++)
            dataResult[i] = dataSource[shift + bytePerSample * i + channelShift];
    }

    return FILE_OK;
}

QByteArray SampleReaderWav::loadData(QFile &fi)
{
    // Skip the headers
    fi.seek(_info->dwStart);

    // Read data
    QByteArray data = fi.read(_info->dwLength * _info->wBpsFile / 8 * _info->wChannels);

    // Possibly convert it from WAVE_FORMAT_IEEE_FLOAT to PCM 32
    if (_isIeeeFloat && _info->wBpsFile == 32)
    {
        float * dataF = reinterpret_cast<float *>(data.data());
        qint32 * data32 = reinterpret_cast<qint32 *>(data.data());
        for (int i = 0; i < data.size() / 4; i++)
            data32[i] = static_cast<qint32>(dataF[i] * 2147483647);
    }

    return data;
}

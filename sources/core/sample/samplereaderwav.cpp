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

#include "samplereaderwav.h"
#include "basetypes.h"
#include "utils.h"

// http://soundfile.sapp.org/doc/WaveFormat/

SampleReaderWav::SampleReaderWav(QString filename) : SampleReader(filename),
    _info(nullptr),
    _isIeeeFloat(false)
{

}

SampleReaderWav::SampleReaderResult SampleReaderWav::getInfo(QFile &fi, InfoSound * info)
{
    // Keep a track of the info
    _info = info;

    // Default info
    info->reset();

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
    if (fullLength != fi.size())
    {
        // Should be corrupt but let's try to read more
        fullLength = fi.size();
        //return FILE_CORRUPT;
    }

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
        quint32Reversed sectionSizeTmp;
        in >> sectionSizeTmp;
        quint32 sectionSize = sectionSizeTmp.value;
        if (sectionSize % 2 == 1)
            sectionSize++; // Only even sizes
        pos += 4;

        if (strcmp(section, "fmt ") == 0)
        {
            // Read format of the audio signal
            if (sectionSize < 16 || sectionSize > 40)
                return FILE_CORRUPT;

            quint16Reversed tmp16;
            quint32Reversed tmp32;
            in >> tmp16; // (1: PCM, 3: IEEE float)
            switch (tmp16.value)
            {
            case 0x0001: // PCM
                _isIeeeFloat = false;
                break;
            case 0x0003: // IEE float
                _isIeeeFloat = true;
                break;
            case 0x0006: // 8-bit ITU-T G.711 A-law
            case 0x0007: // 8-bit ITU-T G.711 µ-law
            case 0xFFFE: // Determined by SubFormat
            default:
                return FILE_NOT_SUPPORTED;
                break;
            }

            in >> tmp16;
            info->wChannels = tmp16.value;
            in >> tmp32;
            info->dwSampleRate = tmp32.value;
            in.skipRawData(6); // BytePerSec and BytePerBloc
            in >> tmp16;
            info->wBpsFile = tmp16.value;

            // Skip the rest
            in.skipRawData(static_cast<int>(sectionSize) - 16);

            smplOk = true;
        }
        else if (strcmp(section, "smpl") == 0)
        {
            // Informations about the sample
            if (sectionSize >= 36)
            {
                in.skipRawData(12); // Manufacturer, Product, Sample period

                // Root key
                quint32Reversed tmp32;
                in >> tmp32;
                info->dwRootKey = tmp32.value;
                info->pitchDefined = info->dwRootKey > 0 && info->dwRootKey < 128;
                if (!info->pitchDefined)
                    info->dwRootKey = 60; // back to middle C

                // Tuning
                quint32Reversed tmp32s;
                in >> tmp32s;
                info->iFineTune = qRound(static_cast<double>(tmp32s.value) / 0x80000000 * 50.);
                if (info->iFineTune > 50)
                {
                    info->iFineTune -= 100;
                    info->dwRootKey += 1;
                }

                in.skipRawData(16); // SMPTE Format, SMPTE Offset, Num Sample Loops, Sampler Data

                // Loop points defined?
                if (sectionSize >= 60)
                {
                    in.skipRawData(8); // Cue Point ID, Type

                    // Loop
                    quint32Reversed loopStart, loopEnd;
                    in >> loopStart >> loopEnd;
                    info->loops << QPair<quint32, quint32>(loopStart.value, loopEnd.value + 1);

                    // Skip the rest
                    in.skipRawData(static_cast<int>(sectionSize) - 52);
                }
                else
                    in.skipRawData(static_cast<int>(sectionSize) - 36);
            }
            else
                in.skipRawData(static_cast<int>(sectionSize));
        }
        else if (strcmp(section, "data") == 0)
        {
            if (sectionSize == 0)
                sectionSize = fullLength - pos;
            info->dwStart = pos;
            if (info->wBpsFile != 0 && info->wChannels != 0)
                info->dwLength = qMin(sectionSize, fullLength - pos) / (info->wBpsFile * info->wChannels / 8);
            dataOk = true;

            // Skip it
            in.skipRawData(static_cast<int>(sectionSize));
        }
        else
        {
            // Skip the bloc
            in.skipRawData(static_cast<int>(sectionSize));
        }

        // Update position
        pos += sectionSize;
    }

    // Check that we have what we need
    if (!dataOk || !smplOk)
        return FILE_CORRUPT;

    return FILE_OK;
}

float * SampleReaderWav::getData(SampleReaderResult &result, QFile &fi)
{
    // Skip the headers
    fi.seek(_info->dwStart);

    // Read data
    QByteArray data = fi.read(_info->dwLength * _info->wBpsFile / 8 * _info->wChannels);
    if (data.size() == 0)
    {
        result = FILE_CORRUPT;
        return nullptr;
    }

    // Resize the vector
    float * fData = new float[_info->dwLength];

    if (_isIeeeFloat && _info->wBpsFile == 32)
    {
        float * dataF = reinterpret_cast<float *>(data.data());

        // Get the maximum value (that can exceed 1)
        float maxValue = 1.0;
        for (int i = 0; i < data.size() / 4; i++)
            if (dataF[i] > maxValue)
                maxValue = dataF[i];

        // Clip and copy data
        float multiplier = 1.0f / maxValue;
        for (quint32 i = 0; i < _info->dwLength; i++)
            fData[i] = dataF[i * _info->wChannels + _info->wChannel] * multiplier;
    }
    else
    {
        // Convert to float
        if (_info->wBpsFile == 8)
        {
            const unsigned char * dataSource = (unsigned char *)data.constData();
            for (quint32 i = 0; i < _info->dwLength; i++)
                fData[i] = Utils::int24ToFloat((dataSource[_info->wChannels * i + _info->wChannel] - 128) * 65536);
        }
        else if (_info->wBpsFile == 16)
        {
            const qint16 * dataSource = (const qint16 *)data.constData();
            for (quint32 i = 0; i < _info->dwLength; i++)
                fData[i] = Utils::int24ToFloat(dataSource[_info->wChannels * i + _info->wChannel] * 256);
        }
        else if (_info->wBpsFile == 32)
        {
            const qint32 * dataSource = (const qint32 *)data.constData();
            for (quint32 i = 0; i < _info->dwLength; i++)
                fData[i] = Utils::int24ToFloat(dataSource[_info->wChannels * i + _info->wChannel] / 256);
        }
        else // 3, 5 or more...
        {
            unsigned int bytePerValue = _info->wBpsFile / 8;
            unsigned int shift = bytePerValue - 3;
            const unsigned char * dataSource = reinterpret_cast<const unsigned char *>(data.constData());
            qint32 tmp;
            for (quint32 i = 0; i < _info->dwLength; i++)
            {
                tmp = dataSource[(_info->wChannels * i + _info->wChannel) * bytePerValue + shift + 2];
                tmp = (tmp << 8) | dataSource[(_info->wChannels * i + _info->wChannel) * bytePerValue + shift + 1];
                tmp = (tmp << 8) | dataSource[(_info->wChannels * i + _info->wChannel) * bytePerValue + shift];
                if (tmp & 0x800000)
                    tmp |= 0xff000000;
                fData[i] = Utils::int24ToFloat(tmp);
            }
        }
    }

    result = FILE_OK;
    return fData;
}

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

#include "sf2sdtapart.h"
#include <QDataStream>
#include "sound.h"
#include <vorbis/vorbisenc.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

const quint32 Sf2SdtaPart::BLOCK_SIZE = 1024;

Sf2SdtaPart::Sf2SdtaPart() :
    _isValid(false),
    _sdtaSize(0),
    _smplSize(0),
    _sm24Size(0),
    _startSmplOffset(0),
    _startSm24Offset(0),
    _sample24bits(0)
{

}

void skipData(QDataStream &in, unsigned int number)
{
    // number can be more than the maximum of an int
    int bigSkip = number / 0x7FFFFFFF;
    for (int i = 0; i < bigSkip; i++)
        in.skipRawData(0x7FFFFFFF);
    in.skipRawData(number % 0x7FFFFFFF);
}

QDataStream & operator >> (QDataStream &in, Sf2SdtaPart &sdta)
{
    // 4 char, should be "LIST"
    if (in.readRawData(sdta._LIST, 4) != 4)
        return in;

    // Size of the section "sdta"
    in >> sdta._sdtaSize;

    // 4 char, should be "sdta"
    if (in.readRawData(sdta._sdta, 4) != 4)
        return in;

    // Next block: can be directly the samples or smpl
    QString block;
    {
        char buffer[4];
        if (in.readRawData(buffer, 4) != 4)
            return in;
        block = QString::fromLatin1(buffer, 4);
    }
    if (block == "smpl")
    {
        // Size of the block "smpl"
        in >> sdta._smplSize;

        // Keep the position of "smpl" and skip the block
        sdta._startSmplOffset = 20;
        skipData(in, sdta._smplSize.value);

        // Block sm24?
        {
            char buffer[4];
            if (in.readRawData(buffer, 4) != 4)
                return in;
            block = QString::fromLatin1(buffer, 4);
        }
        if (block == "sm24")
        {
            // Size of the block "sm24"
            in >> sdta._sm24Size;

            // Keep the position of "sm24" and skip the block
            sdta._startSm24Offset = 20 + sdta._smplSize.value + 8;
            skipData(in, sdta._sm24Size.value);
        }
        else
        {
            // No "sm24" block, we go back
            QIODevice * fi = in.device();
            fi->seek(fi->pos() - 4);

            // No "sm24" position
            sdta._startSm24Offset = 0;
        }
    }
    else
    {
        // Memorize the positions and skip the part
        sdta._startSmplOffset = 12;
        sdta._startSm24Offset = 0;
        skipData(in, sdta._sdtaSize.value - 8);
    }

    sdta._isValid = true;
    return in;
}

quint32 Sf2SdtaPart::prepareBeforeWritingData(bool isSf3, double qualityValue)
{
    _isSf3 = isSf3;

    // Constant strings
    memcpy(_LIST, "LIST", 4);
    memcpy(_sdta, "sdta", 4);

    // Sizes
    _smplSize.value = 0;
    _sm24Size.value = 0;
    foreach (Sound * sound, _sounds)
    {
        if (isSf3 && (qualityValue > 0 || sound->isRawDataUnchanged()))
        {
            if (!sound->isRawDataUnchanged())
            {
                // Compress data for knowing the size
                if (!compressSample(sound, qualityValue))
                    return 0;
            }

            // Read the compress size
            char * rawData;
            quint32 rawDataLength;
            sound->getRawData(rawData, rawDataLength);
            _smplSize.value += rawDataLength;

            // Possible padding
            if (rawDataLength % 4 != 0)
            {
                int padNumber = 4 - (rawDataLength % 4);
                _smplSize.value += padNumber;
            }
        }
        else
            _smplSize.value += 2 * (sound->getUInt32(champ_dwLength) + 46); // Extra '0' required by the format
    }
    _sdtaSize.value = _smplSize.value + 12;

    if (_sample24bits)
    {
        _sm24Size.value = _smplSize.value / 2;
        _sm24Size.value += (_sm24Size.value % 2); // Even size
        _sdtaSize.value += _sm24Size.value + 8;
    }

    return 8 + _sdtaSize.value;
}

QDataStream & operator << (QDataStream &out, Sf2SdtaPart &sdta)
{
    // "LIST"
    if (out.writeRawData(sdta._LIST, 4) != 4)
        return out;

    // Size of the section "pdta"
    out << sdta._sdtaSize;

    // "sdta"
    if (out.writeRawData(sdta._sdta, 4) != 4)
        return out;

    // "smpl" with its size
    if (out.writeRawData("smpl", 4) != 4)
        return out;
    out << sdta._smplSize;

    // 16-bit part
    quint32 sampleLength;
    qint16 * data16;
    quint8 * data24;
    AttributeValue val;
    val.dwValue = sdta._position + 5 * 4;
    foreach (Sound * sound, sdta._sounds)
    {
        // Raw data is preferred if available (compressed sf3 data)
        char * rawData = nullptr;
        quint32 rawDataLength = 0;
        sound->getRawData(rawData, rawDataLength);
        if (sdta._isSf3 && rawDataLength > 0)
        {
            if (out.writeRawData(rawData, rawDataLength) != rawDataLength)
                return out;
            sound->set(champ_dwStart16, val);
            val.dwValue += rawDataLength;

            // Possible padding
            if (rawDataLength % 4 != 0)
            {
                int padNumber = 4 - (rawDataLength % 4);
                for (int i = 0; i < padNumber; i++)
                    out.writeRawData("\0", 1);
                val.dwValue += padNumber;
            }
        }
        else
        {
            sound->getData(sampleLength, data16, data24, false, false);
            sampleLength *= sizeof(qint16);
            if (out.writeRawData((char*)data16, sampleLength) != sampleLength)
                return out;
            sound->set(champ_dwStart16, val);

            // Extra 46 values
            for (int i = 0; i < 46; i++)
                out.writeRawData("\0\0", 2);

            val.dwValue += sampleLength + 46 * 2;
        }
    }

    // Possible extra 8-bit part
    if (sdta._sample24bits)
    {
        // "sm24" with its size
        if (out.writeRawData("sm24", 4) != 4)
            return out;
        out << sdta._sm24Size;

        val.dwValue += 2 * 4;
        foreach (Sound * sound, sdta._sounds)
        {
            sound->getData(sampleLength, data16, data24, false, false);
            if (out.writeRawData((char*)data24, sampleLength) != sampleLength)
                return out;
            sound->set(champ_dwStart24, val);

            // Extra 46 values
            for (int i = 0; i < 46; i++)
                out.writeRawData("\0", 1);

            val.dwValue += sampleLength + 46;
        }

        // Possible ending 0 (for an even number of bytes)
        if (val.dwValue % 2)
            out.writeRawData("\0", 1);
    }

    sdta._isValid = true;
    return out;
}

bool Sf2SdtaPart::compressSample(Sound* sound, double quality)
{
    // 16-bit data
    quint32 sampleLength;
    qint16* data16;
    quint8* data24;
    sound->getData(sampleLength, data16, data24, false, false);

    ogg_stream_state os;
    ogg_page         og;
    ogg_packet       op;
    vorbis_info      vi;
    vorbis_dsp_state vd;
    vorbis_block     vb;
    vorbis_comment   vc;

    vorbis_info_init(&vi);
    if (vorbis_encode_init_vbr(&vi, 1, sound->getUInt32(champ_dwSampleRate), quality) != 0)
        return false; // Failed to initialize vorbis

    vorbis_comment_init(&vc);
    vorbis_analysis_init(&vd, &vi);
    vorbis_block_init(&vd, &vb);
    srand(time(nullptr));
    ogg_stream_init(&os, rand());

    ogg_packet header;
    ogg_packet header_comm;
    ogg_packet header_code;

    vorbis_analysis_headerout(&vd, &vc, &header, &header_comm, &header_code);
    ogg_stream_packetin(&os, &header);
    ogg_stream_packetin(&os, &header_comm);
    ogg_stream_packetin(&os, &header_code);

    QByteArray obuf;
    obuf.reserve(1024 * 1024);

    auto append_page = [&](const ogg_page &pg){
        obuf.append(reinterpret_cast<const char*>(pg.header), pg.header_len);
        obuf.append(reinterpret_cast<const char*>(pg.body),   pg.body_len);
    };

    while (ogg_stream_flush(&os, &og) != 0)
        append_page(og);

    quint32 page = 0;
    quint32 max = 0;
    do
    {
        int bufflength = qMin(Sf2SdtaPart::BLOCK_SIZE, sampleLength - page * Sf2SdtaPart::BLOCK_SIZE);
        float **buffer = vorbis_analysis_buffer(&vd, bufflength);

        int j = 0;
        float coef = 1.0f / ((0.5f + 0x7fff) * 1.009f);
        max = qMin((page + 1) * Sf2SdtaPart::BLOCK_SIZE, sampleLength);

        for (quint32 i = page * Sf2SdtaPart::BLOCK_SIZE; i < max; i++)
            buffer[0][j++] = (0.5f + data16[i]) * coef;

        vorbis_analysis_wrote(&vd, bufflength);

        while (vorbis_analysis_blockout(&vd, &vb) == 1)
        {
            vorbis_analysis(&vb, nullptr);
            vorbis_bitrate_addblock(&vb);

            while (vorbis_bitrate_flushpacket(&vd, &op))
            {
                ogg_stream_packetin(&os, &op);
                while (ogg_stream_pageout(&os, &og) != 0)
                    append_page(og);
            }
        }

        page++;
    }
    while (max != sampleLength);

    vorbis_analysis_wrote(&vd, 0);

    while (vorbis_analysis_blockout(&vd, &vb) == 1)
    {
        vorbis_analysis(&vb, nullptr);
        vorbis_bitrate_addblock(&vb);

        while (vorbis_bitrate_flushpacket(&vd, &op))
        {
            ogg_stream_packetin(&os, &op);
            while (ogg_stream_pageout(&os, &og) != 0)
                append_page(og);
        }
    }

    ogg_stream_clear(&os);
    vorbis_block_clear(&vb);
    vorbis_dsp_clear(&vd);
    vorbis_comment_clear(&vc);
    vorbis_info_clear(&vi);

    sound->setRawData(obuf.data(), obuf.size());
    return true;
}

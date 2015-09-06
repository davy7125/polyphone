/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 2 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**          Authors: Davy Triponney                                       **
**                   Werner Schweer (MuseScore)                           **
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 05.09.2015                                           **
***************************************************************************/

#include "oggconverter.h"
#include "vorbis/codec.h"
#include "vorbis/vorbisenc.h"
#include "vorbis/vorbisfile.h"

struct VorbisData {
    int pos;          // current position in audio->data()
    QByteArray data;
};

static VorbisData vorbisData;

static size_t ovRead(void* ptr, size_t size, size_t nmemb, void* datasource);
static int ovSeek(void* datasource, ogg_int64_t offset, int whence);
static long ovTell(void* datasource);

static ov_callbacks ovCallbacks = { ovRead, ovSeek, 0, ovTell };

//---------------------------------------------------------
//   ovRead
//---------------------------------------------------------

static size_t ovRead(void* ptr, size_t size, size_t nmemb, void* datasource)
{
    VorbisData* vd = (VorbisData*)datasource;
    size_t n = size * nmemb;
    if (vd->data.size() < int(vd->pos + n))
        n = vd->data.size() - vd->pos;
    if (n) {
        const char* src = vd->data.data() + vd->pos;
        memcpy(ptr, src, n);
        vd->pos += n;
    }

    return n;
}

//---------------------------------------------------------
//   ovSeek
//---------------------------------------------------------

static int ovSeek(void* datasource, ogg_int64_t offset, int whence)
{
    VorbisData* vd = (VorbisData*)datasource;
    switch(whence) {
    case SEEK_SET:
        vd->pos = offset;
        break;
    case SEEK_CUR:
        vd->pos += offset;
        break;
    case SEEK_END:
        vd->pos = vd->data.size() - offset;
        break;
    }
    return 0;
}

//---------------------------------------------------------
//   ovTell
//---------------------------------------------------------

static long ovTell(void* datasource)
{
    VorbisData* vd = (VorbisData*)datasource;
    return vd->pos;
}

OggConverter::OggConverter(QByteArray array)
{
    vorbisData.pos  = 0;
    vorbisData.data = array;
}

void OggConverter::convert()
{
    _decodedData.clear();
    OggVorbis_File vf;
    vorbisData.pos  = 0;
    if (ov_open_callbacks(&vorbisData, &vf, 0, 0, ovCallbacks) == 0)
    {
        char buffer[4096];
        int numberRead = 0;
        int section = 0;
        do {
            numberRead = ov_read(&vf, buffer, 4096, 0, 2, 1, &section);
            for (int i = 0; i < numberRead; i++)
                _decodedData.append(buffer[i]);
        } while (numberRead);

        ov_clear(&vf);
    }
}

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

#ifndef SAMPLEREADER_H
#define SAMPLEREADER_H

#include <QFile>
#include "infosound.h"

class SampleReader
{
public:
    enum SampleReaderResult {
        FILE_OK,
        FILE_NOT_FOUND,
        FILE_NOT_READABLE,
        FILE_CORRUPT
    };

    SampleReader(QString filename) :
        _filename(filename),
        _result(FILE_OK)
    {

    }
    virtual ~SampleReader() {}

    // Extract general information (sampling rate, ...)
    SampleReaderResult getInfo(InfoSound &info)
    {
        if (_result != FILE_OK)
            return _result;

        QFile fi (_filename);
        if (fi.exists())
        {
            if (fi.open(QFile::ReadOnly | QFile::Unbuffered))
            {
                _result = getInfo(fi, info);
                fi.close();
            }
            else
                _result = FILE_NOT_READABLE;
        }
        else
            _result = FILE_NOT_FOUND;

        return _result;
    }

    // Get sample data (16 bits)
    SampleReaderResult getData16(QByteArray &smpl)
    {
        if (_result != FILE_OK)
            return _result;

        QFile fi (_filename);
        if (fi.exists())
        {
            if (fi.open(QFile::ReadOnly | QFile::Unbuffered))
            {
                _result = getData16(fi, smpl);
                fi.close();
            }
            else
                _result = FILE_NOT_READABLE;
        }
        else
            _result = FILE_NOT_FOUND;

        return _result;
    }

    // Get sample data (extra 8 bits)
    SampleReaderResult getExtraData24(QByteArray &sm24)
    {
        if (_result != FILE_OK)
            return _result;

        QFile fi (_filename);
        if (fi.exists())
        {
            if (fi.open(QFile::ReadOnly | QFile::Unbuffered))
            {
                _result = getExtraData24(fi, sm24);
                fi.close();
            }
            else
                _result = FILE_NOT_READABLE;
        }
        else
            _result = FILE_NOT_FOUND;

        return _result;
    }

protected:
    virtual SampleReaderResult getInfo(QFile &fi, InfoSound &info) = 0;
    virtual SampleReaderResult getData16(QFile &fi, QByteArray &smpl) = 0;
    virtual SampleReaderResult getExtraData24(QFile &fi, QByteArray &sm24) = 0;

private:
    QString _filename;
    SampleReaderResult _result;
};

#endif // SAMPLEREADER_H

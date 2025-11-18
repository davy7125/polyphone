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

#ifndef SOUND_H
#define SOUND_H

#include "basetypes.h"
#include "infosound.h"

class QFile;
class QWidget;
class SampleReader;

class Sound
{
public:
    Sound();
    ~Sound();

    // Get information about the sample loaded
    QString getError() { return _error; }
    QString getFileName() { return _fileName; }
    void getData(quint32 &sampleLength, qint16* &data16, quint8* &data24, bool forceReload, bool getCopy);
    QVector<float> getDataFloat(bool forceReload);
    quint32 getUInt32(AttributeType champ); // For everything but the pitch correction
    qint32 getInt32(AttributeType champ); // For the pitch correction

    // Set data
    void set(AttributeType champ, AttributeValue value);
    bool setFileName(QString qStr, bool tryFindRootKey = true);
    void setDataFloat(QVector<float> data);
    void loadSample(bool forceReload = false);

    // Raw data management
    bool isRawDataObsolete();
    void getRawData(char *& rawData, quint32 &rawDataLength);

private:
    QString _fileName;
    QString _error;
    InfoSound _info;
    qint16* _data16;
    quint8* _data24;
    char* _rawData;
    quint32 _rawDataLength;
    SampleReader* _reader;

    void setData(qint16* data16, quint8* data24, quint32 length);
    void determineRootKey();
    void decompressData();
};

#endif // SOUND_H

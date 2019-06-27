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
    Sound(QString filename = "", bool tryFindRootkey = true);
    ~Sound();

    // Get information about the sample loaded
    InfoSound getInfo() { return _info; }
    QString getFileName() { return this->_fileName; }
    QByteArray getData(quint16 wBps);
    quint32 getUInt32(AttributeType champ); // For everything but the pitch correction
    qint32 getInt32(AttributeType champ); // For the pitch correction

    // Set data
    void set(AttributeType champ, AttributeValue value);
    void setFileName(QString qStr, bool tryFindRootKey = true);
    void setData(QByteArray data, quint16 wBps);
    void setRam(bool ram);

private:
    QString _fileName;
    InfoSound _info;
    QByteArray _smpl;
    QByteArray _sm24;
    SampleReader * _reader;

    void determineRootKey();
};

#endif // SOUND_H

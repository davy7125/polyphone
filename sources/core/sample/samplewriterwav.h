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

#ifndef SAMPLEWRITERWAV_H
#define SAMPLEWRITERWAV_H

#include "basetypes.h"
#include "sound.h"

class SampleWriterWav
{
public:
    SampleWriterWav(QString fileName);

    void write(Sound * sound);
    void write(Sound * leftSound, Sound * rightSound);

private:
    void write(QByteArray &baData, InfoSound &info);
    static void resample(int );
    static QByteArray convertTo16bit(qint16 * data16, quint32 length);
    static QByteArray convertTo24bit(qint16 * data16, quint8 * data24, quint32 length);
    static QByteArray from2MonoTo1Stereo(QByteArray baData1, QByteArray baData2, quint16 wBps);

    QString _fileName;
};

#endif // SAMPLEWRITERWAV_H

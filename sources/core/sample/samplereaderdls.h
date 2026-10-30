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

#ifndef SAMPLEREADERDLS_H
#define SAMPLEREADERDLS_H

#include "samplereader.h"

// Read a wave of a DLS file, the position of its "LIST wave" chunk being dwStart
class SampleReaderDls: public SampleReader
{
public:
    SampleReaderDls(QString filename);
    ~SampleReaderDls() override {}

    // Number of frames, channels and sample rate of a wave, when they cannot be computed from the format
    static SampleReaderResult readWaveInfo(QFile &fi, quint32 waveListPosition, quint32 &length, quint16 &channels, quint32 &sampleRate);

protected:
    SampleReaderResult getInfo(QFile &fi, InfoSound *info) override;
    SampleReaderResult getRawData(QFile &fi, char* &rawData, quint32 &length) override;
    SampleReaderResult getData(QFile &fi, qint16* &data16, quint8* &data24, const char* rawData, quint32 rawDataLength) override;

private:
    // Build a wav file in memory with the chunks "fmt ", "fact" and "data" of a wave
    static SampleReaderResult extractWaveFile(QFile &fi, quint32 waveListPosition, QByteArray &waveFile);

    InfoSound * _info;
};

#endif // SAMPLEREADERDLS_H

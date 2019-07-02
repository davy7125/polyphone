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

#include "samplereaderfactory.h"
#include "samplereaderwav.h"
#include "samplereadersf2.h"
#include "samplereaderflac.h"
#include <QFileInfo>

SampleReader * SampleReaderFactory::getSampleReader(QString filename)
{
    QFileInfo fileInfo(filename);
    QString ext = fileInfo.suffix().toLower();

    // Soundfont?
    if (ext.compare("sf2") == 0 || ext.compare("sf3") == 0)
        return new SampleReaderSf2(filename);

    // Wav file?
    if (ext.compare("wav") == 0)
        return new SampleReaderWav(filename);

    // Flac file?
    if (ext.compare("flac") == 0)
        return new SampleReaderFlac(filename);

    // TODO: check if there is a library for reading extra formats

    return nullptr;
}

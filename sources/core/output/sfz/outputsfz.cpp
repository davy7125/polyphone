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

#include "outputsfz.h"
#include "soundfontmanager.h"
#include "conversion_sfz.h"

OutputSfz::OutputSfz() : AbstractOutput() {}

void OutputSfz::processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int sf2Index, QMap<QString, QVariant> & options)
{
    Q_UNUSED(sm)

    // Directory (possibly remove the extension .sfz)
    if (fileName.endsWith(".sfz"))
        fileName = QFileInfo(fileName).absolutePath() + "/" + QFileInfo(fileName).completeBaseName();

    // Options
    bool presetPrefix = options["prefix"].toBool();
    bool bankDir = options["bankdir"].toBool();
    bool gmSort = options["gmsort"].toBool();

    // Convert
    error = ConversionSfz().convert(fileName, EltID(elementSf2, sf2Index), presetPrefix, bankDir, gmSort);
    success = error.isEmpty();
}

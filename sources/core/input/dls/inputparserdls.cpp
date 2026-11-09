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

#include "inputparserdls.h"
#include <QFile>
#include "soundfontmanager.h"
#include "soundfontmanagerbuilder.h"
#include "sf2filesource.h"
#include "dlsreader.h"

InputParserDls::InputParserDls() : AbstractInputParser() {}

void InputParserDls::processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int &sf2Index, QString &tempFilePath)
{
    Q_UNUSED(tempFilePath)

    // Open and read the file
    QFile file(fileName);
    if (!file.exists())
    {
        success = false;
        error = tr("Cannot find file \"%1\".").arg(fileName);
        return;
    }
    if (!file.open(QIODevice::ReadOnly))
    {
        success = false;
        error = tr("Access denied for reading file \"%1\".").arg(fileName);
        return;
    }

    Sf2FileSource source(file);
    SoundfontManagerBuilder builder(sm, -1);
    DlsReader reader;
    success = reader.read(source, fileName.toStdString(), builder);
    file.close();
    if (success)
        sf2Index = builder.getSf2Index();
    else
        error = tr("This is not a DLS file.");
}

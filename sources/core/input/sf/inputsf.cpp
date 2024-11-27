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

#include "inputsf.h"
#include "inputparsersf2.h"
#include "inputparsersf3.h"
#include "sf2header.h"

AbstractInputParser * InputSf::getParser(QString filename)
{
    // Find the right parser depending on the sf version
    QFile fi(filename);
    if (!fi.exists())
        return nullptr;

    if (!fi.open(QIODevice::ReadOnly))
        return nullptr;

    // Parse the header of the file
    QDataStream stream(&fi);
    Sf2Header header;
    stream >> header;

    // Close the file
    fi.close();

    return header.getVersion("ifil").wMajor > 2 ?
               (AbstractInputParser *)(new InputParserSf3()) :
               (AbstractInputParser *)(new InputParserSf2());
}

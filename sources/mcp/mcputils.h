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

#ifndef MCPUTILS_H
#define MCPUTILS_H

#include <QJsonObject>
#include <QJsonArray>
#include "basetypes.h"

// Conversions shared by the MCP functions
// Identifiers of the elements are strings: "sf0", "sf0/smpl3", "sf0/inst2", "sf0/inst2/div5", "sf0/inst2/mod1",
// "sf0/inst2/div5/mod0", "sf0/prst1", "sf0/prst1/div0", ...
class McpUtils
{
public:
    // Identifier of an element
    static QString idToString(EltID id);

    // Element of an open soundfont from its identifier, throw an McpException if not valid or not allowed
    static EltID readId(QString text, QList<ElementType> allowedTypes = QList<ElementType>());

    // Name of the type of an element ("sample", "instrument", "instrument_division", ...)
    static QString typeName(ElementType type);

    // Name of a key, 60 being "C4"
    static QString keyName(int key);

    // Key from a number or a name such as "C#4", throw an McpException if not valid
    static int readKey(QJsonValue value, QString argumentName);

    // Identifier and name of an element
    static QJsonObject brief(EltID id);

    // Indexes of the open soundfonts (temporary soundfonts of the explorer excluded)
    static QList<int> getSoundfontIndexes();
};

#endif // MCPUTILS_H

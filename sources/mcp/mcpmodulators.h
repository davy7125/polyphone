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

#ifndef MCPMODULATORS_H
#define MCPMODULATORS_H

#include <QJsonObject>
#include <QJsonArray>
#include "basetypes.h"

// Modulators of the instruments, presets and divisions in a readable form for the AI
class McpModulators
{
public:
    // Modulators of an instrument, preset (global) or division
    static QJsonArray getAll(EltID idParent);

    // Description of a modulator
    static QJsonObject toJson(EltID idMod);

    // Add a modulator to an instrument, preset or division, return its identifier
    static EltID add(EltID idParent, QJsonObject description);

    // Change the specified fields of a modulator
    static void update(EltID idMod, QJsonObject description);

    // Text describing the fields of a modulator (for the function descriptions)
    static const char * FORMAT;

private:
    static EltID getModId(EltID idParent, int indexMod);
    static QJsonValue sourceToJson(SFModulator source);
    static SFModulator readSource(QJsonValue value, QString fieldName);
    static QString nativeUnit(AttributeType champ);
};

#endif // MCPMODULATORS_H

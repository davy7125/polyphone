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

#ifndef MCPATTRIBUTES_H
#define MCPATTRIBUTES_H

#include <QJsonObject>
#include <QJsonArray>
#include "basetypes.h"

// Parameters of the instruments, presets and their divisions, named and expressed in real units for the AI
class McpAttributes
{
public:
    // Description of all parameters
    static QJsonArray getCatalog();

    // Parameters explicitly set in an instrument / preset (global division) or in a division
    static QJsonObject getValues(EltID id);

    // Value of a parameter that is set, null otherwise
    static QJsonValue getValue(EltID id, QString name);

    // Change or remove a parameter, throw an McpException if the name or the value is not valid
    static void setValue(EltID id, QString name, QJsonValue value);
    static void resetValue(EltID id, QString name);

    // Find a parameter name from the attribute type (empty if not exposed)
    static QString getName(AttributeType champ);

    // Attribute type from a parameter name, champ_unknown if not found
    static AttributeType getChamp(QString name);

private:
    struct Definition
    {
        const char * name;
        AttributeType champ;
        AttributeType coarseChamp; // For the offsets, otherwise champ_unknown
        bool instrumentOnly;
        const char * unit; // For instruments
        const char * presetUnit; // For presets: values added to or multiplying the instrument values
        const char * description;
    };

    static const QList<Definition> &getDefinitions();
    static const Definition * find(QString name);
    static void getRealLimits(const Definition &definition, bool isPrst, double &min, double &max);
};

#endif // MCPATTRIBUTES_H

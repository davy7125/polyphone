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

#include "mcputils.h"
#include "mcpfunction.h"
#include "soundfontmanager.h"
#include <QRegularExpression>

QString McpUtils::idToString(EltID id)
{
    QString text = QString("sf%1").arg(id.indexSf2);
    switch (id.typeElement)
    {
    case elementSmpl:
        text += QString("/smpl%1").arg(id.indexElt);
        break;
    case elementInst: case elementInstSmpl: case elementInstMod: case elementInstSmplMod:
        text += QString("/inst%1").arg(id.indexElt);
        break;
    case elementPrst: case elementPrstInst: case elementPrstMod: case elementPrstInstMod:
        text += QString("/prst%1").arg(id.indexElt);
        break;
    default:
        break;
    }

    if (id.typeElement == elementInstSmpl || id.typeElement == elementPrstInst ||
        id.typeElement == elementInstSmplMod || id.typeElement == elementPrstInstMod)
        text += QString("/div%1").arg(id.indexElt2);
    if (id.typeElement == elementInstMod || id.typeElement == elementPrstMod ||
        id.typeElement == elementInstSmplMod || id.typeElement == elementPrstInstMod)
        text += QString("/mod%1").arg(id.indexMod);
    return text;
}

EltID McpUtils::readId(QString text, QList<ElementType> allowedTypes)
{
    static QRegularExpression regex("^sf(\\d+)(?:/(smpl|inst|prst)(\\d+)(?:/div(\\d+))?(?:/mod(\\d+))?)?$");
    QRegularExpressionMatch match = regex.match(text.trimmed());
    if (!match.hasMatch())
        throw McpException("invalid element identifier \"" + text + "\" (examples: \"sf0\", \"sf0/smpl3\", \"sf0/inst2/div5\")");

    EltID id(elementSf2, match.captured(1).toInt());
    QString kind = match.captured(2);
    bool hasDivision = !match.captured(4).isEmpty();
    bool hasMod = !match.captured(5).isEmpty();
    if (!kind.isEmpty())
    {
        id.indexElt = match.captured(3).toInt();
        if (kind == "smpl")
        {
            if (hasDivision || hasMod)
                throw McpException("invalid element identifier \"" + text + "\": a sample has no divisions or modulators");
            id.typeElement = elementSmpl;
        }
        else
        {
            bool isPrst = (kind == "prst");
            if (hasDivision)
            {
                id.indexElt2 = match.captured(4).toInt();
                id.typeElement = isPrst ? elementPrstInst : elementInstSmpl;
            }
            else
                id.typeElement = isPrst ? elementPrst : elementInst;

            if (hasMod)
            {
                id.indexMod = match.captured(5).toInt();
                if (id.typeElement == elementInst)
                    id.typeElement = elementInstMod;
                else if (id.typeElement == elementInstSmpl)
                    id.typeElement = elementInstSmplMod;
                else if (id.typeElement == elementPrst)
                    id.typeElement = elementPrstMod;
                else
                    id.typeElement = elementPrstInstMod;
            }
        }
    }

    if (!allowedTypes.isEmpty() && !allowedTypes.contains(id.typeElement))
    {
        QStringList names;
        foreach (ElementType type, allowedTypes)
            names << typeName(type);
        throw McpException("element \"" + text + "\" is a " + typeName(id.typeElement) + ", expected: " + names.join(", "));
    }

    SoundfontManager * sm = SoundfontManager::getInstance();
    if (!sm->isValid(id) || sm->isTemporary(id.indexSf2))
        throw McpException("element \"" + text + "\" doesn't exist");
    return id;
}

QString McpUtils::typeName(ElementType type)
{
    switch (type)
    {
    case elementSf2: return "soundfont";
    case elementSmpl: return "sample";
    case elementInst: return "instrument";
    case elementPrst: return "preset";
    case elementInstSmpl: return "instrument_division";
    case elementPrstInst: return "preset_division";
    case elementInstMod: return "instrument_modulator";
    case elementPrstMod: return "preset_modulator";
    case elementInstSmplMod: return "instrument_division_modulator";
    case elementPrstInstMod: return "preset_division_modulator";
    default: return "unknown";
    }
}

QString McpUtils::keyName(int key)
{
    static const char * names[12] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    if (key < 0 || key > 127)
        return "";
    return QString(names[key % 12]) + QString::number(key / 12 - 1);
}

int McpUtils::readKey(QJsonValue value, QString argumentName)
{
    if (value.isDouble())
    {
        double number = value.toDouble();
        if (number != qRound(number) || number < 0 || number > 127)
            throw McpException("argument \"" + argumentName + "\" must be a key between 0 and 127");
        return qRound(number);
    }

    if (value.isString())
    {
        static QRegularExpression regex("^([A-Ga-g])([#b]?)(-?\\d+)$");
        QRegularExpressionMatch match = regex.match(value.toString().trimmed());
        if (match.hasMatch())
        {
            static const int offsets[7] = { 9, 11, 0, 2, 4, 5, 7 }; // A to G
            int key = offsets[match.captured(1).toUpper()[0].toLatin1() - 'A'] + 12 * (match.captured(3).toInt() + 1);
            if (match.captured(2) == "#")
                key++;
            else if (match.captured(2) == "b")
                key--;
            if (key >= 0 && key <= 127)
                return key;
        }
    }

    throw McpException("argument \"" + argumentName + "\" must be a key number (0-127) or a note name such as \"C4\" (= 60)");
}

QJsonObject McpUtils::brief(EltID id)
{
    QJsonObject result;
    result["id"] = idToString(id);
    if (id.typeElement == elementSf2 || id.typeElement == elementSmpl ||
        id.typeElement == elementInst || id.typeElement == elementPrst)
        result["name"] = SoundfontManager::getInstance()->getQstr(id, champ_name);
    return result;
}

QList<int> McpUtils::getSoundfontIndexes()
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    EltID idSf2(elementSf2);
    QList<int> result;
    foreach (int i, sm->getSiblings(idSf2))
        if (!sm->isTemporary(i))
            result << i;
    return result;
}

double McpUtils::round(double value, int significantDigits)
{
    if (value == 0 || !std::isfinite(value))
        return std::isfinite(value) ? value : 0;
    double magnitude = std::pow(10.0, significantDigits - 1 - static_cast<int>(std::floor(std::log10(std::fabs(value)))));
    return std::round(value * magnitude) / magnitude;
}

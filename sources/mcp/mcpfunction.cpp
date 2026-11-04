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

#include "mcpfunction.h"
#include "mcputils.h"
#include "soundfontmanager.h"
#include <QJsonDocument>

QJsonValue McpArguments::getRequired(QString key) const
{
    if (!contains(key))
        throw McpException("missing argument \"" + key + "\"");
    return _arguments.value(key);
}

double McpArguments::readNumber(QString key, QJsonValue value, double min, double max) const
{
    if (!value.isDouble())
        throw McpException("argument \"" + key + "\" must be a number");
    double number = value.toDouble();
    if (number < min || number > max)
        throw McpException(QString("argument \"%1\" must be between %2 and %3").arg(key).arg(min).arg(max));
    return number;
}

QString McpArguments::getString(QString key) const
{
    QJsonValue value = getRequired(key);
    if (!value.isString())
        throw McpException("argument \"" + key + "\" must be a string");
    return value.toString();
}

QString McpArguments::getString(QString key, QString defaultValue) const
{
    return contains(key) ? getString(key) : defaultValue;
}

int McpArguments::getInt(QString key, int min, int max) const
{
    double number = readNumber(key, getRequired(key), min, max);
    if (number != qRound(number))
        throw McpException("argument \"" + key + "\" must be an integer");
    return qRound(number);
}

int McpArguments::getInt(QString key, int min, int max, int defaultValue) const
{
    return contains(key) ? getInt(key, min, max) : defaultValue;
}

double McpArguments::getDouble(QString key, double min, double max) const
{
    return readNumber(key, getRequired(key), min, max);
}

double McpArguments::getDouble(QString key, double min, double max, double defaultValue) const
{
    return contains(key) ? getDouble(key, min, max) : defaultValue;
}

bool McpArguments::getBool(QString key, bool defaultValue) const
{
    if (!contains(key))
        return defaultValue;
    QJsonValue value = _arguments.value(key);
    if (!value.isBool())
        throw McpException("argument \"" + key + "\" must be a boolean");
    return value.toBool();
}

QJsonObject McpArguments::getObject(QString key) const
{
    QJsonValue value = getRequired(key);
    if (!value.isObject())
        throw McpException("argument \"" + key + "\" must be an object");
    return value.toObject();
}

QJsonArray McpArguments::getArray(QString key) const
{
    QJsonValue value = getRequired(key);
    if (!value.isArray())
        throw McpException("argument \"" + key + "\" must be an array");
    return value.toArray();
}

EltID McpArguments::getId(QString key, QList<ElementType> allowedTypes) const
{
    return McpUtils::readId(getString(key), allowedTypes);
}

IdList McpArguments::getIds(QString key, QList<ElementType> allowedTypes) const
{
    QJsonValue value = getRequired(key);
    IdList ids;
    if (value.isString())
        ids << McpUtils::readId(value.toString(), allowedTypes);
    else if (value.isArray())
    {
        for (const QJsonValue item : value.toArray())
        {
            if (!item.isString())
                throw McpException("argument \"" + key + "\" must contain element identifiers");
            ids << McpUtils::readId(item.toString(), allowedTypes);
        }
    }
    else
        throw McpException("argument \"" + key + "\" must be an element identifier or an array of identifiers");

    if (ids.isEmpty())
        throw McpException("argument \"" + key + "\" is empty");
    return ids;
}

int McpArguments::getKey(QString key) const
{
    return McpUtils::readKey(getRequired(key), key);
}

int McpArguments::getKey(QString key, int defaultValue) const
{
    return contains(key) ? getKey(key) : defaultValue;
}

McpFunction::McpFunction(QString name, Category category, bool readOnly, QString description, const char * inputSchema,
                         Callback callback) :
    _name(name),
    _category(category),
    _readOnly(readOnly),
    _description(description),
    _callback(callback)
{
    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(QByteArray(inputSchema), &error);
    if (error.error != QJsonParseError::NoError || !document.isObject())
        qWarning() << "MCP function" << name << "- invalid schema:" << error.errorString();
    _inputSchema = document.object();
    if (!_inputSchema.contains("type"))
        _inputSchema["type"] = "object";
}

QJsonObject McpFunction::getDefinition() const
{
    QJsonObject annotations;
    annotations["readOnlyHint"] = _readOnly;
    annotations["openWorldHint"] = false;

    QJsonObject definition;
    definition["name"] = _name;
    definition["description"] = _description;
    definition["inputSchema"] = _inputSchema;
    definition["annotations"] = annotations;
    return definition;
}

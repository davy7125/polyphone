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

#ifndef MCPFUNCTION_H
#define MCPFUNCTION_H

#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include <functional>
#include "basetypes.h"

// Error while executing a function, returned to the AI as the result of the function
class McpException
{
public:
    McpException(QString message) : _message(message) {}
    QString message() const { return _message; }

private:
    QString _message;
};

// Result of a function: data given to the AI as a JSON text, and possible images
class McpResult
{
public:
    McpResult(QJsonValue data = QJsonValue()) : data(data) {}
    McpResult(QJsonObject data) : data(data) {}
    McpResult(QString text) : data(text) {}

    QJsonValue data;
    QList<QByteArray> pngImages;
};

// Arguments of a function, the getters throwing an McpException if a value is missing or not valid
class McpArguments
{
public:
    McpArguments(QJsonObject arguments) : _arguments(arguments) {}

    bool contains(QString key) const { return _arguments.contains(key) && !_arguments.value(key).isNull(); }
    QJsonValue value(QString key) const { return _arguments.value(key); }

    // Without default value, the argument is required
    QString getString(QString key) const;
    QString getString(QString key, QString defaultValue) const;
    int getInt(QString key, int min, int max) const;
    int getInt(QString key, int min, int max, int defaultValue) const;
    double getDouble(QString key, double min, double max) const;
    double getDouble(QString key, double min, double max, double defaultValue) const;
    bool getBool(QString key, bool defaultValue) const;
    QJsonObject getObject(QString key) const;
    QJsonArray getArray(QString key) const;

    // Element of an open soundfont (string identifier), possibly restricted to some types
    EltID getId(QString key, QList<ElementType> allowedTypes = QList<ElementType>()) const;

    // List of elements: one identifier or an array of identifiers
    IdList getIds(QString key, QList<ElementType> allowedTypes = QList<ElementType>()) const;

    // MIDI key: number or note name such as "C4" (60) or "F#2"
    int getKey(QString key) const;
    int getKey(QString key, int defaultValue) const;

private:
    QJsonValue getRequired(QString key) const;
    double readNumber(QString key, QJsonValue value, double min, double max) const;

    QJsonObject _arguments;
};

// Function that an AI can call
class McpFunction
{
public:
    enum Category
    {
        CATEGORY_BROWSING,
        CATEGORY_EDITING,
        CATEGORY_TOOLS,
        CATEGORY_ANALYSIS
    };

    typedef std::function<McpResult(const McpArguments &)> Callback;

    // The input schema is a JSON text describing the arguments (type "object")
    // A read-only function doesn't change the soundfonts, otherwise the changes are grouped in one undoable step
    McpFunction(QString name, Category category, bool readOnly, QString description, const char * inputSchema,
                Callback callback);

    QString getName() const { return _name; }
    Category getCategory() const { return _category; }
    bool isReadOnly() const { return _readOnly; }
    QString getDescription() const { return _description; }

    // Definition for the "tools/list" request
    QJsonObject getDefinition() const;

    // Execute the function, throw an McpException in case of error
    McpResult call(const McpArguments &arguments) const { return _callback(arguments); }

private:
    QString _name;
    Category _category;
    bool _readOnly;
    QString _description;
    QJsonObject _inputSchema;
    Callback _callback;
};

#endif // MCPFUNCTION_H

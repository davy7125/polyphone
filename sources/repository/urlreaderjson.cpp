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

#include "urlreaderjson.h"
#include <QJsonDocument>

UrlReaderJson::UrlReaderJson(QString url) : UrlReader(url) {}

void UrlReaderJson::processData()
{
    QJsonDocument document = QJsonDocument::fromJson(this->getRawData());

    // Extract data from the document
    QJsonObject rootObject = document.object();

    // Error message
    QJsonValue value = rootObject.value("err_msg");
    if (value.isString())
        _errorMessage = value.toString();
    else
    {
        setInvalid("Missing 'err_msg'");
        return;
    }

    // Error code
    value = rootObject.value("err_code");
    if (value.isString())
        _errorCode = value.toString();
    else
    {
        setInvalid("Missing 'err_code'");
        return;
    }

    // Response id
    value = rootObject.value("response_id");
    if (value.isString())
        _responseId = value.toString();
    else
    {
        setInvalid("Missing 'response_id'");
        return;
    }

    // Api
    value = rootObject.value("api");
    if (value.isString())
        _api = value.toString();
    else
    {
        setInvalid("Missing 'api'");
        return;
    }

    // Response id
    value = rootObject.value("version");
    if (value.isString())
        _version = value.toString();
    else
    {
        setInvalid("Missing 'version'");
        return;
    }

    // Data
    value = rootObject.value("data");
    if (value.isObject())
        _data = value.toObject();
    else
    {
        setInvalid("Missing 'data'");
        return;
    }
}

void UrlReaderJson::setInvalid(QString message)
{
    _errorMessage = message;
}

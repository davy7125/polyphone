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

#ifndef URLREADERJSON_H
#define URLREADERJSON_H

#include "urlreader.h"
#include <QJsonObject>

class UrlReaderJson : public UrlReader
{
public:
    // Constructor, initialized with a url
    UrlReaderJson(QString url);

    // Get methods
    QString getErrorMessage() { return _errorMessage; }
    QString getErrorCode() { return _errorCode; }
    QString getRespondeId() { return _responseId; }
    QString getApi() { return _api; }
    QString getVersion() { return _version; }
    QJsonObject getData() { return _data; }
    bool isValid() { return _errorMessage.isEmpty(); }

protected:
    virtual void processData();

private:
    void setInvalid(QString message);

    QString _errorMessage;
    QString _errorCode;
    QString _responseId;
    QString _api;
    QString _version;
    QJsonObject _data;
};

#endif // URLREADERJSON_H

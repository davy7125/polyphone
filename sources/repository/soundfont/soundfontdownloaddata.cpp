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

#include "soundfontdownloaddata.h"
#include <QJsonObject>
#include <QObject>

SoundfontDownloadData::SoundfontDownloadData(const QJsonObject &data)
{
    if (data.contains("rename") && data.value("rename").isString())
        _name = data.value("rename").toString();
    if (data.contains("description") && data.value("description").isString())
        _description = data.value("description").toString();
    if (data.contains("size"))
        _size = data.value("size").toString().toLongLong();
    if (data.contains("created") && data.value("created").isString())
        _createdTime = convertDate(data.value("created").toString());
    if (data.contains("modified") && data.value("modified").isString())
        _updatedTime = convertDate(data.value("modified").toString());
    if (data.contains("ordering"))
        _ordering = data.value("ordering").toString().toInt();
    if (data.contains("id"))
        _id = data.value("id").toString().toInt();
}

QDateTime SoundfontDownloadData::convertDate(QString txt)
{
    return QDateTime::fromString(txt, "yyyy-MM-dd hh:mm:ss");
}

QString SoundfontDownloadData::getFormattedSize()
{
    if (_size < 1024)
        return QString::number(_size) + " " + QObject::trUtf8("bytes");
    if (_size < 1024 * 1024)
        return QString::number(static_cast<double>(_size) / 1024, 'f', 2) + " " + QObject::trUtf8("kB", "kilobytes");
    if (_size < 1024 * 1024 * 1024)
        return QString::number(static_cast<double>(_size) / 1024 / 1024, 'f', 2) + " " + QObject::trUtf8("MB", "megabytes");
    if (_size / 1024 < 1024 * 1024 * 1024)
        return QString::number(static_cast<double>(_size) / 1024 / 1024 / 1024, 'f', 2) + " " + QObject::trUtf8("GB", "gigabytes");
    return QString::number(static_cast<double>(_size) / 1024 / 1024 / 1024 / 1024, 'f', 2) + " " + QObject::trUtf8("TB", "terabytes");
}

QString SoundfontDownloadData::getLastDate()
{
    QDateTime lastDate = (_updatedTime.isValid() ? _updatedTime : _createdTime);
    return lastDate.date().toString(Qt::SystemLocaleShortDate);
}

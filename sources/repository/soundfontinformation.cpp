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

#include "soundfontinformation.h"
#include <QObject>

SoundfontInformation::SoundfontInformation(int id, QString title) :
    _id(id),
    _title(title),
    _author(""),
    _dateTime(QDateTime()),
    _licenseFlags(LicenseFlag::NONE),
    _license(""),
    _downloadNumber(-1),
    _commentNumber(-1),
    _categoryId(-1),
    _rating(-1),
    _website("")
{

}

QString SoundfontInformation::getAuthor() {
    if (_author.isEmpty())
        return QObject::trUtf8("Unknown author");
    return _author;
}

void SoundfontInformation::addProperty(Property property, QString propertyValue)
{
    _properties[property].append(propertyValue);
}

void SoundfontInformation::addTag(QString tag)
{
    if (!_tags.contains(tag))
        _tags.append(tag);
}

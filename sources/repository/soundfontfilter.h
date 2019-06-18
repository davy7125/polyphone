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

#ifndef SOUNDFONTFILTER_H
#define SOUNDFONTFILTER_H

#include <QString>
#include "soundfontinformation.h"

class SoundfontFilter
{
public:
    SoundfontFilter();

    // Filter on the text or author
    void setSearchText(QString text) { _searchText = text; }
    QString searchText() { return _searchText; }

    // Filter on the category
    void setCategory(int catId) { _catId = catId; }
    int category() { return _catId; }

    // Filter on the license
    void setLicenseFlags(SoundfontInformation::LicenseFlags licenseFlags) { _licenseFlags = licenseFlags; }
    SoundfontInformation::LicenseFlags licenseFlags() { return _licenseFlags; }

    // Filter on properties
    void setProperties(SoundfontInformation::Property type, QStringList properties) { _properties[type] = properties; }
    QStringList properties(SoundfontInformation::Property type) { return _properties[type]; }

    // Filter on tags
    void setTags(QStringList tags) { _tags = tags; }
    QStringList tags() { return _tags; }

    bool match(SoundfontInformation * soundfontInformation);

private:
    bool isMatching(QStringList valuesToFind, QStringList values);

    QString _searchText;
    int _catId;
    SoundfontInformation::LicenseFlags _licenseFlags;
    QMap<SoundfontInformation::Property, QStringList> _properties;
    QStringList _tags;
};

#endif // SOUNDFONTFILTER_H

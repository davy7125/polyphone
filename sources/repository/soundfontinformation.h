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

#ifndef SOUNDFONTINFORMATION_H
#define SOUNDFONTINFORMATION_H

#include <QString>
#include <QMap>
#include <QDateTime>

class SoundfontInformation
{
public:
    enum Property
    {
        SAMPLE_SOURCE,
        TIMBRE,
        ARTICULATION,
        GENRE,
        MIDI_STANDARD
    };

    enum LicenseFlag
    {
        NONE = 0x00,
        COMMERCIAL_USE = 0x01,
        SHARE_MODIFIED = 0x02
    };
    Q_DECLARE_FLAGS(LicenseFlags, LicenseFlag)

    // Constructor, with two mandatory attributes
    SoundfontInformation(int id, QString title);

    int getId() { return _id; }
    QString getTitle() { return _title; }

    void setDateTime(QDateTime dateTime) { _dateTime = dateTime; }
    QDateTime getDateTime() { return _dateTime; }

    void setAuthor(QString author) { _author = author; }
    QString getAuthor();

    void setLicenseFlag(LicenseFlags flags) { _licenseFlags = flags; }
    LicenseFlags getLicenseFlag() { return _licenseFlags; }
    void setLicense(QString license) { _license = license; }
    QString getLicense() { return _license; }

    void setDownloadNumber(int downloadNumber) { _downloadNumber = downloadNumber; }
    int getDownloadNumber() { return _downloadNumber; }

    void setRating(double rating) { _rating = rating; }
    double getRating() { return _rating; }

    void setCommentNumber(int commentNumber) { _commentNumber = commentNumber; }
    int getCommentNumber() { return _commentNumber; }

    void setCategoryId(int categoryId) { _categoryId = categoryId; }
    int getCategoryId() { return _categoryId; }

    // Additional properties
    void addProperty(Property property, QString propertyValue);
    const QMap<Property, QStringList>& getProperties() { return _properties; }

    // Tags
    void addTag(QString tag);
    const QStringList getTags() { return _tags; }

    // Website
    void setWebsite(QString website) { _website = website; }
    QString getWebsite() { return _website; }

private:
    int _id;
    QString _title, _author;
    QDateTime _dateTime;
    LicenseFlags _licenseFlags;
    QString _license;
    int _downloadNumber, _commentNumber, _categoryId;
    double _rating;
    QMap<Property, QStringList> _properties;
    QStringList _tags;
    QString _website;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(SoundfontInformation::LicenseFlags)

#endif // SOUNDFONTINFORMATION_H

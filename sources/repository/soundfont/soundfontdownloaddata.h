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

#ifndef SOUNDFONTDOWNLOADDATA_H
#define SOUNDFONTDOWNLOADDATA_H

#include <QString>
#include <QDateTime>
class QJsonObject;

class SoundfontDownloadData
{
public:
    SoundfontDownloadData(const QJsonObject &data);

    // Getters
    QString getName() const { return _name; }
    QString getDescription() const { return _description; }
    qint64 getSize() const { return _size; }
    QDateTime getCreatedTime() const { return _createdTime; }
    QDateTime getUpdatedTime() const { return _updatedTime; }
    int getOrdering() const { return _ordering; }
    int getId() const { return _id; }

    // Format
    QString getFormattedSize();
    QString getLastDate();

private:
    QDateTime convertDate(QString txt);

    QString _name;
    QString _description;
    qint64 _size;
    QDateTime _createdTime;
    QDateTime _updatedTime;
    int _ordering;
    int _id;
};

#endif // SOUNDFONTDOWNLOADDATA_H

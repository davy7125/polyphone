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

#ifndef SOUNDFONTCOMMENTDATA_H
#define SOUNDFONTCOMMENTDATA_H

#include <QList>
#include <QDateTime>
class QJsonObject;

class SoundfontCommentData
{
public:
    SoundfontCommentData(const QJsonObject &data);

    // Return true if the comment has been added as a child (recursive function)
    bool addComment(SoundfontCommentData * orphan);
    QList<SoundfontCommentData *> getChildren() { return _children; }

    // Getters
    int getId() const { return _id; }
    int getParentId() const { return _parentId; }
    QDateTime getCreationTime() const { return _created; }
    QString getText() const { return _content; }
    QString getUserName() const { return _userName; }

private:
    QDateTime convertDate(QString txt);

    int _id;
    int _parentId;
    QDateTime _created;
    QString _content;
    QString _userName;

    QList<SoundfontCommentData*> _children;
};

#endif // SOUNDFONTCOMMENTDATA_H

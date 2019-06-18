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

#include "soundfontcommentdata.h"
#include <QJsonObject>

SoundfontCommentData::SoundfontCommentData(const QJsonObject &data)
{
    if (data.contains("id"))
        _id = data.value("id").toString().toInt();
    if (data.contains("name") && data.value("name").isString())
        _userName = data.value("name").toString();
    if (data.contains("comment") && data.value("comment").isString())
        _content = data.value("comment").toString();
    if (data.contains("created") && data.value("created").isString())
        _created = convertDate(data.value("created").toString());
    if (data.contains("parent_id"))
        _parentId = data.value("parent_id").toString().toInt();
}

bool SoundfontCommentData::addComment(SoundfontCommentData * orphan)
{
    // Can be added here?
    if (orphan->getParentId() == _id)
    {
        _children << orphan;
        return true;
    }

    // Or in children?
    foreach (SoundfontCommentData * child, _children)
        if (child->addComment(orphan))
            return true;

    return false;
}

QDateTime SoundfontCommentData::convertDate(QString txt)
{
    return QDateTime::fromString(txt, "yyyy-MM-dd hh:mm:ss");
}

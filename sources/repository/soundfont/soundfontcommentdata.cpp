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

#include "soundfontdownloaddata.h"
#include <QJsonObject>

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
}

QDateTime SoundfontDownloadData::convertDate(QString txt)
{
    return QDateTime::fromString(txt, "yyyy-MM-dd hh:mm:ss");
}

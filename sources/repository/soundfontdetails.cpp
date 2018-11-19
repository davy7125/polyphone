#include "soundfontdetails.h"
#include "soundfontdescriptiondata.h"
#include "soundfontdownloaddata.h"
#include "soundfontcommentdata.h"
#include <QJsonObject>
#include <QJsonArray>

SoundfontDetails::SoundfontDetails(const QJsonObject &data) :
    _description(nullptr)
{
    // Details
    if (data.contains("details") && data.value("details").isObject())
        _description = new SoundfontDescriptionData(data.value("details").toObject());

    // Downloads
    if (data.contains("downloads") && data.value("downloads").isArray())
    {
        foreach (QJsonValue value, data.value("downloads").toArray())
            if (value.isObject())
                _downloads << new SoundfontDownloadData(value.toObject());
    }

    // Comments
    if (data.contains("comments") && data.value("comments").isArray())
    {
        QJsonArray comments = data.value("comments").toArray();
        foreach (QJsonValue value, comments)
        {
            // New comment
            SoundfontCommentData * orphan = new SoundfontCommentData(value.toObject());

            // First try to attach the comment to a parent
            foreach (SoundfontCommentData * comment, _comments)
            {
                if (comment->addComment(orphan))
                {
                    orphan = nullptr;
                    break;
                }
            }

            // Or add it with no parent
            if (orphan != nullptr)
                _comments << orphan;
        }
    }

}

SoundfontDetails::~SoundfontDetails()
{
    delete _description;
    while (!_downloads.isEmpty())
        delete _downloads.takeFirst();
    while (!_comments.isEmpty())
        delete _comments.takeFirst();
}

SoundfontDescriptionData *SoundfontDetails::getDescription()
{
    return _description;
}

QList<SoundfontDownloadData *> SoundfontDetails::getDownloads()
{
    return _downloads;
}

QList<SoundfontCommentData *> SoundfontDetails::getComments()
{
    return _comments;
}

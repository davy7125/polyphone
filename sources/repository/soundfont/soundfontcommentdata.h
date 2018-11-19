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

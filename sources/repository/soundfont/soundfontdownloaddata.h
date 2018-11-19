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

private:
    QDateTime convertDate(QString txt);

    QString _name;
    QString _description;
    qint64 _size;
    QDateTime _createdTime;
    QDateTime _updatedTime;
    int _ordering;
};

#endif // SOUNDFONTDOWNLOADDATA_H

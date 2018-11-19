#ifndef SOUNDFONTDETAILS_H
#define SOUNDFONTDETAILS_H

#include <QList>
class QJsonObject;
class SoundfontDescriptionData;
class SoundfontDownloadData;
class SoundfontCommentData;

class SoundfontDetails
{
public:
    /// Build a new soundfont details object
    SoundfontDetails(const QJsonObject &data);
    ~SoundfontDetails();

    /// Description of the soundfonts
    SoundfontDescriptionData * getDescription();

    /// List of all possible downloads
    QList<SoundfontDownloadData*> getDownloads();

    /// All comments
    QList<SoundfontCommentData*> getComments();

private:
    SoundfontDescriptionData * _description;
    QList<SoundfontDownloadData*> _downloads;
    QList<SoundfontCommentData*> _comments;
};

#endif // SOUNDFONTDETAILS_H

#ifndef SOUNDFONTINFORMATION_H
#define SOUNDFONTINFORMATION_H

#include <QString>
#include <QMap>
#include <QDateTime>

class SoundfontInformation
{
public:
    enum class Property
    {
        SAMPLE_SOURCE,
        TIMBRE,
        ARTICULATION,
        GENRE,
        MIDI_STANDARD
    };

    enum class LicenseFlag
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
    const QMap<Property, QStringList> getProperties() { return _properties; }

    // Tags
    void addTag(QString tag);
    const QStringList getTags() { return _tags; }

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
};

Q_DECLARE_OPERATORS_FOR_FLAGS(SoundfontInformation::LicenseFlags)

#endif // SOUNDFONTINFORMATION_H

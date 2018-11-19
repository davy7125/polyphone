#include "soundfontinformation.h"
#include <QObject>

SoundfontInformation::SoundfontInformation(int id, QString title) :
    _id(id),
    _title(title),
    _author(""),
    _dateTime(QDateTime()),
    _licenseFlags(LicenseFlag::NONE),
    _license(""),
    _downloadNumber(-1),
    _commentNumber(-1),
    _categoryId(-1),
    _rating(-1),
    _website("")
{

}

QString SoundfontInformation::getAuthor() {
    if (_author.isEmpty())
        return QObject::trUtf8("Auteur inconnu");
    return _author;
}

void SoundfontInformation::addProperty(Property property, QString propertyValue)
{
    _properties[property].append(propertyValue);
}

void SoundfontInformation::addTag(QString tag)
{
    if (!_tags.contains(tag))
        _tags.append(tag);
}

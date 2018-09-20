#ifndef SOUNDFONTFILTER_H
#define SOUNDFONTFILTER_H

#include <QString>
#include "soundfontinformation.h"

class SoundfontFilter
{
public:
    SoundfontFilter();

    // Filter on the text or author
    void setSearchText(QString text) { _searchText = text; }
    QString searchText() { return _searchText; }

    // Filter on the category
    void setCategory(int catId) { _catId = catId; }
    int category() { return _catId; }

    // Filter on the license
    void setLicenseFlags(SoundfontInformation::LicenseFlags licenseFlags) { _licenseFlags = licenseFlags; }
    SoundfontInformation::LicenseFlags licenseFlags() { return _licenseFlags; }

    // Filter on properties
    void setProperties(SoundfontInformation::Property type, QStringList properties) { _properties[type] = properties; }
    QStringList properties(SoundfontInformation::Property type) { return _properties[type]; }

    // Filter on tags
    void setTags(QStringList tags) { _tags = tags; }
    QStringList tags() { return _tags; }

    bool match(SoundfontInformation * soundfontInformation);

private:
    bool isMatching(QStringList valuesToFind, QStringList values);

    QString _searchText;
    int _catId;
    SoundfontInformation::LicenseFlags _licenseFlags;
    QMap<SoundfontInformation::Property, QStringList> _properties;
    QStringList _tags;
};

#endif // SOUNDFONTFILTER_H

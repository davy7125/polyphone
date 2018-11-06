#include "soundfontfilter.h"

SoundfontFilter::SoundfontFilter() :
    _searchText(""),
    _catId(-1),
    _licenseFlags(SoundfontInformation::LicenseFlag::NONE)
{

}

bool SoundfontFilter::match(SoundfontInformation * soundfontInformation)
{
    // Search text
    if (!_searchText.isEmpty())
    {
        QString search = _searchText;

        // Author specified?
        QRegExp regExp("Author:\"([^\"]*)\"");
        regExp.indexIn(search);
        QStringList capturedText = regExp.capturedTexts();
        if (capturedText.count() == 2 && !capturedText[1].isEmpty())
        {
            // Check that the author matched
            if (soundfontInformation->getAuthor() != capturedText[1])
                return false;

            // Remove the author part
            search = search.replace(regExp, "").trimmed();
        }

        // Search the remaining part in the title
        if (!soundfontInformation->getTitle().toLower().contains(search.toLower()))
            return false;
    }

    // Category
    if (_catId != -1 && soundfontInformation->getCategoryId() != _catId)
        return false;

    // License
    if ((int)_licenseFlags != (int)SoundfontInformation::LicenseFlag::NONE)
        if ((soundfontInformation->getLicenseFlag() & _licenseFlags) != _licenseFlags)
            return false;

    // Properties
    QMap<SoundfontInformation::Property, QStringList> properties = soundfontInformation->getProperties();
    if (!_properties[SoundfontInformation::Property::SAMPLE_SOURCE].isEmpty() &&
            !isMatching(_properties[SoundfontInformation::Property::SAMPLE_SOURCE],
                        properties[SoundfontInformation::Property::SAMPLE_SOURCE]))
        return false;
    if (!_properties[SoundfontInformation::Property::TIMBRE].isEmpty() &&
            !isMatching(_properties[SoundfontInformation::Property::TIMBRE],
                        properties[SoundfontInformation::Property::TIMBRE]))
        return false;
    if (!_properties[SoundfontInformation::Property::ARTICULATION].isEmpty() &&
            !isMatching(_properties[SoundfontInformation::Property::ARTICULATION],
                        properties[SoundfontInformation::Property::ARTICULATION]))
        return false;
    if (!_properties[SoundfontInformation::Property::GENRE].isEmpty() &&
            !isMatching(_properties[SoundfontInformation::Property::GENRE],
                        properties[SoundfontInformation::Property::GENRE]))
        return false;
    if (!_properties[SoundfontInformation::Property::MIDI_STANDARD].isEmpty() &&
            !isMatching(_properties[SoundfontInformation::Property::MIDI_STANDARD],
                        properties[SoundfontInformation::Property::MIDI_STANDARD]))
        return false;

    // Tags
    if (!_tags.empty() && !isMatching(_tags, soundfontInformation->getTags()))
        return false;

    return true;
}

bool SoundfontFilter::isMatching(QStringList valuesToFind, QStringList values)
{
    foreach (QString valueToFind, valuesToFind)
    {
        if (values.contains(valueToFind))
            return true;
    }
    return false;
}

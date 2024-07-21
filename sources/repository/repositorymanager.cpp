/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program. If not, see http://www.gnu.org/licenses/.    **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "repositorymanager.h"
#include "contextmanager.h"
#include "urlreaderjson.h"
#include "usermanager.h"
#include <QDateTime>
#include <QJsonArray>
#include <QJsonValue>
#include <QMessageBox>
#include <QApplication>
#include <QDebug>
#include "soundfontdetails.h"

//const QString RepositoryManager::BASE_URL = "http://www.polyphone.local/api/soundfonts/";
const QString RepositoryManager::BASE_URL = "https://www.polyphone.io/api/soundfonts/";
RepositoryManager * RepositoryManager::s_instance = nullptr;

RepositoryManager * RepositoryManager::getInstance()
{
    if (s_instance == nullptr)
        s_instance = new RepositoryManager();
    return s_instance;
}

void RepositoryManager::kill()
{
    if (s_instance != nullptr)
    {
        delete s_instance;
        s_instance = nullptr;
    }
}

RepositoryManager::RepositoryManager() :
    _urlReaderJsonList(new UrlReaderJson(RepositoryManager::BASE_URL + "list"))
{
    fillPropertyTranslation();
    fillLicenseLabelAndLink();
    connect(_urlReaderJsonList, SIGNAL(downloadCompleted(QString)), this, SLOT(soundfontListAvailable(QString)), Qt::DirectConnection);
}

RepositoryManager::~RepositoryManager()
{
    QList<int> keys = _soundfontInfos.keys();
    foreach (int key, keys)
        delete _soundfontInfos.take(key);
}

void RepositoryManager::fillPropertyTranslation()
{
    // SAMPLE SOURCE
    _propertyTranslations[SoundfontInformation::Property::SAMPLE_SOURCE]["acoustic"] = tr("acoustic instrument");
    _propertyTranslations[SoundfontInformation::Property::SAMPLE_SOURCE]["electric"] = tr("electronic instrument");
    _propertyTranslations[SoundfontInformation::Property::SAMPLE_SOURCE]["records"] = tr("various records");
    _propertyTranslations[SoundfontInformation::Property::SAMPLE_SOURCE]["additive"] = tr("additive synthesis");
    _propertyTranslations[SoundfontInformation::Property::SAMPLE_SOURCE]["subtractive"] = tr("subtrative synthesis");
    _propertyTranslations[SoundfontInformation::Property::SAMPLE_SOURCE]["wavetable"] = tr("wavetable synthesis");
    _propertyTranslations[SoundfontInformation::Property::SAMPLE_SOURCE]["model"] = tr("model synthesis");
    _propertyTranslations[SoundfontInformation::Property::SAMPLE_SOURCE]["fm"] = tr("FM synthesis");
    _propertyTranslations[SoundfontInformation::Property::SAMPLE_SOURCE]["vector"] = tr("vector synthesis");
    _propertyTranslations[SoundfontInformation::Property::SAMPLE_SOURCE]["granular"] = tr("granular synthesis");
    _propertyTranslations[SoundfontInformation::Property::SAMPLE_SOURCE]["other_synthesis"] = tr("other synthesis");

    // TIMBRE
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["high"] = tr("high");
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["low"] = tr("low");
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["distorted"] = tr("distorted");
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["clean"] = tr("clean");
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["bright"] = tr("bright");
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["dark"] = tr("dark");
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["warm"] = tr("warm");
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["cold"] = tr("cold");
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["fat"] = tr("fat");
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["thin"] = tr("thin");
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["hard"] = tr("hard");
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["soft"] = tr("soft");
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["muted"] = tr("muted");
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["detuned"] = tr("detuned");
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["dissonant"] = tr("dissonant");
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["noisy"] = tr("noisy");
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["metallic"] = tr("metallic");
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["wooden"] = tr("wooden");
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["exotic"] = tr("exotic");

    // ARTICULATION
    _propertyTranslations[SoundfontInformation::Property::ARTICULATION]["slow_attack"] = tr("slow attack");
    _propertyTranslations[SoundfontInformation::Property::ARTICULATION]["decaying"] = tr("decaying");
    _propertyTranslations[SoundfontInformation::Property::ARTICULATION]["sustained"] = tr("sustained");
    _propertyTranslations[SoundfontInformation::Property::ARTICULATION]["long_release"] = tr("long release");
    _propertyTranslations[SoundfontInformation::Property::ARTICULATION]["percussive"] = tr("percussive");
    _propertyTranslations[SoundfontInformation::Property::ARTICULATION]["long_evolving"] = tr("long / evolving");
    _propertyTranslations[SoundfontInformation::Property::ARTICULATION]["pulsating"] = tr("pulsating");
    _propertyTranslations[SoundfontInformation::Property::ARTICULATION]["echoing"] = tr("echoing");
    _propertyTranslations[SoundfontInformation::Property::ARTICULATION]["pad"] = tr("pad");
    _propertyTranslations[SoundfontInformation::Property::ARTICULATION]["lead"] = tr("lead");
    _propertyTranslations[SoundfontInformation::Property::ARTICULATION]["monophonic"] = tr("monophonic");
    _propertyTranslations[SoundfontInformation::Property::ARTICULATION]["chord"] = tr("chord");
    _propertyTranslations[SoundfontInformation::Property::ARTICULATION]["glide_pitch_mod"] = tr("glide / pitch mod");
    _propertyTranslations[SoundfontInformation::Property::ARTICULATION]["sweep_filter_mod"] = tr("sweep / filter mod");
    _propertyTranslations[SoundfontInformation::Property::ARTICULATION]["arpeggiated"] = tr("arpeggiated");
    _propertyTranslations[SoundfontInformation::Property::ARTICULATION]["expressive"] = tr("expressive");
    _propertyTranslations[SoundfontInformation::Property::ARTICULATION]["complex"] = tr("complex");
    _propertyTranslations[SoundfontInformation::Property::ARTICULATION]["randomized"] = tr("randomized");

    // GENRE
    _propertyTranslations[SoundfontInformation::Property::GENRE]["classic"] = tr("classic music");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["electronic"] = tr("electronic music");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["trance"] = tr("trance");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["ambient"] = tr("ambient music");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["ambiant"] = tr("ambient music");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["house"] = tr("house");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["techno_electro"] = tr("techno / electro");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["dnb_breaks"] = tr("D'n'B / breaks");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["industrial"] = tr("industrial");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["experimental"] = tr("experimental");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["funk_soul"] = tr("funk / soul");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["reggae_dub"] = tr("reggae / dub");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["latin_afro_cuban"] = tr("latin / afro-cuban");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["pop_rock"] = tr("pop / rock");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["metal"] = tr("metal music");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["hiphop_rap"] = tr("hiphop / rap");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["jazz_swing"] = tr("jazz / swing");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["folk_country"] = tr("folk / country");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["ethnic_world"] = tr("ethnic / world");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["ethnic / world"] = tr("ethnic / world");

    // No translations for MIDI STANDARDS
    _propertyTranslations[SoundfontInformation::Property::MIDI_STANDARD]["no_standards"] = "no standards"; // Shouldn't be used
    _propertyTranslations[SoundfontInformation::Property::MIDI_STANDARD]["gm1"] = "GM1";
    _propertyTranslations[SoundfontInformation::Property::MIDI_STANDARD]["gm2"] = "GM2";
    _propertyTranslations[SoundfontInformation::Property::MIDI_STANDARD]["gm_lite"] = "GM Lite";
    _propertyTranslations[SoundfontInformation::Property::MIDI_STANDARD]["roland_mt32"] = "Roland MT-32";
    _propertyTranslations[SoundfontInformation::Property::MIDI_STANDARD]["roland_gs"] = "Roland GS";
    _propertyTranslations[SoundfontInformation::Property::MIDI_STANDARD]["yamaha_xg1"] = "Yamaha XG1";
    _propertyTranslations[SoundfontInformation::Property::MIDI_STANDARD]["yamaha_xg2"] = "Yamaha XG2";
    _propertyTranslations[SoundfontInformation::Property::MIDI_STANDARD]["yamaha_xg3"] = "Yamaha XG3";
}

void RepositoryManager::fillLicenseLabelAndLink()
{
    // Labels
    _licenseLabels["public_domain"] = tr("public domain");
    _licenseLabels["give_credit"] = tr("give credit");
    _licenseLabels["give_credit_no_restrictions"] = tr("give credit, don't distribute with more restrictions");
    _licenseLabels["modifications_forbidden"] = tr("don't distribute a modified version");
    _licenseLabels["personal_use_and_share"] = tr("personal use");
    _licenseLabels["personal_use_and_share_no_restrictions"] = tr("personal use, don't distribute with more restrictions");
    _licenseLabels["personal_use_only"] = tr("personal use, don't distribute a modified version");

    // Links
    _licenseUrl = "https://www.polyphone-soundfonts.com/license#";
}

void RepositoryManager::initialize()
{   
    // Reset
    QList<int> keys = _soundfontInfos.keys();
    foreach (int key, keys)
        delete _soundfontInfos.take(key);

    // Notify the list is downloading
    emit(initializing());

    // Download the list
    _urlReaderJsonList->download();
}

void RepositoryManager::soundfontListAvailable(QString error)
{
    // Get an error if any
    if (error.isEmpty() && !_urlReaderJsonList->getErrorCode().isEmpty())
        error = _urlReaderJsonList->getErrorMessage() + " (" + _urlReaderJsonList->getErrorCode() + ")";

    // If everything is ok, load the soundfont information
    if (error.isEmpty())
        error = loadSoundfontInfo();

    // Notify that the list is ready
    if (!error.isEmpty())
        qCritical() << "RepositoryManager::soundfontListAvailable() - received error" << error;
    emit(ready(error));
}

QString RepositoryManager::loadSoundfontInfo()
{
    // Extract the soundfont array
    QJsonObject object = _urlReaderJsonList->getData();
    if (!object.value("soundfonts").isArray())
        return "bad data: 'soundfonts' is not an array";
    QJsonArray jsonSoundfonts = object.value("soundfonts").toArray();

    // Load the information of all soundfonts
    for (int i = 0; i < jsonSoundfonts.count(); i++)
    {
        QJsonValue jsonSoundfont = jsonSoundfonts[i];

        // Get the object
        if (!jsonSoundfont.isObject())
        {
            qWarning() << "RepositoryManager::loadSoundfontInfo() - a soundfont is not an object";
            continue;
        }
        QJsonObject jsonObject = jsonSoundfont.toObject();

        // ID
        QJsonValue valueTmp = jsonObject.value("id");
        int id = valueTmp.isDouble() ? static_cast<int>(valueTmp.toDouble()) : -1;

        // Title
        valueTmp = jsonObject.value("title");
        QString title = valueTmp.isString() ? valueTmp.toString() : "";

        if (id == -1 || title.isEmpty())
        {
            qWarning() << "RepositoryManager::loadSoundfontInfo() - no id or no title for a soundfont json object";
            continue;
        }

        // Create a new soundfont information
        SoundfontInformation * si = new SoundfontInformation(id, title);
        _soundfontInfos[id] = si;
        //qDebug() << "RepositoryManager::loadSoundfontInfo() - soundfont" << id << "added (" << title << ")";

        // Date
        valueTmp = jsonObject.value("date");
        if (valueTmp.isString())
            si->setDateTime(QDateTime::fromString(valueTmp.toString(), "yyyy-MM-dd HH:mm:ss"));

        // Author
        valueTmp = jsonObject.value("author");
        if (valueTmp.isString())
            si->setAuthor(valueTmp.toString());

        // License
        valueTmp = jsonObject.value("license");
        if (valueTmp.isString())
            si->setLicense(valueTmp.toString());
        valueTmp = jsonObject.value("license_option");
        if (valueTmp.isString())
        {
            QString tmp = valueTmp.toString();
            SoundfontInformation::LicenseFlags flags = SoundfontInformation::LicenseFlag::NONE;
            if (tmp.contains("used_commercially"))
                flags |= SoundfontInformation::LicenseFlag::COMMERCIAL_USE;
            if (tmp.contains("shared_modified"))
                flags |= SoundfontInformation::LicenseFlag::SHARE_MODIFIED;
            si->setLicenseFlag(flags);
        }

        // Download number
        valueTmp = jsonObject.value("downloads");
        if (valueTmp.isDouble())
            si->setDownloadNumber(static_cast<int>(valueTmp.toDouble()));

        // Rating
        valueTmp = jsonObject.value("rating");
        if (valueTmp.isDouble())
            si->setRating(valueTmp.toDouble());

        // Comment number
        valueTmp = jsonObject.value("comment_number");
        if (valueTmp.isDouble())
            si->setCommentNumber(static_cast<int>(valueTmp.toDouble()));

        // Website
        valueTmp = jsonObject.value("website");
        if (valueTmp.isString())
            si->setWebsite(valueTmp.toString());

        // Category id and name
        valueTmp = jsonObject.value("category_id");
        if (valueTmp.isDouble())
            si->setCategoryId(static_cast<int>(valueTmp.toDouble()));
        valueTmp = jsonObject.value("category_title");
        if (valueTmp.isString() && si->getCategoryId() != -1)
            _categoryNames[si->getCategoryId()] = valueTmp.toString();

        // Additional properties
        loadProperty(si, jsonObject.value("sample_source"), SoundfontInformation::Property::SAMPLE_SOURCE);
        loadProperty(si, jsonObject.value("timbre"), SoundfontInformation::Property::TIMBRE);
        loadProperty(si, jsonObject.value("articulation"), SoundfontInformation::Property::ARTICULATION);
        loadProperty(si, jsonObject.value("genre"), SoundfontInformation::Property::GENRE);
        loadProperty(si, jsonObject.value("midi_standard"), SoundfontInformation::Property::MIDI_STANDARD);

        // Tags
        valueTmp = jsonObject.value("tags");
        if (valueTmp.isArray())
        {
            QJsonArray jsonTags = valueTmp.toArray();
            for (int j = 0; j < jsonTags.count(); j++)
            {
                QJsonValue jsonTag = jsonTags[j];
                if (jsonTag.isString())
                {
                    QString tag = jsonTag.toString();
                    si->addTag(tag);
                    if (!_tags.contains(tag))
                        _tags.append(tag);
                }
            }
        }
    }

    // No error until here
    return "";
}

void RepositoryManager::loadProperty(SoundfontInformation * si, const QJsonValue &value, SoundfontInformation::Property property)
{
    if (value.isString())
    {
        QStringList split = value.toString().split('|');
        foreach (QString value, split)
        {
            // Skip "no_standards", which is meaningless for a search
            if (property == SoundfontInformation::Property::MIDI_STANDARD && value == "no_standards")
                continue;

            // Translate the property and add it to the soundfont information
            value = getPropertyTranslation(property, value);
            si->addProperty(property, value);

            // Update the list of all properties
            if (!_properties[property].contains(value))
                _properties[property].append(value);
        }
    }
}

QString RepositoryManager::getPropertyTranslation(SoundfontInformation::Property property, QString value)
{
    if (_propertyTranslations[property].contains(value))
        return _propertyTranslations[property][value];
    qDebug() << "no translation for " << value;
    return value;
}

SoundfontInformation * RepositoryManager::getSoundfontInformation(int id)
{
    return _soundfontInfos.contains(id) ? _soundfontInfos[id] : nullptr;
}

QList<SoundfontInformation *> RepositoryManager::getSoundfontInformation(SoundfontFilter * filter)
{
    QList<SoundfontInformation *> results;

    foreach (SoundfontInformation * soundfontInfo, _soundfontInfos)
    {
        if (filter->match(soundfontInfo))
            results.append(soundfontInfo);
    }

    return results;
}

QString RepositoryManager::getCategoryName(int id)
{
    QString result;

    // Try to find a translated categories
    switch (id) {
    case 10:
        result = tr("piano");
        break;
    case 11:
        result = tr("organ");
        break;
    case 12:
        result = tr("synthesizer");
        break;
    case 13:
        result = tr("harpsichord");
        break;
    case 14:
        result = tr("guitar");
        break;
    case 15:
        result = tr("bass");
        break;
    case 16:
        result = tr("plucked strings");
        break;
    case 17:
        result = tr("bowed strings");
        break;
    case 18:
        result = tr("flute");
        break;
    case 19:
        result = tr("reed");
        break;
    case 20:
        result = tr("brass");
        break;
    case 21:
        result = tr("vocal");
        break;
    case 22:
        result = tr("unpitched percussion");
        break;
    case 23:
        result = tr("melodic percussion");
        break;
    case 24:
        result = tr("sound effect");
        break;
    case 25:
        result = tr("soundscape");
        break;
    case 26:
        result = tr("loop / rythm");
        break;
    case 27:
        result = tr("instrument set");
        break;
    case 28:
        result = tr("unclassifiable");
        break;
    default:
        // Search in the map
        if (_categoryNames.contains(id))
            result = _categoryNames[id];
        else
            result = tr("Category %1").arg(id);
        break;
    }

    return result;
}

QString RepositoryManager::getLicenseLabel(QString licenseKey)
{
    if (_licenseLabels.contains(licenseKey))
        return _licenseLabels[licenseKey];
    return licenseKey;
}

QString RepositoryManager::getLicenseLink(QString licenseKey)
{
    return _licenseUrl + licenseKey;
}

void RepositoryManager::openSoundfont(int soundfontId, bool daily)
{
    if (!daily)
    {
        // Check that the user is allowed to open the soundfont
        QString error = "";
        switch (UserManager::getInstance()->getConnectionState())
        {
        case UserManager::PENDING:
            error = tr("Please wait...");
            break;
        case UserManager::BANNED:
            error = tr("Your account has been banned.");
            break;
        case UserManager::DISCONNECTED:
        case UserManager::CONNECTED:
        case UserManager::FAILED:
            error = tr("A Premium account allows you to browse and download all soundfonts available online.");
            break;
        case UserManager::CONNECTED_PREMIUM: case UserManager::CONNECTED_ADMIN:
            error = ""; // ok
            break;
        }

        if (!error.isEmpty())
        {
            QMessageBox::warning(QApplication::activeWindow(), tr("Warning"), error);
            return;
        }
    }

    // Ask to open the page
    emit(openSoundfont(soundfontId));
}

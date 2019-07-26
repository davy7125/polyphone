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

//const QString RepositoryManager::BASE_URL = "http://localhost/polyphone-soundfonts/fr/api/soundfonts/";
const QString RepositoryManager::BASE_URL = "https://www.polyphone-soundfonts.com/en/api/soundfonts/";
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
    {
        delete _soundfontInfos.take(key);
    }
}

void RepositoryManager::fillPropertyTranslation()
{
    // SAMPLE SOURCE
    _propertyTranslations[SoundfontInformation::Property::SAMPLE_SOURCE]["acoustic"] = trUtf8("acoustic instrument");
    _propertyTranslations[SoundfontInformation::Property::SAMPLE_SOURCE]["electric"] = trUtf8("electronic instrument");
    _propertyTranslations[SoundfontInformation::Property::SAMPLE_SOURCE]["records"] = trUtf8("various records");
    _propertyTranslations[SoundfontInformation::Property::SAMPLE_SOURCE]["additive"] = trUtf8("additive synthesis");
    _propertyTranslations[SoundfontInformation::Property::SAMPLE_SOURCE]["subtractive"] = trUtf8("subtrative synthesis");
    _propertyTranslations[SoundfontInformation::Property::SAMPLE_SOURCE]["wavetable"] = trUtf8("wavetable synthesis");
    _propertyTranslations[SoundfontInformation::Property::SAMPLE_SOURCE]["model"] = trUtf8("model synthesis");
    _propertyTranslations[SoundfontInformation::Property::SAMPLE_SOURCE]["fm"] = trUtf8("FM synthesis");
    _propertyTranslations[SoundfontInformation::Property::SAMPLE_SOURCE]["vector"] = trUtf8("vector synthesis");
    _propertyTranslations[SoundfontInformation::Property::SAMPLE_SOURCE]["granular"] = trUtf8("granular synthesis");
    _propertyTranslations[SoundfontInformation::Property::SAMPLE_SOURCE]["other_synthesis"] = trUtf8("other synthesis");

    // TIMBRE
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["high"] = trUtf8("high");
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["low"] = trUtf8("low");
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["distorted"] = trUtf8("distorted");
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["clean"] = trUtf8("clean");
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["bright"] = trUtf8("bright");
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["dark"] = trUtf8("dark");
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["warm"] = trUtf8("warm");
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["cold"] = trUtf8("cold");
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["fat"] = trUtf8("fat");
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["thin"] = trUtf8("thin");
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["hard"] = trUtf8("hard");
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["soft"] = trUtf8("soft");
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["muted"] = trUtf8("muted");
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["detuned"] = trUtf8("detuned");
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["dissonant"] = trUtf8("dissonant");
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["noisy"] = trUtf8("noisy");
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["metallic"] = trUtf8("metallic");
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["wooden"] = trUtf8("wooden");
    _propertyTranslations[SoundfontInformation::Property::TIMBRE]["exotic"] = trUtf8("exotic");

    // ARTICULATION
    _propertyTranslations[SoundfontInformation::Property::ARTICULATION]["slow_attack"] = trUtf8("slow attack");
    _propertyTranslations[SoundfontInformation::Property::ARTICULATION]["decaying"] = trUtf8("decaying");
    _propertyTranslations[SoundfontInformation::Property::ARTICULATION]["sustained"] = trUtf8("sustained");
    _propertyTranslations[SoundfontInformation::Property::ARTICULATION]["long_release"] = trUtf8("long release");
    _propertyTranslations[SoundfontInformation::Property::ARTICULATION]["percussive"] = trUtf8("percussive");
    _propertyTranslations[SoundfontInformation::Property::ARTICULATION]["long_evolving"] = trUtf8("long / evolving");
    _propertyTranslations[SoundfontInformation::Property::ARTICULATION]["pulsating"] = trUtf8("pulsating");
    _propertyTranslations[SoundfontInformation::Property::ARTICULATION]["echoing"] = trUtf8("echoing");
    _propertyTranslations[SoundfontInformation::Property::ARTICULATION]["pad"] = trUtf8("pad");
    _propertyTranslations[SoundfontInformation::Property::ARTICULATION]["lead"] = trUtf8("lead");
    _propertyTranslations[SoundfontInformation::Property::ARTICULATION]["monophonic"] = trUtf8("monophonic");
    _propertyTranslations[SoundfontInformation::Property::ARTICULATION]["chord"] = trUtf8("chord");
    _propertyTranslations[SoundfontInformation::Property::ARTICULATION]["glide_pitch_mod"] = trUtf8("glide / pitch mod");
    _propertyTranslations[SoundfontInformation::Property::ARTICULATION]["sweep_filter_mod"] = trUtf8("sweep / filter mod");
    _propertyTranslations[SoundfontInformation::Property::ARTICULATION]["arpeggiated"] = trUtf8("arpeggiated");
    _propertyTranslations[SoundfontInformation::Property::ARTICULATION]["expressive"] = trUtf8("expressive");
    _propertyTranslations[SoundfontInformation::Property::ARTICULATION]["complex"] = trUtf8("complex");
    _propertyTranslations[SoundfontInformation::Property::ARTICULATION]["randomized"] = trUtf8("randomized");

    // GENRE
    _propertyTranslations[SoundfontInformation::Property::GENRE]["classic"] = trUtf8("classic music");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["electronic"] = trUtf8("electronic music");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["trance"] = trUtf8("trance");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["ambient"] = trUtf8("ambient music");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["ambiant"] = trUtf8("ambient music");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["house"] = trUtf8("house");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["techno_electro"] = trUtf8("techno / electro");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["dnb_breaks"] = trUtf8("D'n'B / breaks");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["industrial"] = trUtf8("industrial");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["experimental"] = trUtf8("experimental");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["funk_soul"] = trUtf8("funk / soul");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["reggae_dub"] = trUtf8("reggae / dub");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["latin_afro_cuban"] = trUtf8("latin / afro-cuban");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["pop_rock"] = trUtf8("pop / rock");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["metal"] = trUtf8("metal music");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["hiphop_rap"] = trUtf8("hiphop / rap");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["jazz_swing"] = trUtf8("jazz / swing");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["folk_country"] = trUtf8("folk / country");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["ethnic_world"] = trUtf8("ethnic / world");
    _propertyTranslations[SoundfontInformation::Property::GENRE]["ethnic / world"] = trUtf8("ethnic / world");

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
    _licenseLabels["public_domain"] = trUtf8("public domain");
    _licenseLabels["give_credit"] = trUtf8("give credit");
    _licenseLabels["give_credit_no_restrictions"] = trUtf8("give credit, don't distribute with more restrictions");
    _licenseLabels["modifications_forbidden"] = trUtf8("don't distribute a modified version");
    _licenseLabels["personal_use_and_share"] = trUtf8("personal use");
    _licenseLabels["personal_use_and_share_no_restrictions"] = trUtf8("personal use, don't distribute with more restrictions");
    _licenseLabels["personal_use_only"] = trUtf8("personal use, don't distribute a modified version");

    // Links
    QString language = ContextManager::configuration()->getValue(
                ConfManager::SECTION_NONE, "language",
                QLocale::system().name().section('_', 0, 0)).toString();
    _licenseUrl = "https://www.polyphone-soundfonts.com/";
    _licenseUrl += QString(language != "fr" ? "en/license" : "fr/licence") + "#";
}

void RepositoryManager::initialize()
{   
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
    foreach (QJsonValue jsonSoundfont, jsonSoundfonts)
    {
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
            foreach (QJsonValue jsonTag, jsonTags)
            {
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
        result = trUtf8("piano");
        break;
    case 11:
        result = trUtf8("organ");
        break;
    case 12:
        result = trUtf8("synthesizer");
        break;
    case 13:
        result = trUtf8("harpsichord");
        break;
    case 14:
        result = trUtf8("guitar");
        break;
    case 15:
        result = trUtf8("bass");
        break;
    case 16:
        result = trUtf8("plucked strings");
        break;
    case 17:
        result = trUtf8("bowed strings");
        break;
    case 18:
        result = trUtf8("flute");
        break;
    case 19:
        result = trUtf8("reed");
        break;
    case 20:
        result = trUtf8("brass");
        break;
    case 21:
        result = trUtf8("vocal");
        break;
    case 22:
        result = trUtf8("unpitched percussion");
        break;
    case 23:
        result = trUtf8("melodic percussion");
        break;
    case 24:
        result = trUtf8("sound effect");
        break;
    case 25:
        result = trUtf8("soundscape");
        break;
    case 26:
        result = trUtf8("loop / rythm");
        break;
    case 27:
        result = trUtf8("instrument set");
        break;
    case 28:
        result = trUtf8("unclassifiable");
        break;
    default:
        // Search in the map
        if (_categoryNames.contains(id))
            result = _categoryNames[id];
        else
            result = trUtf8("Category %1").arg(id);
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
            error = trUtf8("Please wait...");
            break;
        case UserManager::BANNED:
            error = trUtf8("Your account has been banned.");
            break;
        case UserManager::DISCONNECTED:
        case UserManager::CONNECTED:
        case UserManager::FAILED:
            error = trUtf8("A Premium account allows you to browse and download all soundfonts available online.");
            break;
        case UserManager::CONNECTED_PREMIUM:
            error = ""; // ok
            break;
        }

        if (!error.isEmpty())
        {
            QMessageBox::warning(QApplication::activeWindow(), trUtf8("Warning"), error);
            return;
        }
    }

    // Ask to open the page
    emit(openSoundfont(soundfontId));
}

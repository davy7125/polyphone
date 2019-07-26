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

#ifndef REPOSITORYMANAGER_H
#define REPOSITORYMANAGER_H

#include <QObject>
#include <QList>
#include <QMap>
#include "soundfontinformation.h"
#include "soundfontfilter.h"
class UrlReaderJson;

class RepositoryManager: public QObject
{
    Q_OBJECT

public:
    static const QString BASE_URL;

    // Singleton: get the instance of the object and finally kill it
    static RepositoryManager * getInstance();
    static void kill();

    // Initialize the soundfont list. The signal "ready()" is emitted when the initialization is complete
    void initialize();

    // Get soundfont information
    SoundfontInformation * getSoundfontInformation(int id);
    QList<SoundfontInformation *> getSoundfontInformation(SoundfontFilter *filter);

    // Get the categories, properties, license and tags
    QList<int> getCategories() { return _categoryNames.keys(); }
    QString getCategoryName(int id);
    QStringList getProperties(SoundfontInformation::Property property) { return _properties[property]; }
    QString getLicenseLabel(QString licenseKey);
    QMap<QString, QString> getLicenseLabels() { return _licenseLabels; }
    QString getLicenseLink(QString licenseKey);
    QStringList getTags() { return _tags; }

    // Ask for opening a soundfont
    void openSoundfont(int soundfontId, bool daily);

signals:
    // Notify that the list is downloading
    void initializing();

    // Emitted when the initialization is complete
    // Possibly contains an error if the soundfont list cannot be downloaded
    void ready(QString error);

    // Open a soundfont from the repository
    void openSoundfont(int id);

private slots:
    void soundfontListAvailable(QString error);

private:
    RepositoryManager();
    ~RepositoryManager();
    void fillPropertyTranslation();
    void fillLicenseLabelAndLink();
    QString loadSoundfontInfo();
    void loadProperty(SoundfontInformation * si, const QJsonValue &value, SoundfontInformation::Property property);
    QString getPropertyTranslation(SoundfontInformation::Property property, QString value);

    static RepositoryManager * s_instance;
    QMap<int, SoundfontInformation *> _soundfontInfos; // Classified by ID
    UrlReaderJson * _urlReaderJsonList;
    QMap<int, QString> _categoryNames;
    QMap<SoundfontInformation::Property, QStringList> _properties;
    QMap<SoundfontInformation::Property, QMap<QString, QString>> _propertyTranslations;
    QMap<QString, QString> _licenseLabels;
    QString _licenseUrl;
    QStringList _tags;
};

#endif // REPOSITORYMANAGER_H

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

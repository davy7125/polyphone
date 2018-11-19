#ifndef DETAILSMANAGER_H
#define DETAILSMANAGER_H

#include <QObject>
#include <QMutex>
#include <QMap>
class SoundfontDetails;
class UrlReaderJson;

class DetailsManager: public QObject
{
    Q_OBJECT

public:
    // Singleton: get the instance of the object and finally kill it
    static DetailsManager * getInstance();
    static void kill();

    // Ask for details. The signal "detailsReady" will be then emitted
    void askForSoundfontDetails(int id);

    // Get the details of a soundfont
    SoundfontDetails * getDetails(int id);

signals:
    // Emitted when soundfont details are ready
    void detailsReady(int id);

    // Emitted when it was not possible to get the details
    void detailsFailed(int id, QString error);

private slots:
    void downloadCompleted(QString error);

private:
    DetailsManager();
    ~DetailsManager();

    void run();

    static DetailsManager * s_instance;
    QMap<int, SoundfontDetails *> _soundfontDetails; // Classified by ID
    UrlReaderJson * _urlReaderDetails;
    int _currentReaderDetailId;
    QList<int> _stack;
    QMutex _mutex;
};

#endif // DETAILSMANAGER_H

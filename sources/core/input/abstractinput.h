#ifndef ABSTRACTINPUT_H
#define ABSTRACTINPUT_H

#include <QObject>
#include <QString>

template<class T>
class QFutureWatcher;
class SoundfontManager;

class AbstractInput: public QObject
{
    Q_OBJECT

public:
    AbstractInput();
    virtual ~AbstractInput();

    /// Initialize the parser (done by the InputFactory)
    void initialize(QString fileName, SoundfontManager * sm);

    /// Process a file. The function returns immediately and the signal "finished" is sent later
    void process();

    /// Return true after having processed the file if it was successful
    bool isSuccess() { return _isSuccess; }

    /// Return a reason when the process couldn't be done (isSuccess being false)
    QString getError() { return _error; }

    /// Index of the soundfont that has been created
    int getSf2Index() { return _sf2Index; }

    /// Name of the file to open
    QString getFileName() { return _fileName; }

signals:
    /// Signal emitted when the file is processed
    void finished();

protected slots:
    virtual void processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int &sf2Index) = 0;

private:
    void processAsync();

    QFutureWatcher<void> * _futureWatcher;
    SoundfontManager * _sm;
    QString _fileName;
    bool _isSuccess;
    QString _error;
    int _sf2Index;
};

#endif // ABSTRACTINPUT_H

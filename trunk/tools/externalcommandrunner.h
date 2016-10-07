#ifndef EXTERNALCOMMANDRUNNER_H
#define EXTERNALCOMMANDRUNNER_H

#include <QObject>
#include <QList>
#include <QProcess>
#include "pile_sf2.h"
class QProgressDialog;
class QTemporaryFile;

class ExternalCommandRunner : public QObject
{
    Q_OBJECT
public:
    /// Create a new instance
    ExternalCommandRunner(Pile_sf2 * sf2, QWidget * parent);
    ~ExternalCommandRunner();

    /// Run a command for each id
    /// Will destroy the object once it is completed (deleteLater)
    void run(QList<EltID> ids, QString command, bool stereo, bool replaceInfo);

private slots:
    /// Called each time a sample has been processed
    void onProcessStateChanged(QProcess::ProcessState state);

signals:
    void finished();

private:
    void processOne();
    void storeStereoIds(QList<EltID> ids);
    void import(EltID id, Sound &sound);

    /// Reference to the parent widget and the sf2
    QWidget * _parent;
    Pile_sf2 * _sf2;

    /// All samples to process with the command (stereo samples allowed)
    QList<QList<EltID> > _remainingIds;

    /// Program to execute
    QString _program;

    /// Arguments associated to the program
    QStringList _arguments;

    /// True if information in samples are replaced
    bool _replaceInfo;

    /// Index of the argument that will contain the sample path
    int _indexWav;

    /// Progress dialog blocking the interaction with the main window
    QProgressDialog * _progress;

    /// Current process
    QProcess * _process;

    /// Path of the temporary file
    QString _pathTempFile;

    /// Current samples that are being edited (only one if mono)
    EltID _id1, _id2;
};

#endif // EXTERNALCOMMANDRUNNER_H

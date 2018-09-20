#ifndef TOOLEXTERNALCOMMAND_H
#define TOOLEXTERNALCOMMAND_H

#include "abstracttooliterating.h"
#include <QObject>
#include <QProcess>
#include "sound.h"
class QProcess;

class ToolExternalCommand: public AbstractToolIterating
{
    Q_OBJECT

public:
    ToolExternalCommand();
    ~ToolExternalCommand();

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/command_line.svg";
    }

    QString getLabel() const override
    {
        return trUtf8("Commande externe");
    }

    QString getCategory() const override
    {
        return trUtf8("Échantillon");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return "smpl:command";
    }

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;

private slots:
    void onProcessStateChanged(QProcess::ProcessState state);

private:
    void storeStereoIds(QList<EltID> ids);
    void import(EltID id, Sound &sound);

    /// All samples than have been processed
    QList<EltID> _processedIds;

    /// Program to execute
    QString _program;

    /// Arguments associated to the program
    QStringList _arguments;

    /// True if information in samples are replaced
    bool _replaceInfo;

    /// Index of the argument that will contain the sample path
    int _indexWav;

    /// Path of the temporary file
    QString _pathTempFile;

    /// Current samples that are being edited (only one if mono)
    EltID _id1, _id2;

    QProcess * _process;
    SoundfontManager * _sm;
};

#endif // TOOLEXTERNALCOMMAND_H

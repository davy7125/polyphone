#ifndef TOOLEXTERNALCOMMAND_H
#define TOOLEXTERNALCOMMAND_H

#include "abstracttooliterating.h"
#include <QObject>
#include "sound.h"
class QProcess;

class ToolExternalCommand: public AbstractToolIterating
{
    Q_OBJECT

public:
    ToolExternalCommand();

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/command_line.svg";
    }

    QString getLabel() const override
    {
        return trUtf8("Commande externe...");
    }

    QString getCategory() const override
    {
        return trUtf8("Traitement des échantillons");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return "smpl:command";
    }

    /// Method executed before the iterating process
    void beforeProcess(IdList ids) override;

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;

protected:
    /// Get the warning to display after the tool is run
    QString getWarning() override;

private:
    void storeStereoIds(QList<EltID> ids);
    void import(EltID id, Sound &sound, SoundfontManager * sm, bool replaceInfo);

    /// All samples than have been processed
    QList<EltID> _processedIds;

    QString _warning;
};

#endif // TOOLEXTERNALCOMMAND_H

#ifndef TOOLSOUNDFONTEXPORT_H
#define TOOLSOUNDFONTEXPORT_H

#include "abstracttoolonestep.h"

class ToolSoundfontExport: public AbstractToolOneStep
{
public:
    ToolSoundfontExport();
    virtual ~ToolSoundfontExport();

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/file-export.svg";
    }

    QString getCategory() const override
    {
        return trUtf8("Fichiers");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return "sf2:export";
    }

    /// Process asynchronously run
    void process(SoundfontManager * sm, IdList ids, AbstractToolParameters * parameters) override;

protected:
    QString getLabelInternal() const override
    {
        return trUtf8("Exporter");
    }

    /// Return true if the tool can be used on the specified ids
    bool isCompatible(IdList ids) override;

    /// Get the warning to display after the tool is run
    QString getWarning() override;

    /// Get a confirmation message after the tool is run
    QString getConfirmation() override;

private:
    EltID mergeSoundfonts(SoundfontManager * sm, QMap<int,  QList<int> > presets);
    QString getName(SoundfontManager * sm, QList<int> sf2Indexes);
    QString getFilePath(QString directory, QString name, int format);
    QString _error;
};

#endif // TOOLSOUNDFONTEXPORT_H

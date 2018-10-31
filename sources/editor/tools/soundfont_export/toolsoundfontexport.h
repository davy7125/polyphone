#ifndef TOOLSOUNDFONTEXPORT_H
#define TOOLSOUNDFONTEXPORT_H

#include "abstracttool.h"
#include <QObject>

class ToolSoundfontExport: public AbstractTool
{
public:
    ToolSoundfontExport();

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/file-export.svg";
    }

    QString getLabel() const override
    {
        return trUtf8("Exporter...");
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

protected:
    /// Return true if the tool can be used on the specified ids
    bool isCompatible(IdList ids) override;

    /// Run the tool, emit the signal "finished" at the end
    void run(SoundfontManager * sm, QWidget * parent, IdList ids, AbstractToolParameters * parameters) override;

    /// Get a confirmation message after the tool is run
    QString getConfirmation() override;
};

#endif // TOOLSOUNDFONTEXPORT_H

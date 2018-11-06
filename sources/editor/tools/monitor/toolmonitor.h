#ifndef TOOLMONITOR_H
#define TOOLMONITOR_H

#include "abstracttool.h"

class ToolMonitor: public AbstractTool
{
public:
    ToolMonitor();

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/monitor.svg";
    }

    QString getLabel() const override
    {
        return trUtf8("Visualiser les paramètres...");
    }

    QString getCategory() const override
    {
        return trUtf8("Analyse");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return _isInst ? "inst:monitor" : "prst:monitor";
    }

protected:
    /// Return true if the tool can be used on the specified ids
    bool isCompatible(IdList ids) override;

    /// Run the tool, emit the signal "finished" at the end
    void runInternal(SoundfontManager * sm, QWidget * parent, IdList ids, AbstractToolParameters * parameters) override;

private:
    bool _isInst;
};

#endif // TOOLMONITOR_H

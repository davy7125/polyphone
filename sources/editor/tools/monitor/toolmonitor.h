#ifndef TOOLMONITOR_H
#define TOOLMONITOR_H

#include "abstracttool.h"

class ToolMonitor: public AbstractTool
{
    Q_OBJECT

public:
    ToolMonitor();

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/monitor.svg";
    }

    QString getCategory() const override
    {
        return trUtf8("Analyze");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return _isInst ? "inst:monitor" : "prst:monitor";
    }

protected:
    QString getLabelInternal() const override
    {
        return trUtf8("Display parameters");
    }

    /// Return true if the tool can be used on the specified ids
    bool isCompatible(IdList ids) override;

    /// Run the tool, emit the signal "finished" at the end
    void runInternal(SoundfontManager * sm, QWidget * parent, IdList ids, AbstractToolParameters * parameters) override;

private:
    bool _isInst;
};

#endif // TOOLMONITOR_H

#ifndef TOOLMONITOR_H
#define TOOLMONITOR_H

#include "abstracttooliterating.h"

class ToolMonitor: public AbstractToolIterating
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

    /// Method executed before the iterating process
    void beforeProcess(IdList ids) override;

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;

private:
    bool _isInst;
};

#endif // TOOLMONITOR_H

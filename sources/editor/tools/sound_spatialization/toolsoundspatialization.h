#ifndef TOOLSOUNDSPATIALIZATION_H
#define TOOLSOUNDSPATIALIZATION_H

#include "abstracttooliterating.h"

class ToolSoundSpatialization: public AbstractToolIterating
{
public:
    ToolSoundSpatialization();

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/spatialization.svg";
    }

    QString getCategory() const override
    {
        return trUtf8("Édition rapide");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return _isInst ? "inst:soundSpatialization" : "prst:soundSpatialization";
    }

    /// Method executed before the iterating process
    void beforeProcess(IdList ids) override;

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;

protected:
    QString getLabelInternal() const override
    {
        return trUtf8("Spatialiser le son");
    }

private:
    bool _isInst;
};

#endif // TOOLSOUNDSPATIALIZATION_H

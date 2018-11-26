#ifndef TOOLCELESTETUNING_H
#define TOOLCELESTETUNING_H

#include "abstracttooliterating.h"
#include <QObject>

class ToolCelesteTuning: public AbstractToolIterating
{
public:
    ToolCelesteTuning();

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/celeste_tuning.svg";
    }

    QString getCategory() const override
    {
        return trUtf8("Édition rapide");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return "inst:celesteTuning";
    }

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;

protected:
    QString getLabelInternal() const override
    {
        return trUtf8("Désaccorder");
    }
};

#endif // TOOLCELESTETUNING_H

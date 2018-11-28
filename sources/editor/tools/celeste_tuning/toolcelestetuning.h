#ifndef TOOLCELESTETUNING_H
#define TOOLCELESTETUNING_H

#include "abstracttooliterating.h"
#include <QObject>

class ToolCelesteTuning: public AbstractToolIterating
{
    Q_OBJECT

public:
    ToolCelesteTuning();

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/celeste_tuning.svg";
    }

    QString getCategory() const override
    {
        return trUtf8("Fast editing");
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
        return trUtf8("Detune");
    }
};

#endif // TOOLCELESTETUNING_H

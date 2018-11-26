#ifndef TOOLAUTODISTRIBUTION_H
#define TOOLAUTODISTRIBUTION_H

#include "abstracttooliterating.h"

class ToolAutoDistribution: public AbstractToolIterating
{
public:
    ToolAutoDistribution() : AbstractToolIterating(elementInst) {}

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/auto_distribution.svg";
    }

    QString getCategory() const override
    {
        return trUtf8("Édition rapide");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return "inst:autoDistribution";
    }

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;

protected:
    QString getLabelInternal() const override
    {
        return trUtf8("Répartir automatiquement les échantillons");
    }
};

#endif // TOOLAUTODISTRIBUTION_H

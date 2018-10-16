#ifndef TOOLMIXTURECREATION_H
#define TOOLMIXTURECREATION_H

#include "abstracttooliterating.h"

class ToolMixtureCreation: public AbstractToolIterating
{
public:
    ToolMixtureCreation();

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/mixture.svg";
    }

    QString getLabel() const override
    {
        return trUtf8("Créer une mixture...");
    }

    QString getCategory() const override
    {
        return trUtf8("Transformation");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return "inst:mixtureCreation";
    }

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;
};

#endif // TOOLMIXTURECREATION_H

#ifndef TOOLCLEANUNUSED_H
#define TOOLCLEANUNUSED_H

#include "abstracttooliterating.h"

class ToolCleanUnused: public AbstractToolIterating
{
public:
    ToolCleanUnused() : AbstractToolIterating(elementSf2) {}

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/remove_unused.svg";
    }

    QString getLabel() const override
    {
        return trUtf8("Enlever les éléments non utilisés");
    }

    QString getCategory() const override
    {
        return trUtf8("Nettoyage");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return "sf2:cleanUnused";
    }

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;
};

#endif // TOOLCLEANUNUSED_H

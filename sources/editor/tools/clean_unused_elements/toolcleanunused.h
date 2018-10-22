#ifndef TOOLCLEANUNUSED_H
#define TOOLCLEANUNUSED_H

#include "abstracttooliterating.h"

class ToolCleanUnused: public AbstractToolIterating
{
public:
    ToolCleanUnused() : AbstractToolIterating(elementSf2, NULL, NULL, false) {}

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

    /// Method executed before the iterating process
    void beforeProcess(IdList ids) override;

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;

    /// Get a confirmation message after the tool is run
    QString getConfirmation() override;

private:
    int _unusedSmpl;
    int _unusedInst;
};

#endif // TOOLCLEANUNUSED_H

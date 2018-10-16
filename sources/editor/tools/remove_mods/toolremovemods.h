#ifndef TOOLREMOVEMODS_H
#define TOOLREMOVEMODS_H

#include "abstracttooliterating.h"

class ToolRemoveMods: public AbstractToolIterating
{
public:
    ToolRemoveMods();

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/remove_mods.svg";
    }

    QString getLabel() const override
    {
        return trUtf8("Enlever les modulateurs");
    }

    QString getCategory() const override
    {
        return trUtf8("Nettoyage");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return _isInst ? "inst:removeMods" : "prst:removeMods";
    }

    /// Method executed before the iterating process
    void beforeProcess(IdList ids) override;

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;

private:
    bool _isInst;
};

#endif // TOOLREMOVEMODS_H

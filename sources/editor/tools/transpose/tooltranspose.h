#ifndef TOOLTRANSPOSE_H
#define TOOLTRANSPOSE_H

#include "abstracttooliterating.h"

class ToolTranspose: public AbstractToolIterating
{
public:
    ToolTranspose();

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/transpose2.svg";
    }

    QString getLabel() const override
    {
        return trUtf8("Transposer...");
    }

    QString getCategory() const override
    {
        return trUtf8("Édition rapide");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return "inst:transpose";
    }

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;
};

#endif // TOOLTRANSPOSE_H

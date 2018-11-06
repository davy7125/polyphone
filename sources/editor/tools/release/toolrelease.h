#ifndef TOOLRELEASE_H
#define TOOLRELEASE_H

#include "abstracttooliterating.h"
#include <QObject>

class ToolRelease: public AbstractToolIterating
{
public:
    ToolRelease();

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/release.svg";
    }

    QString getLabel() const override
    {
        return trUtf8("Release naturelle...");
    }

    QString getCategory() const override
    {
        return trUtf8("Édition rapide");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return "inst:release";
    }

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;
};

#endif // TOOLRELEASE_H

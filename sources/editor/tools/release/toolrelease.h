#ifndef TOOLRELEASE_H
#define TOOLRELEASE_H

#include "abstracttooliterating.h"
#include <QObject>

class ToolRelease: public AbstractToolIterating
{
    Q_OBJECT

public:
    ToolRelease();

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/release.svg";
    }

    QString getCategory() const override
    {
        return trUtf8("Fast editing");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return "inst:release";
    }

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;

protected:
    QString getLabelInternal() const override
    {
        return trUtf8("Natural release");
    }
};

#endif // TOOLRELEASE_H

#ifndef TOOLCHANGEVOLUME_H
#define TOOLCHANGEVOLUME_H

#include "abstracttooliterating.h"
#include <QObject>

class ToolChangeVolume: public AbstractToolIterating
{
    Q_OBJECT

public:
    ToolChangeVolume();

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/change_volume.svg";
    }

    QString getCategory() const override
    {
        return trUtf8("Sample processing");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return "smpl:changeVolume";
    }

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;

protected:
    QString getLabelInternal() const override
    {
        return trUtf8("Change volume");
    }
};

#endif // TOOLCHANGEVOLUME_H

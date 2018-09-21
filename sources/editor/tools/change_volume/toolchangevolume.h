#ifndef TOOLCHANGEVOLUME_H
#define TOOLCHANGEVOLUME_H

#include "abstracttooliterating.h"
#include <QObject>

class ToolChangeVolume: public AbstractToolIterating
{
public:
    ToolChangeVolume();

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/change_volume.svg";
    }

    QString getLabel() const override
    {
        return trUtf8("Modifier volume...");
    }

    QString getCategory() const override
    {
        return trUtf8("Échantillon");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return "smpl:changeVolume";
    }

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;
};

#endif // TOOLCHANGEVOLUME_H

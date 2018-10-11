#ifndef TOOLUNLINKSAMPLE_H
#define TOOLUNLINKSAMPLE_H

#include "abstracttooliterating.h"
#include <QObject>

class ToolUnlinkSample: public AbstractToolIterating
{
public:
    ToolUnlinkSample() : AbstractToolIterating(elementSmpl, NULL, NULL, false) {}

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/unlink.svg";
    }

    QString getLabel() const override
    {
        return trUtf8("Dissocier");
    }

    QString getCategory() const override
    {
        return trUtf8("Échantillons stéréo");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return "smpl:unlink";
    }

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;

private:
    void unlink(SoundfontManager * sm, EltID idSample);
};

#endif // TOOLUNLINKSAMPLE_H

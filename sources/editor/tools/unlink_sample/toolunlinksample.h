#ifndef TOOLUNLINKSAMPLE_H
#define TOOLUNLINKSAMPLE_H

#include "abstracttooliterating.h"
#include <QObject>

class ToolUnlinkSample: public AbstractToolIterating
{
public:
    ToolUnlinkSample() : AbstractToolIterating(elementSmpl)
    {
        _async = false;
    }

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/unlink.svg";
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

protected:
    QString getLabelInternal() const override
    {
        return trUtf8("Dissocier");
    }

private:
    void unlink(SoundfontManager * sm, EltID idSample);
};

#endif // TOOLUNLINKSAMPLE_H

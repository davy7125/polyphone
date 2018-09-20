#ifndef TOOLTRIMEND_H
#define TOOLTRIMEND_H

#include "abstracttooliterating.h"
#include <QObject>

class ToolTrimEnd: public AbstractToolIterating
{
public:
    ToolTrimEnd() : AbstractToolIterating(elementSmpl) {}

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/trim_loop.svg";
    }

    QString getLabel() const override
    {
        return trUtf8("Ajuster à la fin de boucle");
    }

    QString getCategory() const override
    {
        return trUtf8("Échantillon");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return "smpl:trimEnd";
    }

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;
};

#endif // TOOLTRIMEND_H

#ifndef TOOLTRIMSTART_H
#define TOOLTRIMSTART_H

#include "abstracttooliterating.h"
#include <QObject>

class ToolTrimStart: public AbstractToolIterating
{
public:
    ToolTrimStart() : AbstractToolIterating(elementSmpl) {}

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/remove_blank.svg";
    }

    QString getLabel() const override
    {
        return trUtf8("Enlever le silence initial");
    }

    QString getCategory() const override
    {
        return trUtf8("Traitement des échantillons");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return "smpl:trimStart";
    }

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;
};

#endif // TOOLTRIMSTART_H

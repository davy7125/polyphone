#ifndef TOOLTRIMSTART_H
#define TOOLTRIMSTART_H

#include "abstracttooliterating.h"
#include <QObject>

class ToolTrimStart: public AbstractToolIterating
{
    Q_OBJECT

public:
    ToolTrimStart() : AbstractToolIterating(elementSmpl) {}

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/remove_blank.svg";
    }

    QString getCategory() const override
    {
        return trUtf8("Sample processing");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return "smpl:trimStart";
    }

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;

    /// Trim a sample
    static void trim(EltID id);

protected:
    QString getLabelInternal() const override
    {
        return trUtf8("Remove blank at start");
    }
};

#endif // TOOLTRIMSTART_H

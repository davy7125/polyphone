#ifndef TOOLAUTOLOOP_H
#define TOOLAUTOLOOP_H

#include "abstracttooliterating.h"
#include <QObject>

class ToolAutoLoop: public AbstractToolIterating
{
public:
    ToolAutoLoop() : AbstractToolIterating(elementSmpl) {}

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/loop.svg";
    }

    QString getLabel() const override
    {
        return trUtf8("Bouclage automatique");
    }

    QString getCategory() const override
    {
        return trUtf8("Échantillon");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return "smpl:autoLoop";
    }

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;

protected:
    /// Get the warning to display once the tool is run
    QString getWarning() override;

private:
    QStringList _samplesNotLooped;
};

#endif // TOOLAUTOLOOP_H

#ifndef TOOLCHANGEATTENUATION_H
#define TOOLCHANGEATTENUATION_H

#include "abstracttooliterating.h"
#include <QObject>

class ToolChangeAttenuation: public AbstractToolIterating
{
    Q_OBJECT

public:
    ToolChangeAttenuation();

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/change_attenuation.svg";
    }

    QString getCategory() const override
    {
        return trUtf8("Fast editing");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return _isInst ? "inst:changeAttenuation" : "prst:changeAttenuation";
    }

    /// Method executed before the iterating process
    void beforeProcess(IdList ids) override;

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;

protected:
    QString getLabelInternal() const override
    {
        return trUtf8("Change attenuations");
    }

private:
    int limitOffset(int offset);
    bool _isInst;
};

#endif // TOOLCHANGEATTENUATION_H

#ifndef TOOLCHANGEATTENUATION_H
#define TOOLCHANGEATTENUATION_H

#include "abstracttooliterating.h"
#include <QObject>

class ToolChangeAttenuation: public AbstractToolIterating
{
public:
    ToolChangeAttenuation();

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/change_attenuation.svg";
    }

    QString getLabel() const override
    {
        return trUtf8("Modifier les atténuations...");
    }

    QString getCategory() const override
    {
        return trUtf8("Édition rapide");
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

private:
    int limitOffset(int offset);
    bool _isInst;
};

#endif // TOOLCHANGEATTENUATION_H

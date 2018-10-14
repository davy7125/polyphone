#ifndef TOOLGLOBALSETTINGS_H
#define TOOLGLOBALSETTINGS_H

#include "abstracttooliterating.h"
#include <QObject>

class ToolGlobalSettings: public AbstractToolIterating
{
public:
    ToolGlobalSettings();

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/global_setting.svg";
    }

    QString getLabel() const override
    {
        return trUtf8("Paramètrage global...");
    }

    QString getCategory() const override
    {
        return trUtf8("Édition rapide");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return _isInst ? "inst:globalSettings" : "prst:globalSettings";
    }

    /// Method executed before the iterating process
    void beforeProcess(IdList ids) override;

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;

private:
    bool _isInst;
};

#endif // TOOLGLOBALSETTINGS_H

#ifndef TOOLGLOBALSETTINGS_H
#define TOOLGLOBALSETTINGS_H

#include "abstracttooliterating.h"
#include <QObject>

class ToolGlobalSettings: public AbstractToolIterating
{
    Q_OBJECT

public:
    ToolGlobalSettings();

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/global_setting.svg";
    }

    QString getCategory() const override
    {
        return trUtf8("Fast editing");
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

protected:
    QString getLabelInternal() const override
    {
        return trUtf8("Key-based configuration");
    }

private:
    bool _isInst;
};

#endif // TOOLGLOBALSETTINGS_H

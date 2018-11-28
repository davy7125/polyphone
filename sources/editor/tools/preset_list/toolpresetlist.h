#ifndef TOOLPRESETLIST_H
#define TOOLPRESETLIST_H

#include "abstracttooliterating.h"

class ToolPresetList: public AbstractToolIterating
{
    Q_OBJECT

public:
    ToolPresetList();

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/preset_list.svg";
    }

    QString getCategory() const override
    {
        return trUtf8("Utility");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return "sf2:presetList";
    }

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;

protected:
    QString getLabelInternal() const override
    {
        return trUtf8("Export preset list");
    }
};

#endif // TOOLPRESETLIST_H

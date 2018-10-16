#ifndef TOOLPRESETLIST_H
#define TOOLPRESETLIST_H

#include "abstracttooliterating.h"

class ToolPresetList: public AbstractToolIterating
{
public:
    ToolPresetList();

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/preset_list.svg";
    }

    QString getLabel() const override
    {
        return trUtf8("Exporter la liste des presets...");
    }

    QString getCategory() const override
    {
        return trUtf8("Utilitaire");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return "sf2:presetList";
    }

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;
};

#endif // TOOLPRESETLIST_H

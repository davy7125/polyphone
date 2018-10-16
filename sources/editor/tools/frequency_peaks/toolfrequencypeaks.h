#ifndef TOOLFREQUENCYPEAKS_H
#define TOOLFREQUENCYPEAKS_H

#include "abstracttooliterating.h"

class ToolFrequencyPeaks: public AbstractToolIterating
{
public:
    ToolFrequencyPeaks() : AbstractToolIterating(elementSmpl) {}

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/peak_export.svg";
    }

    QString getLabel() const override
    {
        return trUtf8("Visualiser les pics de fréquences");
    }

    QString getCategory() const override
    {
        return trUtf8("Analyse");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return "smpl:frequencyPeaks";
    }

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;
};

#endif // TOOLFREQUENCYPEAKS_H

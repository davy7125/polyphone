#ifndef TOOLFREQUENCYFILTER_H
#define TOOLFREQUENCYFILTER_H

#include "abstracttooliterating.h"
#include <QObject>

class ToolFrequencyFilter: public AbstractToolIterating
{
public:
    ToolFrequencyFilter();

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/filter_frequencies.svg";
    }

    QString getLabel() const override
    {
        return trUtf8("Filtrer des fréquences...");
    }

    QString getCategory() const override
    {
        return trUtf8("Traitement des échantillons");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return "smpl:removeFrequencies";
    }

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;
};

#endif // TOOLFREQUENCYFILTER_H

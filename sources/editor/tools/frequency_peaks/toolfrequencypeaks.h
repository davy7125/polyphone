#ifndef TOOLFREQUENCYPEAKS_H
#define TOOLFREQUENCYPEAKS_H

#include "abstracttoolonestep.h"

class ToolFrequencyPeaks: public AbstractToolOneStep
{
    Q_OBJECT

public:
    ToolFrequencyPeaks();

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/peak_export.svg";
    }

    QString getCategory() const override
    {
        return trUtf8("Analyze");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return "smpl:frequencyPeaks";
    }

    /// Process asynchronously run
    void process(SoundfontManager * sm, IdList ids, AbstractToolParameters * parameters) override;

protected:
    QString getLabelInternal() const override
    {
        return trUtf8("Show peak frequency");
    }

    /// Return true if the tool can be used on the specified ids
    bool isCompatible(IdList ids) override;

    /// Get a confirmation message after the tool is run
    QString getConfirmation() override;
};

#endif // TOOLFREQUENCYPEAKS_H

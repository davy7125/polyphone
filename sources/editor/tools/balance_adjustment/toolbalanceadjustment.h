#ifndef TOOLBALANCEADJUSTMENT_H
#define TOOLBALANCEADJUSTMENT_H

#include "abstracttooliterating.h"
#include <QObject>
#include <QMutex>

class ToolBalanceAdjustment: public AbstractToolIterating
{
public:
    ToolBalanceAdjustment() : AbstractToolIterating(elementSmpl) {}

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/balance.svg";
    }

    QString getLabel() const override
    {
        return trUtf8("Équilibrer la balance");
    }

    QString getCategory() const override
    {
        return trUtf8("Échantillons stéréo");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return "smpl:balance";
    }

    /// Method executed before the iterating process
    void beforeProcess(IdList ids) override;

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;

protected:
    /// Get the warning to display after the tool is run
    QString getWarning() override;

private:
    QMutex _mutex;
    IdList _processedSamples;
    QStringList _monoSamplesInError;
};

#endif // TOOLBALANCEADJUSTMENT_H

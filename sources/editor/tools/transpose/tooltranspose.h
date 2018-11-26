#ifndef TOOLTRANSPOSE_H
#define TOOLTRANSPOSE_H

#include "abstracttooliterating.h"

class ToolTranspose: public AbstractToolIterating
{
public:
    ToolTranspose();

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/transpose2.svg";
    }

    QString getCategory() const override
    {
        return trUtf8("Édition rapide");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return "inst:transpose";
    }

    /// Method executed before the iterating process
    void beforeProcess(IdList ids) override;

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;

protected:
    QString getLabelInternal() const override
    {
        return trUtf8("Transposer");
    }

    /// Get the warning to display after the tool is run
    QString getWarning() override;

private:
    QStringList _elementsInError;
    QMutex _mutex;
};

#endif // TOOLTRANSPOSE_H

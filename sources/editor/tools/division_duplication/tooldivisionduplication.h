#ifndef TOOLDIVISIONDUPLICATION_H
#define TOOLDIVISIONDUPLICATION_H

#include "abstracttooliterating.h"

class ToolDivisionDuplication: public AbstractToolIterating
{
    Q_OBJECT
    
public:
    ToolDivisionDuplication();

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/duplicate.svg";
    }

    QString getCategory() const override
    {
        return trUtf8("Transformation");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return _isInst ? "inst:divisionDuplication" : "prst:divisionDuplication";
    }

    /// Method executed before the iterating process
    void beforeProcess(IdList ids) override;

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;

protected:
    QString getLabelInternal() const override
    {
        return trUtf8("Division duplication");
    }

    /// Get the warning to display after the tool is run
    QString getWarning() override;

private:
    bool _isInst;
    QStringList _elementsInError;
    QMutex _mutex;
};

#endif // TOOLDIVISIONDUPLICATION_H

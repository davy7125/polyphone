#ifndef ABSTRACTTOOLITERATING_H
#define ABSTRACTTOOLITERATING_H

#include "abstracttool.h"
class WaitingToolDialog;
class AbstractToolParameters;

class AbstractToolIterating: public AbstractTool
{
    Q_OBJECT

public:
    /// Constructor, with the type of element to manage (can be elementSmpl, elementInst, elementPrst, elementSf2 or a combination)
    AbstractToolIterating(ElementType elementType, AbstractToolParameters * parameters = nullptr, AbstractToolGui * gui = nullptr);
    AbstractToolIterating(QList<ElementType> elementTypes, AbstractToolParameters * parameters = nullptr, AbstractToolGui * gui = nullptr);
    virtual ~AbstractToolIterating() override;

    /// Return true if the tool can be used on the specified ids
    bool isCompatible(IdList ids) override;

    /// Run the tool on a list of id
    void run(SoundfontManager * sm, QWidget * parent, IdList ids, AbstractToolParameters * parameters) override;

    /// Method executed before the iterating process
    virtual void beforeProcess(IdList ids) { Q_UNUSED(ids) }

    /// Process an element
    virtual void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) = 0;

signals:
    void elementProcessed();

protected:
    // Additional configurations
    bool _openWaitDialogJustInProcess; // false by default
    bool _async; // true by default

private slots:
    void onElementProcessed();
    void onCancel();

private:
    QList<ElementType> _elementTypes;
    WaitingToolDialog * _waitingDialog;
    int _steps;
    int _currentStep;
    bool _canceled;
    IdList _idsToProcess;
    SoundfontManager * _sm;
    AbstractToolParameters * _parameters;
};

#endif // ABSTRACTTOOLITERATING_H

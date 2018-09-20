#ifndef ABSTRACTTOOLITERATING_H
#define ABSTRACTTOOLITERATING_H

#include "abstracttool.h"
class WaitingToolDialog;
class AbstractToolParameters;

class AbstractToolIterating: public AbstractTool
{
    Q_OBJECT

public:
    /// Constructor, with the type of element to manage (can be elementSmpl, elementInst, elementPrst or elementSf2)
    AbstractToolIterating(ElementType elementType, AbstractToolParameters * parameters = NULL, AbstractToolGui * gui = NULL, bool async = true);
    virtual ~AbstractToolIterating();

    /// Return true if the tool can be used on the specified ids
    bool isCompatible(IdList ids) override;

    /// Run the tool on a list of id
    void run(SoundfontManager * sm, QWidget * parent, IdList ids, AbstractToolParameters * parameters) override;

    /// Process an element
    virtual void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) = 0;

signals:
    void elementProcessed();

private slots:
    void onElementProcessed();
    void onCancel();

private:
    ElementType _elementType;
    WaitingToolDialog * _waitingDialog;
    int _steps;
    int _currentStep;
    bool _canceled;
    bool _async;
};

#endif // ABSTRACTTOOLITERATING_H

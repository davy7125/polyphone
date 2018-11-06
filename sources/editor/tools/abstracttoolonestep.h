#ifndef ABSTRACTTOOLONESTEP_H
#define ABSTRACTTOOLONESTEP_H

#include "abstracttool.h"
class WaitingToolDialog;
class AbstractToolParameters;

class AbstractToolOneStep : public AbstractTool
{
    Q_OBJECT

public:
    AbstractToolOneStep(AbstractToolParameters * parameters = nullptr, AbstractToolGui * gui = nullptr);
    virtual ~AbstractToolOneStep() override;

    /// Process asynchronously run
    virtual void process(SoundfontManager * sm, IdList ids, AbstractToolParameters * parameters) = 0;

signals:
    void processed();

protected:
    /// Run the tool, emit the signal "finished" at the end
    void runInternal(SoundfontManager * sm, QWidget * parent, IdList ids, AbstractToolParameters * parameters);

private slots:
    void onProcessed();
    void onCancel();

private:
    WaitingToolDialog * _waitingDialog;
    bool _canceled;
};

#endif // ABSTRACTTOOLONESTEP_H

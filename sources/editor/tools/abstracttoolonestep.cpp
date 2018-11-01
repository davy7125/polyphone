#include "abstracttoolonestep.h"
#include "waitingtooldialog.h"
#include "soundfontmanager.h"

class RunnableToolOneStep: public QRunnable
{
public:
    RunnableToolOneStep(SoundfontManager * sm, AbstractToolOneStep * tool, IdList ids, AbstractToolParameters * parameters) : QRunnable(),
        _sm(sm),
        _tool(tool),
        _ids(ids),
        _parameters(parameters)
    {}

    ~RunnableToolOneStep() override
    {
        _tool->processed();
    }

    void run() override
    {
        _tool->process(_sm, _ids, _parameters);
    }

private:
    SoundfontManager * _sm;
    AbstractToolOneStep * _tool;
    IdList _ids;
    AbstractToolParameters * _parameters;
};

AbstractToolOneStep::AbstractToolOneStep(AbstractToolParameters *parameters, AbstractToolGui *gui) : AbstractTool(parameters, gui),
    _waitingDialog(nullptr)
{
    connect(this, SIGNAL(processed()), this, SLOT(onProcessed()), Qt::QueuedConnection);
}

AbstractToolOneStep::~AbstractToolOneStep()
{
    delete _waitingDialog;
}

void AbstractToolOneStep::run(SoundfontManager * sm, QWidget * parent, IdList ids, AbstractToolParameters * parameters)
{
    // Create and open a progress dialog
    if (_waitingDialog != nullptr)
        delete _waitingDialog;

    _waitingDialog = new WaitingToolDialog(this->getLabel(), 0, parent);
    _waitingDialog->show();
    connect(_waitingDialog, SIGNAL(canceled()), this, SLOT(onCancel()));

    // Process the ids
    _canceled = false;
    QThreadPool::globalInstance()->start(new RunnableToolOneStep(sm, this, ids, parameters));
}

void AbstractToolOneStep::onProcessed()
{
    if (_waitingDialog == nullptr)
        return; // Just in case
    delete _waitingDialog;
    _waitingDialog = nullptr;

    if (_canceled)
    {
        SoundfontManager::getInstance()->revertNewEditing();
        emit(finished(false));
    }
    else
        emit(finished(true));
}

void AbstractToolOneStep::onCancel()
{
    _canceled = true;
}

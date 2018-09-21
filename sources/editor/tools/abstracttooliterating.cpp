#include "abstracttooliterating.h"
#include "soundfontmanager.h"
#include <QThreadPool>
#include <QRunnable>
#include <QPushButton>
#include "waitingtooldialog.h"

class RunnableTool: public QRunnable
{
public:
    RunnableTool(SoundfontManager * sm, AbstractToolIterating * tool, EltID id, AbstractToolParameters * parameters) : QRunnable(),
        _sm(sm),
        _tool(tool),
        _id(id),
        _parameters(parameters)
    {}

    ~RunnableTool()
    {
        _tool->elementProcessed();
    }

    void run() override
    {
        _tool->process(_sm, _id, _parameters);
    }

private:
    SoundfontManager * _sm;
    AbstractToolIterating * _tool;
    EltID _id;
    AbstractToolParameters * _parameters;
};

AbstractToolIterating::AbstractToolIterating(ElementType elementType, AbstractToolParameters *parameters, AbstractToolGui *gui, bool async) :
    AbstractTool(parameters, gui),
    _elementType(elementType),
    _waitingDialog(NULL),
    _async(async)
{
    connect(this, SIGNAL(elementProcessed()), this, SLOT(onElementProcessed()), Qt::QueuedConnection);
}

AbstractToolIterating::~AbstractToolIterating()
{
    delete _waitingDialog;
}

bool AbstractToolIterating::isCompatible(IdList ids)
{
    return ids.areAllWithType(_elementType);
}

void AbstractToolIterating::run(SoundfontManager * sm, QWidget * parent, IdList ids, AbstractToolParameters *parameters)
{
    _sm = sm;
    _parameters = parameters;

    // Number of steps
    _steps = 0;
    QList<EltID> listID = ids.getSelectedIds(_elementType);
    _idsToProcess.clear();
    foreach (EltID id, listID)
    {
        if (_sm->isValid(id))
        {
            _idsToProcess.append(id);
            _steps++;
        }
    }

    if (_steps == 0)
    {
        emit(finished(false));
        return;
    }
    _currentStep = 0;

    // Create and open a progress dialog
    if (_waitingDialog != NULL)
        delete _waitingDialog;
    _waitingDialog = new WaitingToolDialog(this->getLabel(), _steps, parent);
    _waitingDialog->show();
    connect(_waitingDialog, SIGNAL(canceled()), this, SLOT(onCancel()));

    // Process the ids
    this->beforeProcess();
    _canceled = false;
    if (_async)
        foreach (EltID id, _idsToProcess)
            QThreadPool::globalInstance()->start(new RunnableTool(_sm, this, id, _parameters));
    else
        QThreadPool::globalInstance()->start(new RunnableTool(_sm, this, _idsToProcess.takeFirst(), _parameters));
}

void AbstractToolIterating::onElementProcessed()
{
    if (_waitingDialog == NULL)
        return; // Just in case

    _waitingDialog->setValue(++_currentStep);
    if (_currentStep >= _steps)
    {
        delete _waitingDialog;
        _waitingDialog = NULL;
        if (_canceled)
        {
            SoundfontManager::getInstance()->revertNewEditing();
            emit(finished(false));
        }
        else
            emit(finished(true));
    } else if (!_idsToProcess.empty())
        QThreadPool::globalInstance()->start(new RunnableTool(_sm, this, _idsToProcess.takeFirst(), _parameters));
}

void AbstractToolIterating::onCancel()
{
    _canceled = true;
    QThreadPool::globalInstance()->clear();
}

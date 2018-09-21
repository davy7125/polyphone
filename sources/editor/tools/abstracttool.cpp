#include "abstracttool.h"
#include "soundfontmanager.h"
#include "tooldialog.h"
#include "abstracttoolparameters.h"
#include "abstracttoolgui.h"
#include <QMessageBox>

QWidget * AbstractTool::s_parent = NULL;
SoundfontManager * AbstractTool::s_sm = NULL;

AbstractTool::AbstractTool(AbstractToolParameters *parameters, AbstractToolGui *gui) :
    _toolParameters(parameters),
    _toolGui(gui),
    _toolDialog(NULL)
{
    connect(this, SIGNAL(finished(bool)), this, SLOT(onFinished(bool)), Qt::QueuedConnection);
}

AbstractTool::~AbstractTool()
{
    delete _toolGui;
    delete _toolDialog;
    delete _toolParameters;
}

void AbstractTool::initialize(QWidget * parent)
{
    s_parent = parent;
    s_sm = SoundfontManager::getInstance();
}

bool AbstractTool::setIds(IdList ids)
{
    _currentIds = ids;
    return this->isCompatible(_currentIds);
}

void AbstractTool::run()
{
    // Possibly create a dialog if not already done
    if (_toolGui != NULL && _toolDialog == NULL)
    {
        _toolDialog = new ToolDialog(_toolGui, this, s_parent);
        connect(_toolDialog, SIGNAL(validated()), this, SLOT(onParametersValidated()));
    }

    // Possibly display a dialog
    if (_toolDialog == NULL)
        onParametersValidated();
    else
    {
        _toolParameters->loadConfiguration();
        _toolGui->updateInterface(_toolParameters, _currentIds);
        _toolDialog->show();
    }
}

void AbstractTool::onParametersValidated()
{
    // Get the tool parameters
    if (_toolGui != NULL) {
        _toolGui->saveParameters(_toolParameters);
        _toolParameters->saveConfiguration();
        _toolDialog->close();
    }

    // Run the tool, the signal "finished" will be sent
    run(s_sm, s_parent, _currentIds, _toolParameters);
}

void AbstractTool::onFinished(bool updateNeeded)
{
    if (updateNeeded)
    {
        // Display a warning if there is one
        QString warning = getWarning();
        if (!warning.isEmpty())
            QMessageBox::warning(s_parent, trUtf8("Attention"), warning);

        s_sm->endEditing("tool:" + getIdentifier());
    }
}

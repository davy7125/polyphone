/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program. If not, see http://www.gnu.org/licenses/.    **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 01.01.2013                                           **
***************************************************************************/

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

void AbstractToolOneStep::runInternal(SoundfontManager * sm, QWidget * parent, IdList ids, AbstractToolParameters * parameters)
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

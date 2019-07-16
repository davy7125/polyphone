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

    ~RunnableTool() override
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

AbstractToolIterating::AbstractToolIterating(ElementType elementType, AbstractToolParameters *parameters, AbstractToolGui *gui) :
    AbstractTool(parameters, gui),
    _openWaitDialogJustInProcess(false),
    _async(true),
    _waitingDialog(nullptr)
{
    _elementTypes << elementType;
    connect(this, SIGNAL(elementProcessed()), this, SLOT(onElementProcessed()), Qt::QueuedConnection);
}


AbstractToolIterating::AbstractToolIterating(QList<ElementType> elementTypes, AbstractToolParameters * parameters, AbstractToolGui * gui) :
    AbstractTool(parameters, gui),
    _openWaitDialogJustInProcess(false),
    _async(true),
    _elementTypes(elementTypes),
    _waitingDialog(nullptr)
{
    connect(this, SIGNAL(elementProcessed()), this, SLOT(onElementProcessed()), Qt::QueuedConnection);
}

AbstractToolIterating::~AbstractToolIterating()
{
    delete _waitingDialog;
}

bool AbstractToolIterating::isCompatible(IdList ids)
{
    foreach (ElementType type, _elementTypes)
    {
        if (ids.areAllWithType(type))
            return true;
    }
    return false;
}

void AbstractToolIterating::runInternal(SoundfontManager * sm, QWidget * parent, IdList ids, AbstractToolParameters *parameters)
{
    _sm = sm;
    _parameters = parameters;

    // Number of steps
    _steps = 0;
    QList<EltID> listID;
    foreach (ElementType type, _elementTypes)
    {
        listID = ids.getSelectedIds(type);
        if (!listID.isEmpty())
            break;
    }
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
    if (_waitingDialog != nullptr)
        delete _waitingDialog;

    // Before process
    if (_openWaitDialogJustInProcess)
    {
        this->beforeProcess(_idsToProcess);

        _waitingDialog = new WaitingToolDialog(this->getLabel(), _steps, parent);
        _waitingDialog->show();
        connect(_waitingDialog, SIGNAL(canceled()), this, SLOT(onCancel()));
    }
    else
    {
        _waitingDialog = new WaitingToolDialog(this->getLabel(), _steps, parent);
        _waitingDialog->show();
        connect(_waitingDialog, SIGNAL(canceled()), this, SLOT(onCancel()));

        this->beforeProcess(_idsToProcess);
    }

    // Process the ids
    _canceled = false;
    if (_async)
        foreach (EltID id, _idsToProcess)
            QThreadPool::globalInstance()->start(new RunnableTool(_sm, this, id, _parameters));
    else
        QThreadPool::globalInstance()->start(new RunnableTool(_sm, this, _idsToProcess.takeFirst(), _parameters));
}

void AbstractToolIterating::onElementProcessed()
{
    if (_waitingDialog == nullptr)
        return; // Just in case

    if (_canceled)
    {
        delete _waitingDialog;
        _waitingDialog = nullptr;

        // Revert everything that has been done until now
        SoundfontManager::getInstance()->revertNewEditing();
        emit(finished(false));
    }
    else
    {
        _waitingDialog->setValue(++_currentStep);
        if (_currentStep >= _steps)
        {
            delete _waitingDialog;
            _waitingDialog = nullptr;
             emit(finished(true));
        }
        else if (!_async && !_idsToProcess.empty())
        {
            QThreadPool::globalInstance()->start(new RunnableTool(_sm, this, _idsToProcess.takeFirst(), _parameters));
        }
    }

}

void AbstractToolIterating::onCancel()
{
    _canceled = true;
    QThreadPool::globalInstance()->clear();
}

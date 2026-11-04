/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "abstracttool.h"
#include "soundfontmanager.h"
#include "tooldialog.h"
#include "abstracttoolparameters.h"
#include "abstracttoolgui.h"
#include "contextmanager.h"
#include <QMessageBox>
#include <QEventLoop>

QWidget * AbstractTool::s_parent = nullptr;
SoundfontManager * AbstractTool::s_sm = nullptr;

AbstractTool::AbstractTool(AbstractToolParameters *parameters, AbstractToolGui *gui) :
    _toolParameters(parameters),
    _toolGui(gui),
    _toolDialog(nullptr),
    _silent(false)
{
    connect(this, SIGNAL(finished(bool)), this, SLOT(onFinished(bool)), Qt::QueuedConnection);
}

AbstractTool::~AbstractTool()
{
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

ElementType AbstractTool::getCurrentType() const
{
    if (_currentIds.isEmpty())
        return elementUnknown;
    return _currentIds[0].typeElement;
}

void AbstractTool::completeIdsToProcess()
{
    // Possibly complete the id list if stereo samples are found and if stereo editing is enabled
    _idsToProcess = _currentIds;
    if (ContextManager::configuration()->getValue(ConfManager::SECTION_NONE, "stereo_modification", true).toBool())
    {
        SoundfontManager * sm = SoundfontManager::getInstance();
        EltID idLinked;
        foreach (EltID id, _currentIds)
        {
            // Check it is a sample
            if (id.typeElement == elementSmpl)
            {
                SFSampleLink typeLien = sm->get(id, champ_sfSampleType).sfLinkValue;
                if (typeLien != monoSample && typeLien != RomMonoSample)
                {
                    idLinked = id;
                    idLinked.indexElt = sm->get(id, champ_wSampleLink).wValue;
                    if (!_idsToProcess.contains(idLinked))
                        _idsToProcess << idLinked;
                }
            }
        }
    }
}

void AbstractTool::run()
{
    completeIdsToProcess();

    // Possibly display an error instead of running the tool
    QString error = getBlockingError(_idsToProcess);
    if (!error.isEmpty())
    {
        QMessageBox::warning(s_parent, tr("Warning"), error);
        return;
    }

    // Possibly create a dialog if not already done
    if (_toolGui != nullptr && _toolDialog == nullptr)
    {
        _toolDialog = new ToolDialog(_toolGui, this, s_parent);
        connect(_toolDialog, SIGNAL(validated()), this, SLOT(onParametersValidated()));
    }

    // Possibly display a dialog
    if (_toolDialog == nullptr)
        onParametersValidated();
    else
    {
        _toolParameters->setConfigurationSubSection(this->getIdentifier(true));
        _toolParameters->loadConfiguration();
        _toolGui->updateInterface(_toolParameters, _idsToProcess);
        _toolDialog->show();
    }
}

void AbstractTool::onParametersValidated()
{
    // Get the tool parameters
    if (_toolGui != nullptr)
    {
        _toolGui->saveParameters(_toolParameters);
        _toolParameters->saveConfiguration();
        _toolDialog->close();
    }

    // Run the tool, the signal "finished" will be sent
    runInternal(s_sm, s_parent, _idsToProcess, _toolParameters);
}

void AbstractTool::onFinished(bool updateNeeded)
{
    // Run without dialog: the caller gets the messages and ends the editing
    if (_silent)
    {
        _silent = false;
        return;
    }

    // Display a warning if there is one
    QString warning = getWarning();
    if (!warning.isEmpty())
        QMessageBox::warning(s_parent, tr("Warning"), warning);

    // Possibly display a confirmation
    QString info = getConfirmation();
    if (!info.isEmpty())
        QMessageBox::information(s_parent, tr("Information"), info);

    if (updateNeeded)
        s_sm->endEditing("tool:" + getIdentifier(false));
}

QString AbstractTool::runWithoutDialog(IdList ids, std::function<QString(AbstractToolParameters *)> setParameters, QString &message)
{
    message.clear();
    if (s_sm == nullptr)
        s_sm = SoundfontManager::getInstance();
    if (!setIds(ids) && !canRunWithoutIds())
        return tr("The tool cannot be used with these elements.");
    completeIdsToProcess();

    QString error = getBlockingError(_idsToProcess);
    if (!error.isEmpty())
        return error;

    // Parameters: those of the configuration, then the specified ones
    if (_toolParameters != nullptr)
    {
        _toolParameters->setConfigurationSubSection(this->getIdentifier(true));
        _toolParameters->loadConfiguration();
    }
    error = setParameters(_toolParameters);
    if (!error.isEmpty())
        return error;

    if (!runSynchronously(s_sm, _idsToProcess, _toolParameters))
    {
        // The tool uses other threads and the event loop: wait for the end
        QEventLoop loop;
        bool isFinished = false;
        _silent = true;
        QMetaObject::Connection connection = connect(this, &AbstractTool::finished, &loop, [&](bool updateNeeded) {
            Q_UNUSED(updateNeeded)
            isFinished = true;
            loop.quit();
        });
        runInternal(s_sm, s_parent, _idsToProcess, _toolParameters);
        if (!isFinished)
            loop.exec();
        disconnect(connection);
    }

    QStringList messages;
    QString warning = getWarning();
    if (!warning.isEmpty())
        messages << warning;
    QString confirmation = getConfirmation();
    if (!confirmation.isEmpty())
        messages << confirmation;
    message = messages.join("\n");
    return "";
}

bool AbstractTool::runSynchronously(SoundfontManager * sm, IdList ids, AbstractToolParameters * parameters)
{
    Q_UNUSED(sm)
    Q_UNUSED(ids)
    Q_UNUSED(parameters)
    return false;
}

QString AbstractTool::getIdentifier(bool forConfig) const
{
    QString currentType = "";
    switch (getCurrentType())
    {
    case elementSf2: case elementRootSmpl: case elementRootInst: case elementRootPrst:
        currentType = "sf2";
        break;
    case elementSmpl:
        currentType = "sample";
        break;
    case elementInst: case elementInstSmpl:
        currentType = "instrument";
        break;
    case elementPrst: case elementPrstInst:
        currentType = "preset";
        break;
    default:
        currentType = "unknown";
        break;
    }
    return currentType + (forConfig ? "_" : ":") + getToolName();
}

QString AbstractTool::getLabel(bool withPossibleEllipsis) const
{
    return this->getLabelInternal() + (_toolGui != nullptr && withPossibleEllipsis ? "..." : "");
}

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

#include "toolmixturecreation.h"
#include "toolmixturecreation_gui.h"
#include "soundfontmanager.h"
#include "runnablesamplecreator.h"

ToolMixtureCreation::ToolMixtureCreation() : AbstractTool(new ToolMixtureCreation_parameters(), new ToolMixtureCreation_gui()),
    _waitingDialog(nullptr)
{
    connect(this, SIGNAL(elementProcessed(EltID, int, int, double)), this, SLOT(onElementProcessed(EltID, int, int, double)), Qt::QueuedConnection);
}

ToolMixtureCreation::~ToolMixtureCreation()
{
    delete _waitingDialog;
}

bool ToolMixtureCreation::isCompatible(IdList ids)
{
    // Ok if there is only one instrument
    return ids.getSelectedIds(elementInst).count() == 1;
}

void ToolMixtureCreation::runInternal(SoundfontManager * sm, QWidget * parent, IdList ids, AbstractToolParameters * parameters)
{
    ToolMixtureCreation_parameters * params = dynamic_cast<ToolMixtureCreation_parameters *>(parameters);
    _sampleKey.clear();
    _canceled = false;
    _warning = "";

    // Current instrument
    EltID idInst = ids.getSelectedIds(elementInst)[0];
    idInst.typeElement = elementInst;
    EltID idInstSmpl(elementInstSmpl, idInst.indexSf2, idInst.indexElt);
    if (sm->getSiblings(idInstSmpl).empty())
    {
        _warning = trUtf8("The instrument contains no samples.");
        finished(true);
        return;
    }

    // Get the divisions, remove those that are empty
    QList<DivisionInfo> dis = params->getDivisions();
    for (int i = dis.count() - 1; i >= 0; i--)
        if (dis[0].getRanks().empty())
            dis.removeAt(i);
    if (dis.empty())
        return;

    // Number of keys per sample
    int keyNumber = 1;
    switch (params->getDensityType())
    {
    case 0: keyNumber = 1; break;
    case 1: keyNumber = 3; break;
    case 2: keyNumber = 6; break;
    default:
        return;
    }

    // Other parameters
    bool loopEnabled = params->getLoopSample();
    _stereoSamples = params->getStereoSample();
    _instrumentName = params->getInstrumentName();

    // Compute the number of steps (one for each sample)
    _currentStep = 0;
    _steps = 0;
    for (int i = 0; i < dis.length(); i++)
        _steps += qAbs(dis[i].getMaxKey() - dis[i].getMinKey()) / keyNumber + 1;
    if (_stereoSamples)
        _steps *= 2;

    // Create a new instrument
    _idNewInst = idInst;
    _idNewInst.indexElt = sm->add(_idNewInst);
    sm->set(_idNewInst, champ_name, _instrumentName.left(20));
    if (loopEnabled)
    {
        AttributeValue value;
        value.wValue = 1;
        sm->set(_idNewInst, champ_sampleModes, value);
    }

    // Create and open a progress dialog
    if (_waitingDialog != nullptr)
        delete _waitingDialog;
    _waitingDialog = new WaitingToolDialog(this->getLabel(), _steps, parent);
    _waitingDialog->show();
    connect(_waitingDialog, SIGNAL(canceled()), this, SLOT(onCancel()));

    // For each division
    foreach (DivisionInfo di, dis)
    {
        // Corresponding key range
        int noteStart2 = qMin(di.getMinKey(), di.getMaxKey());
        int noteEnd = qMax(di.getMinKey(), di.getMaxKey());
        int noteStart = noteStart2 + (noteEnd - noteStart2) % keyNumber;

        // For each key
        for (int key = noteStart; key <= noteEnd; key += keyNumber)
        {
            // For each side (left / right)
            for (int side = 0; side < 1 + (_stereoSamples ? 1 : 0); side++)
            {
                RunnableSampleCreator * rsc = new RunnableSampleCreator(this, idInst, di, key,
                                                                        qMax(noteStart2, key - keyNumber + 1), // min key
                                                                        loopEnabled, _stereoSamples, side);
                QThreadPool::globalInstance()->start(rsc);
            }
        }
    }
}

void ToolMixtureCreation::onElementProcessed(EltID idSmpl, int key, int minKey, double minAtt)
{
    if (_waitingDialog == NULL)
        return; // Just in case
    SoundfontManager * sm = SoundfontManager::getInstance();

    // Sample name
    QString name;
    if (_stereoSamples)
        name = _instrumentName.left(15);
    else
        name = _instrumentName.left(16);
    QString str2 = QString("%1").arg(key, 3, 10, QChar('0'));
    bool isRight = false;
    if (_stereoSamples)
    {
        isRight = (sm->get(idSmpl, champ_sfSampleType).sfLinkValue == rightSample);
        if (isRight)
            name = name + ' ' + str2 + 'R';
        else
            name = name + ' ' + str2 + 'L';
    }
    else
        name = name + ' ' + str2;
    sm->set(idSmpl, champ_name, name);

    // Link samples
    AttributeValue value;
    if (_stereoSamples)
    {
        if (_sampleKey.contains(key))
        {
            EltID idLink = _sampleKey.take(key);
            value.wValue = idLink.indexElt;
            sm->set(idSmpl, champ_wSampleLink, value);
            value.wValue = idSmpl.indexElt;
            sm->set(idLink, champ_wSampleLink, value);
        }
        else
            _sampleKey[key] = idSmpl;
    }

    // Ajout du sample dans l'instrument
    EltID idInstSmpl = _idNewInst;
    idInstSmpl.typeElement = elementInstSmpl;
    idInstSmpl.indexElt2 = sm->add(idInstSmpl);

    // Configuration
    value.wValue = idSmpl.indexElt;
    sm->set(idInstSmpl, champ_sampleID, value);
    value.rValue.byLo = minKey;
    value.rValue.byHi = key;
    sm->set(idInstSmpl, champ_keyRange, value);
    if (_stereoSamples)
    {
        if (isRight)
            value.shValue = 500;
        else
            value.shValue = -500;
    }
    else
        value.shValue = 0;
    sm->set(idInstSmpl, champ_pan, value);
    value.wValue = minAtt * 10;
    sm->set(idInstSmpl, champ_initialAttenuation, value);

    // Update the waiting dialog
    _waitingDialog->setValue(++_currentStep);
    if (_currentStep >= _steps)
    {
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
}

void ToolMixtureCreation::onCancel()
{
    _canceled = true;
    QThreadPool::globalInstance()->clear();
}

QString ToolMixtureCreation::getWarning()
{
    return _warning;
}

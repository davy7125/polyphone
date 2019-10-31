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

#include "toolchords.h"
#include "toolchords_gui.h"
#include "soundfontmanager.h"
#include "runnablechordcreator.h"

ToolChords::ToolChords() : AbstractTool(new ToolChords_parameters(), new ToolChords_gui()),
    _waitingDialog(nullptr)
{
    connect(this, SIGNAL(elementProcessed(EltID, quint32, quint32, double)),
            this, SLOT(onElementProcessed(EltID, quint32, quint32, double)), Qt::QueuedConnection);
}

ToolChords::~ToolChords()
{
    delete _waitingDialog;
}

bool ToolChords::isCompatible(IdList ids)
{
    // Ok if there is only one instrument
    return ids.getSelectedIds(elementInst).count() == 1;
}

void ToolChords::runInternal(SoundfontManager * sm, QWidget * parent, IdList ids, AbstractToolParameters * parameters)
{
    ToolChords_parameters * params = dynamic_cast<ToolChords_parameters *>(parameters);
    _canceled = false;
    _warning = "";
    bool loopEnabled = params->getLoopSample();

    // Current instrument
    EltID idInst = ids.getSelectedIds(elementInst)[0];
    idInst.typeElement = elementInst;
    EltID idInstSmpl(elementInstSmpl, idInst.indexSf2, idInst.indexElt);
    if (sm->getSiblings(idInstSmpl).empty())
    {
        _warning = tr("The instrument contains no samples.");
        finished(true);
        return;
    }

    // Create a new instrument
    _instrumentName = params->getInstrumentName();
    _idNewInst = idInst;
    _idNewInst.indexElt = sm->add(_idNewInst);
    sm->set(_idNewInst, champ_name, _instrumentName.left(20));
    if (loopEnabled)
    {
        AttributeValue value;
        value.wValue = 1;
        sm->set(_idNewInst, champ_sampleModes, value);
    }

    // Number of keys per sample
    quint32 keyNumber = 1;
    switch (params->getDensityType())
    {
    case 0: keyNumber = 1; break;
    case 1: keyNumber = 3; break;
    case 2: keyNumber = 6; break;
    default:
        return;
    }

    // Compute the number of steps
    _currentStep = 0;
    RangesType range = this->getInstrumentRange(idInst);
    _steps = static_cast<quint32>(qAbs(range.byHi - range.byLo)) / keyNumber + 1;
    _stereoSamples = params->getStereoSample();
    if (_stereoSamples)
        _steps *= 2;

    // Create and open a progress dialog
    if (_waitingDialog != nullptr)
        delete _waitingDialog;
    _waitingDialog = new WaitingToolDialog(this->getLabel(), _steps, parent);
    _waitingDialog->show();
    connect(_waitingDialog, SIGNAL(canceled()), this, SLOT(onCancel()));

    // For each division
    quint32 noteStart2 = range.byLo;
    quint32 noteEnd = range.byHi;
    quint32 noteStart = noteStart2 + (noteEnd - noteStart2) % keyNumber;

    // For each key
    ChordInfo ci = params->getChordConfiguration();
    for (quint32 key = noteStart; key <= noteEnd; key += keyNumber)
    {
        // For each side (left / right)
        for (int side = 0; side < 1 + (_stereoSamples ? 1 : 0); side++)
        {
            RunnableChordCreator * rcc = new RunnableChordCreator(
                        this, idInst, ci, key,
                        qMax(noteStart2, key - keyNumber + 1), // min key
                        loopEnabled, _stereoSamples, side);
            QThreadPool::globalInstance()->start(rcc);
        }
    }
}

void ToolChords::onElementProcessed(EltID idSmpl, quint32 key, quint32 minKey, double minAtt)
{
    if (_waitingDialog == nullptr)
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
            value.wValue = static_cast<quint16>(idLink.indexElt);
            sm->set(idSmpl, champ_wSampleLink, value);
            value.wValue = static_cast<quint16>(idSmpl.indexElt);
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
    value.wValue = static_cast<quint16>(idSmpl.indexElt);
    sm->set(idInstSmpl, champ_sampleID, value);
    value.rValue.byLo = static_cast<quint8>(minKey);
    value.rValue.byHi = static_cast<quint8>(key);
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
    value.wValue = static_cast<quint16>(minAtt * 10. + 0.5);
    sm->set(idInstSmpl, champ_initialAttenuation, value);

    // Update the waiting dialog
    _waitingDialog->setValue(static_cast<int>(++_currentStep));
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

void ToolChords::onCancel()
{
    _canceled = true;
    QThreadPool::globalInstance()->clear();
}

QString ToolChords::getWarning()
{
    return _warning;
}

RangesType ToolChords::getInstrumentRange(EltID idInst)
{
    RangesType result;
    result.byLo = 127;
    result.byHi = 0;

    // Check the range of all divisions
    SoundfontManager * sm = SoundfontManager::getInstance();
    EltID idInstSmpl(elementInstSmpl, idInst.indexSf2, idInst.indexElt);
    bool useDefault = false;
    foreach (int index, sm->getSiblings(idInstSmpl))
    {
        idInstSmpl.indexElt2 = index;

        if (sm->isSet(idInstSmpl, champ_keyRange))
        {
            RangesType rangeTmp = sm->get(idInstSmpl, champ_keyRange).rValue;
            if (rangeTmp.byLo < result.byLo)
                result.byLo = rangeTmp.byLo;
            if (rangeTmp.byHi > result.byHi)
                result.byHi = rangeTmp.byHi;
        }
        else
            useDefault = true;
    }

    // Check the range of the instrument, if needed
    if (useDefault && sm->isSet(idInst, champ_keyRange))
    {
        RangesType defaultRange = sm->get(idInst, champ_keyRange).rValue;
        if (defaultRange.byLo < result.byLo)
            result.byLo = defaultRange.byLo;
        if (defaultRange.byHi > result.byHi)
            result.byHi = defaultRange.byHi;
    }

    // Final check
    if (result.byHi < result.byLo)
    {
        quint8 tmp = result.byLo;
        result.byLo = result.byHi;
        result.byHi = tmp;
    }

    return result;
}

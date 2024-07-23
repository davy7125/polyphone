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

#include "toolmonitor_gui.h"
#include "ui_toolmonitor_gui.h"
#include "toolmonitor_parameters.h"
#include "soundfontmanager.h"
#include "contextmanager.h"
#include "segment.h"

ToolMonitor_gui::ToolMonitor_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolMonitor_gui)
{
    ui->setupUi(this);

    _attributeList << champ_initialAttenuation
                   << champ_pan
                   << champ_coarseTune
                   << champ_fineTune
                   << champ_scaleTuning
                   << champ_initialFilterFc
                   << champ_initialFilterQ
                   << champ_delayVolEnv
                   << champ_attackVolEnv
                   << champ_holdVolEnv
                   << champ_decayVolEnv
                   << champ_sustainVolEnv
                   << champ_releaseVolEnv
                   << champ_keynumToVolEnvHold
                   << champ_keynumToVolEnvDecay
                   << champ_delayModEnv
                   << champ_attackModEnv
                   << champ_holdModEnv
                   << champ_decayModEnv
                   << champ_sustainModEnv
                   << champ_releaseModEnv
                   << champ_modEnvToPitch
                   << champ_modEnvToFilterFc
                   << champ_keynumToModEnvHold
                   << champ_keynumToModEnvDecay
                   << champ_delayModLFO
                   << champ_freqModLFO
                   << champ_modLfoToPitch
                   << champ_modLfoToFilterFc
                   << champ_modLfoToVolume
                   << champ_delayVibLFO
                   << champ_freqVibLFO
                   << champ_vibLfoToPitch
                   << champ_chorusEffectsSend
                   << champ_reverbEffectsSend;
}

ToolMonitor_gui::~ToolMonitor_gui()
{
    delete ui;
}

void ToolMonitor_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    _isInst = (ids.isEmpty() || ids[0].typeElement == elementInst || ids[0].typeElement == elementInstSmpl);
    _initialID = ids.empty() ? EltID(elementUnknown) : ids[0];
    ToolMonitor_parameters * params = dynamic_cast<ToolMonitor_parameters *>(parameters);

    ui->comboParameter->blockSignals(true);
    ui->comboParameter->clear();
    for (int i = 0; i < _attributeList.size(); i++)
    {
        ui->comboParameter->addItem(Attribute::getDescription(_attributeList.at(i), !_isInst));
        ui->comboParameter->setItemData(i, static_cast<int>(_attributeList.at(i)));
    }
    ui->comboParameter->setCurrentIndex(_isInst ? params->getInstAttribute() : params->getPrstAttribute());
    ui->comboParameter->blockSignals(false);
    ui->checkLog->blockSignals(true);
    ui->checkLog->setChecked(_isInst ? params->getInstLog() : params->getPrstLog());
    ui->checkLog->blockSignals(false);

    // Draw
    this->on_checkLog_stateChanged(_isInst ? params->getInstLog() : params->getPrstLog());
    this->on_comboParameter_currentIndexChanged(_isInst ? params->getInstAttribute() : params->getPrstAttribute());

    // Legends
    ui->widgetLegendDefined->plot(ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND), 5);
    ui->widgetLegendDefault->plot(ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND), 2);
    QColor color = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT);
    color.setAlpha(180);
    ui->widgetLegendMoyenne->plot(color, 2);
}

void ToolMonitor_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolMonitor_parameters * params = dynamic_cast<ToolMonitor_parameters *>(parameters);
    if (_isInst)
    {
        params->setInstLog(ui->checkLog->isChecked());
        params->setInstAttribute(ui->comboParameter->currentIndex());
    }
    else
    {
        params->setPrstLog(ui->checkLog->isChecked());
        params->setPrstAttribute(ui->comboParameter->currentIndex());
    }
}

void ToolMonitor_gui::on_comboParameter_currentIndexChanged(int index)
{
    // Create data
    SoundfontManager * sm = SoundfontManager::getInstance();
    double globalValue = 0;
    bool globalValueSet = false;
    QList<Segment *> segments;
    AttributeType champ = static_cast<AttributeType>(ui->comboParameter->itemData(index).toInt());

    EltID id = _initialID;
    if (id.typeElement != elementUnknown)
    {
        // Global value
        if (_isInst)
            id.typeElement = elementInst;
        else
            id.typeElement = elementPrst;

        if (sm->isSet(id, champ))
        {
            globalValueSet = true;
            globalValue = Attribute::toRealValue(champ, !_isInst, sm->get(id, champ));
        }
        else
        {
            globalValueSet = false;
            globalValue = Attribute::getDefaultRealValue(champ, !_isInst);
        }

        if (_isInst)
            id.typeElement = elementInstSmpl;
        else
            id.typeElement = elementPrstInst;

        // Overrides
        foreach (int i, sm->getSiblings(id))
        {
            id.indexElt2 = i;
            if (sm->isSet(id, champ))
            {
                // Add a segment for a value defined in a division
                RangesType keyRange;
                if (sm->isSet(id, champ_keyRange))
                    keyRange = sm->get(id, champ_keyRange).rValue;
                else
                {
                    keyRange.byLo = 0;
                    keyRange.byHi = 127;
                }

                RangesType velRange;
                if (sm->isSet(id, champ_velRange))
                    velRange = sm->get(id, champ_velRange).rValue;
                else
                {
                    velRange.byLo = 0;
                    velRange.byHi = 127;
                }

                float val = static_cast<float>(Attribute::toRealValue(champ, !_isInst, sm->get(id, champ)));
                segments << new Segment(keyRange.byLo, keyRange.byHi, velRange.byLo, velRange.byHi, val);
            }
        }
    }

    // Send data to the graphics
    ui->graphVisualizer->setData(globalValue, globalValueSet, segments);
}

void ToolMonitor_gui::on_checkLog_stateChanged(int arg1)
{
    // Update scale
    ui->graphVisualizer->setIsLog(arg1);
}

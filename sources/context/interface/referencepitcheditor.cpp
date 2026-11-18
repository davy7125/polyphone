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

#include "referencepitcheditor.h"
#include "contextmanager.h"
#include "temperamentsettings.h"
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QGridLayout>

ReferencePitchEditor::ReferencePitchEditor(QWidget * parent) : QWidget(parent),
    _isSaving(false)
{
    _comboKey = new QComboBox();
    _comboKey->setToolTip(tr("Note sounding at this frequency, whatever the temperament"));
    _spinFrequency = new QDoubleSpinBox();
    _spinFrequency->setDecimals(1);
    _spinFrequency->setSingleStep(0.1);
    _spinFrequency->setSuffix(" " + tr("Hz", "unit for Herz"));
    _spinFrequency->setToolTip(_comboKey->toolTip());

    // The note and the frequency share the width
    QHBoxLayout * layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);
    layout->addWidget(_comboKey, 1);
    _labelEqual = new QLabel("=");
    layout->addWidget(_labelEqual);
    layout->addWidget(_spinFrequency, 1);

    updateNames();
    connect(_comboKey, SIGNAL(currentIndexChanged(int)), this, SLOT(onKeyChanged(int)));
    connect(_spinFrequency, SIGNAL(valueChanged(double)), this, SLOT(onFrequencyChanged(double)));
    connect(ContextManager::configuration(), SIGNAL(soundEngineConfigurationChanged()), this, SLOT(updateValues()));
    connect(ContextManager::configuration(), SIGNAL(interfaceChanged()), this, SLOT(updateNames()));
}

void ReferencePitchEditor::moveToGrid(QGridLayout * grid, int row, int column)
{
    QWidget * widgets[3] = { _comboKey, _labelEqual, _spinFrequency };
    for (int i = 0; i < 3; i++)
    {
        this->layout()->removeWidget(widgets[i]);
        grid->addWidget(widgets[i], row, column + i);
    }
    this->hide();
}

void ReferencePitchEditor::updateNames()
{
    // Notes from A#3 to A4 (see TemperamentSettings): "69 (A)" if the keys are numbered, otherwise "A4", "A3" or "A5"
    // depending on the name of the middle C
    KeyNameManager * keyNames = ContextManager::keyName();
    _comboKey->blockSignals(true);
    _comboKey->clear();
    for (int key = 0; key < 12; key++)
    {
        unsigned int keyNum = static_cast<unsigned int>(key <= 9 ? 60 + key : 48 + key);
        _comboKey->addItem(keyNames->getNameMiddleC() == KeyNameManager::MIDDLE_C_60 ?
                               QString::number(keyNum) + " (" + keyNames->getKeyName(keyNum, true, false, false, true) + ")" :
                               keyNames->getKeyName(keyNum));
    }
    _comboKey->blockSignals(false);
    updateValues();
}

void ReferencePitchEditor::updateValues()
{
    if (_isSaving)
        return;

    int key = TemperamentSettings::getReferencePitchKey();
    _comboKey->blockSignals(true);
    _comboKey->setCurrentIndex(key);
    _comboKey->blockSignals(false);

    _spinFrequency->blockSignals(true);
    _spinFrequency->setRange(TemperamentSettings::getReferencePitchMinimum(key), TemperamentSettings::getReferencePitchMaximum(key));
    _spinFrequency->setValue(TemperamentSettings::getReferencePitch(key));
    _spinFrequency->blockSignals(false);
}

void ReferencePitchEditor::reset()
{
    TemperamentSettings::setReferencePitch(9, 440.0);
}

void ReferencePitchEditor::onKeyChanged(int index)
{
    // Same tuning, the frequency of the other note being shown
    TemperamentSettings::setReferencePitch(index, TemperamentSettings::getReferencePitch(index));
}

void ReferencePitchEditor::onFrequencyChanged(double value)
{
    _isSaving = true;
    TemperamentSettings::setReferencePitch(_comboKey->currentIndex(), value);
    _isSaving = false;
}

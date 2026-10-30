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

#include "effectarea.h"
#include "ui_effectarea.h"
#include "contextmanager.h"

EffectArea::EffectArea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EffectArea)
{
    ui->setupUi(this);
    updateValues();

    // The same values can be changed in the sound settings
    connect(ContextManager::configuration(), SIGNAL(soundEngineConfigurationChanged()), this, SLOT(updateValues()));
}

EffectArea::~EffectArea()
{
    delete ui;
}

void EffectArea::updateValues()
{
    // Reverb
    ui->pushReverb->blockSignals(true);
    ui->pushReverb->setChecked(ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "rev_on", true).toBool());
    ui->pushReverb->blockSignals(false);

    ui->dialRevLevel->blockSignals(true);
    ui->dialRevLevel->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "rev_level", 0).toInt());
    ui->dialRevLevel->blockSignals(false);

    ui->dialRevSize->blockSignals(true);
    ui->dialRevSize->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "rev_size", 0).toInt());
    ui->dialRevSize->blockSignals(false);

    ui->dialRevWidth->blockSignals(true);
    ui->dialRevWidth->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "rev_width", 0).toInt());
    ui->dialRevWidth->blockSignals(false);

    ui->dialRevDamping->blockSignals(true);
    ui->dialRevDamping->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "rev_damping", 0).toInt());
    ui->dialRevDamping->blockSignals(false);

    // Chorus
    ui->pushChorus->blockSignals(true);
    ui->pushChorus->setChecked(ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "cho_on", true).toBool());
    ui->pushChorus->blockSignals(false);

    ui->dialChoLevel->blockSignals(true);
    ui->dialChoLevel->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "cho_level", 0).toInt());
    ui->dialChoLevel->blockSignals(false);

    ui->dialChoDepth->blockSignals(true);
    ui->dialChoDepth->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "cho_depth", 0).toInt());
    ui->dialChoDepth->blockSignals(false);

    ui->dialChoFrequency->blockSignals(true);
    ui->dialChoFrequency->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "cho_frequency", 0).toInt());
    ui->dialChoFrequency->blockSignals(false);

    // Volume
    int gain = ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "gain", 0).toInt();
    ui->sliderGain->blockSignals(true);
    ui->sliderGain->setValue(gain);
    ui->sliderGain->blockSignals(false);
    updateGainLabel(gain);
}

void EffectArea::on_pushReverb_toggled(bool checked)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "rev_on", checked);
}

void EffectArea::on_dialRevLevel_valueChanged(int value)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "rev_level", value);
}

void EffectArea::on_dialRevSize_valueChanged(int value)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "rev_size", value);
}

void EffectArea::on_dialRevWidth_valueChanged(int value)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "rev_width", value);
}

void EffectArea::on_dialRevDamping_valueChanged(int value)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "rev_damping", value);
}

void EffectArea::on_pushChorus_toggled(bool checked)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "cho_on", checked);
}

void EffectArea::on_dialChoLevel_valueChanged(int value)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "cho_level", value);
}

void EffectArea::on_dialChoDepth_valueChanged(int value)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "cho_depth", value);
}

void EffectArea::on_dialChoFrequency_valueChanged(int value)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "cho_frequency", value);
}

void EffectArea::on_sliderGain_valueChanged(int value)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "gain", value);
    updateGainLabel(value);
}

void EffectArea::updateGainLabel(int value)
{
    ui->labelGain->setText((value >= 0 ? "+" : "") + QString::number(value) + " " + tr("dB", "unit for decibels"));
}

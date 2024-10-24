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

#include "toolmergesamples_gui.h"
#include "ui_toolmergesamples_gui.h"
#include "toolmergesamples_parameters.h"

ToolMergeSamples_gui::ToolMergeSamples_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolMergeSamples_gui)
{
    ui->setupUi(this);
}

ToolMergeSamples_gui::~ToolMergeSamples_gui()
{
    delete ui;
}

void ToolMergeSamples_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    Q_UNUSED(ids)
    ToolMergeSamples_parameters * params = (ToolMergeSamples_parameters *) parameters;
    
    // Preset & samples parameters
    ui->lineName->setText(params->getPresetName());
    ui->comboFreq->setCurrentIndex(params->getDensityType());
    ui->checkLoop->setChecked(params->getLoopSample());
    ui->checkStereo->setChecked(params->getStereoSample());
    ui->spinSustainDuration->setValue(params->getSampleSustainDuration());
    ui->spinReleaseDuration->setValue(params->getSampleReleaseDuration());
}

void ToolMergeSamples_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolMergeSamples_parameters * params = (ToolMergeSamples_parameters *) parameters;
    
    // Preset & samples parameters
    params->setPresetName(ui->lineName->text());
    params->setDensityType(ui->comboFreq->currentIndex());
    params->setLoopSample(ui->checkLoop->isChecked());
    params->setStereoSample(ui->checkStereo->isChecked());
    params->setSampleSustainDuration(ui->spinSustainDuration->value());
    params->setSampleReleaseDuration(ui->spinReleaseDuration->value());
}

void ToolMergeSamples_gui::on_pushCancel_clicked()
{
    emit this->canceled();
}

void ToolMergeSamples_gui::on_pushOk_clicked()
{
    emit this->validated();
}

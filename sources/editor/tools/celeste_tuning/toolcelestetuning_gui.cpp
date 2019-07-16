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

#include "toolcelestetuning_gui.h"
#include "ui_toolcelestetuning_gui.h"
#include "toolcelestetuning_parameters.h"
#include "contextmanager.h"

ToolCelesteTuning_gui::ToolCelesteTuning_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolCelesteTuning_gui)
{
    ui->setupUi(this);
}

ToolCelesteTuning_gui::~ToolCelesteTuning_gui()
{
    delete ui;
}

void ToolCelesteTuning_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    Q_UNUSED(ids)
    ToolCelesteTuning_parameters * params = dynamic_cast<ToolCelesteTuning_parameters *>(parameters);

    // Load parameters
    ui->doubleSpinHerz->setValue(params->getBaseFrequency());
    ui->doubleSpinDiv->setValue(params->getCoefficient());
    ui->label->setText(trUtf8("Number of beats per second") + " (" + trUtf8("key") + " " + ContextManager::keyName()->getKeyName(60) + ")");
}

void ToolCelesteTuning_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolCelesteTuning_parameters * params = dynamic_cast<ToolCelesteTuning_parameters *>(parameters);

    // Save current parameters
    params->setBaseFrequency(ui->doubleSpinHerz->value());
    params->setCoefficient(ui->doubleSpinDiv->value());
}

void ToolCelesteTuning_gui::on_buttonBox_accepted()
{
    emit(this->validated());
}

void ToolCelesteTuning_gui::on_buttonBox_rejected()
{
    emit(this->canceled());
}

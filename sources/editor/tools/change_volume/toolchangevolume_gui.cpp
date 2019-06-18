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

#include "toolchangevolume_gui.h"
#include "ui_toolchangevolume_gui.h"
#include "toolchangevolume_parameters.h"

ToolChangeVolume_gui::ToolChangeVolume_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolChangeVolume_gui)
{
    ui->setupUi(this);
}

ToolChangeVolume_gui::~ToolChangeVolume_gui()
{
    delete ui;
}

void ToolChangeVolume_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    Q_UNUSED(ids)
    ToolChangeVolume_parameters * params = (ToolChangeVolume_parameters *) parameters;

    // Mode
    switch (params->getMode())
    {
    case 1:
        ui->radioMultiply->setChecked(true);
        break;
    case 2:
        ui->radioNormalize->setChecked(true);
        break;
    default:
        ui->radioAdd->setChecked(true);
    }

    // Values
    ui->doubleSpinAdd->setValue(params->getAddValue());
    ui->doubleSpinMultiply->setValue(params->getMultiplyValue());
    ui->doubleSpinNormalize->setValue(params->getNormalizeValue());
}

void ToolChangeVolume_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolChangeVolume_parameters * params = (ToolChangeVolume_parameters *) parameters;

    // Mode
    int mode = 0;
    if (ui->radioMultiply->isChecked())
        mode = 1;
    else if (ui->radioNormalize->isChecked())
        mode = 2;
    params->setMode(mode);

    // Values
    params->setAddValue(ui->doubleSpinAdd->value());
    params->setMultiplyValue(ui->doubleSpinMultiply->value());
    params->setNormalizeValue(ui->doubleSpinNormalize->value());
}

void ToolChangeVolume_gui::on_radioAdd_toggled(bool checked)
{
    ui->doubleSpinAdd->setEnabled(checked);
}

void ToolChangeVolume_gui::on_radioMultiply_toggled(bool checked)
{
    ui->doubleSpinMultiply->setEnabled(checked);
}

void ToolChangeVolume_gui::on_radioNormalize_toggled(bool checked)
{
    ui->doubleSpinNormalize->setEnabled(checked);
}

void ToolChangeVolume_gui::on_buttonBox_accepted()
{
    emit(this->validated());
}

void ToolChangeVolume_gui::on_buttonBox_rejected()
{
    emit(this->canceled());
}

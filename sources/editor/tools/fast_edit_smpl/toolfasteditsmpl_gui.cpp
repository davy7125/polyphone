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

#include "toolfasteditsmpl_gui.h"
#include "ui_toolfasteditsmpl_gui.h"
#include "toolfasteditsmpl_parameters.h"
#include "basetypes.h"

ToolFastEditSmpl_gui::ToolFastEditSmpl_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolFastEditSmpl_gui)
{
    ui->setupUi(this);

    // Possible parameters to edit
    QList<AttributeType> types;
    types << champ_byOriginalPitch
          << champ_chPitchCorrection
          << champ_dwStartLoop
          << champ_dwEndLoop;
    foreach (AttributeType type, types)
        ui->comboParameter->addItem(Attribute::getDescription(type, false), type);
}

ToolFastEditSmpl_gui::~ToolFastEditSmpl_gui()
{
    delete ui;
}

void ToolFastEditSmpl_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    Q_UNUSED(ids)
    ToolFastEditSmpl_parameters * params = dynamic_cast<ToolFastEditSmpl_parameters *>(parameters);

    // Mode
    if (params->getMode() == 1)
        ui->radioMultiply->setChecked(true);
    else
        ui->radioAdd->setChecked(true);

    // Values
    ui->spinAdd->setValue(params->getAddValue());
    ui->doubleSpinMultiply->setValue(params->getMultiplyValue());

    // Parameter
    int paramToSelect = params->getParameter();
    for (int i = 0; i < ui->comboParameter->count(); i++)
    {
        if (ui->comboParameter->itemData(i).toInt() == paramToSelect)
        {
            ui->comboParameter->setCurrentIndex(i);
            break;
        }
    }
}

void ToolFastEditSmpl_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolFastEditSmpl_parameters * params = dynamic_cast<ToolFastEditSmpl_parameters *>(parameters);

    // Mode
    params->setMode(ui->radioMultiply->isChecked() ? 1 : 0);

    // Values
    params->setAddValue(ui->spinAdd->value());
    params->setMultiplyValue(ui->doubleSpinMultiply->value());

    // Parameter
    params->setParameter(ui->comboParameter->currentData().toInt());
}

void ToolFastEditSmpl_gui::on_radioAdd_toggled(bool checked)
{
    ui->spinAdd->setEnabled(checked);
}

void ToolFastEditSmpl_gui::on_radioMultiply_toggled(bool checked)
{
    ui->doubleSpinMultiply->setEnabled(checked);
}

void ToolFastEditSmpl_gui::on_pushOk_clicked()
{
    emit(this->validated());
}

void ToolFastEditSmpl_gui::on_pushCancel_clicked()
{
    emit(this->canceled());
}

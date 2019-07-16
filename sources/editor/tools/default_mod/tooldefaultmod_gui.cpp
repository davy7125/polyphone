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

#include "tooldefaultmod_gui.h"
#include "ui_tooldefaultmod_gui.h"
#include "tooldefaultmod_parameters.h"
#include "modulatorcell.h"

ToolDefaultMod_gui::ToolDefaultMod_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolDefaultMod_gui),
    _initialized(false)
{
    ui->setupUi(this);
}

ToolDefaultMod_gui::~ToolDefaultMod_gui()
{
    delete ui;
}

void ToolDefaultMod_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    // The interface doesn't depend on the parameters or ids
    Q_UNUSED(parameters)
    Q_UNUSED(ids)

    // Possibly fill the list with all default modulators
    if (!_initialized)
    {
        ModulatorData modData;
        for (quint16 i = 0; i < 10; i++)
        {
            modData.loadDefaultModulator(i);
            modData.index = i;

            // Add a new cell
            ModulatorCell * cell = new ModulatorCell(modData);
            QListWidgetItem * item = new QListWidgetItem();
            item->setSizeHint(cell->size());
            ui->listWidget->addItem(item);
            ui->listWidget->setItemWidget(item, cell);
        }

        _initialized = true;
    }

    // Clear the selection
    for (int i = 0; i < ui->listWidget->count(); i++)
        ui->listWidget->setItemSelected(ui->listWidget->item(i), false);
}

void ToolDefaultMod_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolDefaultMod_parameters * params = dynamic_cast<ToolDefaultMod_parameters *>(parameters);

    // Get the selected modulators
    QList<ModulatorData> modData = ui->listWidget->getSelectedModulators();

    // Possibly set the amount of all modulators to 0
    if (_isActionDisable)
        for (int i = 0; i < modData.count(); i++)
            modData[i].amount = 0;

    // Store the result
    params->setModulators(modData);
}

void ToolDefaultMod_gui::on_pushCancel_clicked()
{
    emit(canceled());
}

void ToolDefaultMod_gui::on_pushOverride_clicked()
{
    _isActionDisable = false;
    emit(validated());
}

void ToolDefaultMod_gui::on_pushDisable_clicked()
{
    _isActionDisable = true;
    emit(validated());
}

void ToolDefaultMod_gui::on_listWidget_itemSelectionChanged()
{
    ui->pushDisable->setEnabled(!ui->listWidget->selectedItems().empty());
    ui->pushOverride->setEnabled(!ui->listWidget->selectedItems().empty());
}

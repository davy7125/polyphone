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

#include "tooldefaultmod_gui.h"
#include "ui_tooldefaultmod_gui.h"
#include "tooldefaultmod_parameters.h"
#include "modulatorcell.h"
#include "contextmanager.h"

ToolDefaultMod_gui::ToolDefaultMod_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolDefaultMod_gui)
{
    ui->setupUi(this);

    // "Override" and "Disable" both validate the tool
    // Same role so that they stay together (a second accept role would be placed before "Cancel" on macOS and GNOME)
    _pushOverride = ui->buttonBox->addButton(tr("&Override"), QDialogButtonBox::YesRole);
    _pushOverride->setEnabled(false);
    _pushDisable = ui->buttonBox->addButton(tr("&Disable"), QDialogButtonBox::YesRole);
    _pushDisable->setEnabled(false);
    ui->buttonBox->addButton(tr("&Cancel"), QDialogButtonBox::RejectRole);
}

ToolDefaultMod_gui::~ToolDefaultMod_gui()
{
    delete ui;
}

void ToolDefaultMod_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    // The interface does not depend on the parameters or ids
    Q_UNUSED(parameters)
    Q_UNUSED(ids)

    // Fill the list with all default modulators
    ui->listWidget->clear();
    int number;
    const ModulatorData * const modData = ModulatorData::getDefaultModulators(
        ContextManager::configuration()->getModulatorVelToFilterType(), number);
    for (quint16 i = 0; i < number; i++)
    {
        // Add a new cell
        ModulatorCell * cell = new ModulatorCell(modData[i], i);
        QListWidgetItem * item = new QListWidgetItem();
        item->setSizeHint(cell->size());
        ui->listWidget->addItem(item);
        ui->listWidget->setItemWidget(item, cell);
    }
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

void ToolDefaultMod_gui::showEvent(QShowEvent * event)
{
    // Without accept role, the button box doesn't choose a default button
    // This must be done once the gui is in its dialog, otherwise the dialog takes the first button
    _pushOverride->setDefault(true);
    AbstractToolGui::showEvent(event);
}

void ToolDefaultMod_gui::on_buttonBox_clicked(QAbstractButton * button)
{
    if (button == _pushOverride || button == _pushDisable)
    {
        _isActionDisable = (button == _pushDisable);
        emit validated();
    }
}

void ToolDefaultMod_gui::on_buttonBox_rejected()
{
    emit canceled();
}

void ToolDefaultMod_gui::on_listWidget_itemSelectionChanged()
{
    _pushDisable->setEnabled(!ui->listWidget->selectedItems().empty());
    _pushOverride->setEnabled(!ui->listWidget->selectedItems().empty());
}

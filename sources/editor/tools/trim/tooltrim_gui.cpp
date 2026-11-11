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

#include "tooltrim_gui.h"
#include "ui_tooltrim_gui.h"
#include "tooltrim_parameters.h"
#include <QPushButton>

ToolTrim_gui::ToolTrim_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolTrim_gui)
{
    ui->setupUi(this);
    _okButton = ui->buttonBox->addButton(tr("&Ok"), QDialogButtonBox::AcceptRole);
    ui->buttonBox->addButton(tr("&Cancel"), QDialogButtonBox::RejectRole);

    connect(ui->checkStart, SIGNAL(toggled(bool)), this, SLOT(updateEnabledState()));
    connect(ui->radioEndUnchanged, SIGNAL(toggled(bool)), this, SLOT(updateEnabledState()));
    connect(ui->radioEndLoop, SIGNAL(toggled(bool)), this, SLOT(updateEnabledState()));
    connect(ui->radioEndBlank, SIGNAL(toggled(bool)), this, SLOT(updateEnabledState()));
    connect(ui->radioLevel, SIGNAL(toggled(bool)), this, SLOT(updateEnabledState()));
}

ToolTrim_gui::~ToolTrim_gui()
{
    delete ui;
}

void ToolTrim_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    Q_UNUSED(ids)
    ToolTrim_parameters * params = dynamic_cast<ToolTrim_parameters *>(parameters);

    ui->checkStart->setChecked(params->getStart());
    switch (params->getEndMode())
    {
    case ToolTrim_parameters::END_UNCHANGED:
        ui->radioEndUnchanged->setChecked(true);
        break;
    case ToolTrim_parameters::END_LOOP:
        ui->radioEndLoop->setChecked(true);
        break;
    default:
        ui->radioEndBlank->setChecked(true);
    }
    if (params->getAutomaticThreshold())
        ui->radioAutomatic->setChecked(true);
    else
        ui->radioLevel->setChecked(true);
    ui->spinThreshold->setValue(qRound(params->getThreshold()));
    updateEnabledState();
}

void ToolTrim_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolTrim_parameters * params = dynamic_cast<ToolTrim_parameters *>(parameters);

    params->setStart(ui->checkStart->isChecked());
    if (ui->radioEndUnchanged->isChecked())
        params->setEndMode(ToolTrim_parameters::END_UNCHANGED);
    else if (ui->radioEndLoop->isChecked())
        params->setEndMode(ToolTrim_parameters::END_LOOP);
    else
        params->setEndMode(ToolTrim_parameters::END_BLANK);
    params->setAutomaticThreshold(ui->radioAutomatic->isChecked());
    params->setThreshold(ui->spinThreshold->value());
}

void ToolTrim_gui::updateEnabledState()
{
    // The level of the blank is used for the start and the end of the sound, not for the loop
    bool blankUsed = ui->checkStart->isChecked() || ui->radioEndBlank->isChecked();
    ui->labelThreshold->setEnabled(blankUsed);
    ui->radioAutomatic->setEnabled(blankUsed);
    ui->radioLevel->setEnabled(blankUsed);
    ui->spinThreshold->setEnabled(blankUsed && ui->radioLevel->isChecked());

    // Nothing to do?
    _okButton->setEnabled(ui->checkStart->isChecked() || !ui->radioEndUnchanged->isChecked());
}

void ToolTrim_gui::on_buttonBox_accepted()
{
    emit this->validated();
}

void ToolTrim_gui::on_buttonBox_rejected()
{
    emit this->canceled();
}

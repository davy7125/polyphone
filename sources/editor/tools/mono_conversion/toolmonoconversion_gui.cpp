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

#include "toolmonoconversion_gui.h"
#include "ui_toolmonoconversion_gui.h"
#include "toolmonoconversion_parameters.h"
#include "toolmonoconversion.h"
#include "soundfontmanager.h"

ToolMonoConversion_gui::ToolMonoConversion_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolMonoConversion_gui)
{
    ui->setupUi(this);
    ui->buttonBox->addButton(tr("&Ok"), QDialogButtonBox::AcceptRole);
    ui->buttonBox->addButton(tr("&Cancel"), QDialogButtonBox::RejectRole);
}

ToolMonoConversion_gui::~ToolMonoConversion_gui()
{
    delete ui;
}

void ToolMonoConversion_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    ToolMonoConversion_parameters * params = dynamic_cast<ToolMonoConversion_parameters *>(parameters);

    // Left and right channels cannot be distinguished if a sample has the type "link"
    SoundfontManager * sm = SoundfontManager::getInstance();
    bool linkedFound = false;
    EltID idLinked;
    foreach (EltID id, ids)
    {
        if (ToolMonoConversion::getLinkedSample(sm, id, idLinked))
        {
            SFSampleLink type = sm->get(id, champ_sfSampleType).sfLinkValue;
            SFSampleLink typeLinked = sm->get(idLinked, champ_sfSampleType).sfLinkValue;
            if (type == linkedSample || type == RomLinkedSample || typeLinked == linkedSample || typeLinked == RomLinkedSample)
            {
                linkedFound = true;
                break;
            }
        }
    }
    QString toolTip = linkedFound ? tr("Not available with samples of type \"link\": the left and right channels cannot be distinguished.") : "";
    ui->radioLeft->setEnabled(!linkedFound);
    ui->radioLeft->setToolTip(toolTip);
    ui->radioRight->setEnabled(!linkedFound);
    ui->radioRight->setToolTip(toolTip);

    // Mode
    switch (params->getMode())
    {
    case 0:
        ui->radioSum->setChecked(true);
        break;
    case 2:
        ui->radioPowerSum->setChecked(true);
        break;
    case 3:
        if (linkedFound)
            ui->radioHalfSum->setChecked(true);
        else
            ui->radioLeft->setChecked(true);
        break;
    case 4:
        if (linkedFound)
            ui->radioHalfSum->setChecked(true);
        else
            ui->radioRight->setChecked(true);
        break;
    default:
        ui->radioHalfSum->setChecked(true);
    }

    // Instruments
    ui->checkMergeDivisions->setChecked(params->getMergeDivisions());
}

void ToolMonoConversion_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolMonoConversion_parameters * params = dynamic_cast<ToolMonoConversion_parameters *>(parameters);

    // Mode
    int mode = 1;
    if (ui->radioSum->isChecked())
        mode = 0;
    else if (ui->radioPowerSum->isChecked())
        mode = 2;
    else if (ui->radioLeft->isChecked())
        mode = 3;
    else if (ui->radioRight->isChecked())
        mode = 4;
    params->setMode(mode);

    // Instruments
    params->setMergeDivisions(ui->checkMergeDivisions->isChecked());
}

void ToolMonoConversion_gui::on_buttonBox_accepted()
{
    emit this->validated();
}

void ToolMonoConversion_gui::on_buttonBox_rejected()
{
    emit this->canceled();
}

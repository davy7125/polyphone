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

#include "tooltranspose_gui.h"
#include "ui_tooltranspose_gui.h"
#include "tooltranspose_parameters.h"
#include "contextmanager.h"

ToolTranspose_gui::ToolTranspose_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolTranspose_gui)
{
    ui->setupUi(this);
}

ToolTranspose_gui::~ToolTranspose_gui()
{
    delete ui;
}

void ToolTranspose_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    Q_UNUSED(ids)
    ToolTranspose_parameters * params = (ToolTranspose_parameters *) parameters;

    // Load parameters
    ui->spinTon->setValue(params->getSemiTones());
    ui->checkKeyRange->setChecked(params->getAdaptKeyRanges());
}

void ToolTranspose_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolTranspose_parameters * params = (ToolTranspose_parameters *) parameters;

    // Save current parameters
    params->setSemiTones(ui->spinTon->value());
    params->setAdaptKeyRanges(ui->checkKeyRange->isChecked());
}

void ToolTranspose_gui::on_buttonBox_accepted()
{
    emit(this->validated());
}

void ToolTranspose_gui::on_buttonBox_rejected()
{
    emit(this->canceled());
}

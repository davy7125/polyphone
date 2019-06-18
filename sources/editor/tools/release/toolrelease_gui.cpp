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

#include "toolrelease_gui.h"
#include "ui_toolrelease_gui.h"
#include "toolrelease_parameters.h"
#include "contextmanager.h"

ToolRelease_gui::ToolRelease_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolRelease_gui)
{
    ui->setupUi(this);
}

ToolRelease_gui::~ToolRelease_gui()
{
    delete ui;
}

void ToolRelease_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    Q_UNUSED(ids)
    ToolRelease_parameters * params = (ToolRelease_parameters *) parameters;

    // Load parameters
    ui->doubleDuree36->setValue(params->getBaseDuration());
    ui->doubleDivision->setValue(params->getEvolution());
    ui->doubleDeTune->setValue(params->getDetune());
    ui->label->setText(trUtf8("Release time (key %1)").arg(ContextManager::keyName()->getKeyName(36)));
}

void ToolRelease_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolRelease_parameters * params = (ToolRelease_parameters *) parameters;

    // Save current parameters
    params->setBaseDuration(ui->doubleDuree36->value());
    params->setEvolution(ui->doubleDivision->value());
    params->setDetune(ui->doubleDeTune->value());
}

void ToolRelease_gui::on_buttonBox_accepted()
{
    emit(this->validated());
}

void ToolRelease_gui::on_buttonBox_rejected()
{
    emit(this->canceled());
}

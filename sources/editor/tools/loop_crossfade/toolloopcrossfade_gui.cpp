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

#include "toolloopcrossfade_gui.h"
#include "ui_toolloopcrossfade_gui.h"
#include "toolloopcrossfade_parameters.h"

ToolLoopCrossfade_gui::ToolLoopCrossfade_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolLoopCrossfade_gui)
{
    ui->setupUi(this);
}

ToolLoopCrossfade_gui::~ToolLoopCrossfade_gui()
{
    delete ui;
}

void ToolLoopCrossfade_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    Q_UNUSED(ids)
    ToolLoopCrossfade_parameters * params = dynamic_cast<ToolLoopCrossfade_parameters *>(parameters);
    ui->spinDuration->setValue(params->getDurationMs());
}

void ToolLoopCrossfade_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolLoopCrossfade_parameters * params = dynamic_cast<ToolLoopCrossfade_parameters *>(parameters);
    params->setDurationMs(ui->spinDuration->value());
}

void ToolLoopCrossfade_gui::on_pushCancel_clicked()
{
    emit this->canceled();
}

void ToolLoopCrossfade_gui::on_pushOk_clicked()
{
    emit this->validated();
}

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

#include "tooltransposesmpl_gui.h"
#include "ui_tooltransposesmpl_gui.h"
#include "tooltransposesmpl_parameters.h"

ToolTransposeSmpl_gui::ToolTransposeSmpl_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolTransposeSmpl_gui)
{
    ui->setupUi(this);
}

ToolTransposeSmpl_gui::~ToolTransposeSmpl_gui()
{
    delete ui;
}

void ToolTransposeSmpl_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    Q_UNUSED(ids)
    ToolTransposeSmpl_parameters * params = (ToolTransposeSmpl_parameters *)parameters;
    ui->spinTon->setValue(params->getSemiTones());
}

void ToolTransposeSmpl_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolTransposeSmpl_parameters * params = (ToolTransposeSmpl_parameters *)parameters;
    params->setSemiTones(ui->spinTon->value());
}

void ToolTransposeSmpl_gui::on_buttonBox_accepted()
{
    emit(this->validated());
}

void ToolTransposeSmpl_gui::on_buttonBox_rejected()
{
    emit(this->canceled());
}

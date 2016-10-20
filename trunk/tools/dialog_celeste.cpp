/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2016 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://polyphone-soundfonts.com                      **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "dialog_celeste.h"
#include "ui_dialog_celeste.h"
#include "confmanager.h"
#include "keynamemanager.h"

DialogCeleste::DialogCeleste(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogCeleste)
{
    // Prepare the interface
    ui->setupUi(this);
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint));

    // Recall previous values
    ui->doubleSpinHerz->setValue(ConfManager::getInstance()->getToolValue(
                                     ConfManager::TOOL_TYPE_INSTRUMENT, "ondulation", "herz", 4.).toDouble());
    ui->doubleSpinDiv->setValue(ConfManager::getInstance()->getToolValue(
                                    ConfManager::TOOL_TYPE_INSTRUMENT, "ondulation", "division", 1.).toDouble());

    ui->label->setText(trUtf8("Nombre de battements par secondes (note ") +
                       KeyNameManager::getInstance()->getKeyName(60) + ")");
}

DialogCeleste::~DialogCeleste()
{
    delete ui;
}

// ACCEPTATION

void DialogCeleste::accept()
{
    // Save current parameters
    ConfManager::getInstance()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "ondulation", "herz", ui->doubleSpinHerz->value());
    ConfManager::getInstance()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "ondulation", "division", ui->doubleSpinDiv->value());

    // Send values
    this->accepted(ui->doubleSpinHerz->value(), ui->doubleSpinDiv->value());
    QDialog::accept();
}

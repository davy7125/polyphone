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

#include "dialog_transposition.h"
#include "ui_dialog_transposition.h"
#include "confmanager.h"

DialogTransposition::DialogTransposition(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogTransposition)
{
    // Prepare the interface
    ui->setupUi(this);
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint));
    this->setAttribute(Qt::WA_DeleteOnClose, true);

    // Recall last configuration
    ui->spinTon->setValue(ConfManager::getInstance()->getToolValue(
                              ConfManager::TOOL_TYPE_INSTRUMENT, "transposition", "ton", 12.).toDouble());
    ui->checkKeyRange->setChecked(ConfManager::getInstance()->getToolValue(
                                      ConfManager::TOOL_TYPE_INSTRUMENT, "transposition", "keyrange", 12).toBool());
}

DialogTransposition::~DialogTransposition()
{
    delete ui;
}

void DialogTransposition::on_buttonBox_accepted()
{
    // Save configuration
    ConfManager::getInstance()->setToolValue(
                ConfManager::TOOL_TYPE_INSTRUMENT, "transposition", "ton", ui->spinTon->value());
    ConfManager::getInstance()->setToolValue(
                ConfManager::TOOL_TYPE_INSTRUMENT, "transposition", "keyrange", ui->checkKeyRange->isChecked());

    // Emit signal and quit
    accepted(ui->spinTon->value(), ui->checkKeyRange->isChecked());
    this->close();
}

void DialogTransposition::on_buttonBox_rejected()
{
    this->close();
}

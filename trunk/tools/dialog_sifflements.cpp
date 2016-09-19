/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
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

#include "dialog_sifflements.h"
#include "ui_dialog_sifflements.h"
#include "confmanager.h"

DialogSifflements::DialogSifflements(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSifflements)
{
    // Preparation of the interface
    ui->setupUi(this);
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint));

    // Recall previous values
    ui->spinFreq1->setValue(ConfManager::getInstance()->getToolValue(ConfManager::TOOL_TYPE_SAMPLE, "sifflements", "debut", 8000).toInt());
    ui->spinFreq2->setValue(ConfManager::getInstance()->getToolValue(ConfManager::TOOL_TYPE_SAMPLE, "sifflements", "fin", 20000).toInt());
    ui->doubleSpinRaideur->setValue(ConfManager::getInstance()->getToolValue(ConfManager::TOOL_TYPE_SAMPLE, "sifflements", "raideur", 5.).toDouble());
}

DialogSifflements::~DialogSifflements()
{
    delete ui;
}

void DialogSifflements::accept()
{
    // Save current values
    ConfManager::getInstance()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "sifflements", "debut", ui->spinFreq1->value());
    ConfManager::getInstance()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "sifflements", "fin", ui->spinFreq2->value());
    ConfManager::getInstance()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "sifflements", "raideur", ui->doubleSpinRaideur->value());

    // Send accepted signal and close the window
    emit(this->accepted(ui->spinFreq1->value(),
                        ui->spinFreq2->value(),
                        ui->doubleSpinRaideur->value()));
    QDialog::accept();
}

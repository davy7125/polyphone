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
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "dialog_release.h"
#include "ui_dialog_release.h"
#include "confmanager.h"
#include "keynamemanager.h"

DialogRelease::DialogRelease(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogRelease)
{
    // Preparation of the interface
    ui->setupUi(this);
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint));

    // Recall previous parameters
    ui->doubleDuree36->setValue(ConfManager::getInstance()->getToolValue(
                                    ConfManager::TOOL_TYPE_INSTRUMENT, "release", "dureeDo", 0.3).toDouble());
    ui->doubleDivision->setValue(ConfManager::getInstance()->getToolValue(
                                     ConfManager::TOOL_TYPE_INSTRUMENT, "release", "division", 2.).toDouble());
    ui->doubleDeTune->setValue(ConfManager::getInstance()->getToolValue(
                                   ConfManager::TOOL_TYPE_INSTRUMENT, "release", "desaccordage", 0.).toDouble());
    ui->label->setText(trUtf8("Durée release (note ") +
                              KeyNameManager::getInstance()->getKeyName(36) + ")");
}

DialogRelease::~DialogRelease()
{
    delete ui;
}

// ACCEPTATION

void DialogRelease::accept()
{
    // Save current parameters
    ConfManager::getInstance()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "release", "dureeDo", ui->doubleDuree36->value());
    ConfManager::getInstance()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "release", "division", ui->doubleDivision->value());
    ConfManager::getInstance()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "release", "desaccordage", ui->doubleDeTune->value());

    // Accept
    this->accepted(ui->doubleDuree36->value(),
                   ui->doubleDivision->value(),
                   ui->doubleDeTune->value());
    QDialog::accept();
}

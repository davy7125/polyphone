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

#include "dialog_change_volume.h"
#include "ui_dialog_change_volume.h"
#include "confmanager.h"

DialogChangeVolume::DialogChangeVolume(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogChangeVolume)
{
    ui->setupUi(this);
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint));

    // Load configuration
    switch (ConfManager::getInstance()->getToolValue(ConfManager::TOOL_TYPE_SAMPLE, "volume", "mode", 0).toInt())
    {
    case 1:
        ui->radioMultiply->setChecked(true);
        break;
    case 2:
        ui->radioNormalize->setChecked(true);
        break;
    default:
        ui->radioAdd->setChecked(true);
    }

    ui->doubleSpinAdd->setValue(ConfManager::getInstance()->getToolValue(
                                    ConfManager::TOOL_TYPE_SAMPLE, "volume", "add", 0.).toDouble());
    ui->doubleSpinMultiply->setValue(ConfManager::getInstance()->getToolValue(
                                         ConfManager::TOOL_TYPE_SAMPLE, "volume", "multiply", 1.).toDouble());
    ui->doubleSpinNormalize->setValue(ConfManager::getInstance()->getToolValue(
                                          ConfManager::TOOL_TYPE_SAMPLE, "volume", "normalize", 90.).toDouble());
}

DialogChangeVolume::~DialogChangeVolume()
{
    delete ui;
}

void DialogChangeVolume::accept()
{
    int mode = 0;
    double value = ui->doubleSpinAdd->value();
    if (ui->radioMultiply->isChecked())
    {
        mode = 1;
        value = ui->doubleSpinMultiply->value();
    }
    else if (ui->radioNormalize->isChecked())
    {
        mode = 2;
        value = ui->doubleSpinNormalize->value();
    }

    // Save parameters
    ConfManager::getInstance()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "volume", "mode", mode);
    ConfManager::getInstance()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "volume", "add", ui->doubleSpinAdd->value());
    ConfManager::getInstance()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "volume", "multiply", ui->doubleSpinMultiply->value());
    ConfManager::getInstance()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "volume", "normalize", ui->doubleSpinNormalize->value());

    emit(accepted(mode, value));
    QDialog::accept();
}

void DialogChangeVolume::on_radioAdd_toggled(bool checked)
{
    ui->doubleSpinAdd->setEnabled(checked);
}

void DialogChangeVolume::on_radioMultiply_toggled(bool checked)
{
    ui->doubleSpinMultiply->setEnabled(checked);
}

void DialogChangeVolume::on_radioNormalize_toggled(bool checked)
{
    ui->doubleSpinNormalize->setEnabled(checked);
}

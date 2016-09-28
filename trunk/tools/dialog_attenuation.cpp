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

#include "dialog_attenuation.h"
#include "ui_dialog_attenuation.h"
#include "confmanager.h"

DialogAttenuation::DialogAttenuation(double minInst, double maxInst, double minPrst, double maxPrst, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAttenuation)
{
    // Prepare the interface
    ui->setupUi(this);
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint));

    // Show the attenuation range for the instruments
    ui->labelRange->setText("[" + formatDouble(minInst) + ", " + formatDouble(maxInst) + "]");

    // Compute and display minimum and maximum offsets for the instruments
    double minOffset = -minInst;
    double maxOffset = 144. - maxInst;
    ui->doubleSpinValue->setMinimum(minOffset);
    ui->doubleSpinValue->setMaximum(maxOffset);
    ui->labelPossibleOffset->setText("[" + formatDouble(minOffset) + ", " + formatDouble(maxOffset) + "]");

    // Recall previous value for the instruments
    double previousVal = ConfManager::getInstance()->getToolValue(ConfManager::TOOL_TYPE_SF2, "attenuation", "dB", 0.).toDouble();
    if (previousVal < minOffset || previousVal > maxOffset)
        ui->doubleSpinValue->setValue(0);
    else
        ui->doubleSpinValue->setValue(previousVal);


    if (minPrst <= maxPrst)
    {
        // Show the attenuation ranges for the presets
        ui->labelRange2->setText("[" + formatDouble(minPrst) + ", " + formatDouble(maxPrst) + "]");

        // Compute and display minimum and maximum offsets for the presets
        double minOffsetPrst = -144. -minPrst;
        double maxOffsetPrst = 144. - maxPrst;
        ui->doubleSpinValue2->setMinimum(minOffsetPrst);
        ui->doubleSpinValue2->setMaximum(maxOffsetPrst);
        ui->labelPossibleOffset2->setText("[" + formatDouble(minOffsetPrst) + ", " + formatDouble(maxOffsetPrst) + "]");

        // Recall previous value for the presets
        double previousVal2 = ConfManager::getInstance()->getToolValue(ConfManager::TOOL_TYPE_SF2, "attenuation", "dB_prst", 0.).toDouble();
        if (previousVal2 < minOffsetPrst || previousVal2 > maxOffsetPrst)
            ui->doubleSpinValue2->setValue(0);
        else
            ui->doubleSpinValue2->setValue(previousVal2);
    }
    else
    {
        ui->groupBox2->setEnabled(false);
        ui->labelRange2->setText("-");
        ui->labelPossibleOffset2->setText("-");
        ui->doubleSpinValue2->setValue(0);
    }
}

DialogAttenuation::~DialogAttenuation()
{
    delete ui;
}

QString DialogAttenuation::formatDouble(double value)
{
    return (value > 0 ? "+" : "") + QString::number(value, 'f', 1);
}

void DialogAttenuation::accept()
{
    // Save current parameter
    ConfManager::getInstance()->setToolValue(ConfManager::TOOL_TYPE_SF2, "attenuation", "dB", ui->doubleSpinValue->value());
    ConfManager::getInstance()->setToolValue(ConfManager::TOOL_TYPE_SF2, "attenuation", "dB_prst", ui->doubleSpinValue2->value());

    // Send value
    emit(accepted(ui->doubleSpinValue->value(), ui->doubleSpinValue2->value()));
    QDialog::accept();
}

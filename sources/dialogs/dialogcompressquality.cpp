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

#include "dialogcompressquality.h"
#include "ui_dialogcompressquality.h"
#include "contextmanager.h"

DialogCompressQuality::DialogCompressQuality(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogCompressQuality)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint));

    int quality = ContextManager::configuration()->getValue(ConfManager::SECTION_TOOLS, "sf2_export", "quality", 1).toInt();
    switch (quality)
    {
    case 0:
        ui->radioButtonLow->setChecked(true);
        break;
    case 2:
        ui->radioButtonHigh->setChecked(true);
        break;
    default:
        ui->radioButtonMiddle->setChecked(true);
        break;
    }
}

DialogCompressQuality::~DialogCompressQuality()
{
    delete ui;
}

void DialogCompressQuality::on_buttonBox_accepted()
{
    int quality = -1;
    if (ui->radioButtonHigh->isChecked())
    {
        quality = 2;
        onOk(1.0);
    }
    if (ui->radioButtonMiddle->isChecked())
    {
        quality = 1;
        onOk(0.6);
    }
    if (ui->radioButtonLow->isChecked())
    {
        quality = 0;
        onOk(0.2);
    }

    if (quality != -1)
        ContextManager::configuration()->setValue(ConfManager::SECTION_TOOLS, "sf2_export", "quality", quality);
}

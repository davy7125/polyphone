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

#include "dialogcreateelements.h"
#include "ui_dialogcreateelements.h"
#include "contextmanager.h"
#include "soundfontmanager.h"

DialogCreateElements::DialogCreateElements(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogCreateElements)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint));
}

DialogCreateElements::~DialogCreateElements()
{
    delete ui;
}

void DialogCreateElements::initialize(IdList ids)
{
    if (ids.empty())
        return;

    // Convert InstSmpl into Smpl and PrstInst into Inst
    for (int i = 0; i < ids.count(); i++)
    {
        if (ids[i].typeElement == elementInstSmpl)
        {
            ids[i].indexElt = SoundfontManager::getInstance()->get(ids[i], champ_sampleID).wValue;
            ids[i].typeElement = elementSmpl;
        }
        else if (ids[i].typeElement == elementPrstInst)
        {
            ids[i].indexElt = SoundfontManager::getInstance()->get(ids[i], champ_instrument).wValue;
            ids[i].typeElement = elementInst;
        }
    }

    // Store the list
    _ids = ids;

    if (ids[0].typeElement == elementSmpl)
    {
        if (ids.count() == 1)
        {
            ui->stackedWidget->setCurrentIndex(1);
            ui->labelOneChoice->setText(trUtf8("Create an instrument comprising the sample %1?")
                                        .arg("<b>" + SoundfontManager::getInstance()->getQstr(ids[0], champ_name) + "</b>"));
        }
        else
        {
            ui->stackedWidget->setCurrentIndex(0);
            ui->labelChoice->setText(trUtf8("Create an instrument"));
            ui->radioOnePerElement->setText(trUtf8("for each sample"));
            ui->radioOnlyOne->setText(trUtf8("comprising the %1 samples").arg(ids.count()));
        }
    }
    else
    {
        if (ids.count() == 1)
        {
            ui->stackedWidget->setCurrentIndex(1);
            ui->labelOneChoice->setText(trUtf8("Create a preset comprising the instrument %1?")
                                        .arg("<b>" + SoundfontManager::getInstance()->getQstr(ids[0], champ_name) + "</b>"));
        }
        else
        {
            ui->stackedWidget->setCurrentIndex(0);
            ui->labelChoice->setText(trUtf8("Create a preset"));
            ui->radioOnePerElement->setText(trUtf8("for each instrument"));
            ui->radioOnlyOne->setText(trUtf8("comprising the %1 instruments").arg(ids.count()));
        }
    }

    // Restore previous setting
    if (ContextManager::configuration()->getValue(ConfManager::SECTION_NONE, "create_element_dialog", 0).toInt() == 1)
        ui->radioOnePerElement->setChecked(true);
    else
        ui->radioOnlyOne->setChecked(true);
}

void DialogCreateElements::on_buttonBox_accepted()
{
    // Store the option
    ContextManager::configuration()->setValue(ConfManager::SECTION_NONE, "create_element_dialog", ui->radioOnePerElement->isChecked() ? 1 : 0);

    emit(createElements(_ids, ui->radioOnePerElement->isChecked() || _ids.count() == 1));
}

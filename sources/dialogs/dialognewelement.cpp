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

#include "dialognewelement.h"
#include "ui_dialognewelement.h"
#include "contextmanager.h"
#include "latinvalidator.h"

DialogNewElement::DialogNewElement(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogNewElement)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint));
    ui->lineEdit->setMaxLength(20);
    ui->lineEdit->setValidator(new LatinValidator(ui->lineEdit));

    // Restore the checkbox state
    ui->checkLink->setChecked(ContextManager::configuration()->getValue(ConfManager::SECTION_TOOLS, "new_element", true).toBool());
}

DialogNewElement::~DialogNewElement()
{
    delete ui;
}

void DialogNewElement::initialize(bool isPrst, bool withPossibleLinkedElements, QString defaultName)
{
    this->setWindowTitle(isPrst ? tr("Create a new preset") : tr("Create a new instrument"));
    ui->lineEdit->setPlaceholderText((isPrst ? tr("Name of the new preset") : tr("Name of the new instrument")) + "...");
    ui->checkLink->setText(isPrst ? tr("Link selected instruments") :
                                    tr("Link selected samples"));
    ui->lineEdit->setText(defaultName);
    ui->checkLink->setEnabled(withPossibleLinkedElements);
    ui->lineEdit->selectAll();
}

void DialogNewElement::on_pushCancel_clicked()
{
    QDialog::reject();
}

void DialogNewElement::on_pushOk_clicked()
{
    // Save the checkbox state
    ContextManager::configuration()->setValue(ConfManager::SECTION_TOOLS, "new_element", ui->checkLink->isChecked());

    emit onOk(ui->lineEdit->text(), ui->checkLink->isChecked() && ui->checkLink->isEnabled());
    QDialog::close();
}

/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2014 Davy Triponney                                **
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


#include "dialog_rename.h"
#include "ui_dialog_rename.h"

DialogRename::DialogRename(QString defaultValue, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogRename)
{
    ui->setupUi(this);
    this->ui->lineEdit->setText(defaultValue);
    this->ui->lineEdit->selectAll();
    this->ui->lineEdit->setFocus();
}

DialogRename::~DialogRename()
{
    delete ui;
}

void DialogRename::accept()
{
    int type = -1;
    if (ui->radioRootkey->isChecked())
        type = 0;
    else if (ui->radioIncrement->isChecked())
        type = 1;
    else if (ui->radioPrefix->isChecked())
        type = 2;
    else if (ui->radioSuffix->isChecked())
        type = 3;
    if (type >= 0 && !ui->lineEdit->text().isEmpty())
        this->updateNames(ui->lineEdit->text(), type);

    QDialog::accept();
}

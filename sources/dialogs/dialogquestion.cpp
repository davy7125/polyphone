/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2020 Davy Triponney                                **
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

#include "dialogquestion.h"
#include "ui_dialogquestion.h"

DialogQuestion::DialogQuestion(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogQuestion)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint));
}

DialogQuestion::~DialogQuestion()
{
    delete ui;
}

void DialogQuestion::initialize(QString title, QString placeHolder, QString defaultValue)
{
    this->setWindowTitle(title);
    ui->lineEdit->setPlaceholderText(placeHolder);
    ui->lineEdit->setText(defaultValue);
    ui->lineEdit->selectAll();
}

void DialogQuestion::setTextLimit(int textLimit)
{
    ui->lineEdit->setMaxLength(textLimit);
    ui->lineEdit->selectAll();
}

void DialogQuestion::on_pushCancel_clicked()
{
    QDialog::reject();
}

void DialogQuestion::on_pushOk_clicked()
{
    emit(onOk(ui->lineEdit->text()));
    QDialog::close();
}

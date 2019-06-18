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

#include "waitingtooldialog.h"
#include "ui_waitingtooldialog.h"
#include <QCloseEvent>

WaitingToolDialog::WaitingToolDialog(QString title, int stepNumber, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WaitingToolDialog),
    _stepNumber(stepNumber),
    _isCanceled(false)
{
    ui->setupUi(this);
    this->setWindowTitle(title);
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint));
    this->setWindowModality(Qt::WindowModal);
    this->setFixedWidth(350);

    // Progress bar
    ui->progressBar->setMinimum(0);
    this->setValue(0);
}

WaitingToolDialog::~WaitingToolDialog()
{
    delete ui;
}

void WaitingToolDialog::on_pushCancel_clicked()
{
    this->cancel();
}

void WaitingToolDialog::setValue(int value)
{
    if (_isCanceled)
        return;
    ui->progressBar->setMaximum(value == 0 ? 0 : _stepNumber);
    ui->progressBar->setValue(value);
}

void WaitingToolDialog::closeEvent(QCloseEvent *event)
{
    this->cancel();
    event->ignore();
}

void WaitingToolDialog::cancel()
{
    if (_isCanceled)
        return;

    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(0);
    ui->labelProcess->setText(trUtf8("Canceling..."));
    _isCanceled = true;
    emit(canceled());
}

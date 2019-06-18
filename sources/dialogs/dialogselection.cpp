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

#include "dialogselection.h"
#include "ui_dialogselection.h"
#include "soundfontmanager.h"

DialogSelection::DialogSelection(SoundfontManager *sf2, EltID id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSelection)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint));

    // Fill the list
    QString qStr;
    int initIndex = id.indexElt;
    foreach (int i, sf2->getSiblings(id))
    {
        if (i != initIndex)
        {
            id.indexElt = i;
            if (id.typeElement == elementInst)
                qStr = sf2->getQstr(id, champ_name);
            else
                qStr = QString("%1:%2 %3")
                        .arg(sf2->get(id, champ_wBank).wValue, 3, 10, QChar('0'))
                        .arg(sf2->get(id, champ_wPreset).wValue, 3, 10, QChar('0'))
                        .arg(sf2->getQstr(id, champ_name));
            QListWidgetItem * item = new QListWidgetItem(qStr);
            item->setCheckState(Qt::Checked);
            item->setData(Qt::UserRole, i);
            ui->listWidget->addItem(item);
        }
    }

    // Sort
    ui->listWidget->sortItems();
}

DialogSelection::~DialogSelection()
{
    delete ui;
}

void DialogSelection::on_pushSelectAll_clicked()
{
    // Tout sélectionner
    for (int i = 0; i < ui->listWidget->count(); i++)
        ui->listWidget->item(i)->setCheckState(Qt::Checked);
}

void DialogSelection::on_pushSelectNone_clicked()
{
    // Tout décocher
    for (int i = 0; i < ui->listWidget->count(); i++)
        ui->listWidget->item(i)->setCheckState(Qt::Unchecked);
}

void DialogSelection::on_pushCancel_clicked()
{
    QDialog::close();
}

void DialogSelection::on_pushOk_clicked()
{
    // Création liste index
    QList<int> listIndex;
    for (int i = 0; i < ui->listWidget->count(); i++)
    {
        QListWidgetItem *checkBoxItem = ui->listWidget->item(i);
        if (checkBoxItem->checkState() == Qt::Checked)
            listIndex << checkBoxItem->data(Qt::UserRole).toInt();
    }

    emit(listChosen(listIndex));
    QDialog::accept();
}

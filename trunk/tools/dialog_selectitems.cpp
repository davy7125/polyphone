/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013 Davy Triponney                                     **
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

#include "dialog_selectitems.h"
#include "ui_dialog_selectitems.h"

DialogSelectItems::DialogSelectItems(EltID id, QList<EltID> listChecked, Pile_sf2 *sf2, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSelectItems),
    _sf2(sf2)
{
    ui->setupUi(this);
    if (id.typeElement == elementInstSmpl)
        id.typeElement = elementInst;
    else if (id.typeElement == elementPrstInst)
        id.typeElement = elementPrst;
    if (id.typeElement != elementSmpl && id.typeElement != elementInst &&
            id.typeElement != elementPrst)
        return;
    _initialID = id;
    // Remplissage de la liste
    for (int i = 0; i < _sf2->count(id); i++)
    {
        id.indexElt = i;
        if (!_sf2->get(id, champ_hidden).bValue)
        {
            // Checkbox
            QListWidgetItem *checkBoxItem = new QListWidgetItem();
            if (id.typeElement != elementPrst)
                checkBoxItem->setText(_sf2->getQstr(id, champ_name));
            else
            {
                char T[40];
                sprintf(T, "%.3d:%.3d %s", _sf2->get(id, champ_wBank).wValue,
                        _sf2->get(id, champ_wPreset).wValue,
                        _sf2->getQstr(id, champ_name).toStdString().c_str());
                checkBoxItem->setText(T);
            }
            if (id.indexElt == _initialID.indexElt)
            {
                checkBoxItem->setCheckState(Qt::Checked);
                checkBoxItem->setFlags(Qt::NoItemFlags);
            }
            else
            {
                bool checked = false;
                int j = 0;
                while (j < listChecked.size() && !checked)
                {
                    if (listChecked[j].indexElt == i)
                        checked = true;
                    j++;
                }
                if (checked)
                    checkBoxItem->setCheckState(Qt::Checked);
                else
                    checkBoxItem->setCheckState(Qt::Unchecked);
            }
            checkBoxItem->setData(Qt::UserRole, i);
            this->ui->listWidget->addItem(checkBoxItem);
        }
    }
    this->ui->listWidget->sortItems();
}

DialogSelectItems::~DialogSelectItems()
{
    delete ui;
}

void DialogSelectItems::accept()
{
    // Création liste ID
    QList<EltID> listID;
    EltID id = _initialID;
    for (int i = 0; i < this->ui->listWidget->count(); i++)
    {
        QListWidgetItem *checkBoxItem = this->ui->listWidget->item(i);
        if (checkBoxItem->checkState() == Qt::Checked)
        {
            id.indexElt = checkBoxItem->data(Qt::UserRole).toInt();
            listID.append(id);
        }
    }
    emit(accepted(listID));
    QDialog::accept();
}

void DialogSelectItems::on_pushSelectAll_clicked()
{
    // Tout sélectionner
    for (int i = 0; i < this->ui->listWidget->count(); i++)
        this->ui->listWidget->item(i)->setCheckState(Qt::Checked);
}

void DialogSelectItems::on_pushSelectCurrent_clicked()
{
    // Sélectionner uniquement l'ID de départ
    for (int i = 0; i < this->ui->listWidget->count(); i++)
    {
        if (this->ui->listWidget->item(i)->data(Qt::UserRole).toInt() != _initialID.indexElt)
            this->ui->listWidget->item(i)->setCheckState(Qt::Unchecked);
    }
}

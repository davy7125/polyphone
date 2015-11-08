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
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "dialog_list.h"
#include "ui_dialog_list.h"
#include "mainwindow.h"

DialogList::DialogList(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogList)
{
    ui->setupUi(this);
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint) | Qt::Tool);
}

DialogList::~DialogList()
{
    delete ui;
}

void DialogList::showDialog(EltID idSrc, ModeListDialog mode)
{
    _mode = mode;
    if ((mode == MODE_ASSOCIATION &&
        (idSrc.typeElement != elementInst && idSrc.typeElement != elementSmpl)) ||
        (mode == MODE_REMPLACEMENT &&
        (idSrc.typeElement != elementInstSmpl && idSrc.typeElement != elementPrstInst)))
        return;
    // Titre
    ElementType element;
    if (idSrc.typeElement == elementInstSmpl)
    {
        this->setWindowTitle(trUtf8("Liste des échantillons"));
        element = elementSmpl;
    }
    else if (idSrc.typeElement == elementSmpl || idSrc.typeElement == elementPrstInst)
    {
        this->setWindowTitle(trUtf8("Liste des instruments"));
        element = elementInst;
    }
    else
    {
        this->setWindowTitle(trUtf8("Liste des presets"));
        element = elementPrst;
    }
    // Remplissage de la liste
    this->ui->listWidget->clear();
    this->ui->listWidget->scrollToTop();
    EltID id(element, idSrc.indexSf2, 0, 0, 0);
    ListWidgetItem *item;
    for (int i = 0; i < this->sf2->count(id); i++)
    {
        id.indexElt = i;
        if (!this->sf2->get(id, champ_hidden).bValue)
        {
            if (id.typeElement == elementPrst)
                item = new ListWidgetItem(QString("%1:%2 %3")
                                          .arg(this->sf2->get(id, champ_wBank).wValue, 3, 10, QChar('0'))
                                          .arg(this->sf2->get(id, champ_wPreset).wValue, 3, 10, QChar('0'))
                                          .arg(this->sf2->getQstr(id, champ_name)));
            else
                item = new ListWidgetItem(this->sf2->getQstr(id, champ_name));
            item->id = id;
            this->ui->listWidget->addItem(item);
        }
    }
    this->ui->listWidget->clearSelection();
    this->ui->listWidget->sortItems();
    // Affichage du dialogue
    this->setWindowModality(Qt::ApplicationModal);
    this->show();
}

void DialogList::accept()
{
    // élément sélectionné ?
    if (this->ui->listWidget->selectedItems().count())
    {
        ListWidgetItem *item = dynamic_cast<ListWidgetItem *>(this->ui->listWidget->currentItem());
        // Association de MainWindow
        if (_mode == MODE_ASSOCIATION)
            this->window->associer(item->id);
        else
            this->window->remplacer(item->id);
    }
    QDialog::accept();
}

void DialogList::reject()
{
    this->window->updateDo();
    QDialog::reject();
}

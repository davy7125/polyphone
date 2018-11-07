/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2017 Davy Triponney                                **
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

#include "dialog_list.h"
#include "ui_dialog_list.h"
#include "soundfontmanager.h"

DialogList::DialogList(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogList)
{
    ui->setupUi(this);
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint));
}

DialogList::~DialogList()
{
    delete ui;
}

void DialogList::showDialog(EltID idSrc, bool isAssociation)
{
    _isAssociation = isAssociation;
    if ((_isAssociation && (idSrc.typeElement != elementInst && idSrc.typeElement != elementSmpl)) ||
            (!_isAssociation && (idSrc.typeElement != elementInstSmpl && idSrc.typeElement != elementPrstInst)))
        return;

    // Title
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

    // Fill the list
    ui->listWidget->clear();
    ui->listWidget->scrollToTop();
    EltID id(element, idSrc.indexSf2, 0, 0, 0);
    ListWidgetItem *item;
    SoundfontManager * sm = SoundfontManager::getInstance();
    foreach (int i, sm->getSiblings(id))
    {
        id.indexElt = i;
        if (id.typeElement == elementPrst)
            item = new ListWidgetItem(QString("%1:%2 %3")
                                      .arg(sm->get(id, champ_wBank).wValue, 3, 10, QChar('0'))
                                      .arg(sm->get(id, champ_wPreset).wValue, 3, 10, QChar('0'))
                                      .arg(sm->getQstr(id, champ_name)));
        else
            item = new ListWidgetItem(sm->getQstr(id, champ_name));
        item->id = id;
        ui->listWidget->addItem(item);
    }
    ui->listWidget->clearSelection();
    ui->listWidget->sortItems();

    // Display the dialog
    this->show();
}

void DialogList::accept()
{
    // élément sélectionné ?
    if (ui->listWidget->selectedItems().count())
    {
        ListWidgetItem *item = dynamic_cast<ListWidgetItem *>(ui->listWidget->currentItem());
        emit(elementSelected(item->id, _isAssociation));
    }
    QDialog::accept();
}

void DialogList::reject()
{
    QDialog::reject();
}

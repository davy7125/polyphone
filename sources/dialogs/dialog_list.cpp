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

    // Title and type of elements to display
    ElementType element;
    if (idSrc.typeElement == elementInstSmpl)
    {
        this->setWindowTitle(tr("Sample list"));
        element = elementSmpl;
    }
    else if (idSrc.typeElement == elementSmpl || idSrc.typeElement == elementPrstInst)
    {
        this->setWindowTitle(tr("Instrument list"));
        element = elementInst;
    }
    else
    {
        this->setWindowTitle(tr("Preset list"));
        element = elementPrst;
    }

    // Id to select
    int selectedId = -1;
    SoundfontManager * sm = SoundfontManager::getInstance();
    if (!isAssociation)
        selectedId = sm->get(idSrc, idSrc.typeElement == elementInstSmpl ? champ_sampleID : champ_instrument).wValue;

    // Fill the list
    ui->listWidget->clear();
    ui->listWidget->clearSelection();
    ui->listWidget->scrollToTop();
    ui->lineSearch->clear();
    EltID id(element, idSrc.indexSf2, 0, 0, 0);
    ListWidgetItem *item;
    ListWidgetItem * selectedItem = nullptr;
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
        if (i == selectedId)
            selectedItem = item;
    }
    ui->listWidget->sortItems();

    // Selection
    if (selectedItem != nullptr)
        ui->listWidget->setCurrentItem(selectedItem);

    // Display the dialog
    this->show();

    // Scroll
    if (selectedItem != nullptr)
        ui->listWidget->scrollToItem(selectedItem, QAbstractItemView::PositionAtCenter);

    // Focus on the filter
    ui->lineSearch->setFocus();
}

void DialogList::on_pushCancel_clicked()
{
    QDialog::reject();
}

void DialogList::on_pushOk_clicked()
{
    // An element is selected?
    if (ui->listWidget->selectedItems().count())
    {
        ListWidgetItem *item = dynamic_cast<ListWidgetItem *>(ui->listWidget->currentItem());
        emit elementSelected(item->id, _isAssociation);
    }
    QDialog::accept();
}

void DialogList::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    Q_UNUSED(item)
    on_pushOk_clicked();
}

void DialogList::on_lineSearch_textChanged(const QString &arg1)
{
    QString filter = arg1.toLower();
    for (int i = 0; i < ui->listWidget->count(); i++)
        ui->listWidget->item(i)->setHidden(!ui->listWidget->item(i)->text().toLower().contains(filter));
}


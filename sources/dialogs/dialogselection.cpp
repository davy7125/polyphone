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

#include "dialogselection.h"
#include "ui_dialogselection.h"
#include "soundfontmanager.h"
#include "utils.h"
#include <QShortcut>
#include <QKeyEvent>

DialogSelection::DialogSelection(SoundfontManager *sf2, EltID id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSelection),
    _isPreset(id.typeElement == elementPrst)
{
    ui->setupUi(this);
    ui->buttonBox->addButton(tr("&Duplicate"), QDialogButtonBox::AcceptRole);
    ui->buttonBox->addButton(tr("&Cancel"), QDialogButtonBox::RejectRole);
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
            item->setData(Qt::UserRole + 1, Utils::simplifySpaces(Utils::removeAccents(qStr).toLower()));
            ui->listWidget->addItem(item);
        }
    }

    // Sort
    ui->listWidget->sortItems();

    // Filter field, focused with Ctrl+F
    QShortcut * shortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_F), this);
    connect(shortcut, &QShortcut::activated, this, [this]() {
        ui->lineSearch->selectAll();
        ui->lineSearch->setFocus();
    });
    ui->lineSearch->installEventFilter(this);
    ui->listWidget->installEventFilter(this);
    connect(ui->listWidget, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(updateCheckedCount()));
    updateCheckedCount();
    ui->lineSearch->setFocus();
}

DialogSelection::~DialogSelection()
{
    delete ui;
}

void DialogSelection::on_pushSelectAll_clicked()
{
    // Tick all elements displayed by the filter
    setCheckState(getVisibleItems(), Qt::Checked);
}

void DialogSelection::on_pushSelectNone_clicked()
{
    // Untick all elements displayed by the filter
    setCheckState(getVisibleItems(), Qt::Unchecked);
}

QList<QListWidgetItem *> DialogSelection::getVisibleItems()
{
    QList<QListWidgetItem *> items;
    for (int i = 0; i < ui->listWidget->count(); i++)
        if (!ui->listWidget->item(i)->isHidden())
            items << ui->listWidget->item(i);
    return items;
}

void DialogSelection::setCheckState(QList<QListWidgetItem *> items, Qt::CheckState state)
{
    // Count updated once at the end
    ui->listWidget->blockSignals(true);
    foreach (QListWidgetItem * item, items)
        item->setCheckState(state);
    ui->listWidget->blockSignals(false);
    updateCheckedCount();
}

void DialogSelection::on_lineSearch_textChanged(const QString &text)
{
    // Elements whose name (and numbers for presets) contain the filter, whatever the case, the accents and the number of spaces
    QString filter = Utils::simplifySpaces(Utils::removeAccents(text).toLower());
    for (int i = 0; i < ui->listWidget->count(); i++)
    {
        QListWidgetItem * item = ui->listWidget->item(i);
        bool visible = item->data(Qt::UserRole + 1).toString().contains(filter);
        item->setHidden(!visible);
        if (!visible)
            item->setSelected(false);
    }

    // Checked elements may now be hidden
    updateCheckedCount();
}

void DialogSelection::on_listWidget_itemDoubleClicked(QListWidgetItem * item)
{
    // A double click anywhere on the row ticks or unticks the element
    item->setCheckState(item->checkState() == Qt::Checked ? Qt::Unchecked : Qt::Checked);
}

void DialogSelection::updateCheckedCount()
{
    int nbChecked = 0;
    int nbHiddenChecked = 0;
    for (int i = 0; i < ui->listWidget->count(); i++)
    {
        if (ui->listWidget->item(i)->checkState() == Qt::Checked)
        {
            nbChecked++;
            if (ui->listWidget->item(i)->isHidden())
                nbHiddenChecked++;
        }
    }

    int nbElements = ui->listWidget->count();
    if (_isPreset)
    {
        if (nbHiddenChecked > 0)
            ui->labelCount->setText(tr("Checked presets: %1 / %2 (hidden: %n)", "", nbHiddenChecked).arg(nbChecked).arg(nbElements));
        else
            ui->labelCount->setText(tr("Checked presets: %1 / %2").arg(nbChecked).arg(nbElements));
    }
    else
    {
        if (nbHiddenChecked > 0)
            ui->labelCount->setText(tr("Checked instruments: %1 / %2 (hidden: %n)", "", nbHiddenChecked).arg(nbChecked).arg(nbElements));
        else
            ui->labelCount->setText(tr("Checked instruments: %1 / %2").arg(nbChecked).arg(nbElements));
    }
}

bool DialogSelection::eventFilter(QObject * obj, QEvent * event)
{
    if (event->type() != QEvent::KeyPress)
        return QDialog::eventFilter(obj, event);

    QKeyEvent * keyEvent = static_cast<QKeyEvent *>(event);
    bool isEnter = (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) &&
                   !(keyEvent->modifiers() & Qt::ControlModifier);
    if (obj == ui->lineSearch && (keyEvent->key() == Qt::Key_Down || isEnter))
    {
        // Go to the first element displayed (if any)
        // Enter doesn't start the duplication, which is still possible with Ctrl+Enter
        QList<QListWidgetItem *> items = getVisibleItems();
        if (!items.isEmpty())
        {
            ui->listWidget->setFocus();
            ui->listWidget->setCurrentItem(items.first());
        }
        return true;
    }

    if (obj == ui->listWidget && keyEvent->key() == Qt::Key_Space && keyEvent->modifiers() == Qt::NoModifier)
    {
        // An element hidden by the filter cannot be changed
        QListWidgetItem * currentItem = ui->listWidget->currentItem();
        if (currentItem != nullptr && currentItem->isHidden())
            return true;

        // The opposite state of the current element is applied to all selected elements
        if (currentItem != nullptr && currentItem->isSelected())
        {
            setCheckState(ui->listWidget->selectedItems(), currentItem->checkState() == Qt::Checked ? Qt::Unchecked : Qt::Checked);
            return true;
        }
    }

    return QDialog::eventFilter(obj, event);
}

void DialogSelection::on_buttonBox_rejected()
{
    QDialog::close();
}

void DialogSelection::on_buttonBox_accepted()
{
    // Création liste index
    QList<int> listIndex;
    for (int i = 0; i < ui->listWidget->count(); i++)
    {
        QListWidgetItem *checkBoxItem = ui->listWidget->item(i);
        if (checkBoxItem->checkState() == Qt::Checked)
            listIndex << checkBoxItem->data(Qt::UserRole).toInt();
    }

    emit listChosen(listIndex);
    QDialog::accept();
}

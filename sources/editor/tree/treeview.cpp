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

#include "treeview.h"
#include "treeitemdelegate.h"
#include "treesortfilterproxy.h"
#include "soundfontmanager.h"
#include <QMouseEvent>
#include <QScrollBar>
#include <QApplication>
#include <QMessageBox>
#include "treeviewmenu.h"
#include "duplicator.h"
#include "sampleloader.h"
#include "dialogcreateelements.h"
#include "utils.h"
#include "tools/auto_distribution/toolautodistribution.h"

TreeView::TreeView(QWidget * parent) : QTreeView(parent),
    _fixingSelection(false),
    _sf2Index(-1),
    _bestMatchSample(-1),
    _bestMatchInstrument(-1),
    _bestMatchPreset(-1),
    _expandClicked(false)
{
    this->setItemDelegate(new TreeItemDelegate(this));
    this->viewport()->setAutoFillBackground(false);
    this->sortByColumn(0, Qt::AscendingOrder);

    // Menu
    _menu = new TreeViewMenu(parent);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(openMenu(QPoint)));
    connect(_menu, SIGNAL(selectionChanged(IdList)), this, SLOT(onSelectionChanged(IdList)));

    // Drag & drop
    this->setAcceptDrops(true);
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->setDragEnabled(true);
    this->viewport()->setAcceptDrops(true);
    this->setDropIndicatorShown(true);
    this->setDragDropMode(QAbstractItemView::InternalMove);
}

void TreeView::mousePressEvent(QMouseEvent * event)
{
    _expandClicked = false;
    QModelIndex index = this->indexAt(event->pos());
    if (index.isValid())
    {
        EltID currentId = index.data(Qt::UserRole).value<EltID>();
        if (currentId.typeElement == elementRootSmpl ||
                currentId.typeElement == elementRootInst || currentId.typeElement == elementRootPrst ||
                currentId.typeElement == elementInst || currentId.typeElement == elementPrst)
        {
            // Expand / collapse the element if the click is on the arrow
            if (event->pos().x() > this->viewport()->width() - 34) // 40 is the width of the arrow + 3 MARGINS
            {
                _expandClicked = true;
                if (this->isExpanded(index))
                    this->collapse(index);
                else
                    this->expand(index);
                event->accept();
                return;
            }
        }
    }

    QTreeView::mousePressEvent(event);
}

void TreeView::mouseReleaseEvent(QMouseEvent *event)
{
    // Possibly ignore the event
    if (_expandClicked)
    {
        _expandClicked = false;
        event->accept();
        return;
    }
    QTreeView::mouseReleaseEvent(event);
}

void TreeView::mouseMoveEvent(QMouseEvent *event)
{
    // Possibly ignore the event
    if (_expandClicked)
    {
        event->accept();
        return;
    }
    QTreeView::mouseMoveEvent(event);
}

void TreeView::mouseDoubleClickEvent(QMouseEvent * event)
{
    QModelIndex index = this->indexAt(event->pos());
    if (index.isValid())
    {
        EltID currentId = index.data(Qt::UserRole).value<EltID>();
        if (currentId.typeElement == elementInstSmpl)
        {
            // Find the corresponding sample
            EltID id(elementSmpl, _sf2Index, SoundfontManager::getInstance()->get(currentId, champ_sampleID).wValue, -1, -1);
            TreeSortFilterProxy * proxy = dynamic_cast<TreeSortFilterProxy *>(this->model());
            if (!proxy->isFiltered(id))
                this->onSelectionChanged(IdList(id));
            event->accept();
            return;
        }
        else if (currentId.typeElement == elementPrstInst)
        {
            // Find the corresponding instrument
            EltID id(elementInst, _sf2Index, SoundfontManager::getInstance()->get(currentId, champ_instrument).wValue, -1, -1);
            TreeSortFilterProxy * proxy = dynamic_cast<TreeSortFilterProxy *>(this->model());
            if (!proxy->isFiltered(id))
                this->onSelectionChanged(IdList(id));
            event->accept();
            return;
        }
    }

    QTreeView::mouseDoubleClickEvent(event);
}

void TreeView::keyPressEvent(QKeyEvent * event)
{
    // Validate or cancel a drag & drop
    if (event->modifiers() == Qt::NoModifier && event->key() == Qt::Key_Enter)
    {
        if (_dropDestID.typeElement == elementRootInst)
        {
            // Possibly create one or more instruments based on samples
            foreach (EltID idSource, _draggedIds)
                if (idSource.typeElement != elementSmpl && idSource.typeElement != elementInstSmpl)
                    return;
            DialogCreateElements * dial = new DialogCreateElements(this);
            dial->initialize(_draggedIds);
            connect(dial, SIGNAL(createElements(IdList,bool)), this, SLOT(onCreateElements(IdList,bool)));
            dial->open();
        }
        else if (_dropDestID.typeElement == elementRootPrst)
        {
            // Possibly create one or more presets based on instruments
            foreach (EltID idSource, _draggedIds)
                if (idSource.typeElement != elementInst && idSource.typeElement != elementPrstInst)
                    return;
            DialogCreateElements * dial = new DialogCreateElements(this);
            dial->initialize(_draggedIds);
            connect(dial, SIGNAL(createElements(IdList,bool)), this, SLOT(onCreateElements(IdList,bool)));
            dial->open();
        }
        else
        {
            SoundfontManager * sm = SoundfontManager::getInstance();
            Duplicator duplicator;
            IdList newIds;
            foreach (EltID idSource, _draggedIds)
            {
                if ((idSource.typeElement == elementSmpl || idSource.typeElement == elementInst || idSource.typeElement == elementPrst ||
                     idSource.typeElement == elementInstSmpl || idSource.typeElement == elementPrstInst) && sm->isValid(idSource))
                {
                    EltID id = duplicator.copy(idSource, _dropDestID);
                    if (id.typeElement != elementUnknown)
                        newIds << id;
                }
            }

            if (!newIds.isEmpty())
            {
                sm->endEditing("command:drop");
                onSelectionChanged(newIds);
            }
        }
    }

    // No more drag & drop
    _draggedIds.clear();
    _dropDestID.typeElement = elementUnknown;

    if (event->key() == Qt::Key_Delete)
    {
        // Delete the selection
        _menu->initialize(getSelectedIds());
        _menu->remove();
        event->accept();
    }
    else if (event->key() == Qt::Key_F2)
    {
        // Rename the selection
        _menu->initialize(getSelectedIds());
        _menu->rename();
        event->accept();
    }
    else if (event->matches(QKeySequence::Copy))
    {
        // Copy the selection
        _menu->initialize(getSelectedIds());
        _menu->copy();
        event->accept();
    }
    else if (event->matches(QKeySequence::Paste))
    {
        // Paste the selection
        _menu->initialize(getSelectedIds());
        _menu->paste();
        event->accept();
    }
    else if (event->key() == Qt::Key_Space)
    {
        emit(spacePressed());
        event->accept();
    }
    else if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_D)
    {
        _menu->initialize(getSelectedIds());
        _menu->duplicate();
        event->accept();
    }
    else if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_F)
    {
        emit(focusOnSearch());
        event->accept();
    }
    else
        QTreeView::keyPressEvent(event);
}

void TreeView::setBestMatch(int sampleId, int instrumentId, int presetId)
{
    _bestMatchSample = sampleId;
    _bestMatchInstrument = instrumentId;
    _bestMatchPreset = presetId;
}

void TreeView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    // Keep a track of the last selected index
    if (!selected.indexes().isEmpty())
        _lastSelectedId = selected.indexes()[0].data(Qt::UserRole).value<EltID>();

    // Apply the change and check the selection if we are not already fixing it
    QTreeView::selectionChanged(selected, deselected);
    if (_fixingSelection)
        return;
    _fixingSelection = true;

    // Reset the selection if not valid
    if (!isSelectionValid())
        this->clearSelection();

    // First attempt to select an index if empty: reselect the selected index or the unselected index
    if (this->selectedIndexes().isEmpty())
    {
        if (!selected.indexes().isEmpty())
        {
            // Id to reselect
            EltID id = selected.indexes()[0].data(Qt::UserRole).value<EltID>();
            bool isHidden = selected.indexes()[0].data(Qt::UserRole + 1).toBool();
            TreeSortFilterProxy * proxy = dynamic_cast<TreeSortFilterProxy *>(this->model());
            if (!proxy->isFiltered(id) && !isHidden)
                this->setCurrentIndex(selected.indexes()[0]);
        }
        else if (!deselected.indexes().isEmpty())
        {
            // Id to reselect
            EltID id = deselected.indexes()[0].data(Qt::UserRole).value<EltID>();
            bool isHidden = deselected.indexes()[0].data(Qt::UserRole + 1).toBool();
            TreeSortFilterProxy * proxy = dynamic_cast<TreeSortFilterProxy *>(this->model());
            if (!proxy->isFiltered(id) && !isHidden)
                this->setCurrentIndex(deselected.indexes()[0]);
        }
    }

    // Second attempt to select an index if empty: take the best match based on the filter
    if (this->selectedIndexes().isEmpty())
    {
        QModelIndex index;
        bool ok = false;
        switch (_lastSelectedId.typeElement)
        {
        case elementSmpl:
            // First sample, then instrument and finally preset
            if (_bestMatchSample != -1)
            {
                index = getIndex(EltID(elementSmpl, _sf2Index, _bestMatchSample));
                if (!index.data(Qt::UserRole + 1).toBool())
                {
                    this->setCurrentIndex(index);
                    ok = true;
                }
            }
            if (!ok && _bestMatchInstrument != -1)
            {
                index = getIndex(EltID(elementInst, _sf2Index, _bestMatchInstrument));
                if (!index.data(Qt::UserRole + 1).toBool())
                {
                    this->setCurrentIndex(index);
                    ok = true;
                }
            }
            if (!ok && _bestMatchPreset != -1)
            {
                index = getIndex(EltID(elementPrst, _sf2Index, _bestMatchPreset));
                if (!index.data(Qt::UserRole + 1).toBool())
                    this->setCurrentIndex(index);
            }
            break;
        case elementInst: case elementInstSmpl:
            // First instrument, then sample and finally preset
            if (_bestMatchInstrument != -1)
            {
                index = getIndex(EltID(elementInst, _sf2Index, _bestMatchInstrument));
                if (!index.data(Qt::UserRole + 1).toBool())
                {
                    this->setCurrentIndex(index);
                    ok = true;
                }
            }
            if (!ok && _bestMatchSample != -1)
            {
                index = getIndex(EltID(elementSmpl, _sf2Index, _bestMatchSample));
                if (!index.data(Qt::UserRole + 1).toBool())
                {
                    this->setCurrentIndex(index);
                    ok = true;
                }
            }
            if (!ok && _bestMatchPreset != -1)
            {
                index = getIndex(EltID(elementPrst, _sf2Index, _bestMatchPreset));
                if (!index.data(Qt::UserRole + 1).toBool())
                    this->setCurrentIndex(index);
            }
            break;
        case elementPrst: case elementPrstInst:
            // First preset, then instrument and finally sample
            if (_bestMatchPreset != -1)
            {
                index = getIndex(EltID(elementPrst, _sf2Index, _bestMatchPreset));
                if (!index.data(Qt::UserRole + 1).toBool())
                {
                    this->setCurrentIndex(index);
                    ok = true;
                }
            }
            if (!ok && _bestMatchInstrument != -1)
            {
                index = getIndex(EltID(elementInst, _sf2Index, _bestMatchInstrument));
                if (!index.data(Qt::UserRole + 1).toBool())
                {
                    this->setCurrentIndex(index);
                    ok = true;
                }
            }
            if (!ok && _bestMatchSample != -1)
            {
                index = getIndex(EltID(elementSmpl, _sf2Index, _bestMatchSample));
                if (!index.data(Qt::UserRole + 1).toBool())
                    this->setCurrentIndex(index);
            }
            break;
        default:
            break;
        }
    }

    // Third attempt to select an index if empty: take the element at the same position or above in the tree
    if (this->selectedIndexes().isEmpty())
    {
        if (!deselected.indexes().isEmpty())
        {
            TreeSortFilterProxy * proxy = dynamic_cast<TreeSortFilterProxy *>(this->model());
            QModelIndex indexInitial = deselected.indexes()[0];

            if (indexInitial.isValid())
            {
                QModelIndex parent = indexInitial.parent();

                // We try first to select the same position
                bool ok = false;
                for (int i = indexInitial.row(); i < this->model()->rowCount(parent); i++)
                {
                    QModelIndex sibling = this->model()->index(i, 0, parent);
                    if (!sibling.data(Qt::UserRole + 1).toBool() && !proxy->isFiltered(sibling.data(Qt::UserRole).value<EltID>()))
                    {
                        this->setCurrentIndex(sibling);
                        ok = true;
                        break;
                    }
                }

                // Then a position above
                if (!ok)
                {
                    for (int i = indexInitial.row() - 1; i >= 0; i--)
                    {
                        QModelIndex sibling = this->model()->index(i, 0, parent);
                        if (!sibling.data(Qt::UserRole + 1).toBool() && !proxy->isFiltered(sibling.data(Qt::UserRole).value<EltID>()))
                        {
                            this->setCurrentIndex(sibling);
                            ok = true;
                            break;
                        }
                    }
                }

                // Finally the parent is selected
                if (!ok)
                    this->setCurrentIndex(parent);
            }
        }
    }

    // Fourth attempt to select an index if empty: take the root
    if (this->selectedIndexes().isEmpty())
        this->setCurrentIndex(this->model()->index(0, 0));

    _fixingSelection = false;
    emit(selectionChanged(getSelectedIds()));
}

IdList TreeView::getSelectedIds()
{
    IdList selectedIds;
    foreach (QModelIndex index, this->selectedIndexes())
        selectedIds << index.data(Qt::UserRole).value<EltID>();
    return selectedIds;
}

bool TreeView::isSelectionValid()
{
    // Check the consistency of the selection
    QModelIndexList indexes = this->selectedIndexes();
    EltID idTmp(elementUnknown, -1, -1, -1, -1);
    bool resetSelection = false;
    bool differentType = false;
    bool differentElementId = false;
    foreach (QModelIndex index, indexes)
    {
        if (index.isValid())
        {
            EltID currentId = index.data(Qt::UserRole).value<EltID>();
            switch (idTmp.typeElement)
            {
            case elementUnknown:
                idTmp = currentId;
                break;
            case elementSf2: case elementRootSmpl: case elementRootInst: case elementRootPrst: case elementSmpl:
                resetSelection = (currentId.typeElement != idTmp.typeElement);
                break;
            case elementInst:
                if (currentId.typeElement == elementInst)
                {
                    differentElementId = true;
                    if (differentType)
                        resetSelection = true;
                }
                else if (currentId.typeElement == elementInstSmpl)
                {
                    differentType = true;
                    differentElementId |= (currentId.indexElt != idTmp.indexElt);
                    if (differentElementId)
                        resetSelection = true;
                }
                else
                    resetSelection = true;
                break;
            case elementInstSmpl:
                if (currentId.typeElement == elementInstSmpl)
                    resetSelection = (idTmp.indexElt != currentId.indexElt);
                else if (currentId.typeElement == elementInst)
                {
                    differentType = true;
                    resetSelection = (idTmp.indexElt != currentId.indexElt);
                }
                else
                    resetSelection = true;
                break;
            case elementPrst:
                if (currentId.typeElement == elementPrst)
                {
                    differentElementId = true;
                    if (differentType)
                        resetSelection = true;
                }
                else if (currentId.typeElement == elementPrstInst)
                {
                    differentType = true;
                    differentElementId |= (currentId.indexElt != idTmp.indexElt);
                    if (differentElementId)
                        resetSelection = true;
                }
                else
                    resetSelection = true;
                break;
            case elementPrstInst:
                if (currentId.typeElement == elementPrstInst)
                    resetSelection = (idTmp.indexElt != currentId.indexElt);
                else if (currentId.typeElement == elementPrst)
                {
                    differentType = true;
                    resetSelection = (idTmp.indexElt != currentId.indexElt);
                }
                else
                    resetSelection = true;
                break;
            default:
                resetSelection = true;
                break;
            }
        }

        if (resetSelection)
            return false;
    }

    return true;
}

void TreeView::onSelectionChanged(const IdList &selectedIds)
{
    if (selectedIds.isEmpty())
        return;
    _fixingSelection = true;
    for (int i = 0; i < selectedIds.count(); i++)
    {
        if (i == selectedIds.count() - 1)
            _fixingSelection = false;
        this->select(selectedIds[i], i == 0 ? QItemSelectionModel::ClearAndSelect : QItemSelectionModel::Select);
    }
    expandAndScrollToSelection();
}

bool TreeView::select(EltID id, QItemSelectionModel::SelectionFlag flags)
{
    QModelIndex index = getIndex(id);
    if (!index.isValid())
        return false;

    // Change the current item: maj + click will work
    if (flags & QItemSelectionModel::Clear)
        this->setCurrentIndex(index);

    // Change the selection
    this->selectionModel()->select(index, flags);

    return true;
}

void TreeView::expandAndScrollToSelection()
{
    // Expand the tree so that all selected items are displayed
    // Note: expand(index.parent()) is not working here!?
    foreach (QModelIndex index, this->selectedIndexes())
    {
        EltID id = index.data(Qt::UserRole).value<EltID>();
        switch (id.typeElement)
        {
        case elementSmpl:
            this->expand(this->model()->index(1, 0));
            break;
        case elementInst:
            this->expand(this->model()->index(2, 0));
            break;
        case elementPrst:
            this->expand(this->model()->index(3, 0));
            break;
        case elementInstSmpl:
        {
            QModelIndex root = this->model()->index(2, 0);
            this->expand(root);
            this->expand(this->model()->index(index.parent().row(), 0, root));
        }
            break;
        case elementPrstInst:
        {
            QModelIndex root = this->model()->index(3, 0);
            this->expand(root);
            this->expand(this->model()->index(index.parent().row(), 0, root));
        }
            break;
        default:
            break;
        }

        QModelIndex indexTmp = index.parent();
        while (indexTmp.isValid())
        {
            if (!this->isExpanded(indexTmp))
                this->expand(indexTmp);
            indexTmp = indexTmp.parent();
        }
    }

    // Scroll to the first selected index
    if (!this->selectedIndexes().empty())
    {
        this->scrollContentsBy(0, -10); // Hack because scrollTo sometimes doesn't work without this function
        this->scrollTo(this->selectedIndexes()[0], ScrollHint::PositionAtCenter);
    }
}

QModelIndex TreeView::getIndex(EltID id)
{
    switch (id.typeElement)
    {
    case elementSf2:
        return this->model()->index(0, 0);
    case elementRootSmpl:
        return this->model()->index(1, 0);
    case elementRootInst:
        return this->model()->index(2, 0);
    case elementRootPrst:
        return this->model()->index(3, 0);
    case elementSmpl:
    {
        QModelIndex root = this->model()->index(1, 0);
        int nbElt = this->model()->rowCount(root);
        for (int i = 0; i < nbElt; i++)
        {
            QModelIndex sub = this->model()->index(i, 0, root);
            if (sub.isValid() && sub.data(Qt::UserRole).value<EltID>().indexElt == id.indexElt)
                return sub;
        }
    }
        break;
    case elementInst:
    {
        QModelIndex root = this->model()->index(2, 0);
        int nbElt = this->model()->rowCount(root);
        for (int i = 0; i < nbElt; i++)
        {
            QModelIndex sub = this->model()->index(i, 0, root);
            if (sub.isValid() && sub.data(Qt::UserRole).value<EltID>().indexElt == id.indexElt)
                return sub;
        }
    }
        break;
    case elementInstSmpl:
    {
        QModelIndex root = this->model()->index(2, 0);
        int nbElt = this->model()->rowCount(root);
        for (int i = 0; i < nbElt; i++)
        {
            QModelIndex sub = this->model()->index(i, 0, root);
            if (sub.isValid() && sub.data(Qt::UserRole).value<EltID>().indexElt == id.indexElt)
            {
                int nbElt2 = this->model()->rowCount(sub);
                for (int j = 0; j < nbElt2; j++)
                {
                    QModelIndex subSub = this->model()->index(j, 0, sub);
                    if (subSub.isValid() && subSub.data(Qt::UserRole).value<EltID>().indexElt2 == id.indexElt2)
                        return subSub;
                }
            }
        }
    }
        break;
    case elementPrst:
    {
        QModelIndex root = this->model()->index(3, 0);
        int nbElt = this->model()->rowCount(root);
        for (int i = 0; i < nbElt; i++)
        {
            QModelIndex sub = this->model()->index(i, 0, root);
            if (sub.isValid() && sub.data(Qt::UserRole).value<EltID>().indexElt == id.indexElt)
                return sub;
        }
    }
        break;
    case elementPrstInst:
    {
        QModelIndex root = this->model()->index(3, 0);
        int nbElt = this->model()->rowCount(root);
        for (int i = 0; i < nbElt; i++)
        {
            QModelIndex sub = this->model()->index(i, 0, root);
            if (sub.isValid() && sub.data(Qt::UserRole).value<EltID>().indexElt == id.indexElt)
            {
                int nbElt2 = this->model()->rowCount(sub);
                for (int j = 0; j < nbElt2; j++)
                {
                    QModelIndex subSub = this->model()->index(j, 0, sub);
                    if (subSub.isValid() && subSub.data(Qt::UserRole).value<EltID>().indexElt2 == id.indexElt2)
                        return subSub;
                }
            }
        }
    }
        break;
    default:
        break;
    }

    return QModelIndex();
}

void TreeView::saveExpandedState()
{
    _expandedIds.clear();

    // Vertical scroll position
    _verticalScrollValue = this->verticalScrollBar()->value();

    // Headers?
    QModelIndex elt = this->model()->index(1, 0);
    if (elt.isValid() && this->isExpanded(elt))
        _expandedIds << EltID(elementRootSmpl, _sf2Index);
    elt = this->model()->index(2, 0);
    if (elt.isValid() && this->isExpanded(elt))
        _expandedIds << EltID(elementRootInst, _sf2Index);
    elt = this->model()->index(3, 0);
    if (elt.isValid() && this->isExpanded(elt))
        _expandedIds << EltID(elementRootPrst, _sf2Index);

    // Instruments?
    elt = this->model()->index(2, 0);
    if (elt.isValid())
    {
        for (int i = 0; i < this->model()->rowCount(elt); i++)
        {
            QModelIndex child = this->model()->index(i, 0, elt);
            if (child.isValid() && this->isExpanded(child))
                _expandedIds << child.data(Qt::UserRole).value<EltID>();
        }
    }

    // Presets?
    elt = this->model()->index(3, 0);
    for (int i = 0; i < this->model()->rowCount(elt); i++)
    {
        QModelIndex child = this->model()->index(i, 0, elt);
        if (child.isValid() && this->isExpanded(child))
            _expandedIds << child.data(Qt::UserRole).value<EltID>();
    }
}

void TreeView::restoreExpandedState()
{
    foreach (EltID id, _expandedIds)
    {
        switch (id.typeElement)
        {
        case elementRootSmpl:
            this->expand(this->model()->index(1, 0));
            break;
        case elementRootInst:
            this->expand(this->model()->index(2, 0));
            break;
        case elementRootPrst:
            this->expand(this->model()->index(3, 0));
            break;
        case elementInst: case elementPrst:
            this->expand(getIndex(id));
            break;
        default:
            break;
        }
    }
    _expandedIds.clear();

    // Restore the vertical scroll position
    this->verticalScrollBar()->setValue(_verticalScrollValue);
}

void TreeView::openMenu(const QPoint &point)
{
    // Open the menu if the selected ids allow it
    IdList ids = getSelectedIds();
    if (ids.getSelectedIds(elementSmpl).count() + ids.getSelectedIds(elementInst).count() + ids.getSelectedIds(elementPrst).count() > 0)
    {
        _menu->initialize(ids);
        _menu->exec(this->viewport()->mapToGlobal(point));
    }
}

void TreeView::dragEnterEvent(QDragEnterEvent * event)
{
    // Start date time, for preventing unwanted actions
    _startDrag = QDateTime::currentDateTime();

    // Current selection
    _draggedIds.clear();
    IdList ids = getSelectedIds();
    if (ids.sameType())
    {
        _draggedIds = ids;
        event->acceptProposedAction();
    }
    else
        event->ignore();
}

void TreeView::dragMoveEvent(QDragMoveEvent * event)
{
    event->accept();
}

void TreeView::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls() && event->source() == nullptr)
    {
        QList<QUrl> urls = event->mimeData()->urls();
        SoundfontManager * sm = SoundfontManager::getInstance();
        int replace = 0;
        SampleLoader sl(dynamic_cast<QWidget*>(this->parent()));
        IdList smplList;

        for (int i = 0; i < urls.count(); i++)
        {
            QString path = QUrl::fromPercentEncoding(urls.at(i).toEncoded()).replace('\\', '/');
            if (!path.isEmpty())
            {
                QString extension = path.split(".").last().toLower();
                if (extension == "wav" || extension == "flac" || extension == "ogg")
                {
                    path = Utils::fixFilePath(path);
                    smplList << sl.load(path, _sf2Index, &replace);
                }
            }
        }
        sm->endEditing("command:dropSmpl");
        if (!smplList.isEmpty())
            this->onSelectionChanged(smplList);
    }
    else if (!_draggedIds.empty())
    {
        // Destination
#if QT_VERSION < 0x060000
        QModelIndex index = this->indexAt(event->pos());
#else
        QModelIndex index = this->indexAt(event->position().toPoint());
#endif

        // Possibly prevent unwanted actions
        if (!index.isValid() || _startDrag.msecsTo(QDateTime::currentDateTime()) < 150)
            return;

        _dropDestID = index.data(Qt::UserRole).value<EltID>();

        // Trick: trigger a key "enter" event that will validate the drag & drop
        QApplication::postEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier));
    }
}

void TreeView::onCreateElements(IdList ids, bool oneForEach)
{
    if (ids.empty())
        return;
    bool isSmpl = (ids[0].typeElement == elementSmpl);
    int indexSf2 = ids[0].indexSf2;

    // List of all instrument or preset names
    SoundfontManager * sm = SoundfontManager::getInstance();
    QStringList names;
    EltID idTmp(isSmpl ? elementInst : elementPrst, indexSf2);
    foreach (int i, sm->getSiblings(idTmp))
        names << sm->getQstr(EltID(isSmpl ? elementInst : elementPrst, indexSf2, i), champ_name);

    Duplicator duplicator;
    IdList createdElements;
    if (oneForEach)
    {
        while (!ids.empty())
        {
            EltID id = ids.takeFirst();
            EltID otherId = id;
            otherId.indexElt = -1;

            if (isSmpl)
            {
                // A linked sample is possibly in the list
                SFSampleLink linkType = sm->get(id, champ_sfSampleType).sfLinkValue;
                if (linkType != monoSample && linkType != RomMonoSample)
                {
                    quint16 otherSampleId = sm->get(id, champ_wSampleLink).wValue;
                    foreach (EltID idTmp, ids)
                    {
                        if (idTmp.indexElt == otherSampleId)
                        {
                            otherId.indexElt = otherSampleId;
                            ids.removeOne(idTmp);
                            break;
                        }
                    }
                }
            }

            // List of elements for the new instrument or preset
            IdList tmpIds;
            tmpIds << id;
            if (otherId.indexElt != -1)
                tmpIds << otherId;

            // Create the element
            EltID newId = createElement(tmpIds, names, &duplicator);
            if (newId.typeElement == elementUnknown)
                break;
            createdElements << newId;
        }
    }
    else
    {
        EltID newId = createElement(ids, names, &duplicator);
        if (newId.typeElement != elementUnknown)
            createdElements << newId;
    }

    if (!createdElements.empty())
    {
        sm->endEditing("command:fastAdd");
        onSelectionChanged(createdElements);
    }
}

EltID TreeView::createElement(IdList ids, QStringList &existingNames, Duplicator * duplicator)
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    bool isSmpl = (ids[0].typeElement == elementSmpl);

    // Default name
    QStringList currentNames;
    foreach (EltID id, ids)
        currentNames << SoundfontManager::getInstance()->getQstr(id, champ_name);
    QString elementName = Utils::commonPart(currentNames);
    if (elementName.isEmpty())
        elementName = isSmpl ? tr("instrument") : tr("preset");

    // Possibly add a suffix
    if (existingNames.contains(elementName))
    {
        int suffix = 1;
        while (existingNames.contains(elementName + "-" + QString::number(suffix)))
            suffix++;
        elementName += "-" + QString::number(suffix);
    }
    existingNames << elementName;

    // Choose a free bank / preset for a preset
    EltID newElement(isSmpl ? elementInst : elementPrst, ids[0].indexSf2);
    int nBank = -1, nPreset = -1;
    if (!isSmpl)
    {
        sm->firstAvailablePresetBank(newElement, nBank, nPreset);
        if (nBank < 0 || nPreset < 0)
        {
            // Cannot create more presets
            QMessageBox::warning(this, tr("Warning"), tr("Cannot create more presets."));
            return EltID(elementUnknown);
        }
    }

    // Create an element
    newElement.indexElt = sm->add(newElement);
    sm->set(newElement, champ_name, elementName);

    if (isSmpl)
    {
        bool sampleLooped = true;
        bool samePitch = false;
        QMap<unsigned char, QPair<bool, bool> > pitches;

        // Link all dragged samples
        foreach (EltID id, ids)
        {
            // Check the sample is looped
            if (sm->get(id, champ_dwStartLoop).dwValue == sm->get(id, champ_dwEndLoop).dwValue)
                sampleLooped = false;

            // Store the pitch
            int bPitch = sm->get(id, champ_byOriginalPitch).bValue;
            SFSampleLink sampleType = sm->get(id, champ_sfSampleType).sfLinkValue;
            if (!pitches.contains(bPitch))
            {
                pitches[bPitch].first = false;
                pitches[bPitch].second = false;
            }
            if (sampleType != rightSample && sampleType != RomRightSample)
            {
                if (pitches[bPitch].first)
                    samePitch = true;
                else
                    pitches[bPitch].first = true;
            }
            if (sampleType != leftSample && sampleType != RomLeftSample)
            {
                if (pitches[bPitch].second)
                    samePitch = true;
                else
                    pitches[bPitch].second = true;
            }

            duplicator->copy(id, newElement);
        }

        // If all samples are looped, enable it in the instrument
        if (!ids.empty() && sampleLooped)
        {
            AttributeValue val;
            val.wValue = 1;
            sm->set(newElement, champ_sampleModes, val);
        }

        // If there is more than 1 pitch and no more than 1 sample per pitch / side, distribute them
        if (!samePitch && pitches.count() > 1)
        {
            ToolAutoDistribution tool;
            tool.process(sm, newElement, NULL);
        }
    }
    else
    {
        // Set a bank and preset number
        AttributeValue val;
        val.wValue = static_cast<quint16>(nBank);
        sm->set(newElement, champ_wBank, val);
        val.wValue = static_cast<quint16>(nPreset);
        sm->set(newElement, champ_wPreset, val);

        // Link all dragged instruments
        foreach (EltID id, ids)
            duplicator->copy(id, newElement);
    }

    // The element has been created
    return newElement;
}


#include "treeview.h"
#include "treeitemdelegate.h"
#include "treesortfilterproxy.h"
#include "soundfontmanager.h"
#include <QMouseEvent>
#include <QScrollBar>

TreeView::TreeView(QWidget * parent) : QTreeView(parent),
    _fixingSelection(false),
    _sf2Index(-1),
    _bestMatchSample(-1),
    _bestMatchInstrument(-1),
    _bestMatchPreset(-1)
{
    this->setItemDelegate(new TreeItemDelegate(this));
    this->viewport()->setAutoFillBackground(false);
    this->sortByColumn(0, Qt::AscendingOrder);
}

void TreeView::mousePressEvent(QMouseEvent * event)
{
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
            TreeSortFilterProxy * proxy = (TreeSortFilterProxy *)this->model();
            if (!proxy->isFiltered(id))
                this->onSelectionChanged(IdList(id));
            event->accept();
            return;
        }
        else if (currentId.typeElement == elementPrstInst)
        {
            // Find the corresponding instrument
            EltID id(elementInst, _sf2Index, SoundfontManager::getInstance()->get(currentId, champ_instrument).wValue, -1, -1);
            TreeSortFilterProxy * proxy = (TreeSortFilterProxy *)this->model();
            if (!proxy->isFiltered(id))
                this->onSelectionChanged(IdList(id));
            event->accept();
            return;
        }
    }

    QTreeView::mouseDoubleClickEvent(event);
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

    // Reset the selection if not valid
    if (!isSelectionValid())
    {
        _fixingSelection = true;
        this->clearSelection();
        _fixingSelection = false;
    }

    // First attempt to select an index if empty: reselect the selected index or the unselected index
    if (this->selectedIndexes().isEmpty())
    {
        _fixingSelection = true;
        if (!selected.indexes().isEmpty())
        {
            // Id to reselect
            EltID id = selected.indexes()[0].data(Qt::UserRole).value<EltID>();
            TreeSortFilterProxy * proxy = (TreeSortFilterProxy *)this->model();
            if (!proxy->isFiltered(id))
                this->setCurrentIndex(selected.indexes()[0]);
        }
        else if (!deselected.indexes().isEmpty())
        {
            // Id to reselect
            EltID id = deselected.indexes()[0].data(Qt::UserRole).value<EltID>();
            TreeSortFilterProxy * proxy = (TreeSortFilterProxy *)this->model();
            if (!proxy->isFiltered(id))
                this->setCurrentIndex(deselected.indexes()[0]);
        }
        _fixingSelection = false;
    }

    // Second attempt to select an index if empty: take the best match based on the filter
    if (this->selectedIndexes().isEmpty())
    {
        switch (_lastSelectedId.typeElement)
        {
        case elementSmpl:
            // First sample, then instrument and finally preset
            if (_bestMatchSample != -1)
                onSelectionChanged(EltID(elementSmpl, _sf2Index, _bestMatchSample, -1, -1));
            if (this->selectedIndexes().isEmpty() && _bestMatchInstrument != -1)
                onSelectionChanged(EltID(elementInst, _sf2Index, _bestMatchInstrument, -1, -1));
            if (this->selectedIndexes().isEmpty() && _bestMatchPreset != -1)
                onSelectionChanged(EltID(elementPrst, _sf2Index, _bestMatchPreset, -1, -1));
            break;
        case elementInst: case elementInstSmpl:
            // First instrument, then sample and finally preset
            if (_bestMatchInstrument != -1)
                onSelectionChanged(EltID(elementInst, _sf2Index, _bestMatchInstrument, -1, -1));
            if (this->selectedIndexes().isEmpty() && _bestMatchSample != -1)
                onSelectionChanged(EltID(elementSmpl, _sf2Index, _bestMatchSample, -1, -1));
            if (this->selectedIndexes().isEmpty() && _bestMatchPreset != -1)
                onSelectionChanged(EltID(elementPrst, _sf2Index, _bestMatchPreset, -1, -1));
            break;
        case elementPrst: case elementPrstInst:
            // First preset, then instrument and finally sample
            if (_bestMatchPreset != -1)
                onSelectionChanged(EltID(elementPrst, _sf2Index, _bestMatchPreset, -1, -1));
            if (this->selectedIndexes().isEmpty() && _bestMatchInstrument != -1)
                onSelectionChanged(EltID(elementInst, _sf2Index, _bestMatchInstrument, -1, -1));
            if (this->selectedIndexes().isEmpty() && _bestMatchSample != -1)
                onSelectionChanged(EltID(elementSmpl, _sf2Index, _bestMatchSample, -1, -1));
            break;
        default:
            break;
        }
    }

    // Third attempt to select an index if empty: take the root
    if (this->selectedIndexes().isEmpty())
    {
        _fixingSelection = true;
        this->setCurrentIndex(this->model()->index(0, 0));
        _fixingSelection = false;
    }

    IdList selectedIds;
    foreach (QModelIndex index, this->selectedIndexes())
        selectedIds << index.data(Qt::UserRole).value<EltID>();
    emit(selectionChanged(selectedIds));
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
            this->expand(root.child(index.parent().row(), 0));
        }
            break;
        case elementPrstInst:
        {
            QModelIndex root = this->model()->index(3, 0);
            this->expand(root);
            this->expand(root.child(index.parent().row(), 0));
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
            QModelIndex sub = root.child(i, 0);
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
            QModelIndex sub = root.child(i, 0);
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
            QModelIndex sub = root.child(i, 0);
            if (sub.isValid() && sub.data(Qt::UserRole).value<EltID>().indexElt == id.indexElt)
            {
                int nbElt2 = this->model()->rowCount(sub);
                for (int j = 0; j < nbElt2; j++)
                {
                    QModelIndex subSub = sub.child(j, 0);
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
            QModelIndex sub = root.child(i, 0);
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
            QModelIndex sub = root.child(i, 0);
            if (sub.isValid() && sub.data(Qt::UserRole).value<EltID>().indexElt == id.indexElt)
            {
                int nbElt2 = this->model()->rowCount(sub);
                for (int j = 0; j < nbElt2; j++)
                {
                    QModelIndex subSub = sub.child(j, 0);
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
    if (this->isExpanded(this->model()->index(1, 0)))
        _expandedIds << EltID(elementRootSmpl, _sf2Index);
    if (this->isExpanded(this->model()->index(2, 0)))
        _expandedIds << EltID(elementRootInst, _sf2Index);
    if (this->isExpanded(this->model()->index(3, 0)))
        _expandedIds << EltID(elementRootPrst, _sf2Index);

    // Instruments?
    QModelIndex parent = this->model()->index(2, 0);
    for (int i = 0; i < this->model()->rowCount(parent); i++)
    {
        QModelIndex child = parent.child(i, 0);
        if (this->isExpanded(child))
            _expandedIds << child.data(Qt::UserRole).value<EltID>();
    }

    // Presets?
    parent = this->model()->index(3, 0);
    for (int i = 0; i < this->model()->rowCount(parent); i++)
    {
        QModelIndex child = parent.child(i, 0);
        if (this->isExpanded(child))
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

    // Restore the vertical scroll position
    this->verticalScrollBar()->setValue(_verticalScrollValue);
}

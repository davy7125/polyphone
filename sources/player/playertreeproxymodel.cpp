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

#include "playertreeproxymodel.h"
#include <QListView>
#include "basetypes.h"
#include "soundfontmanager.h"
#include "treeitem.h"

PlayerTreeProxyModel::PlayerTreeProxyModel(int indexSf2, QAbstractItemModel * model) : QAbstractProxyModel(),
    _indexSf2(indexSf2)
{
    // Get the root preset index
    _rootIndex = model->index(3, 0);

    // Scan bank and preset numbers
    EltID idPrst = EltID(elementPrst, indexSf2);
    SoundfontManager * sm = SoundfontManager::getInstance();
    foreach (int index, sm->getSiblings(idPrst))
    {
        // Store the current preset id in the tree
        idPrst.indexElt = index;
        quint16 bank = sm->get(idPrst, champ_wBank).wValue;
        quint16 preset = sm->get(idPrst, champ_wPreset).wValue;
        _idByBankPreset[bank][preset] = index;
        _bankPresetById[index] = QPair<quint16, quint16>(bank, preset);
    }

    this->setSourceModel(model);
}

int PlayerTreeProxyModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

int PlayerTreeProxyModel::rowCount(const QModelIndex &parent) const
{
    // By default: no children for presets
    int result = 0;

    if (!parent.isValid())
    {
        // Root: as many children as banks
        result = _idByBankPreset.keys().count();
    }
    else if (parent.internalPointer() == nullptr)
    {
        // Bank: number of presets sharing this bank
        if (parent.row() >= _idByBankPreset.count())
            return 0;
        int bank = _idByBankPreset.keys()[parent.row()];
        result = _idByBankPreset[bank].count();
    }

    return result;
}

QModelIndex PlayerTreeProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    // Bank?
    if (!parent.isValid())
        return createIndex(row, column, nullptr);

    // Otherwise we keep the same data than the source
    if (parent.row() >= _idByBankPreset.count())
        return QModelIndex();
    int bank = _idByBankPreset.keys()[parent.row()];

    if (row >= _idByBankPreset[bank].count())
        return QModelIndex();
    int preset = _idByBankPreset[bank].keys()[row];

    // A simplification is made: preset id = position in the model
    return createIndex(row, column, sourceModel()->index(_idByBankPreset[bank][preset], column, _rootIndex).internalPointer());
}

QModelIndex PlayerTreeProxyModel::parent(const QModelIndex &child) const
{
    // Parent of a bank?
    if (!child.isValid() || child.internalPointer() == nullptr)
        return QModelIndex();

    // Otherwise parent of a preset
    int id = this->getPresetId(child);
    if (id == -1)
        return QModelIndex();

    QPair<quint16, quint16> bankPreset = _bankPresetById[id];
    if (_idByBankPreset.keys().contains(bankPreset.first))
        return createIndex(_idByBankPreset.keys().indexOf(bankPreset.first), 0, nullptr);
    return QModelIndex();
}

QModelIndex PlayerTreeProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    // Root or bank?
    if (!proxyIndex.isValid() || proxyIndex.internalPointer() == nullptr)
        return _rootIndex;

    // Simplification: the position is the preset id
    int id = this->getPresetId(proxyIndex);
    if (id == -1)
        return QModelIndex();
    return sourceModel()->index(id, proxyIndex.column(), _rootIndex);
}

QModelIndex PlayerTreeProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    int id = this->getPresetId(sourceIndex);
    if (id == -1)
        return QModelIndex();

    QPair<quint16, quint16> bankPreset = _bankPresetById[id];
    return createIndex(_idByBankPreset[bankPreset.first].keys().indexOf(bankPreset.second), 0, sourceIndex.internalPointer());
}

QVariant PlayerTreeProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
    if (proxyIndex.isValid() && proxyIndex.internalPointer() == nullptr)
    {
        if (role == Qt::DisplayRole)
        {
            // Display the bank number
            if (proxyIndex.row() < _idByBankPreset.count())
                return tr("Bank %1").arg(_idByBankPreset.keys()[proxyIndex.row()]);
        }
        else if (role == Qt::UserRole + 1)
        {
            // Return the bank number
            if (proxyIndex.row() < _idByBankPreset.count())
                return _idByBankPreset.keys()[proxyIndex.row()];
        }
    }
    else if (proxyIndex.isValid() && proxyIndex.internalPointer() != nullptr)
    {
        if (role == Qt::UserRole + 1)
        {
            // Current bank
            if (proxyIndex.parent().row() < _idByBankPreset.count())
            {
                int bank = _idByBankPreset.keys()[proxyIndex.parent().row()];

                // Return the preset number
                if (proxyIndex.row() < _idByBankPreset[bank].count())
                    return _idByBankPreset[bank].keys()[proxyIndex.row()];
            }
        }
    }

    return QAbstractProxyModel::data(proxyIndex, role);
}

int PlayerTreeProxyModel::getPresetId(const QModelIndex &index) const
{
    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    if (item == nullptr)
        return -1;
    EltID id = item->getId();
    return id.typeElement == elementPrst ? id.indexElt : -1;
}

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

#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include "basetypes.h"
class TreeItem;

class TreeModel: public QAbstractItemModel
{
    Q_OBJECT

public:
    TreeModel(TreeItem * rootItem);

    int columnCount(const QModelIndex &parent) const override;
    int	rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QModelIndex	index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex	parent(const QModelIndex &index) const override;

    /// Drag & drop
    Qt::DropActions supportedDropActions() const override { return Qt::MoveAction; }
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    /// Tree update
    void elementBeingAdded(EltID id);
    void endOfAddition();
    void elementUpdated(EltID id);
    void elementBeingDeleted(EltID id, bool storeExpandedState);
    void endOfDeletion();
    void visibilityChanged(EltID id);

signals:
    void saveExpandedState();
    void restoreExpandedState();

private:
    QModelIndex getParentIndexWithPosition(EltID id, int &position);
    TreeItem * _rootItem;
};

#endif // TREEMODEL_H

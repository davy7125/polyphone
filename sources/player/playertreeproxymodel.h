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

#ifndef PLAYERTREEPROXYMODEL_H
#define PLAYERTREEPROXYMODEL_H

#include <QAbstractProxyModel>
class QListView;

class PlayerTreeProxyModel : public QAbstractProxyModel
{
    Q_OBJECT

public:
    PlayerTreeProxyModel(int indexSf2, QAbstractItemModel * model);

    int columnCount(const QModelIndex &parent) const override;
    int rowCount(const QModelIndex &parent) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const override;

protected:
    int getPresetId(const QModelIndex &index) const;

    int _indexSf2;
    QModelIndex _rootIndex;
    QMap<quint16, QMap<quint16, int> > _idByBankPreset;
    QMap<int, QPair<quint16, quint16> > _bankPresetById;
};

#endif // PLAYERTREEPROXYMODEL_H

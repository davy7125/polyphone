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

#ifndef TREESORTFILTERPROXY_H
#define TREESORTFILTERPROXY_H

#include <QSortFilterProxyModel>
#include "basetypes.h"
class TreeView;
class SoundfontManager;

class TreeSortFilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    TreeSortFilterProxy(int indexSf2, TreeView * treeView, QAbstractItemModel * model);
    bool isFiltered(EltID id);

public slots:
    void filterChanged(QString filter);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

private slots:
    void divisionSortChanged();

private:
    void findMatches(int idSf2, QString filter);
    bool lessThan(QString left, QString right) const;

    int _indexSf2;
    TreeView * _treeView;
    QList<int> _matchingSamples;
    QList<int> _matchingInstruments;
    QList<int> _matchingPresets;
    int _bestMatchSample, _bestMatchInstrument, _bestMatchPreset;
    QString _bestMatchSampleName, _bestMatchInstrumentName, _bestMatchPresetName;
    SoundfontManager * _sm;
    int _sortType;
};

#endif // TREESORTFILTERPROXY_H

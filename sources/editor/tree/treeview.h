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

#ifndef TREEVIEW_H
#define TREEVIEW_H

#include <QTreeView>
#include "basetypes.h"
class TreeViewMenu;

class TreeView : public QTreeView
{
    Q_OBJECT

public:
    explicit TreeView(QWidget *parent = nullptr);
    void setBestMatch(int sampleId, int instrumentId, int presetId);
    void setSf2Index(int sf2Index) { _sf2Index = sf2Index; }
    void expandAndScrollToSelection();

public slots:
    void onSelectionChanged(const IdList &selectedIds);
    void saveExpandedState();
    void restoreExpandedState();

signals:
    void selectionChanged(IdList selectedIds);
    void sampleOnOff();
    void focusOnSearch();

protected:
    void mousePressEvent(QMouseEvent * event) override;
    void mouseDoubleClickEvent(QMouseEvent * event) override;
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) override;
    void keyPressEvent(QKeyEvent * event) override;
    void dragEnterEvent(QDragEnterEvent * event) override;
    void dragMoveEvent(QDragMoveEvent * event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void openMenu(const QPoint &point);
    void onCreateElements(IdList ids, bool oneForEach);
    void onKeyPlayed(int key, int vel);

private:
    bool select(EltID id, QItemSelectionModel::SelectionFlag flags = QItemSelectionModel::ClearAndSelect);
    bool isSelectionValid();
    QModelIndex getIndex(EltID id);
    IdList getSelectedIds();

    bool _fixingSelection;
    int _sf2Index;
    int _bestMatchSample;
    int _bestMatchInstrument;
    int _bestMatchPreset;
    EltID _lastSelectedId;
    QList<EltID> _expandedIds;
    int _verticalScrollValue;
    TreeViewMenu * _menu;
    IdList _draggedIds;
};

#endif // TREEVIEW_H

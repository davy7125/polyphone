#ifndef TREEVIEW_H
#define TREEVIEW_H

#include <QTreeView>
#include "sf2_types.h"
#include "idlist.h"

class TreeView : public QTreeView
{
    Q_OBJECT

public:
    explicit TreeView(QWidget *parent = NULL);
    void setBestMatch(int sampleId, int instrumentId, int presetId);
    void setSf2Index(int sf2Index) { _sf2Index = sf2Index; }
    void expandAndScrollToSelection();

public slots:
    void onSelectionChanged(const IdList &selectedIds);
    void saveExpandedState();
    void restoreExpandedState();

signals:
    void selectionChanged(IdList selectedIds);

protected:
    void mousePressEvent(QMouseEvent * event);
    void mouseDoubleClickEvent(QMouseEvent * event);
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) override;

private:
    bool select(EltID id, QItemSelectionModel::SelectionFlag flags = QItemSelectionModel::ClearAndSelect);
    bool isSelectionValid();
    QModelIndex getIndex(EltID id);

    bool _fixingSelection;
    int _sf2Index;
    int _bestMatchSample;
    int _bestMatchInstrument;
    int _bestMatchPreset;
    EltID _lastSelectedId;
    QList<EltID> _expandedIds;
    int _verticalScrollValue;
};

#endif // TREEVIEW_H

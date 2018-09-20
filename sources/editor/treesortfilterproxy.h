#ifndef TREESORTFILTERPROXY_H
#define TREESORTFILTERPROXY_H

#include <QSortFilterProxyModel>
#include "sf2_types.h"
class TreeView;

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
};

#endif // TREESORTFILTERPROXY_H

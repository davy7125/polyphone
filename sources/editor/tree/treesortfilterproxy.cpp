#include "treesortfilterproxy.h"
#include "treeitem.h"
#include "utils.h"
#include "soundfontmanager.h"
#include "treeview.h"
//#include <QAbstractItemModelTester>

TreeSortFilterProxy::TreeSortFilterProxy(int indexSf2, TreeView *treeView, QAbstractItemModel * model) : QSortFilterProxyModel(treeView),
    _indexSf2(indexSf2),
    _treeView(treeView),
    _sm(SoundfontManager::getInstance())
{
    //new QAbstractItemModelTester(model, QAbstractItemModelTester::FailureReportingMode::Warning, this);

    this->setSourceModel(model);
    _treeView->setSf2Index(indexSf2);
    _treeView->setModel(this);
    _treeView->setCurrentIndex(this->index(0, 0));
}

bool TreeSortFilterProxy::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    bool result = true;

    EltID id = left.data(Qt::UserRole).value<EltID>();
    switch (id.typeElement)
    {
    case elementSf2:
        result = true;
        break;
    case elementRootSmpl:
    {
        EltID id2 = right.data(Qt::UserRole).value<EltID>();
        result = (id2.typeElement != elementSf2);
    }
        break;
    case elementRootInst:
    {
        EltID id2 = right.data(Qt::UserRole).value<EltID>();
        result = (id2.typeElement == elementRootPrst);
    }
        break;
    case elementRootPrst:
        result = false;
        break;
    case elementInstSmpl: case elementPrstInst:
    {
        EltID id2 = right.data(Qt::UserRole).value<EltID>();
        int lKey1 = _sm->get(id, champ_keyRange).rValue.byLo;
        int lKey2 = _sm->get(id2, champ_keyRange).rValue.byLo;
        if (lKey1 != lKey2)
            result = (lKey1 < lKey2);
        else
        {
            int lVel1 = _sm->get(id, champ_velRange).rValue.byLo;
            int lVel2 = _sm->get(id2, champ_velRange).rValue.byLo;
            if (lVel1 != lVel2)
                result = (lVel1 < lVel2);
            else
                result = lessThan(left.data(Qt::DisplayRole).toString(), right.data(Qt::DisplayRole).toString());
        }
    }
        break;
    case elementPrst:
    {
        EltID id2 = right.data(Qt::UserRole).value<EltID>();
        result = ((_sm->get(id, champ_wBank).wValue * 1000 + _sm->get(id, champ_wPreset).wValue) <
                  (_sm->get(id2, champ_wBank).wValue * 1000 + _sm->get(id2, champ_wPreset).wValue));
    }
        break;
    default: // Inst, Smpl
        result = lessThan(left.data(Qt::DisplayRole).toString(), right.data(Qt::DisplayRole).toString());
        break;
    }

    return result;
}

bool TreeSortFilterProxy::lessThan(QString left, QString right) const
{
    return Utils::naturalOrder(Utils::removeAccents(left).toLower(), Utils::removeAccents(right).toLower()) < 0;
}

void TreeSortFilterProxy::filterChanged(QString filter)
{
    // Find the matches to prepare the filter
    filter = filter.toLower();
    findMatches(_indexSf2, filter);
    _treeView->setBestMatch(_bestMatchSample, _bestMatchInstrument, _bestMatchPreset);

    // Change the filter
    setFilterFixedString(filter);
    emit(layoutChanged());
    _treeView->expandAndScrollToSelection();
}

bool TreeSortFilterProxy::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index = this->sourceModel()->index(sourceRow, 0, sourceParent);

    // Property isHidden has the priority
    if (index.data(Qt::UserRole + 1).toBool())
        return false;

    // If the filter is empty, the item is visible
    if (filterRegExp().isEmpty())
        return true;

    // Visibility, depending on the items that match
    EltID id = index.data(Qt::UserRole).value<EltID>();
    bool isVisible = true;
    switch (id.typeElement)
    {
    case elementSf2: case elementRootSmpl: case elementRootInst: case elementRootPrst:
        isVisible = true;
        break;
    case elementSmpl:
        isVisible = _matchingSamples.contains(id.indexElt);
        break;
    case elementInst: case elementInstSmpl:
        isVisible = _matchingInstruments.contains(id.indexElt);
        break;
    case elementPrst: case elementPrstInst:
        isVisible = _matchingPresets.contains(id.indexElt);
        break;
    default:
        isVisible = false;
        break;
    }

    return isVisible;
}

void TreeSortFilterProxy::findMatches(int idSf2, QString filter)
{
    // Reinit the matches
    _matchingSamples.clear();
    _matchingInstruments.clear();
    _matchingPresets.clear();
    _bestMatchSample = _bestMatchInstrument = _bestMatchPreset = -1;
    if (filter.isEmpty())
        return;

    // Browse all samples
    EltID idSmpl(elementSmpl, idSf2, -1, -1, -1);
    foreach (int i, _sm->getSiblings(idSmpl))
    {
        idSmpl.indexElt = i;
        QString name = _sm->getQstr(idSmpl, champ_name).toLower();
        if (name.contains(filter))
        {
            _matchingSamples << i;
            if (_bestMatchSample == -1 || lessThan(name, _bestMatchSampleName))
            {
                _bestMatchSample = i;
                _bestMatchSampleName = name;
            }
        }
    }

    // Browse all instruments
    EltID idInst(elementInst, idSf2, -1, -1, -1);
    foreach (int i, _sm->getSiblings(idInst))
    {
        idInst.indexElt = i;
        QString name = _sm->getQstr(idInst, champ_name).toLower();
        if (name.contains(filter))
        {
            _matchingInstruments << i;
            if (_bestMatchInstrument == -1 || lessThan(name, _bestMatchInstrumentName))
            {
                _bestMatchInstrument = i;
                _bestMatchInstrumentName = name;
            }
        }
    }

    // Browse all presets
    EltID idPrst(elementPrst, idSf2, -1, -1, -1);
    foreach (int i, _sm->getSiblings(idPrst))
    {
        idPrst.indexElt = i;
        QString name = QString("%1:%2 %3")
                .arg(_sm->get(idPrst, champ_wBank).wValue, 3, 10, QChar('0'))
                .arg(_sm->get(idPrst, champ_wPreset).wValue, 3, 10, QChar('0'))
                .arg(_sm->getQstr(idPrst, champ_name)).toLower();
        if (name.contains(filter))
        {
            _matchingPresets << i;
            if (_bestMatchPreset == -1 || lessThan(name, _bestMatchPresetName))
            {
                _bestMatchPreset = i;
                _bestMatchPresetName = name;
            }
        }
    }
}

bool TreeSortFilterProxy::isFiltered(EltID id)
{
    bool isFiltered = false;
    if (!filterRegExp().isEmpty())
    {
        switch (id.typeElement)
        {
        case elementSmpl:
            isFiltered = !_matchingSamples.contains(id.indexElt);
            break;
        case elementInst: case elementInstSmpl:
            isFiltered = !_matchingInstruments.contains(id.indexElt);
            break;
        case elementPrst: case elementPrstInst:
            isFiltered = !_matchingPresets.contains(id.indexElt);
            break;
        default:
            break;
        }
    }
    return isFiltered;
}

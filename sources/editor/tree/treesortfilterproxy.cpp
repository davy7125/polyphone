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

#include "treesortfilterproxy.h"
#include "treeitem.h"
#include "utils.h"
#include "soundfontmanager.h"
#include "treeview.h"
#include "contextmanager.h"
//#include <QAbstractItemModelTester>

TreeSortFilterProxy::TreeSortFilterProxy(int indexSf2, TreeView *treeView, QAbstractItemModel * model) : QSortFilterProxyModel(treeView),
    _indexSf2(indexSf2),
    _treeView(treeView),
    _sm(SoundfontManager::getInstance()),
    _sortType(ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "division_sort", 0).toInt())
{
    //new QAbstractItemModelTester(model, QAbstractItemModelTester::FailureReportingMode::Warning, this);
    this->setDynamicSortFilter(true);
    this->setSourceModel(model);
    _treeView->setSf2Index(indexSf2);
    _treeView->setModel(this);
    _treeView->setCurrentIndex(this->index(0, 0));

    connect(ContextManager::configuration(), SIGNAL(divisionSortChanged()), this, SLOT(divisionSortChanged()));
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
        result = (Utils::sortDivisions(id, id2, _sortType) < 0);
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
        result = lessThan(left.data(Qt::UserRole + 2).toString(), right.data(Qt::UserRole + 2).toString());
        break;
    }

    return result;
}

bool TreeSortFilterProxy::lessThan(QString left, QString right) const
{
    return Utils::naturalOrder(left, right) < 0;
}

void TreeSortFilterProxy::filterChanged(QString filter)
{
    // Find the matches to prepare the filter
    filter = Utils::removeAccents(filter).toLower();
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
        QString name = _sm->getQstr(idSmpl, champ_nameSort);
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
        QString name = _sm->getQstr(idInst, champ_nameSort);
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
                .arg(_sm->getQstr(idPrst, champ_nameSort));
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

void TreeSortFilterProxy::divisionSortChanged()
{
    // Update the sort type
    _sortType = ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "division_sort", 0).toInt();

    // Refresh all divisions
    this->invalidate();
}

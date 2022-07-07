/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2020 Davy Triponney                                **
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

#include "pageselector.h"
#include "pagesf2.h"
#include "pageoverviewsmpl.h"
#include "pageoverviewinst.h"
#include "pageoverviewprst.h"
#include "pagesmpl.h"
#include "pageinst.h"
#include "pageprst.h"
#include "pagerange.h"
#include "pageenvelope.h"
#include "extensionmanager.h"

PageSelector::PageSelector()
{
    // List of pages (will be deleted with their parent in the Editor object)
    _pages << new PageSf2()
           << new PageOverviewSmpl()
           << new PageOverviewInst()
           << new PageOverviewPrst()
           << new PageSmpl()
           << new PageInst()
           << new PagePrst()
           << new PageRange()
           << new PageEnvelope();

    // Pages from possible extensions
    for (int i = 0; i < ExtensionManager::view()->count(); i++)
        _pages << ExtensionManager::view()->createPage(i);

    // Sort them per type
    ElementType types[] = {
        elementSf2, elementSmpl, elementInst, elementPrst,
        elementRootSmpl, elementRootInst, elementRootPrst
    };

    for (int i = 0; i < 7; i++)
    {
        foreach (Page * page, _pages)
        {
            if (page->isSuitableFor(types[i]))
                _pagesPerType[types[i]] << page;
        }
    }
}

QList<Page *> PageSelector::getPages(ElementType elementType)
{
    if (elementType == elementInstSmpl)
        elementType = elementInst;
    else if (elementType == elementPrstInst)
        elementType = elementPrst;

    if (_pagesPerType.contains(elementType))
        return _pagesPerType[elementType];
    return QList<Page *>();
}

Page * PageSelector::getLastPage(ElementType elementType)
{
    if (elementType == elementInstSmpl)
        elementType = elementInst;
    else if (elementType == elementPrstInst)
        elementType = elementPrst;

    // Maybe a page has already been selected for the element type
    if (_lastPagePerType.contains(elementType))
        return _lastPagePerType[elementType];

    // Otherwise the first suitable page for the type is returned
    Page * result = _pagesPerType.contains(elementType) ? _pagesPerType[elementType].first() : nullptr;
    if (result != nullptr)
        _lastPagePerType[elementType] = result;

    return result;
}

void PageSelector::setLastPage(ElementType elementType, Page * page)
{
    if (elementType == elementInstSmpl)
        elementType = elementInst;
    else if (elementType == elementPrstInst)
        elementType = elementPrst;
    _lastPagePerType[elementType] = page;
}

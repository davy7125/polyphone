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

#ifndef PAGESELECTOR_H
#define PAGESELECTOR_H

#include "page.h"
#include "basetypes.h"

class PageSelector
{
public:
    PageSelector();

    QList<Page *> getAllPages() { return _pages; }
    QList<Page *> getPages(ElementType elementType);
    Page * getLastPage(ElementType elementType);
    void setLastPage(ElementType elementType, Page * page);

private:
    QList<Page *> _pages;
    QMap<ElementType, QList<Page *> > _pagesPerType;
    QMap<ElementType, Page *> _lastPagePerType;
};

#endif // PAGESELECTOR_H

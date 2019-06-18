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

#include "treeitemfirstlevel.h"
#include "treeitemroot.h"
#include <QMap>

TreeItemFirstLevel::TreeItemFirstLevel(QString label, IndexedElementList<TreeItem *> *children, TreeItemRoot *parent, EltID id) : TreeItem(id, parent),
    _label(label),
    _children(children)
{
    _row = parent->addChild(this);
}

TreeItemFirstLevel::~TreeItemFirstLevel()
{
    ((TreeItemRoot *)this->parent())->removeChild(this);
}

int TreeItemFirstLevel::childCount() const
{
    if (_children == nullptr)
        return 0;
    return _children->positionCount();
}

TreeItem * TreeItemFirstLevel::child(int row)
{
    if (_children == nullptr || row < _children->positionCount())
        return _children->atPosition(row);
    return nullptr;
}

QString TreeItemFirstLevel::display()
{
    return _label;
}

int TreeItemFirstLevel::row()
{
    return _row;
}

int TreeItemFirstLevel::indexOfId(int id)
{
    if (_children == nullptr)
        return -1;
    return _children->positionOfIndex(id);
}

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

#include "treeitemroot.h"

TreeItemRoot::TreeItemRoot(EltID id) : TreeItem(id, nullptr)
{

}

int TreeItemRoot::childCount() const
{
    return _children.count();
}

TreeItem * TreeItemRoot::child(int row)
{
    if (row < _children.count())
        return _children[row];
    return nullptr;
}

QString TreeItemRoot::display()
{
    return "";
}

int TreeItemRoot::row()
{
    return 0;
}

int TreeItemRoot::addChild(TreeItem * treeItem)
{
    _children.append(treeItem);
    return _children.count() - 1;
}

void TreeItemRoot::removeChild(TreeItem * treeItem)
{
    _children.removeAll(treeItem);
}

int TreeItemRoot::indexOfId(int id)
{
    for (int i = 0; i < _children.count(); i++)
        if ((int)_children[i]->getId().typeElement == id)
            return i;
    return -1;
}

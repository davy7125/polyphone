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

#ifndef TREEITEM_H
#define TREEITEM_H

#include <QString>
#include "basetypes.h"
class TreeModel;

class TreeItem
{
public:
    TreeItem(EltID id, TreeItem * parent);
    virtual ~TreeItem();

    // Parent of the item
    TreeItem * parent() { return _parent; }

    // Number of children
    virtual int childCount() const = 0;

    // Find a child
    virtual TreeItem * child(int row) = 0;

    // Current row of the item
    virtual int row() = 0;

    // Position of an id
    virtual int indexOfId(int id) = 0;

    // Data associated to the item
    virtual QString display() = 0;
    virtual QString sortText();
    EltID getId() { return _id; }
    void setHidden(bool isHidden);
    bool isHidden() { return _isHidden; }

    // Trigger signals for updating the view
    void notifyCreated();
    void notifyRename();
    void notifyDeletion(bool storeExpandedState = true);

protected:
    TreeModel * _model;
    EltID _id;
    TreeItem * _parent;
    bool _isHidden;
};

#endif // TREEITEM_H

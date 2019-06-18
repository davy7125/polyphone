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

#include "treeitem.h"
#include "treemodel.h"

TreeItem::TreeItem(EltID id, TreeItem * parent) :
    _model(nullptr),
    _id(id),
    _parent(parent),
    _isHidden(false)
{
    if (parent != nullptr)
        _model = parent->_model;
    if (_model != nullptr)
        _model->elementBeingAdded(_id);
}

void TreeItem::notifyCreated()
{
    if (_model)
        _model->endOfAddition();
}

void TreeItem::notifyRename()
{
    if (_model)
        _model->elementUpdated(_id);
}

void TreeItem::notifyDeletion(bool storeExpandedState)
{
    if (_model)
        _model->elementBeingDeleted(_id, storeExpandedState);
}

TreeItem::~TreeItem()
{
    if (_model)
        _model->endOfDeletion();
}

void TreeItem::setHidden(bool isHidden)
{
    _isHidden = isHidden;
    if (_model)
        _model->visibilityChanged(_id);
}

QString TreeItem::sortText()
{
    return this->display();
}

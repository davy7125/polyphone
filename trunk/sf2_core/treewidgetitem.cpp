/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2016 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://polyphone-soundfonts.com                      **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "treewidgetitem.h"
#include "utils.h"

TreeWidgetItem::TreeWidgetItem(QTreeWidgetItem *parent, EltID id) : QTreeWidgetItem(parent, Type),
    _id(id),
    _hidden(false)
{
    setStyle();
}

TreeWidgetItem::TreeWidgetItem(QTreeWidget *parent, EltID id) : QTreeWidgetItem(parent, Type),
    _id(id),
    _hidden(false)
{
    setStyle();
}

void TreeWidgetItem::setStyle()
{
    QFont font = this->font(0);

    switch (_id.typeElement)
    {
    case elementSf2:
        font.setPointSize(15);
        font.setBold(true);
        this->setFont(0, font);
        this->setSizeHint(0, QSize(0,30));
        this->setIcon(0, QIcon(":/icones/document"));
        break;
    case elementRootSmpl: case elementRootInst: case elementRootPrst:
        font.setPointSize(10);
        font.setBold(true);
        this->setFont(0, font);
        this->setSizeHint(0, QSize(0,23));
        break;
    case elementSmpl: case elementInstSmpl:
        this->setSizeHint(0, QSize(0, 17));
        this->setIcon(0, QIcon(":/icones/wave"));
        break;
    case elementInst: case elementPrstInst:
        this->setSizeHint(0, QSize(0, 17));
        this->setIcon(0, QIcon(":/icones/sound"));
        break;
    case elementPrst:
        this->setSizeHint(0, QSize(0, 17));
        this->setIcon(0, QIcon(":/icones/music"));
        break;
    default:
        break;
    }
}

void TreeWidgetItem::decrementSf2()
{
    _id = EltID(_id.typeElement, _id.indexSf2 - 1, _id.indexElt, _id.indexElt2, _id.indexMod);
    for (int i = 0; i < this->childCount(); i++)
        ((TreeWidgetItem *)this->child(i))->decrementSf2();
}

void TreeWidgetItem::decrementElement()
{
    _id = EltID(_id.typeElement, _id.indexSf2, _id.indexElt - 1, _id.indexElt2, _id.indexMod);
    for (int i = 0; i < this->childCount(); i++)
        ((TreeWidgetItem *)this->child(i))->decrementElement();
}

void TreeWidgetItem::decrementElement2()
{
    _id = EltID(_id.typeElement, _id.indexSf2, _id.indexElt, _id.indexElt2 - 1, _id.indexMod);
}

bool TreeWidgetItem::operator < (const QTreeWidgetItem &other) const
{
    return Utils::naturalOrder(this->_orderedText, ((const TreeWidgetItem&)other)._orderedText) < 0;
}


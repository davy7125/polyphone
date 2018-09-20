/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2017 Davy Triponney                                **
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

#ifndef TREEWIDGETITEM_H
#define TREEWIDGETITEM_H

#include <QTreeWidgetItem>
#include "sf2_types.h"

class TreeWidgetItem : public QTreeWidgetItem
{
public:
    /// Constructor for a root item
    TreeWidgetItem(QTreeWidget *parent, EltID id);

    /// Constructor of an item below another one
    TreeWidgetItem(QTreeWidgetItem *parent, EltID id);

    /// The element order is based on this string, not on the string displayed
    void setOrderedText(QString orderedText) { _orderedText = orderedText; }

    /// Get or set the hidden attribute of the element (non definitive removal)
    void setEltHidden(bool isHidden) { _hidden = isHidden; this->setHidden(isHidden); }
    bool getEltHidden() { return _hidden; }

    /// Get the element id
    EltID getEltID() { return _id; }

    /// Decrement the sf2 index in the element and all childs recursively
    void decrementSf2();

    /// Decrement the element index (smpl, inst or prst) in the element and all childs recursively
    void decrementElement();

    /// Decrement the element2 (InstSmpl or PrstInst) index in the element
    void decrementElement2();

private:
    // Natural order
    bool operator < (const QTreeWidgetItem &other) const;

    /// Apply a style depending on the element type
    void setStyle();

    EltID _id;
    QString _orderedText;
    bool _hidden;
};

#endif // TREEWIDGETITEM_H

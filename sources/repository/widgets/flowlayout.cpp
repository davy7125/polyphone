/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>
#include "flowlayout.h"

FlowLayout::FlowLayout(QWidget *parent, int hSpacing, int vSpacing) : QLayout(parent),
    _hSpace(hSpacing),
    _vSpace(vSpacing)
{
    // No margins
    setContentsMargins(0, 0, 0, 0);
}

FlowLayout::~FlowLayout()
{
    QLayoutItem *item;
    while ((item = takeAt(0)))
        delete item;
}

void FlowLayout::addItem(QLayoutItem *item)
{
    _itemList.append(item);
}

int FlowLayout::count() const
{
    return _itemList.size();
}

QLayoutItem *FlowLayout::itemAt(int index) const
{
    return _itemList.value(index);
}

QLayoutItem *FlowLayout::takeAt(int index)
{
    if (index >= 0 && index < _itemList.size())
        return _itemList.takeAt(index);
    else
        return nullptr;
}

int FlowLayout::heightForWidth(int width) const
{
    return doLayout(QRect(0, 0, width, 0), true);
}

void FlowLayout::setGeometry(const QRect &rect)
{
    QLayout::setGeometry(rect);
    doLayout(rect, false);
}

QSize FlowLayout::sizeHint() const
{
    return minimumSize();
}

QSize FlowLayout::minimumSize() const
{
    // Return the biggest minimum size of all widgets inside the layout
    QSize size;
    foreach (QLayoutItem *item, _itemList)
        size = size.expandedTo(item->minimumSize());
    return size;
}

int FlowLayout::doLayout(const QRect &rect, bool testOnly) const
{
    int maxWidth = rect.width();
    int currentX = 0;
    int currentY = 0;
    int currentLineHeight = 0;

    QLayoutItem *item;
    foreach (item, _itemList)
    {
        QSize itemSize = item->sizeHint();
        if (currentX + itemSize.width() > maxWidth)
        {
            // A new line is needed for the item
            currentY += currentLineHeight + _vSpace;
            currentLineHeight = 0;
            currentX = 0;
        }

        // Possibly place the item
        if (!testOnly)
            item->setGeometry(QRect(QPoint(currentX, currentY), itemSize));
        currentX += itemSize.width() + _hSpace;
        currentLineHeight = qMax(currentLineHeight, itemSize.height());
    }

    return currentY + currentLineHeight;
}

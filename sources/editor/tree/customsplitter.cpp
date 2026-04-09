/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "customsplitter.h"
#include "contextmanager.h"

CustomSplitter::CustomSplitter(QWidget *parent, QWidget *left, QWidget *right, QString configurationKey) : QSplitter(Qt::Horizontal, parent),
    _configurationKey(configurationKey)
{
    // Initial the widgets
    this->setHandleWidth(0);
    this->addWidget(left);
    this->addWidget(right);
    this->setCollapsible(0, false);
    this->setCollapsible(1, false);

    // Restore the geometry
    this->setSizes(fromVariantList(ContextManager::configuration()->getValue(
                                       ConfManager::SECTION_DISPLAY, _configurationKey,
                                       QVariantList()).toList()));

    // Reaction to a manual move
    connect(this, SIGNAL(splitterMoved(int, int)), this, SLOT(splitterMoved(int, int)));
}

void CustomSplitter::resizeEvent(QResizeEvent * event)
{
    QSplitter::resizeEvent(event);
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, _configurationKey,
                                              toVariantList(this->sizes()));
}

void CustomSplitter::splitterMoved(int pos, int index)
{
    Q_UNUSED(pos)
    Q_UNUSED(index)
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, _configurationKey,
                                              toVariantList(this->sizes()));
}

QVariantList CustomSplitter::toVariantList(QList<int> list)
{
    QVariantList listRet;
    foreach (int elt, list)
        listRet << elt;
    return listRet;
}

QList<int> CustomSplitter::fromVariantList(QVariantList list)
{
    QList<int> listRet;
    foreach (QVariant elt, list)
        listRet << elt.toInt();
    return listRet;
}

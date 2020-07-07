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

#include "modulatorsplitter.h"
#include "modulatoreditor.h"
#include "contextmanager.h"
#include <QMouseEvent>

ModulatorSplitter::ModulatorSplitter(QWidget * parent, QWidget * table, ModulatorEditor * modEditor, bool isPrst) :
    QSplitter(Qt::Vertical, parent),
    _isPrst(isPrst),
    _modEditor(modEditor)
{
    // Initialize the elements
    this->setHandleWidth(0);
    this->addWidget(table);
    this->addWidget(_modEditor);
    this->setCollapsible(0, false);
    this->setCollapsible(1, false);
    this->setStretchFactor(0, 1);
    this->setStretchFactor(1, 0);

    // Reactions to a manual move and to a collapse / expand
    connect(this, SIGNAL(splitterMoved(int, int)), this, SLOT(splitterMoved(int, int)));
    connect(modEditor, SIGNAL(expandedStateChanged(bool)), this, SLOT(expandedStateChanged(bool)));

    // Initial state
    _isExpanded = ContextManager::configuration()->getValue(
                ConfManager::SECTION_DISPLAY,
                _isPrst ? "prst_modulator_section_collapsed" : "inst_modulator_section_collapsed", false).toBool();
    _modEditor->initialize(_isPrst);
}

QSplitterHandle * ModulatorSplitter::createHandle()
{
    return new ModulatorSplitterHandle(this->orientation(), this);
}

void ModulatorSplitter::splitterMoved(int pos, int index)
{
    Q_UNUSED(pos)
    Q_UNUSED(index)
    if (_isExpanded)
        ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY,
                                                  _isPrst ? "prst_modsplitter_sizes" : "inst_modsplitter_sizes",
                                                  toVariantList(this->sizes()));
}

void ModulatorSplitter::expandedStateChanged(bool isExpanded)
{
    // Resize the modulator section
    if (isExpanded)
        this->setSizes(fromVariantList(ContextManager::configuration()->getValue(
                                           ConfManager::SECTION_DISPLAY,
                                           _isPrst ? "prst_modsplitter_sizes" : "inst_modsplitter_sizes",
                                           QVariantList()).toList()));
    else
        this->setSizes(QList<int>() << 10000 << 0);

    // Update states
    _isExpanded = isExpanded;
    (dynamic_cast<ModulatorSplitterHandle*>(this->handle(1)))->setMoveEnabled(_isExpanded);
}

QVariantList ModulatorSplitter::toVariantList(QList<int> list)
{
    QVariantList listRet;
    foreach (int elt, list)
        listRet << elt;
    return listRet;
}

QList<int> ModulatorSplitter::fromVariantList(QVariantList list)
{
    QList<int> listRet;
    foreach (QVariant elt, list)
        listRet << elt.toInt();
    return listRet;
}

void ModulatorSplitterHandle::mouseMoveEvent(QMouseEvent * event)
{
    if (_enabled)
        QSplitterHandle::mouseMoveEvent(event);
    else
        event->ignore();
}

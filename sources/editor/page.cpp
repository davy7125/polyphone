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

#include "page.h"
#include "soundfontmanager.h"
#include "contextmanager.h"
#include "extensionmanager.h"
#include <qmath.h>

SoundfontManager * Page::_sf2 = nullptr;
Synth * Page::_synth = nullptr;

Page::Page(QWidget *parent, QString editingSource) : QWidget(parent),
    _preparingPage(false),
    _editingSource(editingSource)
{
    if (_sf2 == nullptr)
        _sf2 = SoundfontManager::getInstance();
    if (_synth == nullptr)
        _synth = ContextManager::audio()->getSynth();
}

void Page::preparePage(QString editingSource, IdList selectedIds)
{
    if (_preparingPage || editingSource == _editingSource)
        return;
    _preparingPage = true;

    // Possibly update the selected ids
    if (!selectedIds.empty())
        _currentIds = selectedIds;

    // Update the interface according to the selected display action
    updateInterface(editingSource);
    _preparingPage = false;
}

void Page::hideEvent(QHideEvent * event)
{
    // Stop all sounds
    ContextManager::midi()->stopAll();

    QWidget::hideEvent(event);
}

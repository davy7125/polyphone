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

#include "page.h"
#include "soundfontmanager.h"
#include "contextmanager.h"
#include <qmath.h>

SoundfontManager * Page::_sf2 = nullptr;
Synth * Page::_synth = nullptr;

Page::Page(QWidget *parent, TypePage typePage, QString editingSource) : QWidget(parent),
    _preparingPage(false),
    _typePage(typePage),
    _editingSource(editingSource),
    _currentDisplayOption(-1)
{
    if (_sf2 == nullptr)
        _sf2 = SoundfontManager::getInstance();
    if (_synth == nullptr)
        _synth = ContextManager::audio()->getSynth();
}

bool Page::preparePage(QString editingSource, IdList selectedIds)
{
    if (_preparingPage || editingSource == this->getEditingSource())
        return true;
    _preparingPage = true;

    // Possibly update the selected ids
    if (!selectedIds.empty())
        _currentIds = selectedIds;

    // Find a suitable display option
    _displayOptions = getDisplayOptions(_currentIds);
    bool currentDisplayOptionValid = false;
    foreach (DisplayOption displayOption, _displayOptions)
    {
        if (displayOption._id == _currentDisplayOption)
        {
            currentDisplayOptionValid = displayOption._isEnabled;
            break;
        }
    }
    if (!currentDisplayOptionValid)
    {
        // Take the first display option that is available
        _currentDisplayOption = -1;
        foreach (DisplayOption displayOption, _displayOptions)
        {
            if (displayOption._isEnabled)
            {
                _currentDisplayOption = displayOption._id;
                break;
            }
        }
    }

    // Select the suitable display option
    for (int i = 0; i < _displayOptions.count(); i++)
        _displayOptions[i]._isSelected = (_displayOptions[i]._id == _currentDisplayOption);

    // Update the interface according to the selected display action
    bool result = updateInterface(editingSource, _currentIds, _currentDisplayOption);

    _preparingPage = false;
    return result;
}

QList<Page::DisplayOption> Page::getDisplayOptions(IdList selectedIds)
{
    Q_UNUSED(selectedIds)

    // By default, no options
    return QList<DisplayOption>();
}

void Page::setDisplayOption(int displayOption)
{
    _currentDisplayOption = displayOption;
    preparePage("command:display");
}

void Page::showEvent(QShowEvent * event)
{
    // Specific display per page
    this->onShow();

    QWidget::showEvent(event);
}

void Page::hideEvent(QHideEvent * event)
{
    // Stop all sounds
    ContextManager::midi()->stopAll();

    QWidget::hideEvent(event);
}

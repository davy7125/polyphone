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

#include "page.h"
#include "soundfontmanager.h"
#include "contextmanager.h"
#include "editor_old.h"
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
    if (_preparingPage)
        return true;
    _preparingPage = true;

    // Possibly update the selected ids
    if (!selectedIds.empty())
        _selectedIds = selectedIds;

    // Find a suitable display option
    _displayOptions = getDisplayOptions(_selectedIds);
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
    bool result = updateInterface(editingSource, _selectedIds, _currentDisplayOption);

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

QString Page::getIndexName(quint16 iVal, int CC)
{
    QString qStr = "";
    if (CC)
    {
        switch (iVal)
        {
        case 0: qStr = "Bank select"; break;
        case 1: qStr = "Modulation wheel"; break;
        case 2: qStr = "Breath controller"; break;
        case 4: qStr = "Foot pedal"; break;
        case 5: qStr = "Portamento time"; break;
        case 6: qStr = "Data entry"; break;
        case 7: qStr = "Main volume"; break;
        case 8: qStr = "Balance"; break;
        case 10: qStr = "Pan position"; break;
        case 11: qStr = "Expression pedal"; break;
        case 12: qStr = "Effect control 1"; break;
        case 13: qStr = "Effect control 2"; break;
        case 16: qStr = "General purpose 1"; break;
        case 17: qStr = "General purpose 2"; break;
        case 18: qStr = "General purpose 3"; break;
        case 19: qStr = "General purpose 4"; break;
        case 32: qStr = "Bank select (fine)"; break;
        case 33: qStr = "Modulation wheel (fine)"; break;
        case 34: qStr = "Breath controller (fine)"; break;
        case 36: qStr = "Foot pedal (fine)"; break;
        case 37: qStr = "Portamento time (fine)"; break;
        case 38: qStr = "Data entry (fine)"; break;
        case 39: qStr = "Main volume (fine)"; break;
        case 40: qStr = "Balance (fine)"; break;
        case 42: qStr = "Pan position (fine)"; break;
        case 43: qStr = "Expression pedal (fine)"; break;
        case 44: qStr = "Effect control 1 (fine)"; break;
        case 45: qStr = "Effect control 2 (fine)"; break;
        case 48: qStr = "General purpose 1 (fine)"; break;
        case 49: qStr = "General purpose 2 (fine)"; break;
        case 50: qStr = "General purpose 3 (fine)"; break;
        case 51: qStr = "General purpose 4 (fine)"; break;
        case 64: qStr = "Hold pedal (on/off)"; break;
        case 65: qStr = "Portamento (on/off)"; break;
        case 66: qStr = "Sostenuto (on/off)"; break;
        case 67: qStr = "Soft pedal (on/off)"; break;
        case 68: qStr = "Legato pedal (on/off)"; break;
        case 69: qStr = "Hold 2 pedal (on/off)"; break;
        case 70: qStr = "Sound controller 1 (variation)"; break;
        case 71: qStr = "Sound controller 2 (timbre)"; break;
        case 72: qStr = "Sound controller 3 (release time)"; break;
        case 73: qStr = "Sound controller 4 (attack time)"; break;
        case 74: qStr = "Sound controller 5 (brightness)"; break;
        case 75: qStr = "Sound controller 6"; break;
        case 76: qStr = "Sound controller 7"; break;
        case 77: qStr = "Sound controller 8"; break;
        case 78: qStr = "Sound controller 9"; break;
        case 79: qStr = "Sound controller 10"; break;
        case 80: qStr = "General purpose 5"; break;
        case 81: qStr = "General purpose 6"; break;
        case 82: qStr = "General purpose 7"; break;
        case 83: qStr = "General purpose 8"; break;
        case 84: qStr = "Portamento control"; break;
        case 91: qStr = "Effects 1 depth (reverb)"; break;
        case 92: qStr = "Effects 2 depth (tremolo)"; break;
        case 93: qStr = "Effects 3 depth (chorus)"; break;
        case 94: qStr = "Effects 4 depth (celeste)"; break;
        case 95: qStr = "Effects 5 depth (phaser)"; break;
        case 96: qStr = "Data increment"; break;
        case 97: qStr = "Data decrement"; break;
        case 98: qStr = "Non-registered parameter (fine)"; break;
        case 99: qStr = "Non-registered parameter"; break;
        case 100: qStr = "Registered parameter (fine)"; break;
        case 101: qStr = "Registered parameter"; break;
        case 120: qStr = "All sound off"; break;
        case 121: qStr = "All controller off"; break;
        case 122: qStr = "Local keyboard (on/off)"; break;
        case 123: qStr = "All notes off"; break;
        case 124: qStr = "Omni mode off"; break;
        case 125: qStr = "Omni mode on"; break;
        case 126: qStr = "Mono on (poly off)"; break;
        case 127: qStr = "Poly on (mono off)"; break;
        default: qStr = "Undefined"; break;
        }
        qStr = "CC-" + QString("%1").arg(iVal, 3, 10, QChar('0')) + " " + qStr;
    }
    else
    {
        switch (iVal)
        {
        case 0: qStr = "No controller, = 1"; break;
        case 2: qStr = "Note-On velocity"; break;
        case 3: qStr = "Note-On key number"; break;
        case 10: qStr = "Poly pressure"; break;
        case 13: qStr = "Channel pressure"; break;
        case 14: qStr = "Pitch wheel"; break;
        case 16: qStr = "Pitch wheel sensitivity"; break;
        case 127: qStr = "Link"; break;
        }
    }
    return qStr;
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

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

#include "playeroptions.h"
#include <QStringList>

PlayerOptions::PlayerOptions(PlayerOptions * options) :
    _isValid(true)
{
    if (options == nullptr)
    {
        _playerChannel = 0;
        _playerMultipleSelection = false;
        _playerKeySelection = false;
    }
    else
    {
        _playerChannel = options->_playerChannel;
        _playerMultipleSelection = options->_playerMultipleSelection;
        _playerKeySelection = options->_playerKeySelection;
    }
}

bool PlayerOptions::parse(QString text)
{
    // Parse the text
    QStringList configurations = text.split('|');

    // Channel
    if (configurations.length() >= 1 && configurations[0] != "all")
    {
        _playerChannel = configurations[0].toInt(&_isValid);
        if (!_isValid || _playerChannel < 1 || _playerChannel > 16)
        {
            _isValid = false;
            return false;
        }
    }

    // Multiple selection
    if (configurations.length() >= 2)
    {
        if (configurations[1] == "off")
            _playerMultipleSelection = false;
        else if (configurations[1] == "on")
            _playerMultipleSelection = true;
        else
        {
            _isValid = false;
            return false;
        }
    }

    // Preset selection by key
    if (configurations.length() >= 3)
    {
        if  (configurations[2] == "off")
            _playerKeySelection = 0;
        else if (configurations[2] == "on")
            _playerKeySelection = 1;
        else if (configurations[2] == "toggle")
            _playerKeySelection = 2;
        else
        {
            _isValid = false;
            return false;
        }
    }

    _isValid = true;
    return true;
}

QString PlayerOptions::toString()
{
    // Channel
    QString partChannel = "";
    if (_playerChannel == 0)
        partChannel = "all";
    else
        partChannel = QString::number(_playerChannel);

    // Multiple selection
    QString partMultipleSelection = _playerMultipleSelection ? "on" : "off";

    // Preset selection
    QString partPresetSelection = "";
    switch (_playerKeySelection)
    {
    case 1:
        partPresetSelection = "on";
        break;
    case 2:
        partPresetSelection = "toggle";
        break;
    case 0: default:
        partPresetSelection = "off";
        break;
    }

    return partChannel + "|" + partMultipleSelection + "|" + partPresetSelection;
}

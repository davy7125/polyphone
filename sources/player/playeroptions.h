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

#ifndef PLAYEROPTIONS_H
#define PLAYEROPTIONS_H

#include <QString>

class PlayerOptions
{
public:
    PlayerOptions(PlayerOptions * options = nullptr);

    bool parse(QString text);

    /// Valid parameters?
    bool isValid() { return _isValid; }

    /// Player option: channel number (0 is all, otherwise 1 to 16)
    int playerChannel() { return _playerChannel; }
    void setPlayerChannel(int channel) { _playerChannel = channel; }

    /// Player option: multiple selection
    bool playerMultipleSelection() { return _playerMultipleSelection; }
    void setMultipleSelection(bool isOn) { _playerMultipleSelection = isOn; }

    /// Player option: preset selection by key (0 is off, 1 is on, 2 is toggle)
    int playerKeySelection() { return _playerKeySelection; }
    void setKeySelection(int keySelection) { _playerKeySelection = keySelection; }

    /// Get the text describing the configurations
    QString toString();

private:
    bool _isValid;
    int _playerChannel;
    bool _playerMultipleSelection;
    int _playerKeySelection;
};

#endif // PLAYEROPTIONS_H

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

#ifndef CORESETTINGS_H
#define CORESETTINGS_H

#include <functional>
#include <QString>

// Preferences of the application read while processing the soundfonts. They are given by the application when it
// starts (ContextManager::provideCoreSettings), so that core does not depend on the configuration of the editor.
// Without application (tests), the default values are the ones of a new configuration.
class CoreSettings
{
public:
    /// All samples loaded in memory when a soundfont is opened (player mode)
    static bool loadAllSamples;

    /// Type of the default modulator "velocity => filter cutoff" (settings of the sound engine), used by the sfz import
    static std::function<int()> modulatorVelToFilterType;

    /// Number of a key from its name in an sfz file, C4 being the middle C (-1 if the name is not valid), possibly
    /// translated as written by the previous versions
    static std::function<int(QString keyName)> sfzKeyNumber;
};

#endif // CORESETTINGS_H

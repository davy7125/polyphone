/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2026 Davy Triponney                                **
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

#ifndef TOOLMISSINGSAMPLES_PARAMETERS_H
#define TOOLMISSINGSAMPLES_PARAMETERS_H

#include "abstracttoolparameters.h"

class ToolMissingSamples_parameters: public AbstractToolParameters
{
public:
    enum Mode
    {
        MODE_KEYS = 0,       // Keys given in a list
        MODE_MAX_SPACING = 1 // Keys found so that two consecutive root keys are at most a number of semitones apart
    };

    /// Load the configuration from the ini file
    void loadConfiguration() override;

    /// Save the configuration in the ini file
    void saveConfiguration() override;

    Mode getMode() { return _mode; }
    void setMode(Mode mode) { _mode = mode; }

    QList<int> getKeys() { return _keys; }
    void setKeys(QList<int> keys) { _keys = keys; }

    int getMaxSpacing() { return _maxSpacing; }
    void setMaxSpacing(int maxSpacing) { _maxSpacing = maxSpacing; }

private:
    Mode _mode;
    QList<int> _keys;
    int _maxSpacing;
};

#endif // TOOLMISSINGSAMPLES_PARAMETERS_H

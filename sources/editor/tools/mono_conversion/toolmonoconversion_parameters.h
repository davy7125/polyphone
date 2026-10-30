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

#ifndef TOOLMONOCONVERSION_PARAMETERS_H
#define TOOLMONOCONVERSION_PARAMETERS_H

#include "abstracttoolparameters.h"

class ToolMonoConversion_parameters: public AbstractToolParameters
{
public:
    /// Load the configuration from the ini file
    void loadConfiguration() override;

    /// Save the configuration in the ini file
    void saveConfiguration() override;

    /// Mode: 0 = L + R, 1 = (L + R) / 2, 2 = (L + R) / sqrt(2), 3 = left channel, 4 = right channel
    int getMode() { return _mode; }
    void setMode(int mode) { _mode = mode; }

    bool getMergeDivisions() { return _mergeDivisions; }
    void setMergeDivisions(bool mergeDivisions) { _mergeDivisions = mergeDivisions; }

private:
    int _mode;
    bool _mergeDivisions;
};

#endif // TOOLMONOCONVERSION_PARAMETERS_H

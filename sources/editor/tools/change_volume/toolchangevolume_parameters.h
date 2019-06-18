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

#ifndef TOOLCHANGEVOLUME_PARAMETERS_H
#define TOOLCHANGEVOLUME_PARAMETERS_H

#include "abstracttoolparameters.h"

class ToolChangeVolume_parameters: public AbstractToolParameters
{
public:
    /// Load the configuration from the ini file
    void loadConfiguration() override;

    /// Save the configuration in the ini file
    void saveConfiguration() override;

    int getMode() { return _mode; }
    void setMode(int mode) { _mode = mode; }

    double getAddValue() { return _addValue; }
    void setAddValue(double addValue) { _addValue = addValue; }

    double getMultiplyValue() { return _multiplyValue; }
    void setMultiplyValue(double multiplyValue) { _multiplyValue = multiplyValue; }

    double getNormalizeValue() { return _normalizeValue; }
    void setNormalizeValue(double normalizeValue) { _normalizeValue = normalizeValue; }

private:
    int _mode;
    double _addValue;
    double _multiplyValue;
    double _normalizeValue;
};

#endif // TOOLCHANGEVOLUME_PARAMETERS_H

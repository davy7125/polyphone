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

#ifndef TOOLTRIM_PARAMETERS_H
#define TOOLTRIM_PARAMETERS_H

#include "abstracttoolparameters.h"

class ToolTrim_parameters: public AbstractToolParameters
{
public:
    enum EndMode
    {
        END_UNCHANGED = 0,
        END_LOOP = 1,
        END_BLANK = 2
    };

    /// Load the configuration from the ini file
    void loadConfiguration() override;

    /// Save the configuration in the ini file
    void saveConfiguration() override;

    /// Remove the blank at the start
    bool getStart() { return _start; }
    void setStart(bool start) { _start = start; }

    /// Processing of the end of the samples
    EndMode getEndMode() { return _endMode; }
    void setEndMode(EndMode endMode) { _endMode = endMode; }

    /// Blank: detection of the background noise or level in dB
    bool getAutomaticThreshold() { return _automaticThreshold; }
    void setAutomaticThreshold(bool automaticThreshold) { _automaticThreshold = automaticThreshold; }
    double getThreshold() { return _threshold; }
    void setThreshold(double threshold) { _threshold = threshold; }

private:
    bool _start;
    EndMode _endMode;
    bool _automaticThreshold;
    double _threshold;
};

#endif // TOOLTRIM_PARAMETERS_H

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

#ifndef TOOLRELEASE_PARAMETERS_H
#define TOOLRELEASE_PARAMETERS_H

#include "abstracttoolparameters.h"

class ToolRelease_parameters: public AbstractToolParameters
{
public:
    /// Load the configuration from the ini file
    void loadConfiguration() override;

    /// Save the configuration in the ini file
    void saveConfiguration() override;

    double getBaseDuration() { return _baseDuration; }
    void setBaseDuration(double baseDuration) { _baseDuration = baseDuration; }

    double getEvolution() { return _evolution; }
    void setEvolution(double evolution) { _evolution = evolution; }

    double getDetune() { return _detune; }
    void setDetune(double detune) { _detune = detune; }

private:
    double _baseDuration;
    double _evolution;
    double _detune;
};

#endif // TOOLRELEASE_PARAMETERS_H

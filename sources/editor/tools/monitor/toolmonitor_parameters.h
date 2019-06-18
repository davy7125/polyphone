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

#ifndef TOOLMONITOR_PARAMETERS_H
#define TOOLMONITOR_PARAMETERS_H

#include "abstracttoolparameters.h"

class ToolMonitor_parameters: public AbstractToolParameters
{
public:
    /// Load the configuration from the ini file
    void loadConfiguration() override;

    /// Save the configuration in the ini file
    void saveConfiguration() override;
    
    int getInstAttribute() { return _instAttribute; }
    void setInstAttribute(int instAttribute) { _instAttribute = instAttribute; }
    int getPrstAttribute() { return _prstAttribute; }
    void setPrstAttribute(int prstAttribute) { _prstAttribute = prstAttribute; }

    bool getInstLog() { return _instLog; }
    void setInstLog(bool instLog) { _instLog = instLog; }
    bool getPrstLog() { return _prstLog; }
    void setPrstLog(bool prstLog) { _prstLog = prstLog; }
private:
    int _instAttribute, _prstAttribute;
    bool _instLog, _prstLog;
};

#endif // TOOLMONITOR_PARAMETERS_H

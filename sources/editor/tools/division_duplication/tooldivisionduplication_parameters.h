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

#ifndef TOOLDIVISIONDUPLICATION_PARAMETERS_H
#define TOOLDIVISIONDUPLICATION_PARAMETERS_H

#include "abstracttoolparameters.h"
#include <QMap>

class ToolDivisionDuplication_parameters: public AbstractToolParameters
{
public:
    /// Load the configuration from the ini file
    void loadConfiguration() override;

    /// Save the configuration in the ini file
    void saveConfiguration() override;
    
    bool getDuplicKey() { return _duplicKey; }
    void setDuplicKey(bool duplicKey) { _duplicKey = duplicKey; }
    
    bool getDuplicVel() { return _duplicVel; }
    void setDuplicVel(bool duplicVel) { _duplicVel = duplicVel; }
    
    QList<QPair<int, int> > getVelocityRanges() { return _velocityRanges; }
    void setVelocityRanges(QList<QPair<int, int> > velocityRanges) { _velocityRanges = velocityRanges; }

private:
    bool _duplicKey, _duplicVel;
    QList<QPair<int, int> > _velocityRanges;
};

#endif // TOOLDIVISIONDUPLICATION_PARAMETERS_H

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

#ifndef TOOLSOUNDSPATIALIZATION_PARAMETERS_H
#define TOOLSOUNDSPATIALIZATION_PARAMETERS_H

#include "abstracttoolparameters.h"
#include <QMap>

class ToolSoundSpatialization_parameters: public AbstractToolParameters
{
public:
    /// Load the configuration from the ini file
    void loadConfiguration() override;

    /// Save the configuration in the ini file
    void saveConfiguration() override;
    
    int getPattern() { return _pattern; }
    void setPattern(int pattern) { _pattern = pattern; }
    
    int getDivisionNumber() { return _divisionNumber; }
    void setDivisionNumber(int divisionNumber) { _divisionNumber = divisionNumber; }
    
    int getSpreading() { return _spreading; }
    void setSpreading(int spreading) { _spreading = spreading; }
    
    int getFilling() { return _filling; }
    void setFilling(int filling) { _filling = filling; }
    
    int getOffset() { return _offset; }
    void setOffset(int offset) { _offset = offset; }
    
    bool getFlip() { return _flip; }
    void setFlip(bool flip) { _flip = flip; }
    
    bool getFlop() { return _flop; }
    void setFlop(bool flop) { _flop = flop; }
    
    /// Computed value, transitional
    QMap<int, double> & getMapPan() { return _mapPan; }
    void setMapPan(QMap<int, double> mapPan) { _mapPan = mapPan; }

private:
    int _pattern;
    int _divisionNumber;
    int _spreading;
    int _filling;
    int _offset;
    bool _flip;
    bool _flop;

    QMap<int, double> _mapPan;
};

#endif // TOOLSOUNDSPATIALIZATION_PARAMETERS_H

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
    
    int getInstPattern() { return _instPattern; }
    void setInstPattern(int instPattern) { _instPattern = instPattern; }
    int getPrstPattern() { return _prstPattern; }
    void setPrstPattern(int prstPattern) { _prstPattern = prstPattern; }
    
    int getInstDivisionNumber() { return _instDivisionNumber; }
    void setInstDivisionNumber(int instDivisionNumber) { _instDivisionNumber = instDivisionNumber; }
    int getPrstDivisionNumber() { return _prstDivisionNumber; }
    void setPrstDivisionNumber(int prstDivisionNumber) { _prstDivisionNumber = prstDivisionNumber; }
    
    int getInstSpreading() { return _instSpreading; }
    void setInstSpreading(int instSpreading) { _instSpreading = instSpreading; }
    int getPrstSpreading() { return _prstSpreading; }
    void setPrstSpreading(int prstSpreading) { _prstSpreading = prstSpreading; }
    
    int getInstFilling() { return _instFilling; }
    void setInstFilling(int instFilling) { _instFilling = instFilling; }
    int getPrstFilling() { return _prstFilling; }
    void setPrstFilling(int prstFilling) { _prstFilling = prstFilling; }
    
    int getInstOffset() { return _instOffset; }
    void setInstOffset(int instOffset) { _instOffset = instOffset; }
    int getPrstOffset() { return _prstOffset; }
    void setPrstOffset(int prstOffset) { _prstOffset = prstOffset; }
    
    bool getInstFlip() { return _instFlip; }
    void setInstFlip(bool instFlip) { _instFlip = instFlip; }
    bool getPrstFlip() { return _prstFlip; }
    void setPrstFlip(bool prstFlip) { _prstFlip = prstFlip; }
    
    bool getInstFlop() { return _instFlop; }
    void setInstFlop(bool instFlop) { _instFlop = instFlop; }
    bool getPrstFlop() { return _prstFlop; }
    void setPrstFlop(bool prstFlop) { _prstFlop = prstFlop; }
    
    /// Computed value, transitional
    QMap<int, double> & getMapPan() { return _mapPan; }
    void setMapPan(QMap<int, double> mapPan) { _mapPan = mapPan; }

private:
    int _instPattern, _prstPattern;
    int _instDivisionNumber, _prstDivisionNumber;
    int _instSpreading, _prstSpreading;
    int _instFilling, _prstFilling;
    int _instOffset, _prstOffset;
    bool _instFlip, _prstFlip;
    bool _instFlop, _prstFlop;

    QMap<int, double> _mapPan;
};

#endif // TOOLSOUNDSPATIALIZATION_PARAMETERS_H

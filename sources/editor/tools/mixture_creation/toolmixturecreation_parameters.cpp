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

#include "toolmixturecreation_parameters.h"
#include "contextmanager.h"

void ToolMixtureCreation_parameters::loadConfiguration()
{
    // Instrument properties
    _instrumentName = getToolValue("name", "").toString();
    _densityType = getToolValue("density", 1).toInt();
    
    // Sample properties
    _stereoSample = getToolValue("stereo", true).toBool();
    _loopSample = getToolValue("loop", true).toBool();
    
    // Divisions
    _divisions.clear();
    QStringList listDivision = getToolValue("divisions", QList<QVariant>()).toStringList();
    foreach (QString divisionStr, listDivision)
    {
        QStringList listTmp = divisionStr.split(' ');
        if (listTmp.count() >= 4)
        {
            // The two first elements of the list are the range of the division
            DivisionInfo division(listTmp.takeFirst().toInt(), listTmp.takeFirst().toInt());
            
            // Then, ranks are specified by two numbers
            while (listTmp.count() >= 2)
                division.addRank(listTmp.takeFirst().toInt(), listTmp.takeFirst().toInt());

            _divisions << division;
        }
    }
}

void ToolMixtureCreation_parameters::saveConfiguration()
{
    // Instrument properties
    setToolValue("name", _instrumentName);
    setToolValue("density", _densityType);
    
    // Sample properties
    setToolValue("stereo", _stereoSample);
    setToolValue("loop", _loopSample);
    
    // Divisions
    QStringList listDivision;
    foreach (DivisionInfo division, _divisions)
    {
        QStringList divisionStr;
        
        // The two first elements are the key range
        divisionStr << QString::number(division.getMinKey()) << QString::number(division.getMaxKey());
        
        // Then the description of the ranks
        foreach (RankInfo rank, division.getRanks())
            divisionStr << QString::number(rank.getOctave()) << QString::number(rank.getOvertoneType());
        
        listDivision << divisionStr.join(' ');
    }
    setToolValue("divisions", listDivision);
}

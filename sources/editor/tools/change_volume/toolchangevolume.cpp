/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2018 Davy Triponney                                **
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

#include "toolchangevolume.h"
#include "toolchangevolume_gui.h"
#include "toolchangevolume_parameters.h"
#include "soundfontmanager.h"
#include <qmath.h>

ToolChangeVolume::ToolChangeVolume() : AbstractToolIterating(elementSmpl, new ToolChangeVolume_parameters(), new ToolChangeVolume_gui())
{

}

void ToolChangeVolume::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    ToolChangeVolume_parameters * params = (ToolChangeVolume_parameters *)parameters;

    // Sample data
    QByteArray baData = sm->getData(id, champ_sampleDataFull24);

    // Change the volume
    double db = 0;
    switch (params->getMode())
    {
    case 0: // Add dB
        // Compute the factor
        baData = Sound::multiplier(baData, qPow(10, params->getAddValue() / 20.0), 24, db);
        break;
    case 1: // Multiply by a factor
        baData = Sound::multiplier(baData, params->getMultiplyValue(), 24, db);
        break;
    case 2: // Normalize
        baData = Sound::normaliser(baData, params->getNormalizeValue() / 100, 24, db);
        break;
    default:
        // Nothing
        return;
    }

    // Update the sample data
    sm->set(id, champ_sampleDataFull24, baData);
}

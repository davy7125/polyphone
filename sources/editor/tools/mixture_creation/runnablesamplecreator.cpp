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

#include "runnablesamplecreator.h"
#include "toolmixturecreation.h"

RunnableSampleCreator::RunnableSampleCreator(ToolMixtureCreation * tool, EltID idInst, DivisionInfo di, int key, int minKey, bool loop, bool stereo, int side) :
    AbstractRunnableMixer(idInst, loop, stereo, side),
    _tool(tool),
    _di(di),
    _key(key),
    _minKey(minKey)
{}

void RunnableSampleCreator::run()
{
    // Keys of the ranks and minimum attenuation for all ranks
    QList<double> keys;
    foreach (RankInfo ri, _di.getRanks())
        keys << static_cast<double>(_key) + ri.getOffset();
    double attMini = getMinimumAttenuation(keys);

    // Mix of the ranks
    QVector<float> vData(SAMPLE_DURATION * SAMPLE_RATE, 0.0f);
    foreach (double key, keys)
        if (key <= 120)
            addKey(vData, key, attMini, 0, 1);

    EltID idSmpl = createSample(vData, _key);
    _tool->elementProcessed(idSmpl, _key, _minKey, attMini);
}

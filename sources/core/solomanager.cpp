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

#include "solomanager.h"
#include "soundfontmanager.h"

SoloManager::SoloManager(SoundfontManager * sm) : _sm(sm) {}

void SoloManager::setMute(EltID id, bool isMute)
{
    AttributeValue val;
    val.bValue = (isMute ? 1 : 0);
    _sm->set(id, champ_mute, val);
}

bool SoloManager::isMute(EltID id)
{
    return _sm->get(id, champ_mute).bValue > 0;
}

void SoloManager::activateSolo(IdList ids)
{
    if (ids.empty())
        return;

    // Analyze the selection
    bool withGlobalInst = false;
    bool withGlobalPrst = false;
    bool withInstDivision = false;
    bool withPrstDivision = false;
    foreach (EltID id, ids)
    {
        switch (id.typeElement)
        {
        case elementInst:
            withGlobalInst = true;
            break;
        case elementPrst:
            withGlobalPrst = true;
            break;
        case elementInstSmpl:
            withInstDivision = true;
            break;
        case elementPrstInst:
            withPrstDivision = true;
            break;
        default:
            return;
        }
    }

    AttributeValue val;

    // Current id is a division of a preset?
    if (withGlobalPrst)
    {
        IdList prstIds = ids.getSelectedIds(elementPrst);
        if (prstIds.count() != 1)
            return;

        // Browse all divisions of the current preset
        EltID idPrstInst = prstIds[0];
        idPrstInst.typeElement = elementPrstInst;
        val.bValue = false;
        QList<int> instIndexes;
        foreach (int subIndex, _sm->getSiblings(idPrstInst))
        {
            // Unmute them
            idPrstInst.indexElt2 = subIndex;
            _sm->set(idPrstInst, champ_mute, val);

            // Targeted instrument
            int instIndex = _sm->get(idPrstInst, champ_instrument).wValue;
            if (!instIndexes.contains(instIndex))
                instIndexes << instIndex;
        }

        // Browse all divisions of the targeted instruments
        foreach (int instIndex, instIndexes)
        {
            EltID idInstSmpl(elementInstSmpl, prstIds[0].indexSf2, instIndex);
            val.bValue = false;
            foreach (int subIndex, _sm->getSiblings(idInstSmpl))
            {
                // Unmute them
                idInstSmpl.indexElt2 = subIndex;
                _sm->set(idInstSmpl, champ_mute, val);
            }
        }
    }
    else if (withGlobalInst)
    {
        IdList instIds = ids.getSelectedIds(elementInst);
        if (instIds.count() != 1)
            return;

        // Browse all divisions of the current instrument
        EltID idInstSmpl = instIds[0];
        idInstSmpl.typeElement = elementInstSmpl;
        val.bValue = false;
        foreach (int subIndex, _sm->getSiblings(idInstSmpl))
        {
            // Unmute them
            idInstSmpl.indexElt2 = subIndex;
            _sm->set(idInstSmpl, champ_mute, val);
        }
    }
    else if (withPrstDivision)
    {
        // First unmute all divisions of the targeted instruments
        val.bValue = false;
        foreach (EltID id, ids)
        {
            EltID idInst(elementInst, id.indexSf2);
            idInst.indexElt = _sm->get(id, champ_instrument).wValue;
            EltID idInstSmpl(elementInstSmpl, idInst.indexSf2, idInst.indexElt);
            foreach (int subIndex, _sm->getSiblings(idInstSmpl))
            {
                idInstSmpl.indexElt2 = subIndex;
                _sm->set(idInstSmpl, champ_mute, val);
            }
        }

        // Then unmute all preset divisions except current id
        EltID idPrstInst(elementPrstInst, ids[0].indexSf2, ids[0].indexElt);
        foreach (int subIndex, _sm->getSiblings(idPrstInst))
        {
            idPrstInst.indexElt2 = subIndex;
            val.bValue = !ids.contains(idPrstInst);
            _sm->set(idPrstInst, champ_mute, val);
        }
    }
    else if (withInstDivision)
    {
        // Unmute all instrument divisions except current id
        EltID idInstSmpl(elementInstSmpl, ids[0].indexSf2, ids[0].indexElt);
        foreach (int subIndex, _sm->getSiblings(idInstSmpl))
        {
            idInstSmpl.indexElt2 = subIndex;
            val.bValue = !ids.contains(idInstSmpl);
            _sm->set(idInstSmpl, champ_mute, val);
        }
    }
}

void SoloManager::unmuteAll(int sf2Index)
{
    // Unmute all divisions of all instruments
    SoundfontManager * sm = SoundfontManager::getInstance();
    AttributeValue val;
    val.bValue = 0;
    EltID idInst(elementInst, sf2Index);
    foreach (int index, sm->getSiblings(idInst))
    {
        idInst.indexElt = index;
        EltID idInstSmpl(elementInstSmpl, idInst.indexSf2, idInst.indexElt);
        foreach (int subIndex, sm->getSiblings(idInstSmpl))
        {
            idInstSmpl.indexElt2 = subIndex;
            sm->set(idInstSmpl, champ_mute, val);
        }
    }

    // Unmute all divisions of all presets
    EltID idPrst(elementPrst, sf2Index);
    foreach (int index, sm->getSiblings(idPrst))
    {
        idPrst.indexElt = index;
        EltID idPrstInst(elementPrstInst, idPrst.indexSf2, idPrst.indexElt);
        foreach (int subIndex, sm->getSiblings(idPrstInst))
        {
            idPrstInst.indexElt2 = subIndex;
            sm->set(idPrstInst, champ_mute, val);
        }
    }
}

bool SoloManager::isSoloOnSelectionEnabled(int sf2Index)
{
    return _soloOnSelectionEnabled.contains(sf2Index);
}

void SoloManager::setSoloOnSelection(bool enable, int sf2Index)
{
    // Already done?
    if ((enable && _soloOnSelectionEnabled.contains(sf2Index)) ||
            (!enable && !_soloOnSelectionEnabled.contains(sf2Index)))
        return;

    if (enable)
    {
        _soloOnSelectionEnabled << sf2Index;
        if (_currentSelectionPerSoundfont.contains(sf2Index))
            activateSolo(_currentSelectionPerSoundfont[sf2Index]);
    }
    else
    {
        _soloOnSelectionEnabled.removeOne(sf2Index);
        unmuteAll(sf2Index);
    }
}

void SoloManager::selectionChanged(IdList ids)
{
    if (ids.empty())
        return;

    // Index of the sf2
    int sf2Index = ids[0].indexSf2;
    _currentSelectionPerSoundfont[sf2Index] = ids;

    if (_soloOnSelectionEnabled.contains(sf2Index))
        activateSolo(ids);
}

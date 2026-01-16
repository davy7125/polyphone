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

#include "duplicationtool.h"
#include <QVector>
#include "soundfonts.h"
#include "soundfont.h"
#include "instprst.h"
#include "division.h"

DuplicationTool::DuplicationTool(EltID id) :
    _sf2(SoundfontManager::getInstance())
{
    _isInst = (id.typeElement == elementInstSmpl);

    // Browse directly in the model for speed
    Soundfont * soundfont = _sf2->getSoundfonts()->getSoundfont(id.indexSf2);
    InstPrst * elt = _isInst ? soundfont->getInstrument(id.indexElt) : soundfont->getPreset(id.indexElt);

    // Keyrange / velrange specified in the global division
    bool globalKeyRangeSet = elt->getGlobalDivision()->isSet(champ_keyRange);
    RangesType globalKeyRange = globalKeyRangeSet ? elt->getGlobalDivision()->getGen(champ_keyRange).rValue : RangesType();
    bool globalVelRangeSet = elt->getGlobalDivision()->isSet(champ_velRange);
    RangesType globalVelRange = globalVelRangeSet ? elt->getGlobalDivision()->getGen(champ_velRange).rValue : RangesType();

    // Sort all divisions by keyrange and velrange
    RangesType keyRange, velRange;
    foreach (Division * division, elt->getDivisions().values())
    {
        if (division->isHidden())
            continue;

        // Key range
        keyRange.byLo = 0;
        keyRange.byHi = 127;

        if (division->isSet(champ_keyRange))
            keyRange = division->getGen(champ_keyRange).rValue;
        else if (globalKeyRangeSet)
        {
            // Maybe the key range is set in the global division
            keyRange.byLo = globalKeyRange.byLo;
            keyRange.byHi = globalKeyRange.byHi;
        }
        else if (!_isInst)
        {
            // Load the full key range from the linked instrument
            InstPrst * instElt = soundfont->getInstrument(division->getGen(champ_instrument).wValue);
            if (instElt != nullptr)
                keyRange = getInstKeyRange(instElt);
        }

        if (keyRange.byHi < keyRange.byLo)
        {
            quint8 tmp = keyRange.byHi;
            keyRange.byHi = keyRange.byLo;
            keyRange.byLo = tmp;
        }

        // Vel range
        velRange.byLo = 0;
        velRange.byHi = 127;

        if (division->isSet(champ_velRange))
            velRange = division->getGen(champ_velRange).rValue;
        else if (globalVelRangeSet)
        {
            // Maybe the vel range is set in the global division
            velRange.byLo = globalVelRange.byLo;
            velRange.byHi = globalVelRange.byHi;
        }
        else if (!_isInst)
        {
            // Load the full vel range from the linked instrument
            InstPrst * instElt = soundfont->getInstrument(division->getGen(champ_instrument).wValue);
            if (instElt != nullptr)
                velRange = getInstVelRange(instElt);
        }

        if (velRange.byHi < velRange.byLo)
        {
            quint8 tmp = velRange.byHi;
            velRange.byHi = velRange.byLo;
            velRange.byLo = tmp;
        }

        // Store the division
        _elts[QPair<int, int>(keyRange.byLo, keyRange.byHi)][QPair<int, int>(velRange.byLo, velRange.byHi)] << division->getId();
    }
}

RangesType DuplicationTool::getInstKeyRange(InstPrst * inst)
{
    if (_keyRangeByInst.contains(inst->getId().indexElt))
        return _keyRangeByInst[inst->getId().indexElt];

    // Initialization of the range
    RangesType instKeyRange;
    instKeyRange.byHi = 0;
    instKeyRange.byLo = 127;

    // Global key range of the instrument, if set
    bool globalSet = inst->getGlobalDivision()->isSet(champ_keyRange);
    RangesType globalKeyRange = globalSet ? inst->getGlobalDivision()->getGen(champ_keyRange).rValue : RangesType();

    RangesType divRange;
    foreach (Division * instDivision, inst->getDivisions().values())
    {
        if (instDivision->isHidden())
            continue;

        if (instDivision->isSet(champ_keyRange))
            divRange = instDivision->getGen(champ_keyRange).rValue;
        else if (globalSet)
        {
            divRange.byLo = globalKeyRange.byLo;
            divRange.byHi = globalKeyRange.byHi;
        }
        else
        {
            divRange.byLo = 0;
            divRange.byHi = 127;
        }

        if (divRange.byHi < divRange.byLo)
        {
            quint8 tmp = divRange.byHi;
            divRange.byHi = divRange.byLo;
            divRange.byLo = tmp;
        }

        // Merge
        if (divRange.byLo < instKeyRange.byLo)
            instKeyRange.byLo = divRange.byLo;
        if (divRange.byHi > instKeyRange.byHi)
            instKeyRange.byHi = divRange.byHi;
    }

    if (instKeyRange.byHi < instKeyRange.byLo)
    {
        instKeyRange.byLo = 0;
        instKeyRange.byHi = 127;
    }

    _keyRangeByInst[inst->getId().indexElt] = instKeyRange;
    return instKeyRange;
}

RangesType DuplicationTool::getInstVelRange(InstPrst * inst)
{
    if (_velRangeByInst.contains(inst->getId().indexElt))
        return _velRangeByInst[inst->getId().indexElt];

    // Initialization of the range
    RangesType instVelRange;
    instVelRange.byHi = 0;
    instVelRange.byLo = 127;

    // Global vel range of the instrument, if set
    bool globalSet = inst->getGlobalDivision()->isSet(champ_velRange);
    RangesType globalVelRange = globalSet ? inst->getGlobalDivision()->getGen(champ_velRange).rValue : RangesType();

    RangesType divRange;
    foreach (Division * instDivision, inst->getDivisions().values())
    {
        if (instDivision->isSet(champ_velRange))
            divRange = instDivision->getGen(champ_velRange).rValue;
        else if (globalSet)
        {
            divRange.byLo = globalVelRange.byLo;
            divRange.byHi = globalVelRange.byHi;
        }
        else
        {
            divRange.byLo = 0;
            divRange.byHi = 127;
        }

        if (divRange.byHi < divRange.byLo)
        {
            quint8 tmp = divRange.byHi;
            divRange.byHi = divRange.byLo;
            divRange.byLo = tmp;
        }

        // Merge
        if (divRange.byLo < instVelRange.byLo)
            instVelRange.byLo = divRange.byLo;
        if (divRange.byHi > instVelRange.byHi)
            instVelRange.byHi = divRange.byHi;
    }

    if (instVelRange.byHi < instVelRange.byLo)
    {
        instVelRange.byLo = 0;
        instVelRange.byHi = 127;
    }

    _velRangeByInst[inst->getId().indexElt] = instVelRange;
    return instVelRange;
}

void DuplicationTool::duplicateByKey()
{
    QPair<int, int> keyRange, velRange;
    foreach (keyRange, _elts.keys())
    {
        // Remove subElts from the list
        QMap<QPair<int, int>, QList<EltID> > subElts = _elts.take(keyRange);

        // Split each element in it
        foreach (velRange, subElts.keys())
            foreach (EltID id, subElts[velRange])
                duplicateByKey(keyRange, velRange, id);
    }
}

void DuplicationTool::duplicateByKey(QPair<int, int> keyRange, QPair<int, int> velRange, EltID id)
{
    // Keyrange of the division
    int keyLow = keyRange.first;
    int keyHigh = keyRange.second;

    if (keyLow != keyHigh)
    {
        // Change keyrange
        AttributeValue val;
        val.rValue.byLo = keyLow;
        val.rValue.byHi = keyLow;
        _sf2->set(id, champ_keyRange, val);
        _elts[QPair<int, int>(keyLow, keyLow)][velRange] << id;

        // Create identical divisions for other keys
        EltID id2 = id;
        for (int i = keyLow + 1; i <= keyHigh; i++)
        {
            id2.indexElt2 = _sf2->add(id);
            _elts[QPair<int, int>(keyLow, keyLow)][velRange] << id2;

            // Copy gens and mods
            duplicateGenMod(id, id2);

            // Change keyrange
            val.rValue.byLo = i;
            val.rValue.byHi = i;
            _sf2->set(id2, champ_keyRange, val);
        }
    }
    else
        _elts[QPair<int, int>(keyLow, keyLow)][velRange] << id;
}

void DuplicationTool::duplicateByVelocity(QList<QPair<int, int> > velocities)
{
    QPair<int, int> keyRange;
    foreach (keyRange, _elts.keys())
        this->duplicateByVelocity(_elts[keyRange], velocities);
}

bool DuplicationTool::lessThan(QPair<int, int> elt1, QPair<int, int> elt2)
{
    return elt1.first < elt2.first || (elt1.first == elt2.first && elt1.second < elt2.second);
}

void DuplicationTool::duplicateByVelocity(QMap<QPair<int, int>, QList<EltID> > elts, QList<QPair<int, int> > velocities)
{
    // Sort ids and velocities by ascending velocity
    QList<QPair<int, int> > orderedOldVel = elts.keys();
    std::sort(orderedOldVel.begin(), orderedOldVel.end(), lessThan);
    std::sort(velocities.begin(), velocities.end(), lessThan);

    // Change the velocities of the existing ids
    int nbExisting = qMin(orderedOldVel.count(), velocities.count());
    for (int i = 0; i < nbExisting; i++)
    {
        QPair<int, int> oldVel = orderedOldVel[i];
        QPair<int, int> newVel = velocities[i];
        foreach (EltID id, elts[oldVel])
        {
            AttributeValue val;
            val.rValue.byLo = qMin(newVel.first, newVel.second);
            val.rValue.byHi = qMax(newVel.first, newVel.second);
            _sf2->set(id, champ_velRange, val);
        }
    }

    // Duplicate the last divisions for the remaining velocity ranges
    QList<EltID> ids = elts[orderedOldVel.last()];
    for (int i = nbExisting; i < velocities.count(); i++)
    {
        QPair<int, int> newVel = velocities[i];
        foreach (EltID id, ids)
        {
            EltID id2 = id;
            id2.indexElt2 = _sf2->add(id);

            // Copy gens and mods
            duplicateGenMod(id, id2);

            // Change velocity range
            AttributeValue val;
            val.rValue.byLo = qMin(newVel.first, newVel.second);
            val.rValue.byHi = qMax(newVel.first, newVel.second);
            _sf2->set(id2, champ_velRange, val);
        }
    }
}

void DuplicationTool::duplicateGenMod(EltID idFrom, EltID idTo)
{
    // Copy gens
    idFrom.typeElement = _isInst ? elementInstSmplGen : elementPrstInstGen;
    foreach (int champ, _sf2->getSiblings(idFrom))
        _sf2->set(idTo, (AttributeType)champ, _sf2->get(idFrom.parent(), (AttributeType)champ));

    // Copy mods
    idFrom.typeElement = idTo.typeElement = _isInst ? elementInstSmplMod : elementPrstInstMod;
    foreach (int j, _sf2->getSiblings(idFrom))
    {
        idFrom.indexMod = j;
        idTo.indexMod = _sf2->add(idTo);
        _sf2->set(idTo, champ_modAmount, _sf2->get(idFrom, champ_modAmount));
        _sf2->set(idTo, champ_sfModSrcOper, _sf2->get(idFrom, champ_sfModSrcOper));
        _sf2->set(idTo, champ_sfModAmtSrcOper, _sf2->get(idFrom, champ_sfModAmtSrcOper));
        _sf2->set(idTo, champ_sfModDestOper, _sf2->get(idFrom, champ_sfModDestOper));
        _sf2->set(idTo, champ_sfModTransOper, _sf2->get(idFrom, champ_sfModTransOper));
    }
}

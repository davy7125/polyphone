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

#include "duplicationtool.h"
#include <QVector>

DuplicationTool::DuplicationTool(EltID id) :
    _sf2(SoundfontManager::getInstance()),
    _id(id)
{
    _isInst = (id.typeElement == elementInstSmpl);

    // List of elements per keyrange and velrange
    foreach (int i, _sf2->getSiblings(_id))
    {
        EltID id = _id;
        id.indexElt2 = i;
        RangesType keyRange;
        if (_sf2->isSet(id, champ_keyRange))
            keyRange = _sf2->get(id, champ_keyRange).rValue;
        else
        {
            keyRange.byLo = 0;
            keyRange.byHi = 127;
        }

        RangesType velRange;
        if (_sf2->isSet(id, champ_velRange))
            velRange = _sf2->get(id, champ_velRange).rValue;
        else
        {
            velRange.byLo = 0;
            velRange.byHi = 127;
        }

        _elts[QPair<int, int>(keyRange.byLo, keyRange.byHi)][QPair<int, int>(velRange.byLo, velRange.byHi)] << id;
    }
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
    // Keyrange of the sample
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
    qSort(orderedOldVel.begin(), orderedOldVel.end(), lessThan);
    qSort(velocities.begin(), velocities.end(), lessThan);

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

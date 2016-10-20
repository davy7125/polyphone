#include "duplicationtool.h"

DuplicationTool::DuplicationTool(Pile_sf2 * sf2, EltID id) :
    _sf2(sf2),
    _id(id)
{
    _isInst = (id.typeElement == elementInstSmpl);

    // List of elements per keyrange and velrange
    int nbLinkedElements = _sf2->count(_id);
    for (int i = 0; i < nbLinkedElements; i++)
    {
        EltID id = _id;
        id.indexElt2 = i;
        if (!_sf2->get(_id, champ_hidden).bValue)
        {
            rangesType keyRange;
            if (_sf2->isSet(id, champ_keyRange))
               keyRange = _sf2->get(id, champ_keyRange).rValue;
            else
            {
                keyRange.byLo = 0;
                keyRange.byHi = 127;
            }

            rangesType velRange;
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
        Valeur val;
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

void DuplicationTool::duplicateByVelocity(QVector<QPair<int, int> > velocities)
{
    QPair<int, int> keyRange;
    foreach (keyRange, _elts.keys())
        this->duplicateByVelocity(_elts[keyRange], velocities);
}

bool DuplicationTool::lessThan(QPair<int, int> elt1, QPair<int, int> elt2)
{
    return elt1.first < elt2.first || (elt1.first == elt2.first && elt1.second < elt2.second);
}

void DuplicationTool::duplicateByVelocity(QMap<QPair<int, int>, QList<EltID> > elts, QVector<QPair<int, int> > velocities)
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
            Valeur val;
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
            Valeur val;
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
    for (int j = 0; j < _sf2->count(idFrom); j++)
    {
        idFrom.indexMod = j;
        _sf2->set(idTo, (Champ)_sf2->get(idFrom, champ_sfGenOper).wValue,
                  _sf2->get(idFrom, champ_sfGenAmount));
    }

    // Copy mods
    idFrom.typeElement = idTo.typeElement = _isInst ? elementInstSmplMod : elementPrstInstMod;
    for (int j = 0; j < _sf2->count(idFrom); j++)
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

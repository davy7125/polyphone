/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2017 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://polyphone-soundfonts.com                      **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "pageoverviewinst.h"
#include "contextmanager.h"

PageOverviewInst::PageOverviewInst(QWidget * parent) : PageOverview(PAGE_INST, elementInst, parent) {}

QString PageOverviewInst::getTitle()
{
    return trUtf8("Instruments");
}

QStringList PageOverviewInst::getHorizontalHeader()
{
    QStringList hHeader;
    hHeader << trUtf8("Utilisé")
            << trUtf8("Nombre\nd'échantillons")
            << trUtf8("Nombre de\nparamètres")
            << trUtf8("Nombre de\nmodulateurs")
            << trUtf8("Étendue totale\n(note)")
            << trUtf8("Étendue totale\n(vélocité)")
            << trUtf8("Atténuation")
            << trUtf8("Lecture\nen boucle")
            << trUtf8("Chorus")
            << trUtf8("Réverbération");
    return hHeader;
}

void PageOverviewInst::prepare(EltID id)
{
    // List of used instruments
    _usedInst.clear();

    id.typeElement = elementPrst;
    foreach (int i, _sf2->getSiblings(id))
    {
        id.indexElt = i;
        EltID idSubElt = id;
        idSubElt.typeElement = elementPrstInst;
        foreach (int j, _sf2->getSiblings(idSubElt))
        {
            idSubElt.indexElt2 = j;
            _usedInst << _sf2->get(idSubElt, champ_instrument).wValue;
        }
    }
}

// Called for each instrument
QStringList PageOverviewInst::getInformation(EltID id)
{
    QStringList info;
    info << isUsed(id)
         << getSampleNumber(id)
         << getParameterNumber(id)
         << getModulatorNumber(id)
         << getKeyRange(id)
         << getVelocityRange(id)
         << getAttenuation(id)
         << getLoop(id)
         << getChorus(id)
         << getReverb(id);
    return info;
}

QString PageOverviewInst::isUsed(EltID id)
{
    return _usedInst.contains(id.indexElt) ? trUtf8("oui") : trUtf8("non");
}

QString PageOverviewInst::getSampleNumber(EltID id)
{
    id.typeElement = elementInstSmpl;
    return QString::number(_sf2->getSiblings(id).count());
}

QString PageOverviewInst::getParameterNumber(EltID id)
{
    int count = 0;

    // Parameters for the global division
    id.typeElement = elementInstGen;
    count += _sf2->getSiblings(id).count();

    // Parameters for the sample divisions
    id.typeElement = elementInstSmpl;
    foreach (int i, _sf2->getSiblings(id))
    {
        id.indexElt2 = i;
        EltID idGen = id;
        idGen.typeElement = elementInstSmplGen;
        count += _sf2->getSiblings(idGen).count() - 1; // sample gen is ignored
    }

    return QString::number(count);
}

QString PageOverviewInst::getModulatorNumber(EltID id)
{
    int count = 0;

    // Modulators for the global division
    id.typeElement = elementInstMod;
    count += _sf2->getSiblings(id).count();

    // Modulators for the sample divisions
    id.typeElement = elementInstSmpl;
    foreach (int i, _sf2->getSiblings(id))
    {
        id.indexElt2 = i;
        EltID idMod = id;
        idMod.typeElement = elementInstSmplMod;
        count += _sf2->getSiblings(idMod).count();
    }

    return QString::number(count);
}

QString PageOverviewInst::getKeyRange(EltID id)
{
    // Global keyrange
    int globalMin = 0;
    int globalMax = 127;
    if (_sf2->isSet(id, champ_keyRange))
    {
        rangesType range = _sf2->get(id, champ_keyRange).rValue;
        globalMin = range.byLo;
        globalMax = range.byHi;
    }

    // Keyrange per division
    int min = 127;
    int max = 0;
    id.typeElement = elementInstSmpl;
    foreach (int i, _sf2->getSiblings(id))
    {
        id.indexElt2 = i;
        if (_sf2->isSet(id, champ_keyRange))
        {
            rangesType range = _sf2->get(id, champ_keyRange).rValue;
            min = qMin(min, (int)range.byLo);
            max = qMax(max, (int)range.byHi);
        }
        else
        {
            min = qMin(min, globalMin);
            max = qMax(max, globalMax);
        }
    }

    if (min > max)
        return "?";
    else if (min == max)
        return ContextManager::keyName()->getKeyName(min);
    else
        return ContextManager::keyName()->getKeyName(min) + " - " + ContextManager::keyName()->getKeyName(max);
}

QString PageOverviewInst::getVelocityRange(EltID id)
{
    // Global velocity range
    int globalMin = 0;
    int globalMax = 127;
    if (_sf2->isSet(id, champ_velocity))
    {
        rangesType range = _sf2->get(id, champ_velocity).rValue;
        globalMin = range.byLo;
        globalMax = range.byHi;
    }

    // Velocity range per division
    int min = 127;
    int max = 0;
    id.typeElement = elementInstSmpl;
    foreach (int i, _sf2->getSiblings(id))
    {
        id.indexElt2 = i;
        if (_sf2->isSet(id, champ_velocity))
        {
            rangesType range = _sf2->get(id, champ_velocity).rValue;
            min = qMin(min, (int)range.byLo);
            max = qMax(max, (int)range.byHi);
        }
        else
        {
            min = qMin(min, globalMin);
            max = qMax(max, globalMax);
        }
    }

    if (min > max)
        return "?";
    else if (min == max)
        return QString::number(min);
    else
        return QString::number(min) + " - " + QString::number(max);
}

QString PageOverviewInst::getAttenuation(EltID id)
{
    return getRange(id, champ_initialAttenuation);
}

QString PageOverviewInst::getLoop(EltID id)
{
    // Global value
    int globalValue = 0;
    if (_sf2->isSet(id, champ_sampleModes))
        globalValue = _sf2->get(id, champ_sampleModes).wValue;

    // Attenuation per division
    QList<int> modes;
    id.typeElement = elementInstSmpl;
    foreach (int i, _sf2->getSiblings(id))
    {
        id.indexElt2 = i;
        int value = globalValue;
        if (_sf2->isSet(id, champ_sampleModes))
            value = _sf2->get(id, champ_sampleModes).wValue;

        if (!modes.contains(value))
            modes << value;
    }

    if (modes.count() == 0)
        return "?";
    else if (modes.count() == 1)
    {
        switch (modes[0])
        {
        case 0:
            return trUtf8("non");
        case 1:
            return trUtf8("oui");
        case 3:
            return trUtf8("oui, jusqu'à la fin");
        }

        return "?";
    }
    else
        return trUtf8("plusieurs modes", "speaking about loop modes");
}

QString PageOverviewInst::getChorus(EltID id)
{
    return getRange(id, champ_chorusEffectsSend);
}

QString PageOverviewInst::getReverb(EltID id)
{
    return getRange(id, champ_reverbEffectsSend);
}

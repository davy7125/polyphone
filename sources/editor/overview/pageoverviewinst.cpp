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
    hHeader << trUtf8("Used")
            << trUtf8("Sample number")
            << trUtf8("Parameter number")
            << trUtf8("Modulator number")
            << trUtf8("Max key range")
            << trUtf8("Max velocity range")
            << trUtf8("Attenuation")
            << trUtf8("Loop playback")
            << trUtf8("Chorus")
            << trUtf8("Reverb");
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
void PageOverviewInst::getInformation(EltID id, QStringList &info, QStringList &order)
{
    _orderMode = false;
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

    _orderMode = true;
    order << isUsed(id)
          << getSampleNumber(id)
          << getParameterNumber(id)
          << getModulatorNumber(id)
          << getKeyRange(id)
          << getVelocityRange(id)
          << getAttenuation(id)
          << getLoop(id)
          << getChorus(id)
          << getReverb(id);
}

QString PageOverviewInst::isUsed(EltID id)
{
    return _usedInst.contains(id.indexElt) ? trUtf8("yes") : trUtf8("no");
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
        RangesType range = _sf2->get(id, champ_keyRange).rValue;
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
            RangesType range = _sf2->get(id, champ_keyRange).rValue;
            min = qMin(min, (int)range.byLo);
            max = qMax(max, (int)range.byHi);
        }
        else
        {
            min = qMin(min, globalMin);
            max = qMax(max, globalMax);
        }
    }

    QString str = "";
    if (min > max)
        str = "?";
    else
    {
        if (_orderMode)
            str = QString("%1-%2").arg(min, 3, 10, QChar('0')).arg(max, 3, 10, QChar('0'));
        else
        {
            if (min == max)
                str = ContextManager::keyName()->getKeyName(min);
            else
                str = ContextManager::keyName()->getKeyName(min) + " - " + ContextManager::keyName()->getKeyName(max);
        }
    }
    return str;
}

QString PageOverviewInst::getVelocityRange(EltID id)
{
    // Global velocity range
    int globalMin = 0;
    int globalMax = 127;
    if (_sf2->isSet(id, champ_velocity))
    {
        RangesType range = _sf2->get(id, champ_velocity).rValue;
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
            RangesType range = _sf2->get(id, champ_velocity).rValue;
            min = qMin(min, (int)range.byLo);
            max = qMax(max, (int)range.byHi);
        }
        else
        {
            min = qMin(min, globalMin);
            max = qMax(max, globalMax);
        }
    }

    QString str = "";
    if (min > max)
        str = "?";
    else
    {
        if (_orderMode)
            str = QString("%1-%2").arg(min, 3, 10, QChar('0')).arg(max, 3, 10, QChar('0'));
        else
        {
            if (min == max)
                str = QString::number(min);
            else
                str = QString::number(min) + " - " + QString::number(max);
        }
    }
    return str;
}

QString PageOverviewInst::getAttenuation(EltID id)
{
    return getRange(_orderMode, id, champ_initialAttenuation);
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
            return trUtf8("no");
        case 1:
            return trUtf8("yes");
        case 3:
            return trUtf8("yes, to the end");
        }

        return "?";
    }
    else
        return trUtf8("several modes", "speaking about loop modes");
}

QString PageOverviewInst::getChorus(EltID id)
{
    return getRange(_orderMode, id, champ_chorusEffectsSend);
}

QString PageOverviewInst::getReverb(EltID id)
{
    return getRange(_orderMode, id, champ_reverbEffectsSend);
}

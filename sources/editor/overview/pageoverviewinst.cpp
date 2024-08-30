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

#include "pageoverviewinst.h"
#include "contextmanager.h"

PageOverviewInst::PageOverviewInst(QWidget * parent) : PageOverview(elementInst, parent) {}

QString PageOverviewInst::getTitle()
{
    return tr("Instruments");
}

QStringList PageOverviewInst::getHorizontalHeader()
{
    QStringList hHeader;
    hHeader << tr("Used")
            << tr("Sample number")
            << tr("Parameter number")
            << tr("Modulator number")
            << tr("Max key range")
            << tr("Max velocity range")
            << tr("Attenuation")
            << tr("Loop playback")
            << tr("Chorus")
            << tr("Reverb");
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
void PageOverviewInst::getInformation(EltID id, QStringList &info, QStringList &order, QList<int> &status)
{
    // Is used
    int iTmp;
    QString strTmp = isUsed(id, iTmp);
    info << strTmp;
    order << strTmp;
    status << iTmp;

    // Sample number
    strTmp = getSampleNumber(id);
    info << strTmp;
    order << strTmp;
    status << 0;

    // Parameter number
    strTmp = getParameterNumber(id);
    info << strTmp;
    order << strTmp;
    status << 0;

    // Modulator number
    strTmp = getModulatorNumber(id);
    info << strTmp;
    order << strTmp;
    status << 0;

    // Key range
    info << getKeyRange(id, false);
    order << getKeyRange(id, true);
    status << 0;

    // Velocity range
    info << getVelocityRange(id, false);
    order << getVelocityRange(id, true);
    status << 0;

    // Attenuation
    info << getAttenuation(id, false);
    order << getAttenuation(id, true);
    status << 0;

    // Loop
    strTmp = getLoop(id);
    info << strTmp;
    order << strTmp;
    status << 0;

    // Chorus
    info << getChorus(id, false);
    order << getChorus(id, true);
    status << 0;

    // Reverb
    info << getReverb(id, false);
    order << getReverb(id, true);
    status << 0;
}

QString PageOverviewInst::isUsed(EltID id, int &status)
{
    if (_usedInst.contains(id.indexElt))
    {
        status = 0;
        return tr("yes");
    }

    status = 2; // Error
    return tr("no");
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
        count += _sf2->getSiblings(idGen).count();
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

QString PageOverviewInst::getKeyRange(EltID id, bool orderMode)
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
        if (orderMode)
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

QString PageOverviewInst::getVelocityRange(EltID id, bool orderMode)
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
        if (orderMode)
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

QString PageOverviewInst::getAttenuation(EltID id, bool orderMode)
{
    return getRange(orderMode, id, champ_initialAttenuation);
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
            return tr("no");
        case 1:
            return tr("yes");
        case 2:
            return tr("release", "speaking about loop modes");
        case 3:
            return tr("yes, to the end");
        }

        return "?";
    }
    else
        return tr("several modes", "speaking about loop modes");
}

QString PageOverviewInst::getChorus(EltID id, bool orderMode)
{
    return getRange(orderMode, id, champ_chorusEffectsSend);
}

QString PageOverviewInst::getReverb(EltID id, bool orderMode)
{
    return getRange(orderMode, id, champ_reverbEffectsSend);
}

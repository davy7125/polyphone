/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2020 Davy Triponney                                **
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

#include "pageoverviewprst.h"
#include "contextmanager.h"

PageOverviewPrst::PageOverviewPrst(QWidget * parent) : PageOverview(PAGE_PRST, elementPrst, parent) {}

QString PageOverviewPrst::getTitle()
{
    return tr("Presets");
}

QStringList PageOverviewPrst::getHorizontalHeader()
{
    QStringList hHeader;
    hHeader << tr("Bank - Preset")
            << tr("Instrument number")
            << tr("Parameter number")
            << tr("Modulator number")
            << tr("Max key range")
            << tr("Max velocity range")
            << tr("Attenuation")
            << tr("Chorus")
            << tr("Reverb");
    return hHeader;
}

// Called for each preset
void PageOverviewPrst::getInformation(EltID id, QStringList &info, QStringList &order, QList<int> &status)
{
    // Bank and preset numbers
    info << getBankAndPreset(id, false);
    order << getBankAndPreset(id, true);
    status << 0;

    // Instrument number
    QString strTmp = getInstrumentNumber(id);
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

    // Chorus
    info << getChorus(id, false);
    order << getChorus(id, true);
    status << 0;

    // Reverb
    info << getReverb(id, false);
    order << getReverb(id, true);
    status << 0;
}

QString PageOverviewPrst::getBankAndPreset(EltID id, bool orderMode)
{
    if (orderMode)
        return QString("%1-%2")
                .arg(_sf2->get(id, champ_wBank).wValue, 3, 10, QChar('0'))
                .arg(_sf2->get(id, champ_wPreset).wValue, 3, 10, QChar('0'));
    return QString::number(_sf2->get(id, champ_wBank).wValue) + " - " + QString::number(_sf2->get(id, champ_wPreset).wValue);
}

QString PageOverviewPrst::getInstrumentNumber(EltID id)
{
    id.typeElement = elementPrstInst;
    return QString::number(_sf2->getSiblings(id).count());
}

QString PageOverviewPrst::getParameterNumber(EltID id)
{
    int count = 0;

    // Parameters for the global division
    id.typeElement = elementPrstGen;
    count += _sf2->getSiblings(id).count();

    // Parameters for the instrument divisions
    id.typeElement = elementPrstInst;
    foreach (int i, _sf2->getSiblings(id))
    {
        id.indexElt2 = i;
        EltID idGen = id;
        idGen.typeElement = elementPrstInstGen;
        count += _sf2->getSiblings(idGen).count();
    }

    return QString::number(count);
}

QString PageOverviewPrst::getModulatorNumber(EltID id)
{
    int count = 0;

    // Modulators for the global division
    id.typeElement = elementPrstMod;
    count += _sf2->getSiblings(id).count();

    // Modulators for the instrument divisions
    id.typeElement = elementPrstInst;
    foreach (int i, _sf2->getSiblings(id))
    {
        id.indexElt2 = i;
        EltID idMod = id;
        idMod.typeElement = elementPrstInstMod;
        count += _sf2->getSiblings(idMod).count();
    }

    return QString::number(count);
}

QString PageOverviewPrst::getKeyRange(EltID id, bool orderMode)
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
    id.typeElement = elementPrstInst;
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

QString PageOverviewPrst::getVelocityRange(EltID id, bool orderMode)
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
    id.typeElement = elementPrstInst;
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

QString PageOverviewPrst::getAttenuation(EltID id, bool orderMode)
{
    return getRange(orderMode, id, champ_initialAttenuation);
}

QString PageOverviewPrst::getChorus(EltID id, bool orderMode)
{
    return getRange(orderMode, id, champ_chorusEffectsSend);
}

QString PageOverviewPrst::getReverb(EltID id, bool orderMode)
{
    return getRange(orderMode, id, champ_reverbEffectsSend);
}

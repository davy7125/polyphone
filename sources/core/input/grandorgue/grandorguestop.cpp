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

#include "grandorguestop.h"
#include "grandorguepipe.h"
#include "grandorguedatathrough.h"
#include "grandorgueranklink.h"
#include "soundfontmanager.h"

GrandOrgueStop::GrandOrgueStop(QString rootDir, GrandOrgueDataThrough *godt, int id) :
    _rootDir(rootDir),
    _godt(godt),
    _anonymousRank(rootDir, godt, -id), // Stops are also a rank and have a negative id
    _writtenInSf2(false)
{

}

GrandOrgueStop::~GrandOrgueStop()
{
    while (!_rankLinks.empty())
        delete _rankLinks.take(_rankLinks.firstKey());
}

void GrandOrgueStop::readData(QString key, QString value)
{
    if (key.startsWith("rank"))
    {
        if (key.length() < 7)
            return;

        // Extract the number of the rank
        key = key.right(key.length() - 4);
        bool ok = false;
        int number = key.left(3).toInt(&ok);
        if (!ok || number < 0)
            return;

        // Property
        QString property = key.length() > 3 ? key.right(key.length() - 3) : "#";

        // Store data
        if (!_rankLinks.contains(number))
            _rankLinks[number] = new GrandOrgueRankLink();
        _rankLinks[number]->readData(property, value);
    }
    else if (key.startsWith("switch"))
    {
        if (key.length() < 9)
            return;

        // If it's followed by a number
        bool ok = false;
        int number = key.right(key.length() - 6).toInt(&ok);
        if (ok && number >= 0)
        {
            // Add the switch number, written in the value
            ok = false;
            number = value.toInt(&ok);
            if (ok && number >= 0 && !_switches.contains(number))
                _switches << number;
        }
    }
    else
    {
        // Fill the rank
        _anonymousRank.readData(key, value);

        // Also store the properties that could be useful for the stop
        if (!key.startsWith("pipe"))
            _properties[key] = value;
    }
}

bool GrandOrgueStop::isValid()
{
    if (_anonymousRank.isValid())
        return true;
    foreach (GrandOrgueRankLink * link, _rankLinks)
        if (link->getRankId() >= 0)
            return true;
    return false;
}

void GrandOrgueStop::preProcess()
{
    // First key of the stop
    int firstKey = 36;
    if (_properties.contains("firstaccessiblepipelogicalkeynumber"))
    {
        bool ok = false;
        int val1 = _properties["firstaccessiblepipelogicalkeynumber"].toInt(&ok);
        if (ok)
            firstKey = val1 + 35;
    }

    // Pre-process all elements
    _anonymousRank.preProcess();
    foreach (GrandOrgueRankLink * link, _rankLinks)
        link->preProcess(firstKey);
}

void GrandOrgueStop::process(SoundfontManager * sm, int sf2Index, QMap<int, GrandOrgueRank*> &ranks, int presetId)
{
    if (!isValid())
        return;

    // If this stop is already written by a switch, and if this stop is not normally displayed, skip it
    if (_writtenInSf2 && _properties.contains("displayed") && _properties["displayed"].toLower() == "n")
        return;

    EltID idPrst(elementPrst, sf2Index);
    if (presetId == -1)
    {
        // Create a new preset
        idPrst.typeElement = elementPrst;
        idPrst.indexElt = sm->add(idPrst);

        // Name
        sm->set(idPrst, champ_name, _properties.contains("name") ? _properties["name"] : QObject::tr("untitled"));

        // Bank and preset numbers
        int bank, preset;
        _godt->getNextBankPreset(bank, preset);
        AttributeValue value;
        value.wValue = bank;
        sm->set(idPrst, champ_wBank, value);
        value.wValue = preset;
        sm->set(idPrst, champ_wPreset, value);
    }
    else
        idPrst.indexElt = presetId;

    // Possibly associate the pipes that are directly included in the stop
    RangesType defaultRange = getDefaultKeyRange();
    if (_anonymousRank.isValid())
    {
        EltID idInst = _anonymousRank.process(sm, sf2Index, getFirstPipeNumber(),
                                              defaultRange.byLo != 0 ? defaultRange.byLo : 36);

        // Link the instrument to the preset
        EltID idPrstInst(elementPrstInst, idPrst.indexSf2, idPrst.indexElt);
        idPrstInst.indexElt2 = sm->add(idPrstInst);
        AttributeValue val;
        val.wValue = static_cast<quint16>(idInst.indexElt);
        sm->set(idPrstInst, champ_instrument, val);

        // Range
        if (defaultRange.byLo != 0 || defaultRange.byHi != 127)
        {
            AttributeValue val;
            val.rValue = defaultRange;
            sm->set(idPrstInst, champ_keyRange, val);
        }
    }

    // Possibly link to specific ranks
    foreach (GrandOrgueRankLink * link, _rankLinks)
    {
        if (link->getRankId() == -1 || !ranks.contains(link->getRankId()))
            continue;

        // Create an instrument
        RangesType keyRange = link->getKeyRange();
        EltID idInst = ranks[link->getRankId()]->process(sm, idPrst.indexSf2, link->getFirstPipeIndex(),
                                                         keyRange.byLo > 0 ? keyRange.byLo : 36);
        if (idInst.indexElt == -1)
            continue; // Skip the invalid instrument

        // Link it to the preset
        EltID idPrstInst(elementPrstInst, idPrst.indexSf2, idPrst.indexElt);
        idPrstInst.indexElt2 = sm->add(idPrstInst);
        AttributeValue val;
        val.wValue = static_cast<quint16>(idInst.indexElt);
        sm->set(idPrstInst, champ_instrument, val);

        // Keyrange
        if (keyRange.byLo != 0 || keyRange.byHi != 127)
        {
            AttributeValue val;
            val.rValue = keyRange;
            sm->set(idPrstInst, champ_keyRange, val);
        }
    }

    _writtenInSf2 = true;
}

int GrandOrgueStop::getFirstPipeNumber()
{
    int result = 1;
    if (_properties.contains("firstaccessiblepipelogicalpipenumber"))
    {
        bool ok = false;
        result = _properties["firstaccessiblepipelogicalpipenumber"].toInt(&ok);
        if (!ok)
            qDebug() << "couldnt't read stop first pipe number" << _properties["firstaccessiblepipelogicalpipenumber"];
    }
    return result;
}

RangesType GrandOrgueStop::getDefaultKeyRange()
{
    RangesType defaultRange;
    defaultRange.byLo = 0;
    defaultRange.byHi = 127;

    if (_properties.contains("firstaccessiblepipelogicalkeynumber") &&
            _properties.contains("numberofaccessiblepipes"))
    {
        bool ok = false;
        int val1 = _properties["firstaccessiblepipelogicalkeynumber"].toInt(&ok);
        if (!ok)
        {
            qDebug() << "couldnt't read stop first pipe key" << _properties["firstaccessiblepipelogicalkeynumber"];
            return defaultRange;
        }
        int val2 = _properties["numberofaccessiblepipes"].toInt(&ok);
        if (!ok)
        {
            qDebug() << "couldnt't read stop key number" << _properties["numberofaccessiblepipes"];
            return defaultRange;
        }
        val1 += 35;
        val2 += val1 - 1;
        if (val1 >= 0 && val2 >= 0 && val1 < 128 && val2 < 128)
        {
            defaultRange.byLo = static_cast<quint8>(val1);
            defaultRange.byHi = static_cast<quint8>(val2);
        }
    }

    return defaultRange;
}

bool GrandOrgueStop::isTriggeredByThisSwitch(int switchNumber)
{
    return _switches.count() == 1 && _switches[0] == switchNumber;
}

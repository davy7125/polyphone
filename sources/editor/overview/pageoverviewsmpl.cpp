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

#include "pageoverviewsmpl.h"
#include "contextmanager.h"

PageOverviewSmpl::PageOverviewSmpl(QWidget * parent) : PageOverview(PAGE_SMPL, elementSmpl, parent) {}

QString PageOverviewSmpl::getTitle()
{
    return trUtf8("Samples");
}

QStringList PageOverviewSmpl::getHorizontalHeader()
{
    QStringList hHeader;
    hHeader << trUtf8("Used")
            << trUtf8("Total\nduration")
            << trUtf8("Loop\nduration")
            << trUtf8("Root\nkey")
            << trUtf8("Correction")
            << trUtf8("Sample\ntype")
            << trUtf8("Linked\nsample")
            << trUtf8("Sample\nrate");
    return hHeader;
}

void PageOverviewSmpl::prepare(EltID id)
{
    // List of used samples
    _usedSmpl.clear();

    id.typeElement = elementInst;
    foreach (int i, _sf2->getSiblings(id))
    {
        id.indexElt = i;
        EltID idSubElt = id;
        idSubElt.typeElement = elementInstSmpl;
        foreach (int j, _sf2->getSiblings(idSubElt))
        {
            idSubElt.indexElt2 = j;
            _usedSmpl << _sf2->get(idSubElt, champ_sampleID).wValue;
        }
    }
}

// Called for each smpl
void PageOverviewSmpl::getInformation(EltID id, QStringList &info, QStringList &order)
{
    _orderMode = false;
    info << isUsed(id)
         << totalLength(id)
         << loopLength(id)
         << rootKey(id)
         << correction(id)
         << type(id)
         << link(id)
         << sampleRate(id);

    _orderMode = true;
    order << isUsed(id)
          << totalLength(id)
          << loopLength(id)
          << rootKey(id)
          << correction(id)
          << type(id)
          << link(id)
          << sampleRate(id);
}

QString PageOverviewSmpl::isUsed(EltID id)
{
    return _usedSmpl.contains(id.indexElt) ? trUtf8("yes") : trUtf8("no");
}

QString PageOverviewSmpl::totalLength(EltID id)
{
    quint32 length = _sf2->get(id, champ_dwLength).dwValue;
    quint32 sampleRate = _sf2->get(id, champ_dwSampleRate).dwValue;
    return QString::number((double)length / sampleRate, 'f', 3) + trUtf8("s", "unit for seconds");
}

QString PageOverviewSmpl::loopLength(EltID id)
{
    quint32 startLoop = _sf2->get(id, champ_dwStartLoop).dwValue;
    quint32 endLoop = _sf2->get(id, champ_dwEndLoop).dwValue;
    quint32 sampleRate = _sf2->get(id, champ_dwSampleRate).dwValue;
    if (startLoop != endLoop)
        return QString::number((double)(endLoop - startLoop) / sampleRate, 'f', 3) + trUtf8("s", "unit for seconds");
    else
        return "-";
}

QString PageOverviewSmpl::rootKey(EltID id)
{
    unsigned char pitch = _sf2->get(id, champ_byOriginalPitch).bValue;
    return _orderMode ? QString::number(pitch) : ContextManager::keyName()->getKeyName(pitch);
}

QString PageOverviewSmpl::correction(EltID id)
{
    return QString::number(_sf2->get(id, champ_chPitchCorrection).cValue);
}

QString PageOverviewSmpl::type(EltID id)
{
    QString type;

    switch (_sf2->get(id, champ_sfSampleType).sfLinkValue)
    {
    case linkInvalid:
        type = trUtf8("Invalid link");
        break;
    case monoSample: case RomMonoSample:
        type = trUtf8("Mono", "opposite to stereo");
        break;
    case rightSample: case RomRightSample:
        type = trUtf8("Stereo right");
        break;
    case leftSample: case RomLeftSample:
        type = trUtf8("Stereo left");
        break;
    case linkedSample: case RomLinkedSample:
        type = trUtf8("Stereo non defined");
        break;
    }

    return type;
}

QString PageOverviewSmpl::link(EltID id)
{
    SFSampleLink type = _sf2->get(id, champ_sfSampleType).sfLinkValue;

    if (type == monoSample || type == RomMonoSample)
        return "-";
    else
    {
        EltID id2 = id;
        id2.indexElt = _sf2->get(id, champ_wSampleLink).wValue;
        if (_sf2->isValid(id2))
            return _sf2->getQstr(id2, champ_name);
        else
            return trUtf8("invalid");
    }
}

QString PageOverviewSmpl::sampleRate(EltID id)
{
    unsigned int sampleRate = _sf2->get(id, champ_dwSampleRate).dwValue;
    return QString::number(sampleRate) + " " + trUtf8("Hz");
}

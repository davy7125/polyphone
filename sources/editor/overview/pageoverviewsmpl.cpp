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

#include "pageoverviewsmpl.h"
#include "contextmanager.h"
#include "sampleutils.h"

PageOverviewSmpl::PageOverviewSmpl(QWidget * parent) : PageOverview(PAGE_SMPL, elementSmpl, parent) {}

QString PageOverviewSmpl::getTitle()
{
    return tr("Samples");
}

QStringList PageOverviewSmpl::getHorizontalHeader()
{
    QStringList hHeader;
    hHeader << tr("Used")
            << tr("Total\nduration")
            << tr("Loop\nduration")
            << tr("Root\nkey")
            << tr("Correction")
            << tr("Sample\ntype")
            << tr("Linked\nsample")
            << tr("Sample\nrate");
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
void PageOverviewSmpl::getInformation(EltID id, QStringList &info, QStringList &order, QList<int> &status)
{
    findLinkedSample(id);

    // Used sample
    int iTmp;
    QString strTmp = isUsed(id, iTmp);
    info << strTmp;
    order << strTmp;
    status << iTmp;

    // Total duration
    strTmp = totalLength(id);
    info << strTmp;
    order << strTmp;
    status << 0;

    // Loop duration and quality
    strTmp = loopLength(id, iTmp);
    info << strTmp;
    order << QString::number(2 - iTmp) + strTmp;
    status << iTmp;

    // Root key
    unsigned char pitch = rootKey(id, iTmp);
    info << ContextManager::keyName()->getKeyName(pitch);
    order << QString::number(2 - iTmp) + QString::number(1000 + pitch);
    status << iTmp;

    // Correction
    char tuning = correction(id, iTmp);
    info << QString::number(tuning);
    order << QString::number(2 - iTmp) + QString::number(2000 + tuning);
    status << iTmp;

    // Type
    strTmp = type(id, iTmp);
    info << strTmp;
    order << QString::number(2 - iTmp) + strTmp;
    status << iTmp;

    // Link
    strTmp = link(iTmp);
    info << strTmp;
    order << QString::number(2 - iTmp) + strTmp;
    status << iTmp;

    // Sample rate
    strTmp = sampleRate(id);
    info << strTmp;
    order << strTmp;
    status << 0;
}

QString PageOverviewSmpl::isUsed(EltID id, int &status)
{
    if (_usedSmpl.contains(id.indexElt))
    {
        status = 0;
        return tr("yes");
    }

    status = 2; // Error
    return tr("no");
}

QString PageOverviewSmpl::totalLength(EltID id)
{
    quint32 length = _sf2->get(id, champ_dwLength).dwValue;
    quint32 sampleRate = _sf2->get(id, champ_dwSampleRate).dwValue;
    return QString::number((double)length / sampleRate, 'f', 3) + tr("s", "unit for seconds");
}

QString PageOverviewSmpl::loopLength(EltID id, int &status)
{
    status = 0; // Info

    // Possibly no loop
    quint32 startLoop = _sf2->get(id, champ_dwStartLoop).dwValue;
    quint32 endLoop = _sf2->get(id, champ_dwEndLoop).dwValue;
    if (startLoop == endLoop)
        return "-";

    // Length of the loop
    QString result;
    quint32 sampleRate = _sf2->get(id, champ_dwSampleRate).dwValue;
    result = QString::number((double)(endLoop - startLoop) / sampleRate, 'f', 3) + tr("s", "unit for seconds");

    // Quality of the loop
    quint32 length = _sf2->get(id, champ_dwLength).dwValue;
    if (endLoop >= length || startLoop > endLoop)
        result += " - /!\\";
    else
    {
        // Get the sample data
        QByteArray baData = _sf2->getData(id, champ_sampleData16);
        float loopQuality = SampleUtils::computeLoopQuality(baData, startLoop, endLoop);
        if (loopQuality >= 0.15)
            status = 2; // Error
        else if (loopQuality >= 0.05)
            status = 1; // Warning
    }

    return result;
}

unsigned char PageOverviewSmpl::rootKey(EltID id, int &status)
{
    status = 0;
    unsigned char result = _sf2->get(id, champ_byOriginalPitch).bValue;

    // Check the pitch of the possible linked sample
    if (_linkedSampleStatus == 1)
    {
        if (result != _sf2->get(_linkedSampleId, champ_byOriginalPitch).bValue)
            status = 1;
    }

    return result;
}

char PageOverviewSmpl::correction(EltID id, int &status)
{
    status = 0;
    char result = _sf2->get(id, champ_chPitchCorrection).cValue;

    // Check the correction of the possible linked sample
    if (_linkedSampleStatus == 1)
    {
        if (result != _sf2->get(_linkedSampleId, champ_chPitchCorrection).cValue)
            status = 1;
    }

    return result;
}

QString PageOverviewSmpl::type(EltID id, int &status)
{
    QString type;
    status = 0;

    // Linked sample type
    SFSampleLink linkedSampleType = linkInvalid;
    if (_linkedSampleStatus == 1)
        linkedSampleType = _sf2->get(_linkedSampleId, champ_sfSampleType).sfLinkValue;

    switch (_sf2->get(id, champ_sfSampleType).sfLinkValue)
    {
    case linkInvalid:
        type = tr("Invalid link");
        status = 2;
        break;
    case monoSample: case RomMonoSample:
        type = tr("Mono", "opposite to stereo");
        break;
    case rightSample:
        type = tr("Stereo right");
        status = (linkedSampleType == leftSample ? 0 : 1);
        break;
    case RomRightSample:
        type = tr("Stereo right");
        status = (linkedSampleType == RomLeftSample ? 0 : 1);
        break;
    case leftSample:
        type = tr("Stereo left");
        status = (linkedSampleType == rightSample ? 0 : 1);
        break;
    case RomLeftSample:
        type = tr("Stereo left");
        status = (linkedSampleType == RomRightSample ? 0 : 1);
        break;
    case linkedSample:
        type = tr("Stereo non defined");
        status = (linkedSampleType == linkedSample ? 0 : 1);
        break;
    case RomLinkedSample:
        type = tr("Stereo non defined");
        status = (linkedSampleType == RomLinkedSample ? 0 : 1);
        break;
    }

    return type;
}

QString PageOverviewSmpl::link(int &status)
{
    switch (_linkedSampleStatus)
    {
    case 0:
        status = 0;
        return "-";
    case 1:
        status = 0;
        return _sf2->getQstr(_linkedSampleId, champ_name);
    default:
        break;
    }

    status = 2;
    return tr("invalid");
}

QString PageOverviewSmpl::sampleRate(EltID id)
{
    unsigned int sampleRate = _sf2->get(id, champ_dwSampleRate).dwValue;
    return QString::number(sampleRate) + " " + tr("Hz");
}

void PageOverviewSmpl::findLinkedSample(EltID id)
{
    SFSampleLink type = _sf2->get(id, champ_sfSampleType).sfLinkValue;

    if (type == monoSample || type == RomMonoSample)
        _linkedSampleStatus = 0;
    else
    {
        _linkedSampleId = id;
        _linkedSampleId.indexElt = _sf2->get(id, champ_wSampleLink).wValue;
        _linkedSampleStatus = _sf2->isValid(_linkedSampleId) ? 1 : 2;
    }
}

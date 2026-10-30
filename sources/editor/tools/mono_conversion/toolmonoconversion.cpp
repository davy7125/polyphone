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

#include "toolmonoconversion.h"
#include "toolmonoconversion_gui.h"
#include "toolmonoconversion_parameters.h"
#include "soundfontmanager.h"
#include "attribute.h"
#include "utils.h"
#include <QRegularExpression>
#include <qmath.h>

ToolMonoConversion::ToolMonoConversion() : AbstractToolIterating(elementSmpl, new ToolMonoConversion_parameters(), new ToolMonoConversion_gui())
{
    // Instruments may be shared by several stereo samples
    _async = false;
}

bool ToolMonoConversion::getLinkedSample(SoundfontManager * sm, EltID idSmpl, EltID &idLinked)
{
    SFSampleLink type = sm->get(idSmpl, champ_sfSampleType).sfLinkValue;
    if (type == linkInvalid || type == monoSample || type == RomMonoSample)
        return false;

    idLinked = idSmpl;
    idLinked.indexElt = sm->get(idSmpl, champ_wSampleLink).wValue;
    return idLinked.indexElt != idSmpl.indexElt && sm->isValid(idLinked);
}

QString ToolMonoConversion::getBlockingError(IdList ids)
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    EltID idLinked;
    foreach (EltID id, ids)
        if (getLinkedSample(sm, id, idLinked))
            return "";
    return tr("At least one stereo sample must be selected.");
}

void ToolMonoConversion::beforeProcess(IdList ids)
{
    Q_UNUSED(ids)
    _processedSamples.clear();
    _sampleRateErrors.clear();
    _clippedSamples.clear();
    _unmergedInstruments.clear();
}

void ToolMonoConversion::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    ToolMonoConversion_parameters * params = static_cast<ToolMonoConversion_parameters *>(parameters);

    // Sample already processed with its linked sample?
    if (_processedSamples.contains(id))
        return;
    _processedSamples << id;

    // Mono samples are skipped
    EltID idLinked;
    if (!getLinkedSample(sm, id, idLinked))
        return;
    _processedSamples << idLinked;

    // Left and right channels (arbitrary order for linked samples)
    EltID idLeft = id;
    EltID idRight = idLinked;
    SFSampleLink type = sm->get(id, champ_sfSampleType).sfLinkValue;
    if (type == rightSample || type == RomRightSample)
    {
        idLeft = idLinked;
        idRight = id;
    }

    // Sample keeping the mono signal and its properties, the other one being removed
    int mode = params->getMode();
    EltID idKept = (mode == 4 ? idRight : idLeft);
    EltID idRemoved = (mode == 4 ? idLeft : idRight);
    QString name = getMonoName(sm->getQstr(idLeft, champ_name), sm->getQstr(idRight, champ_name));

    // Possibly mix both channels
    if (mode < 3)
    {
        if (sm->get(idLeft, champ_dwSampleRate).dwValue != sm->get(idRight, champ_dwSampleRate).dwValue)
        {
            _sampleRateErrors << sm->getQstr(idLeft, champ_name) + " / " + sm->getQstr(idRight, champ_name);
            return;
        }

        float factor = 1.0f;
        if (mode == 1)
            factor = 0.5f;
        else if (mode == 2)
            factor = static_cast<float>(M_SQRT1_2);

        // The shortest channel is completed with zeros
        QVector<float> vDataLeft = sm->getDataFloat(idLeft);
        QVector<float> vDataRight = sm->getDataFloat(idRight);
        QVector<float> vData(qMax(vDataLeft.size(), vDataRight.size()));
        bool clipping = false;
        for (int i = 0; i < vData.size(); i++)
        {
            vData[i] = factor * ((i < vDataLeft.size() ? vDataLeft[i] : 0.0f) +
                                 (i < vDataRight.size() ? vDataRight[i] : 0.0f));
            if (vData[i] > 1.0f || vData[i] < -1.0f)
                clipping = true;
        }
        if (clipping)
            _clippedSamples << name;

        sm->setData(idLeft, vData);
        if (vData.size() != vDataLeft.size())
        {
            AttributeValue val;
            val.dwValue = vData.size();
            sm->set(idLeft, champ_dwLength, val);
        }
    }

    // The kept sample is now mono
    SFSampleLink typeKept = sm->get(idKept, champ_sfSampleType).sfLinkValue;
    AttributeValue value;
    if (typeKept == RomLeftSample || typeKept == RomRightSample || typeKept == RomLinkedSample)
        value.sfLinkValue = RomMonoSample;
    else
        value.sfLinkValue = monoSample;
    sm->set(idKept, champ_sfSampleType, value);
    value.wValue = 0;
    sm->set(idKept, champ_wSampleLink, value);
    if (sm->getQstr(idKept, champ_name) != name)
        sm->set(idKept, champ_name, name);

    // Use the kept sample in the instruments and remove the other one
    updateInstruments(sm, idLeft, idRight, idKept, params->getMergeDivisions());
    sm->remove(idRemoved);
}

void ToolMonoConversion::updateInstruments(SoundfontManager * sm, EltID idLeft, EltID idRight, EltID idKept, bool merge)
{
    EltID idInst(elementInst, idLeft.indexSf2);
    foreach (int indexInst, sm->getSiblings(idInst))
    {
        idInst.indexElt = indexInst;

        // Divisions using the left or the right channel
        QList<EltID> leftDivisions, rightDivisions;
        EltID idDiv(elementInstSmpl, idLeft.indexSf2, indexInst);
        foreach (int indexDiv, sm->getSiblings(idDiv))
        {
            idDiv.indexElt2 = indexDiv;
            int indexSmpl = sm->get(idDiv, champ_sampleID).wValue;
            if (indexSmpl == idLeft.indexElt)
                leftDivisions << idDiv;
            else if (indexSmpl == idRight.indexElt)
                rightDivisions << idDiv;
        }

        // Possibly merge the left and right divisions having the same key and velocity ranges
        if (merge && !leftDivisions.isEmpty() && !rightDivisions.isEmpty())
        {
            int mergeCount = 0;
            foreach (EltID idDivLeft, leftDivisions)
            {
                RangesType keyRange = getValue(sm, idDivLeft, champ_keyRange).rValue;
                RangesType velRange = getValue(sm, idDivLeft, champ_velRange).rValue;
                for (int i = 0; i < rightDivisions.count(); i++)
                {
                    RangesType keyRangeRight = getValue(sm, rightDivisions[i], champ_keyRange).rValue;
                    RangesType velRangeRight = getValue(sm, rightDivisions[i], champ_velRange).rValue;
                    if (keyRange.byLo == keyRangeRight.byLo && keyRange.byHi == keyRangeRight.byHi &&
                        velRange.byLo == velRangeRight.byLo && velRange.byHi == velRangeRight.byHi)
                    {
                        mergeDivisions(sm, idDivLeft, rightDivisions.takeAt(i));
                        mergeCount++;
                        break;
                    }
                }
            }

            // Divisions that couldn't be merged
            QString instName = sm->getQstr(idInst, champ_name);
            if ((mergeCount < leftDivisions.count() || !rightDivisions.isEmpty()) && !_unmergedInstruments.contains(instName))
                _unmergedInstruments << instName;
        }

        // The remaining divisions use the kept sample
        AttributeValue value;
        value.wValue = static_cast<quint16>(idKept.indexElt);
        foreach (EltID idDivTmp, leftDivisions + rightDivisions)
            if (sm->get(idDivTmp, champ_sampleID).wValue != value.wValue)
                sm->set(idDivTmp, champ_sampleID, value);
    }
}

void ToolMonoConversion::mergeDivisions(SoundfontManager * sm, EltID idDivLeft, EltID idDivRight)
{
    // Averaged parameters
    static const QList<AttributeType> averagedAttributes = {
        champ_initialAttenuation, champ_pan, champ_scaleTuning,
        champ_initialFilterFc, champ_initialFilterQ,
        champ_delayVolEnv, champ_attackVolEnv, champ_holdVolEnv, champ_decayVolEnv, champ_sustainVolEnv, champ_releaseVolEnv,
        champ_keynumToVolEnvHold, champ_keynumToVolEnvDecay,
        champ_delayModEnv, champ_attackModEnv, champ_holdModEnv, champ_decayModEnv, champ_sustainModEnv, champ_releaseModEnv,
        champ_keynumToModEnvHold, champ_keynumToModEnvDecay, champ_modEnvToPitch, champ_modEnvToFilterFc,
        champ_delayModLFO, champ_freqModLFO, champ_modLfoToPitch, champ_modLfoToFilterFc, champ_modLfoToVolume,
        champ_delayVibLFO, champ_freqVibLFO, champ_vibLfoToPitch,
        champ_chorusEffectsSend, champ_reverbEffectsSend
    };
    foreach (AttributeType champ, averagedAttributes)
    {
        if (!sm->isSet(idDivLeft, champ) && !sm->isSet(idDivRight, champ))
            continue;

        AttributeValue value;
        value.shValue = static_cast<qint16>(Utils::round32(0.5 * (getValue(sm, idDivLeft, champ).shValue +
                                                                  getValue(sm, idDivRight, champ).shValue)));
        sm->set(idDivLeft, champ, value);
    }

    // Parameters split into a fine and a coarse part
    mergeSplitAttribute(sm, idDivLeft, idDivRight, champ_startAddrsOffset, champ_startAddrsCoarseOffset, 32768);
    mergeSplitAttribute(sm, idDivLeft, idDivRight, champ_endAddrsOffset, champ_endAddrsCoarseOffset, 32768);
    mergeSplitAttribute(sm, idDivLeft, idDivRight, champ_startloopAddrsOffset, champ_startloopAddrsCoarseOffset, 32768);
    mergeSplitAttribute(sm, idDivLeft, idDivRight, champ_endloopAddrsOffset, champ_endloopAddrsCoarseOffset, 32768);
    mergeSplitAttribute(sm, idDivLeft, idDivRight, champ_fineTune, champ_coarseTune, 100);

    // The other parameters and the modulators are those of the left division
    sm->remove(idDivRight);
}

void ToolMonoConversion::mergeSplitAttribute(SoundfontManager * sm, EltID idDivLeft, EltID idDivRight,
                                             AttributeType champFine, AttributeType champCoarse, int coarseFactor)
{
    if (!sm->isSet(idDivLeft, champFine) && !sm->isSet(idDivLeft, champCoarse) &&
        !sm->isSet(idDivRight, champFine) && !sm->isSet(idDivRight, champCoarse))
        return;

    // Average of the combined values
    double left = getValue(sm, idDivLeft, champFine).shValue + coarseFactor * getValue(sm, idDivLeft, champCoarse).shValue;
    double right = getValue(sm, idDivRight, champFine).shValue + coarseFactor * getValue(sm, idDivRight, champCoarse).shValue;
    int total = Utils::round32(0.5 * (left + right));

    // Split again, a part is set unless it is not defined and stays equal to the inherited value
    AttributeType champs[2] = {champFine, champCoarse};
    int values[2] = {total % coarseFactor, total / coarseFactor};
    for (int i = 0; i < 2; i++)
    {
        if (sm->isSet(idDivLeft, champs[i]) || sm->isSet(idDivRight, champs[i]) ||
            values[i] != getValue(sm, idDivLeft, champs[i]).shValue)
        {
            AttributeValue value;
            value.shValue = static_cast<qint16>(values[i]);
            sm->set(idDivLeft, champs[i], value);
        }
    }
}

AttributeValue ToolMonoConversion::getValue(SoundfontManager * sm, EltID idDiv, AttributeType champ)
{
    if (sm->isSet(idDiv, champ))
        return sm->get(idDiv, champ);

    EltID idInst(elementInst, idDiv.indexSf2, idDiv.indexElt);
    if (sm->isSet(idInst, champ))
        return sm->get(idInst, champ);

    return Attribute::getDefaultStoredValue(champ, false);
}

QString ToolMonoConversion::getMonoName(QString leftName, QString rightName)
{
    static const QRegularExpression reOpening("[\\(\\[\\{]\\s*$");
    static const QRegularExpression reClosing("^\\s*[\\)\\]\\}]");
    static const QRegularExpression reEndSeparators("[\\s_\\-.]+$");
    static const QRegularExpression reStartSeparators("^[\\s_\\-.]+");
    static const QStringList leftMarks = {"left", "l", "right", "r"};
    static const QStringList rightMarks = {"right", "r", "left", "l"};

    // Find the last position where the names differ only by a left / right mark
    for (int pos = leftName.size() - 1; pos >= 0; pos--)
    {
        for (int i = 0; i < leftMarks.size(); i++)
        {
            QString before = leftName.left(pos);
            QString after = leftName.mid(pos + leftMarks[i].size());
            if (leftName.mid(pos, leftMarks[i].size()).compare(leftMarks[i], Qt::CaseInsensitive) != 0 ||
                rightName.mid(pos, rightMarks[i].size()).compare(rightMarks[i], Qt::CaseInsensitive) != 0 ||
                rightName.left(pos) != before || rightName.mid(pos + rightMarks[i].size()) != after)
                continue;

            // Remove the brackets around the mark
            QRegularExpressionMatch matchOpening = reOpening.match(before);
            QRegularExpressionMatch matchClosing = reClosing.match(after);
            if (matchOpening.hasMatch() && matchClosing.hasMatch())
            {
                before.chop(matchOpening.capturedLength());
                after.remove(0, matchClosing.capturedLength());
            }

            // Remove the separators that were around the mark
            if (after.isEmpty())
                before.remove(reEndSeparators);
            else if (before.isEmpty() || reEndSeparators.match(before).hasMatch())
                after.remove(reStartSeparators);

            QString name = (before + after).trimmed();
            if (!name.isEmpty())
                return name;
        }
    }

    return leftName;
}

QString ToolMonoConversion::getWarning()
{
    QString txt;

    if (!_sampleRateErrors.isEmpty())
        txt += tr("The following stereo samples have not been converted because the sample rates of their channels differ:") +
               getHtmlList(_sampleRateErrors);

    if (!_clippedSamples.isEmpty())
        txt += tr("Clipping occurred in the following samples:") + getHtmlList(_clippedSamples);

    if (!_unmergedInstruments.isEmpty())
        txt += tr("In the following instruments, some divisions have not been merged because their key or velocity ranges differ:") +
               getHtmlList(_unmergedInstruments);

    return txt;
}

QString ToolMonoConversion::getHtmlList(QStringList elements)
{
    QString txt = "<ul>";
    foreach (QString element, elements)
        txt += "<li>" + element.toHtmlEscaped() + "</li>";
    return txt + "</ul>";
}

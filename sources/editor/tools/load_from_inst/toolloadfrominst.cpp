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

#include "toolloadfrominst.h"
#include "toolloadfrominst_gui.h"
#include "toolloadfrominst_parameters.h"
#include "soundfontmanager.h"
#include "sampleutils.h"

ToolLoadFromInst::ToolLoadFromInst() : AbstractToolOneStep(new ToolLoadFromInst_parameters(), new ToolLoadFromInst_gui())
{

}

bool ToolLoadFromInst::isCompatible(IdList ids)
{
    return !ids.getSelectedIds(elementSmpl).empty();
}

void ToolLoadFromInst::process(SoundfontManager * sm, IdList ids, AbstractToolParameters *parameters)
{
    if (ids.isEmpty())
        return;

    ToolLoadFromInst_parameters * params = dynamic_cast<ToolLoadFromInst_parameters *>(parameters);

    // Scan all instruments for loading the parameter
    QMap<int, QList<ScanResult> > scan;
    EltID idInst(elementInst, ids[0].indexSf2);
    foreach (int i, sm->getSiblings(idInst))
    {
        // Current instrument
        idInst.indexElt = i;

        // Load the parameter from the global division
        ScanResult globalScanResult(idInst);
        readParameter(sm, params->getParameter(), globalScanResult);

        // Browse all divisions
        EltID idInstSmpl = idInst;
        idInstSmpl.typeElement = elementInstSmpl;
        foreach (int j, sm->getSiblings(idInstSmpl))
        {
            idInstSmpl.indexElt2 = j;

            // Load the parameter from the instrument division
            ScanResult scanResult(idInstSmpl, globalScanResult);
            readParameter(sm, params->getParameter(), scanResult);

            if (!scanResult._cannotBeSet && scanResult._isSet)
            {
                // A parameter is defined at the instrument level for a sample
                int sampleId = sm->get(idInstSmpl, champ_sampleID).wValue;
                scan[sampleId] << scanResult;
            }
        }
    }

    // Load the parameter at the sample level
    _failCount = 0;
    foreach (EltID id, ids)
    {
        if (id.typeElement != elementSmpl)
            continue;

        if (!scan.contains(id.indexElt) || !processParameter(sm, params->getParameter(), id, scan[id.indexElt]))
            _failCount++;
    }
}

void ToolLoadFromInst::readParameter(SoundfontManager *sm, int parameter, ScanResult &scanResult)
{
    switch (parameter)
    {
    case 0: // Root key
        if (sm->isSet(scanResult._id, champ_overridingRootKey))
        {
            scanResult._isSet = true;
            scanResult._value1 = sm->get(scanResult._id, champ_overridingRootKey).wValue;
        }
        break;
    case 1: // Tuning (semi-tones)
        if (sm->isSet(scanResult._id, champ_coarseTune))
        {
            scanResult._isSet = true;
            scanResult._value1 = sm->get(scanResult._id, champ_coarseTune).shValue;
        }

        // If the overriding root key is set, the tuning (semi-tones) cannot be put at the sample level
        if (sm->isSet(scanResult._id, champ_overridingRootKey))
            scanResult._cannotBeSet = true;
        break;
    case 2: // Tuning (cents)
        if (sm->isSet(scanResult._id, champ_fineTune))
        {
            scanResult._isSet = true;
            scanResult._value1 = sm->get(scanResult._id, champ_fineTune).shValue;
        }
        break;
    case 3: // Loop start / loop end
        if (sm->isSet(scanResult._id, champ_startloopAddrsCoarseOffset))
        {
            scanResult._isSet = true;
            scanResult._value1 = sm->get(scanResult._id, champ_startloopAddrsCoarseOffset).shValue;
        }
        if (sm->isSet(scanResult._id, champ_startloopAddrsOffset))
        {
            scanResult._isSet = true;
            scanResult._value2 = sm->get(scanResult._id, champ_startloopAddrsOffset).shValue;
        }
        if (sm->isSet(scanResult._id, champ_endloopAddrsCoarseOffset))
        {
            scanResult._isSet = true;
            scanResult._value3 = sm->get(scanResult._id, champ_endloopAddrsCoarseOffset).shValue;
        }
        if (sm->isSet(scanResult._id, champ_endloopAddrsOffset))
        {
            scanResult._isSet = true;
            scanResult._value4 = sm->get(scanResult._id, champ_endloopAddrsOffset).shValue;
        }
        break;
    default:
        scanResult._isSet = false;
        break;
    }
}

bool ToolLoadFromInst::processParameter(SoundfontManager *sm, int parameter, EltID id, QList<ScanResult> values)
{
    // At least one value must be in the list
    if (values.empty())
        return false;

    // Check that all values are the same
    qint64 value1 = values[0]._value1;
    qint64 value2 = values[0]._value2;
    qint64 value3 = values[0]._value3;
    qint64 value4 = values[0]._value4;
    for (int i = 1; i < values.count(); i++)
        if (values[i]._value1 != value1 || values[i]._value2 != value2 ||
                values[i]._value3 != value3 || values[i]._value4 != value4)
            return false;

    // Apply the change at the sample level and possibly at the instrument level
    AttributeValue val;
    switch (parameter)
    {
    case 0: // Root key
        val.wValue = value1;
        sm->set(id, champ_byOriginalPitch, val);

        // Remove the overriding root key if the global value is not set
        for (int i = 0; i < values.count(); i++)
        {
            EltID idInst = values[i]._id;
            idInst.typeElement = elementInst;
            if (!sm->isSet(idInst, champ_overridingRootKey))
                sm->reset(values[i]._id, champ_overridingRootKey);
        }
        break;
    case 1: { // Tuning (semi-tones)
        // Compute the new root key
        int newSmplRootKey = (int)sm->get(id, champ_byOriginalPitch).bValue + value1;
        if (newSmplRootKey <= 0 || newSmplRootKey > 127)
            return false;

        // Update the sample root key
        val.bValue = newSmplRootKey;
        sm->set(id, champ_byOriginalPitch, val);

        // Coarse tune is now 0 at the instrument level
        val.dwValue = 0;
        for (int i = 0; i < values.count(); i++)
        {
            EltID idInst = values[i]._id;
            idInst.typeElement = elementInst;
            if (!sm->isSet(idInst, champ_coarseTune))
                sm->reset(values[i]._id, champ_coarseTune);
            else
                sm->set(values[i]._id, champ_coarseTune, val);
        }
    } break;
    case 2: { // Tuning (cents)
        // Compute the new correction and root key
        int newSmplCorrection = (int)sm->get(id, champ_chPitchCorrection).cValue + value1;
        if (newSmplCorrection < -199 || newSmplCorrection > 199)
            return false;
        int newSmplRootKey = (int)sm->get(id, champ_byOriginalPitch).bValue;
        if (newSmplCorrection < -99)
        {
            newSmplRootKey--;
            newSmplCorrection += 100;
        }
        else if (newSmplCorrection > 99)
        {
            newSmplRootKey++;
            newSmplCorrection -= 100;
        }
        if (newSmplRootKey <= 0 || newSmplRootKey > 127)
            return false;

        // Update the sample correction and root key
        val.cValue = newSmplCorrection;
        sm->set(id, champ_chPitchCorrection, val);
        val.bValue = newSmplRootKey;
        sm->set(id, champ_byOriginalPitch, val);

        // Fine tune is now 0 at the instrument level
        val.dwValue = 0;
        for (int i = 0; i < values.count(); i++)
        {
            EltID idInst = values[i]._id;
            idInst.typeElement = elementInst;
            if (!sm->isSet(idInst, champ_fineTune))
                sm->reset(values[i]._id, champ_fineTune);
            else
                sm->set(values[i]._id, champ_fineTune, val);
        }
    } break;
    case 3: { // Loop start / loop end
        // Sample length
        quint32 smplLength = sm->get(id, champ_dwLength).dwValue;

        // Compute the new loop start / loop end
        qint64 newSmplLoopStart = (qint64)sm->get(id, champ_dwStartLoop).dwValue + 32768 * value1 + value2;
        qint64 newSmplLoopEnd = (qint64)sm->get(id, champ_dwEndLoop).dwValue + 32768 * value3 + value4;
        if (newSmplLoopStart < 0 || newSmplLoopStart >= smplLength ||
                newSmplLoopEnd < 0 || newSmplLoopEnd >= smplLength ||
                newSmplLoopStart > newSmplLoopEnd)
            return false;

        // Update the sample loop start and loop end
        val.dwValue = newSmplLoopStart;
        sm->set(id, champ_dwStartLoop, val);
        val.dwValue = newSmplLoopEnd;
        sm->set(id, champ_dwEndLoop, val);

        // Offsets for loop start and loop end are now 0 at the instrument level
        val.dwValue = 0;
        for (int i = 0; i < values.count(); i++)
        {
            EltID idInst = values[i]._id;
            idInst.typeElement = elementInst;

            if (!sm->isSet(idInst, champ_startloopAddrsCoarseOffset))
                sm->reset(values[i]._id, champ_startloopAddrsCoarseOffset);
            else
                sm->set(values[i]._id, champ_startloopAddrsCoarseOffset, val);

            if (!sm->isSet(idInst, champ_startloopAddrsOffset))
                sm->reset(values[i]._id, champ_startloopAddrsOffset);
            else
                sm->set(values[i]._id, champ_startloopAddrsOffset, val);

            if (!sm->isSet(idInst, champ_endloopAddrsCoarseOffset))
                sm->reset(values[i]._id, champ_endloopAddrsCoarseOffset);
            else
                sm->set(values[i]._id, champ_endloopAddrsCoarseOffset, val);

            if (!sm->isSet(idInst, champ_endloopAddrsOffset))
                sm->reset(values[i]._id, champ_endloopAddrsOffset);
            else
                sm->set(values[i]._id, champ_endloopAddrsOffset, val);
        }
    } break;
    default:
        return false;
    }

    return true;
}

QString ToolLoadFromInst::getConfirmation()
{
    return _failCount == 0 ? tr("Success") : "";
}

QString ToolLoadFromInst::getWarning()
{
    return _failCount != 0 ? tr("Couldn't configure %n sample(s).", "", _failCount) : "";
}

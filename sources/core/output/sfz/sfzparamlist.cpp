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

#include "sfzparamlist.h"
#include "soundfontmanager.h"

int SfzParamList::_globalLoop = -2;

SfzParamList::SfzParamList(SoundfontManager * sf2, EltID id)
{
    if (id.typeElement == elementPrstInst)
    {
        // Load a preset
        loadAttributes(sf2, EltID(elementPrstInstGen, id.indexSf2, id.indexElt, id.indexElt2));
        loadModulators(sf2, EltID(elementPrstInstMod, id.indexSf2, id.indexElt, id.indexElt2));

        // Take into account the global parameters
        loadAttributes(sf2, EltID(elementPrstGen, id.indexSf2, id.indexElt));
        loadModulators(sf2, EltID(elementPrstMod, id.indexSf2, id.indexElt));
    }
}

SfzParamList::SfzParamList(SoundfontManager * sf2, SfzParamList * paramPrst, EltID idInst)
{
    if (idInst.typeElement == elementInst)
    {
        // Load the global parameters of an instrument
        loadAttributes(sf2, EltID(elementInstGen, idInst.indexSf2, idInst.indexElt));
        loadModulators(sf2, EltID(elementInstMod, idInst.indexSf2, idInst.indexElt));

        idInst.typeElement = elementInst;
        if (sf2->isSet(idInst, champ_sampleModes))
            _globalLoop = sf2->get(idInst, champ_sampleModes).wValue;
        else
        {
            // All divisions have the same loop parameter?
            idInst.typeElement = elementInstSmpl;
            _globalLoop = -2;
            foreach (int i, sf2->getSiblings(idInst))
            {
                idInst.indexElt2 = i;
                int valTmp = 0;
                if (sf2->isSet(idInst, champ_sampleModes))
                    valTmp = sf2->get(idInst, champ_sampleModes).wValue;
                if (_globalLoop == -2)
                    _globalLoop = valTmp;
                else if (_globalLoop != valTmp)
                    _globalLoop = -1;
            }
            idInst.typeElement = elementInst;
        }

        if (_globalLoop >= 0)
        {
            // Adapt the global loop
            if (!_attributes.contains(champ_sampleModes))
            {
                _attributes << champ_sampleModes;
                _attributeValues << _globalLoop;
            }
            else
                _attributeValues[_attributes.indexOf(champ_sampleModes)] = _globalLoop;
        }
        else
        {
            // No loop if not defined
            if (!_attributes.contains(champ_sampleModes))
            {
                _attributes << champ_sampleModes;
                _attributeValues << 0;
                _globalLoop = 0;
            }
            else
                _globalLoop = _attributeValues.at(_attributes.indexOf(champ_sampleModes));
        }
    }
    else if (idInst.typeElement == elementInstSmpl)
    {
        // Load the parameters of a division in an instrument
        loadAttributes(sf2, EltID(elementInstSmplGen, idInst.indexSf2, idInst.indexElt, idInst.indexElt2));
        loadModulators(sf2, EltID(elementInstSmplMod, idInst.indexSf2, idInst.indexElt, idInst.indexElt2));

        // Playback of the linked sample
        EltID idSmpl = idInst;
        idSmpl.typeElement = elementSmpl;
        idSmpl.indexElt = sf2->get(idInst, champ_sampleID).wValue;

        if (!_attributes.contains(champ_overridingRootKey))
        {
            EltID instGlob = idInst;
            instGlob.typeElement = elementInst;
            if (!sf2->isSet(instGlob, champ_overridingRootKey))
            {
                _attributes << champ_overridingRootKey;
                _attributeValues << sf2->get(idSmpl, champ_byOriginalPitch).bValue;
            }
        }

        if (!_attributes.contains(champ_fineTune))
        {
            int fineTuneSample = sf2->get(idSmpl, champ_chPitchCorrection).cValue;
            if (fineTuneSample != 0)
            {
                _attributes << champ_fineTune;
                EltID idInstGlobal = idInst;
                idInstGlobal.typeElement = elementInst;
                _attributeValues << sf2->get(idInstGlobal, champ_fineTune).shValue + fineTuneSample;
            }
        }
        else
        {
            _attributeValues[_attributes.indexOf(champ_fineTune)] +=
                    sf2->get(idSmpl, champ_chPitchCorrection).cValue;
        }

        // Delete the sample mode if the global division specifies the same
        if (_attributes.contains(champ_sampleModes))
        {
            int index = _attributes.indexOf(champ_sampleModes);
            if (_attributeValues.at(index) == _globalLoop)
            {
                _attributes.removeAt(index);
                _attributeValues.removeAt(index);
            }
        }

        // Adapt the offsets
        mix(champ_startAddrsCoarseOffset, champ_startAddrsOffset);
        mix(champ_endAddrsCoarseOffset, champ_endAddrsOffset, sf2->get(idSmpl, champ_dwLength).dwValue);
        mix(champ_startloopAddrsCoarseOffset, champ_startloopAddrsOffset, sf2->get(idSmpl, champ_dwStartLoop).dwValue);
        mix(champ_endloopAddrsCoarseOffset, champ_endloopAddrsOffset, sf2->get(idSmpl, champ_dwEndLoop).dwValue);

        // Delete start and end loop if same value
        if (_attributes.contains(champ_startloopAddrsOffset) && _attributes.contains(champ_endloopAddrsOffset))
        {
            int startLoop = _attributeValues.at(_attributes.indexOf(champ_startloopAddrsOffset));
            int endLoop = _attributeValues.at(_attributes.indexOf(champ_endloopAddrsOffset));
            if (startLoop == endLoop)
            {
                int index = _attributes.indexOf(champ_startloopAddrsOffset);
                _attributes.removeAt(index);
                _attributeValues.removeAt(index);
                index = _attributes.indexOf(champ_endloopAddrsOffset);
                _attributes.removeAt(index);
                _attributeValues.removeAt(index);
            }
        }

        // Remove offsets if the full sample length is used
        if (_attributes.contains(champ_startAddrsOffset) && _attributes.contains(champ_endAddrsOffset))
        {
            quint32 start = _attributeValues.at(_attributes.indexOf(champ_startAddrsOffset));
            quint32 end = _attributeValues.at(_attributes.indexOf(champ_endAddrsOffset));
            if (start == 0 && end == sf2->get(idSmpl, champ_dwLength).dwValue)
            {
                int index = _attributes.indexOf(champ_startAddrsOffset);
                _attributes.removeAt(index);
                _attributeValues.removeAt(index);
                index = _attributes.indexOf(champ_endAddrsOffset);
                _attributes.removeAt(index);
                _attributeValues.removeAt(index);
            }
        }

        // Fixed root key
        if (_attributes.contains(champ_keynum))
        {
            int indexKeynum = _attributes.indexOf(champ_keynum);
            int delta = _attributeValues.at(indexKeynum) -
                    _attributeValues.at(_attributes.indexOf(champ_overridingRootKey));

            // Equivalence by using scaleTuning / rootkey / coarse tune
            if (_attributes.contains(champ_scaleTuning))
                _attributeValues[_attributes.indexOf(champ_scaleTuning)] = 0;
            else
            {
                _attributes << champ_scaleTuning;
                _attributeValues << 0;
            }
            _attributeValues[_attributes.indexOf(champ_overridingRootKey)] =
                    _attributeValues.at(indexKeynum);
            if (_attributes.contains(champ_coarseTune))
                _attributeValues[_attributes.indexOf(champ_coarseTune)] += delta;
            else
            {
                _attributes << champ_coarseTune;
                _attributeValues << delta;
            }

            _attributes.removeAt(indexKeynum);
            _attributeValues.removeAt(indexKeynum);
        }

        // Adapt keynum2...
        adaptKeynum2();

        // Adapt LFO
        adaptLfo(sf2, idInst);

        // Volume attenuation of only defined in the global division
        if (!_attributes.contains(champ_initialAttenuation))
        {
            EltID instGlob = idInst;
            instGlob.typeElement = elementInst;
            if (sf2->isSet(instGlob, champ_initialAttenuation))
            {
                _attributes << champ_initialAttenuation;
                _attributeValues << Attribute::toRealValue(champ_initialAttenuation, false, sf2->get(instGlob, champ_initialAttenuation));
            }
        }
    }

    // Merge the two parameter lists
    if (idInst.typeElement == elementInst)
    {
        for (int i = 0; i < paramPrst->_attributes.size(); i++)
            merge(paramPrst->_attributes.at(i), paramPrst->_attributeValues.at(i));
    }
    else if (idInst.typeElement == elementInstSmpl)
    {
        // Merge only with existing elements
        for (int i = 0; i < paramPrst->_attributes.size(); i++)
            if (_attributes.contains(paramPrst->_attributes.at(i)))
                merge(paramPrst->_attributes.at(i), paramPrst->_attributeValues.at(i));
    }

    for (int i = 0; i < paramPrst->_modulators.size(); i++)
    {
        // Matching modulator at the instrument level?
        int matchingIndex = -1;
        for (int j = 0;  j < this->_modulators.size(); j++)
        {
            if (this->_modulators[j] == paramPrst->_modulators[i])
            {
                matchingIndex = j;
                break;
            }
        }
        if (matchingIndex == -1)
        {
            // Only add the modulator for the global division
            if (idInst.typeElement == elementInst)
                this->_modulators << paramPrst->_modulators[i];
        }
        else
        {
            // Merge the modulators
            this->_modulators[matchingIndex].amount += paramPrst->_modulators[i].amount;
        }
    }

    // Try to remove modLfoToPitch if vib LFO exists
    if (_attributes.contains(champ_modLfoToPitch) && !_attributes.contains(champ_vibLfoToPitch))
    {
        int index = _attributes.indexOf(champ_modLfoToPitch);
        _attributes << champ_vibLfoToPitch;
        _attributeValues << _attributeValues.at(index);

        // Copy the parameters
        if (_attributes.contains(champ_delayModLFO))
        {
            int index2 = _attributes.indexOf(champ_delayModLFO);
            if (_attributes.contains(champ_delayVibLFO))
                _attributeValues[_attributes.indexOf(champ_delayVibLFO)] = _attributeValues[index2];
            else
            {
                _attributes << champ_delayVibLFO;
                _attributeValues << _attributeValues[index2];
            }
        }
        if (_attributes.contains(champ_freqModLFO))
        {
            int index2 = _attributes.indexOf(champ_freqModLFO);
            if (_attributes.contains(champ_freqVibLFO))
                _attributeValues[_attributes.indexOf(champ_freqVibLFO)] = _attributeValues[index2];
            else
            {
                _attributes << champ_freqVibLFO;
                _attributeValues << _attributeValues[index2];
            }
        }

        _attributes.removeAt(index);
        _attributeValues.removeAt(index);

        // Completely delete the LFO mod?
        if (!_attributes.contains(champ_modLfoToVolume) && !_attributes.contains(champ_modLfoToFilterFc))
        {
            if (_attributes.contains(champ_delayModLFO))
            {
                int index2 = _attributes.indexOf(champ_delayModLFO);
                _attributes.removeAt(index2);
                _attributeValues.removeAt(index2);
            }
            if (_attributes.contains(champ_freqModLFO))
            {
                int index2 = _attributes.indexOf(champ_freqModLFO);
                _attributes.removeAt(index2);
                _attributeValues.removeAt(index2);
            }
        }
    }

    // Default values
    if (idInst.typeElement == elementInst)
    {
        // Default attack if not defined
        if (!_attributes.contains(champ_attackVolEnv))
        {
            _attributes << champ_attackVolEnv;
            _attributeValues << Attribute::getDefaultRealValue(champ_attackVolEnv, false);
        }

        // Default frequency if not defined
        if (!_attributes.contains(champ_freqModLFO))
        {
            _attributes << champ_freqModLFO;
            _attributeValues << Attribute::getDefaultRealValue(champ_freqModLFO, false);
        }
        if (!_attributes.contains(champ_freqVibLFO))
        {
            _attributes << champ_freqVibLFO;
            _attributeValues << Attribute::getDefaultRealValue(champ_freqVibLFO, false);
        }

        // Default frequency cutoff if not defined
        if (!_attributes.contains(champ_initialFilterFc))
        {
            _attributes << champ_initialFilterFc;
            _attributeValues << Attribute::getDefaultRealValue(champ_initialFilterFc, false);
        }
    }

    // Limits
    for (int i = 0; i < _attributes.size(); i++)
        _attributeValues[i] = limit(_attributeValues.at(i), _attributes.at(i));

    // Volume correction if modLfoToVolume is on
    double correction = 0;
    idInst.typeElement = elementInst;
    if (_attributes.contains(champ_modLfoToVolume))
        correction = qAbs(_attributeValues.at(_attributes.indexOf(champ_modLfoToVolume)));
    else if (sf2->isSet(idInst, champ_modLfoToVolume))
        correction = qAbs((double)sf2->get(idInst, champ_modLfoToVolume).shValue / 10.);
    if (correction != 0)
    {
        if (_attributes.contains(champ_initialAttenuation))
            _attributeValues[_attributes.indexOf(champ_initialAttenuation)] += correction;
        else
        {
            _attributes << champ_initialAttenuation;
            _attributeValues << correction / DB_SF2_TO_REAL_DB;
        }
    }

    // Order
    prepend(champ_velRange);
    prepend(champ_keynum);
    prepend(champ_overridingRootKey);
    prepend(champ_keyRange);
}

void SfzParamList::adaptKeynum2()
{
    int minKey = 0, maxKey = 127;
    if (_attributes.contains(champ_keyRange))
    {
        double keyRange = _attributeValues.at(_attributes.indexOf(champ_keyRange));
        minKey = qRound(keyRange / 1000);
        maxKey = qRound(keyRange - 1000 * minKey);
    }

    adaptKeynum2(minKey, maxKey, champ_decayModEnv, champ_keynumToModEnvDecay);
    adaptKeynum2(minKey, maxKey, champ_holdModEnv, champ_keynumToModEnvHold);
    adaptKeynum2(minKey, maxKey, champ_decayVolEnv, champ_keynumToVolEnvDecay);
    adaptKeynum2(minKey, maxKey, champ_holdVolEnv, champ_keynumToVolEnvHold);
}

void SfzParamList::adaptKeynum2(int minKey, int maxKey, AttributeType champBase, AttributeType champKeynum)
{
    double valBase = 0.001;
    double valMin, valMax;
    double keyNum;

    int indexKeynum = _attributes.indexOf(champKeynum);
    if (indexKeynum != -1)
    {
        keyNum = _attributeValues.at(indexKeynum);

        int indexValBase = _attributes.indexOf(champBase);
        if (indexValBase != -1)
            valBase = _attributeValues.at(indexValBase);

        valMin = getValKeynum(valBase, minKey, keyNum);
        valMax = getValKeynum(valBase, maxKey, keyNum);
        if (minKey == maxKey)
        {
            if (indexValBase == -1)
            {
                _attributes << champBase;
                _attributeValues << valMin;
            }
            else
                _attributeValues[indexValBase] = valMin;
            _attributeValues.removeAt(indexKeynum);
            _attributes.removeAt(indexKeynum);
        }
        else
        {
            keyNum = 127. * (valMin - valMax) / (double)(minKey - maxKey);
            valBase = (valMax * minKey - valMin * maxKey) / (double)(minKey - maxKey);
            _attributeValues[indexKeynum] = keyNum;
            if (indexValBase == -1)
            {
                _attributes << champBase;
                _attributeValues << valBase;
            }
            else
                _attributeValues[indexValBase] = valBase;
        }
    }
}

void SfzParamList::adaptLfo(SoundfontManager * sf2, EltID idInstSmpl)
{
    // We are in an instrument division
    EltID idInst = idInstSmpl;
    idInst.typeElement = elementInst;

    // Copy the global division parameter values
    if (_attributes.contains(champ_modLfoToPitch) || _attributes.contains(champ_modLfoToFilterFc) ||
            _attributes.contains(champ_modLfoToVolume) || _attributes.contains(champ_delayModLFO) ||
            _attributes.contains(champ_freqModLFO))
    {
        if (!_attributes.contains(champ_delayModLFO) && sf2->isSet(idInst, champ_delayModLFO))
        {
            _attributes << champ_delayModLFO;
            _attributeValues << Attribute::toRealValue(champ_delayModLFO, false, sf2->get(idInst, champ_delayModLFO));
        }
        if (!_attributes.contains(champ_freqModLFO) && sf2->isSet(idInst, champ_freqModLFO))
        {
            _attributes << champ_freqModLFO;
            _attributeValues << Attribute::toRealValue(champ_freqModLFO, false, sf2->get(idInst, champ_freqModLFO));
        }
        if (!_attributes.contains(champ_modLfoToPitch) && sf2->isSet(idInst, champ_modLfoToPitch))
        {
            _attributes << champ_modLfoToPitch;
            _attributeValues << Attribute::toRealValue(champ_modLfoToPitch, false, sf2->get(idInst, champ_modLfoToPitch));
        }
        if (!_attributes.contains(champ_modLfoToFilterFc) && sf2->isSet(idInst, champ_modLfoToFilterFc))
        {
            _attributes << champ_modLfoToFilterFc;
            _attributeValues << Attribute::toRealValue(champ_modLfoToFilterFc, false, sf2->get(idInst, champ_modLfoToFilterFc));
        }
        if (!_attributes.contains(champ_modLfoToVolume) && sf2->isSet(idInst, champ_modLfoToVolume))
        {
            _attributes << champ_modLfoToVolume;
            _attributeValues << Attribute::toRealValue(champ_modLfoToVolume, false, sf2->get(idInst, champ_modLfoToVolume));
        }
    }
    if (_attributes.contains(champ_vibLfoToPitch) || _attributes.contains(champ_delayVibLFO) ||
            _attributes.contains(champ_freqVibLFO))
    {
        if (!_attributes.contains(champ_delayVibLFO) && sf2->isSet(idInst, champ_delayVibLFO))
        {
            _attributes << champ_delayVibLFO;
            _attributeValues << Attribute::toRealValue(champ_delayVibLFO, false, sf2->get(idInst, champ_delayVibLFO));
        }
        if (!_attributes.contains(champ_freqVibLFO) && sf2->isSet(idInst, champ_freqVibLFO))
        {
            _attributes << champ_freqVibLFO;
            _attributeValues << Attribute::toRealValue(champ_freqVibLFO, false, sf2->get(idInst, champ_freqVibLFO));
        }
        if (!_attributes.contains(champ_vibLfoToPitch) && sf2->isSet(idInst, champ_vibLfoToPitch))
        {
            _attributes << champ_vibLfoToPitch;
            _attributeValues << Attribute::toRealValue(champ_vibLfoToPitch, false, sf2->get(idInst, champ_vibLfoToPitch));
        }
    }
}

double SfzParamList::getValKeynum(double valBase, int key, double keynum)
{
    return valBase * qPow(2., static_cast<double>(60 - key) * keynum / 1200.);
}

void SfzParamList::prepend(AttributeType attributeType)
{
    if (_attributes.indexOf(attributeType) != -1)
    {
        int index = _attributes.indexOf(attributeType);
        AttributeType chTmp = _attributes.takeAt(index);
        double value = _attributeValues.takeAt(index);
        _attributes.prepend(chTmp);
        _attributeValues.prepend(value);
    }
}

void SfzParamList::loadAttributes(SoundfontManager * sf2, EltID id)
{
    // Load the parameters, do not override existing ones
    bool isPrst = (id.typeElement == elementPrstInstGen || id.typeElement == elementPrstGen);
    EltID parent = id.parent();

    foreach (int attribute, sf2->getSiblings(id))
    {
        if (!_attributes.contains((AttributeType)attribute))
        {
            if (id.typeElement != elementInstGen || (
                        attribute != champ_startAddrsCoarseOffset &&
                        attribute != champ_startAddrsOffset &&
                        attribute != champ_startloopAddrsCoarseOffset &&
                        attribute != champ_startloopAddrsOffset &&
                        attribute != champ_endAddrsCoarseOffset &&
                        attribute != champ_endAddrsOffset &&
                        attribute != champ_endloopAddrsCoarseOffset &&
                        attribute != champ_endloopAddrsOffset &&
                        attribute != champ_keynum &&
                        attribute != champ_keynumToModEnvDecay &&
                        attribute != champ_keynumToModEnvHold &&
                        attribute != champ_keynumToVolEnvDecay &&
                        attribute != champ_keynumToVolEnvHold))
            {
                _attributes << (AttributeType)attribute;
                _attributeValues << Attribute::toRealValue((AttributeType)attribute, isPrst, sf2->get(parent, (AttributeType)attribute));
            }
        }
    }

    if (id.typeElement == elementInstSmplGen)
    {
        id.typeElement = elementInst;

        // Load the offsets from the global division
        getGlobalValue(sf2, id, champ_startAddrsCoarseOffset);
        getGlobalValue(sf2, id, champ_startAddrsOffset);
        getGlobalValue(sf2, id, champ_startloopAddrsCoarseOffset);
        getGlobalValue(sf2, id, champ_startloopAddrsOffset);
        getGlobalValue(sf2, id, champ_endAddrsCoarseOffset);
        getGlobalValue(sf2, id, champ_endAddrsOffset);
        getGlobalValue(sf2, id, champ_endloopAddrsCoarseOffset);
        getGlobalValue(sf2, id, champ_endloopAddrsOffset);

        // Load the rootkey from the global division
        getGlobalValue(sf2, id, champ_keynum);

        // Load keynum2... from the global division, with the modulated values
        getGlobalValue(sf2, id, champ_keynumToModEnvDecay);
        if (_attributes.contains(champ_keynumToModEnvDecay))
            if (_attributeValues.at(_attributes.indexOf(champ_keynumToModEnvDecay)) != 0)
                getGlobalValue(sf2, id, champ_decayModEnv);
        getGlobalValue(sf2, id, champ_keynumToModEnvHold);
        if (_attributes.contains(champ_keynumToModEnvHold))
            if (_attributeValues.at(_attributes.indexOf(champ_keynumToModEnvHold)) != 0)
                getGlobalValue(sf2, id, champ_holdModEnv);
        getGlobalValue(sf2, id, champ_keynumToVolEnvDecay);
        if (_attributes.contains(champ_keynumToVolEnvDecay))
            if (_attributeValues.at(_attributes.indexOf(champ_keynumToVolEnvDecay)) != 0)
                getGlobalValue(sf2, id, champ_decayVolEnv);
        getGlobalValue(sf2, id, champ_keynumToVolEnvHold);
        if (_attributes.contains(champ_keynumToVolEnvHold))
            if (_attributeValues.at(_attributes.indexOf(champ_keynumToVolEnvHold)) != 0)
                getGlobalValue(sf2, id, champ_holdVolEnv);
    }
}

void SfzParamList::loadModulators(SoundfontManager *sf2, EltID id)
{
    // Load the modulators, do not override existing ones
    foreach (int i, sf2->getSiblings(id))
    {
        id.indexMod = i;
        ModulatorData modTmp;
        modTmp.srcOper = sf2->get(id, champ_sfModSrcOper).sfModValue;
        modTmp.destOper = sf2->get(id, champ_sfModDestOper).wValue;
        modTmp.amount = sf2->get(id, champ_modAmount).shValue;
        modTmp.amtSrcOper = sf2->get(id, champ_sfModAmtSrcOper).sfModValue;
        modTmp.transOper = sf2->get(id, champ_sfModTransOper).sfTransValue;
        modTmp.index = static_cast<quint16>(id.indexMod);

        // Skip modulators that are already in the list
        bool inList = false;
        foreach (ModulatorData modData, _modulators)
        {
            if (modData == modTmp)
            {
                inList = true;
                break;
            }
        }
        if (inList)
            continue;

        _modulators << modTmp;
    }
}

void SfzParamList::getGlobalValue(SoundfontManager * sf2, EltID id, AttributeType champ)
{
    // Load a value from the global division
    if (!_attributes.contains(champ) && sf2->isSet(id, champ))
    {
        _attributes << champ;
        _attributeValues << Attribute::toRealValue(champ, false, sf2->get(id, champ));
    }
}

void SfzParamList::mix(AttributeType champCoarse, AttributeType champFine, int addValue)
{
    if (_attributes.contains(champCoarse))
    {
        int indexCoarse = _attributes.indexOf(champCoarse);
        if (_attributes.contains(champFine))
        {
            _attributeValues[_attributes.indexOf(champFine)] += _attributeValues[indexCoarse];
            _attributes.removeAt(indexCoarse);
            _attributeValues.removeAt(indexCoarse);
        }
        else
            _attributes[indexCoarse] = champFine;
    }

    if (_attributes.contains(champFine))
        _attributeValues[_attributes.indexOf(champFine)] += addValue;
    else
    {
        _attributes << champFine;
        _attributeValues << addValue;
    }
}

void SfzParamList::merge(AttributeType attributeType, double value)
{
    int index = _attributes.indexOf(attributeType);
    if (index == -1)
    {
        index = _attributes.size();
        _attributes.append(attributeType);
        _attributeValues.append(Attribute::getDefaultRealValue(attributeType, false));
    }

    switch (attributeType)
    {
    case champ_fineTune: case champ_coarseTune: case champ_initialAttenuation: case champ_startAddrsOffset:
    case champ_endAddrsOffset: case champ_startloopAddrsOffset: case champ_endloopAddrsOffset:
    case champ_pan: case champ_scaleTuning: case champ_initialFilterQ: case champ_reverbEffectsSend:
    case champ_chorusEffectsSend: case champ_keynumToVolEnvHold: case champ_keynumToVolEnvDecay:
    case champ_sustainVolEnv: case champ_keynumToModEnvHold: case champ_keynumToModEnvDecay:
    case champ_sustainModEnv: case champ_modEnvToPitch: case champ_modEnvToFilterFc: case champ_modLfoToPitch:
    case champ_modLfoToFilterFc: case champ_vibLfoToPitch: case champ_modLfoToVolume:
        _attributeValues[index] += value;
        break;
    case champ_keynum: case champ_overridingRootKey: case champ_velocity: case champ_exclusiveClass:
    case champ_sampleModes:
        _attributeValues[index] = value;
        break;
    case champ_initialFilterFc: case champ_delayVolEnv: case champ_attackVolEnv: case champ_holdVolEnv:
    case champ_decayVolEnv: case champ_releaseVolEnv: case champ_delayModEnv: case champ_attackModEnv:
    case champ_holdModEnv: case champ_decayModEnv: case champ_releaseModEnv: case champ_delayModLFO:
    case champ_freqModLFO: case champ_delayVibLFO: case champ_freqVibLFO:
        _attributeValues[index] *= value;
        break;
    case champ_keyRange: case champ_velRange:{
        int minNew = qRound(value / 1000.);
        int maxNew = qRound(value - 1000. * minNew);
        int minOld = qRound(_attributeValues.at(index) / 1000.);
        int maxOld = qRound(_attributeValues.at(index) - 1000. * minOld);
        if (minNew > maxNew)
        {
            int tmp = maxNew;
            maxNew = minNew;
            minNew = tmp;
        }
        if (minOld > maxOld)
        {
            int tmp = maxOld;
            maxOld = minOld;
            minOld = tmp;
        }
        if (maxNew < minOld || maxOld < minNew)
        {
            minNew = 0;
            maxNew = 0;
        }
        else
        {
            minNew = qMax(minOld, minNew);
            maxNew = qMin(maxOld, maxNew);
        }
        _attributeValues[index] = 1000. * minNew + maxNew;
    }break;
    default:
        break;
    }
}

double SfzParamList::limit(double val, AttributeType champ)
{
    double min = 0;
    double max = 0;

    switch (champ)
    {
    case champ_fineTune:
        min = -99; max = 99;
        break;
    case champ_coarseTune:
        min = -120; max = 120;
        break;
    case champ_pan:
        min = -50; max = 50;
        break;
    case champ_initialAttenuation: case champ_sustainVolEnv: case champ_sustainModEnv:
        min = 0; max = 144;
        break;
    case champ_reverbEffectsSend: case champ_chorusEffectsSend:
        min = 0; max = 100;
        break;
    case champ_scaleTuning:
        min = 0; max = 1200;
        break;
    case champ_initialFilterFc:
        min = 18; max = 19914;
        break;
    case champ_initialFilterQ:
        min = 0; max = 96;
        break;
    case champ_delayVolEnv: case champ_holdVolEnv: case champ_holdModEnv:
    case champ_delayModEnv:
        min = 0.001; max = 18;
        break;
    case champ_delayModLFO: case champ_delayVibLFO:
        min = 0.001; max = 20;
        break;
    case champ_freqModLFO: case champ_freqVibLFO:
        min = 0.001; max = 100;
        break;
    case champ_attackVolEnv: case champ_decayVolEnv: case champ_releaseVolEnv: case champ_releaseModEnv:
    case champ_attackModEnv: case champ_decayModEnv:
        min = 0.001; max = 101.6;
        break;
    case champ_keynumToVolEnvHold: case champ_keynumToVolEnvDecay:
    case champ_keynumToModEnvHold: case champ_keynumToModEnvDecay:
        min = -1200; max = 1200;
        break;
    case champ_modLfoToVolume:
        min = -96; max = 96;
        break;
    case champ_modEnvToPitch: case champ_modEnvToFilterFc: case champ_modLfoToPitch:
    case champ_modLfoToFilterFc: case champ_vibLfoToPitch:
        min = -12000; max = 12000;
        break;
    case champ_keynum: case champ_overridingRootKey: case champ_velocity: case champ_exclusiveClass:
        min = 0; max = 127;
        break;
    default:
        break;
    }

    if (min != max)
    {
        if (val < min)
            return min;
        else if (val > max)
            return max;
    }

    return val;
}

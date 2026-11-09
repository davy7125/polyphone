/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2026 Davy Triponney                                **
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


#include "sfzparametergroup.h"
#include <cmath>
#include "synthconstants.h"
#include "textconversions.h"
using namespace TextConversions;

typedef ISoundfontBuilder::Id Id;
static const double PI = 3.14159265358979323846; // Same value as M_PI

void SfzParameterGroup::moveOpcodesInGlobal(SfzParameterRegion &globalZone)
{
    _paramGlobaux.mergeIfNotDefined(globalZone);
}

void SfzParameterGroup::moveKeynumInSamples(SfzParameter::OpCode opCodeKeynum, SfzParameter::OpCode opCodeBase)
{
    if (_paramGlobaux.isDefined(opCodeKeynum))
    {
        double valueKeynum = _paramGlobaux.getDoubleValue(opCodeKeynum);
        double valueBase = 0.001;
        if (_paramGlobaux.isDefined(opCodeBase))
            valueBase = _paramGlobaux.getDoubleValue(opCodeBase);
        for (size_t i = 0; i < _regionList.size(); i++)
        {
            if (!_regionList.at(i).isDefined(opCodeKeynum))
                _regionList[i] << SfzParameter(opCodeKeynum, valueKeynum);
            if (!_regionList.at(i).isDefined(opCodeBase))
                _regionList[i] << SfzParameter(opCodeBase, valueBase);
        }

        _paramGlobaux.removeOpCode(opCodeKeynum);
    }
}

void SfzParameterGroup::moveOpcodeInSamples(SfzParameter::OpCode opcode, ValueType type)
{
    switch (type)
    {
    case VALUE_DOUBLE:
        if (_paramGlobaux.isDefined(opcode))
        {
            double value = _paramGlobaux.getDoubleValue(opcode);
            for (size_t i = 0; i < _regionList.size(); i++)
                if (!_regionList.at(i).isDefined(opcode))
                    _regionList[i] << SfzParameter(opcode, value);
        }
        break;
    case VALUE_INT:
        if (_paramGlobaux.isDefined(opcode))
        {
            int value = _paramGlobaux.getIntValue(opcode);
            for (size_t i = 0; i < _regionList.size(); i++)
                if (!_regionList.at(i).isDefined(opcode))
                    _regionList[i] << SfzParameter(opcode, value);
        }
        break;
    case VALUE_STRING:
        if (_paramGlobaux.isDefined(opcode))
        {
            std::string value = _paramGlobaux.getStrValue(opcode);
            for (size_t i = 0; i < _regionList.size(); i++)
                if (!_regionList.at(i).isDefined(opcode))
                    _regionList[i] << SfzParameter(opcode, value);
        }
        break;
    }
    _paramGlobaux.removeOpCode(opcode);
}

void SfzParameterGroup::moveModInSamples()
{
    moveModInSamples({ SfzParameter::op_modEnvToTon,
                       SfzParameter::op_pitcheg_attack,
                       SfzParameter::op_pitcheg_decay,
                       SfzParameter::op_pitcheg_delay,
                       SfzParameter::op_pitcheg_hold,
                       SfzParameter::op_pitcheg_release,
                       SfzParameter::op_pitcheg_sustain });
    moveModInSamples({ SfzParameter::op_modEnvToFilter,
                       SfzParameter::op_fileg_attack,
                       SfzParameter::op_fileg_decay,
                       SfzParameter::op_fileg_delay,
                       SfzParameter::op_fileg_hold,
                       SfzParameter::op_fileg_release,
                       SfzParameter::op_fileg_sustain });
    moveModInSamples({ SfzParameter::op_modLFOtoVolume,
                       SfzParameter::op_modLFOdelay,
                       SfzParameter::op_modLFOfreq });
    moveModInSamples({ SfzParameter::op_modLFOtoFilter,
                       SfzParameter::op_filLFOdelay,
                       SfzParameter::op_filLFOfreq });
}

void SfzParameterGroup::moveModInSamples(const std::vector<SfzParameter::OpCode> &opCodeList)
{
    for (size_t i = 0; i < _regionList.size(); i++)
    {
        for (size_t j = 0; j < opCodeList.size(); j++)
        {
            SfzParameter::OpCode opCode = opCodeList.at(j);
            if (!_regionList.at(i).isDefined(opCode) && _paramGlobaux.isDefined(opCode))
            {
                // Other associated opcodes?
                bool isPresent = false;
                for (size_t k = 0; k < opCodeList.size(); k++)
                    if (k != j)
                        isPresent |= _regionList.at(i).isDefined(opCodeList.at(k));
                if (isPresent)
                {
                    if (opCode == SfzParameter::op_modEnvToTon || opCode == SfzParameter::op_modEnvToFilter ||
                            opCode == SfzParameter::op_modLFOtoVolume || opCode == SfzParameter::op_modLFOtoFilter)
                        _regionList[i] << SfzParameter(opCode, _paramGlobaux.getIntValue(opCode));
                    else
                        _regionList[i] << SfzParameter(opCode, _paramGlobaux.getDoubleValue(opCode));
                }
            }
        }
    }
}

void SfzParameterGroup::adjustVolume(double offset)
{
    _paramGlobaux.adjustVolume(offset);
    for (size_t i = 0; i < _regionList.size(); i++)
        _regionList[i].adjustVolume(offset);
}

void SfzParameterGroup::adjustCorrection(const std::string &path)
{
    int defaultCorrection = _paramGlobaux.getIntValue(SfzParameter::op_tuningFine);
    for (size_t i = 0; i < _regionList.size(); i++)
        _regionList[i].adjustCorrection(path, defaultCorrection);
}

void SfzParameterGroup::adjustModulationVolume()
{
    double correctionGlobale = 0;
    if (_paramGlobaux.isDefined(SfzParameter::op_modLFOtoVolume))
    {
        correctionGlobale = std::abs(_paramGlobaux.getDoubleValue(SfzParameter::op_modLFOtoVolume));
        _paramGlobaux.adjustVolume(-correctionGlobale);
    }
    for (size_t i = 0; i < _regionList.size(); i++)
    {
        double correction;
        if (_regionList.at(i).isDefined(SfzParameter::op_modLFOtoVolume))
            correction = std::abs(_regionList.at(i).getDoubleValue(SfzParameter::op_modLFOtoVolume));
        else
            correction = correctionGlobale;
        if (correction != 0)
            _regionList[i].adjustVolume(-correction);
    }
}

void SfzParameterGroup::checkSampleValid(const std::string &path)
{
    int size = static_cast<int>(_regionList.size());
    for (int i = size - 1; i >= 0; i--)
        if (!_regionList[static_cast<size_t>(i)].sampleValid(path))
            _regionList.erase(_regionList.begin() + i);
}

void SfzParameterGroup::checkFilter()
{
    _paramGlobaux.checkFilter();
    for (size_t i = 0; i < _regionList.size(); i++)
        _regionList[i].checkFilter();
}

void SfzParameterGroup::checkKeyTrackedFilter()
{
    _paramGlobaux.checkKeyTrackedFilter(false);
    for (size_t i = 0; i < _regionList.size(); i++)
        _regionList[i].checkKeyTrackedFilter(true);
}

bool SfzParameterGroup::isChannel10()
{
    return (_paramGlobaux.getIntValue(SfzParameter::op_chanMin) == 10 &&
            _paramGlobaux.getIntValue(SfzParameter::op_chanMax) == 10);
}

double SfzParameterGroup::getAmpliMax()
{
    double ampliMax = _paramGlobaux.getDoubleValue(SfzParameter::op_volume);
    for (size_t i = 0; i < _regionList.size(); i++)
        ampliMax = std::max(ampliMax, _regionList.at(i).getDoubleValue(SfzParameter::op_volume));
    return ampliMax;
}

void SfzParameterGroup::decode(ISoundfontBuilder * sf2, Id idInst, const std::string &pathSfz, int velToFilterType)
{
    // Fill the parameters of the global division
    _paramGlobaux.decode(sf2, idInst, velToFilterType);

    // Link to samples
    Id idInstSmpl = idInst;
    idInstSmpl.type = ISoundfontBuilder::TYPE_INSTRUMENT_ZONE;
    Id idSmpl = idInst;
    idSmpl.type = ISoundfontBuilder::TYPE_SAMPLE;
    AttributeValue val;
    for (size_t i = 0; i < _regionList.size(); i++)
    {
        // Create samples if needed and get their index
        std::vector<int> listeIndexSmpl = _regionList.at(i).getSampleIndex(sf2, idInst, pathSfz);

        // Process possible offsets
        if (!listeIndexSmpl.empty())
        {
            idSmpl.element = listeIndexSmpl.front();
            _regionList[i].adaptLoop(sf2->get(idSmpl, champ_dwStartLoop).dwValue,
                                            sf2->get(idSmpl, champ_dwEndLoop).dwValue,
                                            sf2->get(idSmpl, champ_dwLength).dwValue);
        }

        if (_regionList.at(i).isDefined(SfzParameter::op_pan) && listeIndexSmpl.size() == 2)
        {
            double pan = _regionList.at(i).getDoubleValue(SfzParameter::op_pan);
            if (std::abs(pan) >= 99.9)
            {
                // Passage en mono
                if (pan < 0)
                    listeIndexSmpl.erase(listeIndexSmpl.begin() + 1);
                else
                    listeIndexSmpl.erase(listeIndexSmpl.begin());
            }
        }

        if (listeIndexSmpl.size() == 1) // Mono sample
        {
            // Create a division InstSmpl
            idInstSmpl.zone = sf2->add(idInstSmpl);

            // Link to the sample
            val.wValue = static_cast<uint16_t>(listeIndexSmpl.front());
            sf2->set(idInstSmpl, champ_sampleID, val);

            // Fill the parameters of this division
            _regionList[i].decode(sf2, idInstSmpl, velToFilterType);

            if (_regionList.at(i).isDefined(SfzParameter::op_pan))
            {
                val.shValue = 5 * _regionList.at(i).getDoubleValue(SfzParameter::op_pan);
                sf2->set(idInstSmpl, champ_pan, val);
            }
        }
        else if (listeIndexSmpl.size() == 2) // Sample stereo
        {
            // Width, between 0 and 1
            double width = 1;
            bool reversed = false;
            if (_regionList.at(i).isDefined(SfzParameter::op_width))
            {
                width = limit(_regionList.at(i).getDoubleValue(SfzParameter::op_width), -100, 100);
                if (width < 0)
                {
                    width = -width;
                    reversed = true;
                }
                width /= 100.;
            }

            // Position of left and right samples, between 0 and 1
            double position = 0.5;
            if (_regionList.at(i).isDefined(SfzParameter::op_position))
            {
                position = limit(_regionList.at(i).getDoubleValue(SfzParameter::op_position), -100, 100);
                position = (position + 100.) / 200.;
            }
            double positionLeft = position * (1 - width);
            double positionRight = 1 - (1 - position) * (1 - width);

            // Process pan
            float coeffLeft, coeffRight;
            if (_regionList.at(i).isDefined(SfzParameter::op_pan))
            {
                double pan = limit(_regionList.at(i).getDoubleValue(SfzParameter::op_pan) + 100, 0, 200);
                pan = pan * PI / 400.; // Between 0 and PI/2
                coeffLeft = cos(pan);
                coeffRight = sin(pan);
            }
            else
                coeffLeft = coeffRight = cos(PI / 4);

            // Attenuation in dB for the left and right samples
            double attenuationLeft = -SfzParameterRegion::gainToDB(coeffLeft);
            double attenuationRight = -SfzParameterRegion::gainToDB(coeffRight);

            for (size_t j = 0; j < listeIndexSmpl.size(); j++)
            {
                // Creation a division InstSmpl
                idInstSmpl.zone = sf2->add(idInstSmpl);

                // Link to the sample
                val.wValue = static_cast<uint16_t>(listeIndexSmpl.at(j));
                sf2->set(idInstSmpl, champ_sampleID, val);

                if ((j == 0 && !reversed) || (j == 1 && reversed)) // Left, if not reversed
                {
                    // Attenuation
                    val.shValue = qtRound(10.f * attenuationLeft / DB_SF2_TO_REAL_DB);
                    sf2->set(idInstSmpl, champ_initialAttenuation, val);

                    // Pan
                    val.shValue = qtRound((positionLeft - 0.5) * 1000);
                    sf2->set(idInstSmpl, champ_pan, val);
                }
                else // Right
                {
                    // Attenuation
                    val.shValue = qtRound(10.f * attenuationRight / DB_SF2_TO_REAL_DB);
                    sf2->set(idInstSmpl, champ_initialAttenuation, val);

                    // Pan
                    val.shValue = qtRound((positionRight - 0.5) * 1000);
                    sf2->set(idInstSmpl, champ_pan, val);
                }

                // Fill the parameters of this division
                _regionList[i].decode(sf2, idInstSmpl, velToFilterType);
            }
        }
    }
}

double SfzParameterGroup::limit(double value, double min, double max)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

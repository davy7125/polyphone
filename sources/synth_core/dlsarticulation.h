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


#ifndef DLSARTICULATION_H
#define DLSARTICULATION_H

#include <cstdint>
#include <map>
#include <vector>
#include "attributetypes.h"
#include "modulatordata.h"
#include "dlsdefinitions.h"

// Conversion of the articulation applying to a DLS region (connection blocks) into sf2 generators and modulators
class DlsArticulation
{
public:
    DlsArticulation(const std::vector<Dls::Connection> &connections);

    // Generators, as stored in a sf2 file
    const std::map<AttributeType, int16_t> &getGenerators() const { return _generators; }

    // Modulators, without the default modulators of the sf2 format
    const std::vector<ModulatorData> &getModulators() const { return _modulators; }

    // Gain in cB (positive is louder), not converted into an sf2 attenuation
    double getGain() const { return _gain; }

    // Tuning in cents
    double getTuning() const { return _tuning; }

    // Modulators to add in each instrument, so that the default modulators of the sf2 format sound like
    // the implied connections of the DLS format
    static std::vector<ModulatorData> getDefaultModulatorCompensation();

    // Add a modulator in a list, or add its amount to an identical modulator
    static void mergeModulator(std::vector<ModulatorData> &modulators, const ModulatorData &modulator);

private:
    void processStaticConnection(const Dls::Connection &connection);
    void processModulatedConnection(Dls::Connection connection);
    void processKeyScaling();
    void addModulator(SFModulator source, SFModulator amountSource, AttributeType destination, double amount);
    void setGenerator(AttributeType champ, double value);
    bool isAbsoluteZero(uint16_t destination);

    static AttributeType getCombinedGenerator(uint16_t source, uint16_t destination);
    static AttributeType getModulatorDestination(uint16_t destination, double &amount);
    static bool getModulatorSource(uint16_t source, uint16_t curve, bool isBipolar, bool isInverted, SFModulator &result);
    static bool isDefaultModulator(const ModulatorData &modulator);
    static int16_t toTimecents(int32_t scale);

    std::vector<Dls::Connection> _connections;
    std::map<uint16_t, int32_t> _staticScales; // Destination => scale, for connections without source and control
    std::map<uint16_t, double> _keyScalings; // Destination => scale, for connections from the key number to envelope times
    std::map<AttributeType, int16_t> _generators;
    std::vector<ModulatorData> _modulators;
    double _gain;
    double _tuning;
    bool _modLfoUsed;
    bool _vibLfoUsed;
};

#endif // DLSARTICULATION_H

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

#ifndef DLSARTICULATION_H
#define DLSARTICULATION_H

#include "basetypes.h"
#include "dlsdefinitions.h"

// Conversion of the articulation applying to a DLS region (connection blocks) into sf2 generators and modulators
class DlsArticulation
{
public:
    DlsArticulation(const QList<Dls::Connection> &connections);

    // Generators, as stored in a sf2 file
    const QMap<AttributeType, qint16> &getGenerators() const { return _generators; }

    // Modulators, without the default modulators of the sf2 format
    const QList<ModulatorData> &getModulators() const { return _modulators; }

    // Gain in cB (positive is louder), not converted into an sf2 attenuation
    double getGain() const { return _gain; }

    // Tuning in cents
    double getTuning() const { return _tuning; }

    // Modulators to add in each instrument, so that the default modulators of the sf2 format sound like
    // the implied connections of the DLS format
    static QList<ModulatorData> getDefaultModulatorCompensation();

    // Add a modulator in a list, or add its amount to an identical modulator
    static void mergeModulator(QList<ModulatorData> &modulators, const ModulatorData &modulator);

private:
    void processStaticConnection(const Dls::Connection &connection);
    void processModulatedConnection(Dls::Connection connection);
    void processKeyScaling();
    void addModulator(SFModulator source, SFModulator amountSource, AttributeType destination, double amount);
    void setGenerator(AttributeType champ, double value);
    bool isAbsoluteZero(quint16 destination);

    static AttributeType getCombinedGenerator(quint16 source, quint16 destination);
    static AttributeType getModulatorDestination(quint16 destination, double &amount);
    static bool getModulatorSource(quint16 source, quint16 curve, bool isBipolar, bool isInverted, SFModulator &result);
    static bool isDefaultModulator(const ModulatorData &modulator);
    static qint16 toTimecents(qint32 scale);

    QList<Dls::Connection> _connections;
    QMap<quint16, qint32> _staticScales; // Destination => scale, for connections without source and control
    QMap<quint16, double> _keyScalings; // Destination => scale, for connections from the key number to envelope times
    QMap<AttributeType, qint16> _generators;
    QList<ModulatorData> _modulators;
    double _gain;
    double _tuning;
    bool _modLfoUsed;
    bool _vibLfoUsed;
};

#endif // DLSARTICULATION_H

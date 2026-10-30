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

#include "dlsarticulation.h"

// Default frequency (5 Hz, in absolute cents) and delay (10 ms, in timecents) of the DLS LFOs
static const double DEFAULT_LFO_FREQUENCY = -851;
static const double DEFAULT_LFO_DELAY = -7973;

DlsArticulation::DlsArticulation(const QList<Dls::Connection> &connections) :
    _gain(0),
    _tuning(0),
    _modLfoUsed(false),
    _vibLfoUsed(false)
{
    // A combination source / control / destination is unique, the last one is kept
    foreach (Dls::Connection connection, connections)
    {
        bool found = false;
        for (int i = 0; i < _connections.count(); i++)
        {
            if (_connections[i].source == connection.source && _connections[i].control == connection.control &&
                _connections[i].destination == connection.destination)
            {
                _connections[i] = connection;
                found = true;
                break;
            }
        }
        if (!found)
            _connections << connection;
    }

    // Static values first, they are required for converting the modulations
    foreach (Dls::Connection connection, _connections)
        if (connection.source == Dls::SRC_NONE && connection.control == Dls::SRC_NONE)
            _staticScales[connection.destination] = connection.scale;

    foreach (Dls::Connection connection, _connections)
    {
        if (connection.source == Dls::SRC_NONE && connection.control == Dls::SRC_NONE)
            processStaticConnection(connection);
        else
            processModulatedConnection(connection);
    }
    processKeyScaling();

    // Default frequency and delay of the LFOs if they are used
    if (_modLfoUsed)
    {
        if (!_staticScales.contains(Dls::DST_LFO_FREQUENCY))
            setGenerator(champ_freqModLFO, DEFAULT_LFO_FREQUENCY);
        if (!_staticScales.contains(Dls::DST_LFO_STARTDELAY))
            setGenerator(champ_delayModLFO, DEFAULT_LFO_DELAY);
    }
    if (_vibLfoUsed)
    {
        if (!_staticScales.contains(Dls::DST_VIB_FREQUENCY))
            setGenerator(champ_freqVibLFO, DEFAULT_LFO_FREQUENCY);
        if (!_staticScales.contains(Dls::DST_VIB_STARTDELAY))
            setGenerator(champ_delayVibLFO, DEFAULT_LFO_DELAY);
    }

    // Remove the modulators that are already default modulators in the sf2 format
    for (int i = _modulators.count() - 1; i >= 0; i--)
        if (isDefaultModulator(_modulators[i]))
            _modulators.removeAt(i);
}

void DlsArticulation::processStaticConnection(const Dls::Connection &connection)
{
    double value = connection.scale / Dls::UNIT_SCALE;
    switch (connection.destination)
    {
    case Dls::DST_GAIN:
        _gain += value;
        break;
    case Dls::DST_PITCH:
        _tuning += value;
        break;
    case Dls::DST_PAN:
        setGenerator(champ_pan, value);
        break;
    case Dls::DST_CHORUS:
        setGenerator(champ_chorusEffectsSend, value);
        break;
    case Dls::DST_REVERB:
        setGenerator(champ_reverbEffectsSend, value);
        break;
    case Dls::DST_LFO_FREQUENCY:
        setGenerator(champ_freqModLFO, value);
        break;
    case Dls::DST_LFO_STARTDELAY:
        setGenerator(champ_delayModLFO, toTimecents(connection.scale));
        break;
    case Dls::DST_VIB_FREQUENCY:
        setGenerator(champ_freqVibLFO, value);
        break;
    case Dls::DST_VIB_STARTDELAY:
        setGenerator(champ_delayVibLFO, toTimecents(connection.scale));
        break;
    case Dls::DST_EG1_DELAYTIME:
        setGenerator(champ_delayVolEnv, toTimecents(connection.scale));
        break;
    case Dls::DST_EG1_ATTACKTIME:
        setGenerator(champ_attackVolEnv, toTimecents(connection.scale));
        break;
    case Dls::DST_EG1_HOLDTIME:
        setGenerator(champ_holdVolEnv, toTimecents(connection.scale));
        break;
    case Dls::DST_EG1_DECAYTIME:
        setGenerator(champ_decayVolEnv, toTimecents(connection.scale));
        break;
    case Dls::DST_EG1_SUSTAINLEVEL:
        // Level in 0.1% of the dynamic range of the envelope => attenuation in cB
        setGenerator(champ_sustainVolEnv, (1000.0 - value) * Dls::EG1_DYNAMIC_RANGE / 100.0);
        break;
    case Dls::DST_EG1_RELEASETIME:
        setGenerator(champ_releaseVolEnv, toTimecents(connection.scale));
        break;
    case Dls::DST_EG2_DELAYTIME:
        setGenerator(champ_delayModEnv, toTimecents(connection.scale));
        break;
    case Dls::DST_EG2_ATTACKTIME:
        setGenerator(champ_attackModEnv, toTimecents(connection.scale));
        break;
    case Dls::DST_EG2_HOLDTIME:
        setGenerator(champ_holdModEnv, toTimecents(connection.scale));
        break;
    case Dls::DST_EG2_DECAYTIME:
        setGenerator(champ_decayModEnv, toTimecents(connection.scale));
        break;
    case Dls::DST_EG2_SUSTAINLEVEL:
        // Level in 0.1% => decrease in 0.1%
        setGenerator(champ_sustainModEnv, 1000.0 - value);
        break;
    case Dls::DST_EG2_RELEASETIME:
        setGenerator(champ_releaseModEnv, toTimecents(connection.scale));
        break;
    case Dls::DST_FILTER_CUTOFF:
        if (connection.scale != Dls::FILTER_DISABLED)
            setGenerator(champ_initialFilterFc, value);
        break;
    case Dls::DST_FILTER_Q:
        setGenerator(champ_initialFilterQ, value);
        break;
    default:
        // No equivalent (shutdown time of EG1, key number, channel outputs)
        break;
    }
}

void DlsArticulation::processModulatedConnection(Dls::Connection connection)
{
    double value = connection.scale / Dls::UNIT_SCALE;

    // The LFOs and envelopes connected to a destination are sf2 generators
    if (connection.source == Dls::SRC_LFO || connection.source == Dls::SRC_VIBRATO ||
        connection.source == Dls::SRC_EG1 || connection.source == Dls::SRC_EG2)
    {
        AttributeType champ = getCombinedGenerator(connection.source, connection.destination);
        if (champ == champ_unknown || connection.scale == 0)
            return; // Including EG1 to gain, which is implied

        if (connection.source == Dls::SRC_LFO)
            _modLfoUsed = true;
        else if (connection.source == Dls::SRC_VIBRATO)
            _vibLfoUsed = true;

        if (connection.control == Dls::SRC_NONE)
            setGenerator(champ, value);
        else
        {
            // The control changes the depth of the modulation
            SFModulator source;
            if (getModulatorSource(connection.control, connection.controlTransform(), connection.isControlBipolar(),
                                   connection.isControlInverted(), source))
                addModulator(source, SFModulator(), champ, value);
        }
        return;
    }

    // A connection without source is equivalent to a connection whose source is the control
    quint16 source = connection.source;
    quint16 sourceTransform = connection.sourceTransform();
    bool isSourceBipolar = connection.isSourceBipolar();
    bool isSourceInverted = connection.isSourceInverted();
    quint16 control = connection.control;
    if (source == Dls::SRC_NONE)
    {
        source = control;
        sourceTransform = connection.controlTransform();
        isSourceBipolar = connection.isControlBipolar();
        isSourceInverted = connection.isControlInverted();
        control = Dls::SRC_NONE;
    }

    // Implied connections for the tuning, already managed by the synthesizer
    if (source == Dls::SRC_RPN1 || source == Dls::SRC_RPN2 || connection.destination == Dls::DST_KEYNUMBER)
        return;

    // Key number to pitch and to the envelopes are sf2 generators
    if (source == Dls::SRC_KEYNUMBER && control == Dls::SRC_NONE)
    {
        switch (connection.destination)
        {
        case Dls::DST_PITCH:
            setGenerator(champ_scaleTuning, value / 128.0);
            return;
        case Dls::DST_EG1_HOLDTIME: case Dls::DST_EG1_DECAYTIME:
        case Dls::DST_EG2_HOLDTIME: case Dls::DST_EG2_DECAYTIME:
            _keyScalings[connection.destination] = value;
            return;
        default:
            break;
        }
    }

    // A time equal to absolute zero cannot be modulated
    if (isAbsoluteZero(connection.destination))
        return;

    // The pan controlled by CC10 is close to the default modulator of the sf2 format
    if (source == Dls::SRC_CC10 && control == Dls::SRC_NONE && connection.destination == Dls::DST_PAN &&
        value >= 500 && value <= 1000)
        return;

    // Destination
    double amount = value;
    AttributeType destination = getModulatorDestination(connection.destination, amount);
    if (destination == champ_unknown)
        return;

    // Sources, the output transform is applied to the source if it has no curve yet
    if (sourceTransform == Dls::TRN_NONE)
        sourceTransform = connection.outputTransform();
    SFModulator modSource, modAmountSource;
    if (!getModulatorSource(source, sourceTransform, isSourceBipolar, isSourceInverted, modSource) ||
        !getModulatorSource(control, connection.controlTransform(), connection.isControlBipolar(),
                            connection.isControlInverted(), modAmountSource))
        return;

    if (destination == champ_fineTune && source == Dls::SRC_PITCHWHEEL && control == Dls::SRC_RPN0)
    {
        // Range of 128 semitones in DLS, 127 in sf2
        amount *= 127.0 / 128.0;
    }
    else if (destination == champ_initialAttenuation)
    {
        // Some banks only specify the output transform of the velocity, volume or expression to the gain,
        // without the invert flag: the attenuation would then increase with the value.
        // The other DLS players read them as the default connection.
        if (source == Dls::SRC_KEYONVELOCITY || source == Dls::SRC_CC7 || source == Dls::SRC_CC11)
            modSource.isDescending = true;

        // Some corrupted banks also specify huge attenuations
        amount = qBound(-960.0, amount, 960.0);
    }

    addModulator(modSource, modAmountSource, destination, amount);
}

void DlsArticulation::processKeyScaling()
{
    // DLS: time = base time + scale * key / 128
    // sf2: time = base time + keynumTo * (60 - key)
    // => keynumTo = -scale / 128 and the base time is corrected by 60 * scale / 128
    const quint16 destinations[4] = { Dls::DST_EG1_HOLDTIME, Dls::DST_EG1_DECAYTIME, Dls::DST_EG2_HOLDTIME, Dls::DST_EG2_DECAYTIME };
    const AttributeType keyChamps[4] = { champ_keynumToVolEnvHold, champ_keynumToVolEnvDecay, champ_keynumToModEnvHold, champ_keynumToModEnvDecay };
    const AttributeType baseChamps[4] = { champ_holdVolEnv, champ_decayVolEnv, champ_holdModEnv, champ_decayModEnv };
    for (int i = 0; i < 4; i++)
    {
        if (!_keyScalings.contains(destinations[i]) || isAbsoluteZero(destinations[i]))
            continue;
        double scale = _keyScalings[destinations[i]];
        setGenerator(keyChamps[i], -scale / 128.0);
        setGenerator(baseChamps[i], _staticScales[destinations[i]] / Dls::UNIT_SCALE + 60.0 * scale / 128.0);
    }
}

bool DlsArticulation::isAbsoluteZero(quint16 destination)
{
    switch (destination)
    {
    case Dls::DST_EG1_DELAYTIME: case Dls::DST_EG1_ATTACKTIME: case Dls::DST_EG1_HOLDTIME:
    case Dls::DST_EG1_DECAYTIME: case Dls::DST_EG1_RELEASETIME: case Dls::DST_EG1_SHUTDOWNTIME:
    case Dls::DST_EG2_DELAYTIME: case Dls::DST_EG2_ATTACKTIME: case Dls::DST_EG2_HOLDTIME:
    case Dls::DST_EG2_DECAYTIME: case Dls::DST_EG2_RELEASETIME:
        // 0 second by default
        return !_staticScales.contains(destination) || _staticScales[destination] == Dls::ABSOLUTE_ZERO_TIME;
    case Dls::DST_LFO_STARTDELAY: case Dls::DST_VIB_STARTDELAY:
        // 10 ms by default
        return _staticScales.contains(destination) && _staticScales[destination] == Dls::ABSOLUTE_ZERO_TIME;
    default:
        break;
    }
    return false;
}

void DlsArticulation::setGenerator(AttributeType champ, double value)
{
    // Not limited yet: as in a synthesizer, the limits apply once the modulations are added
    _generators[champ] = static_cast<qint16>(qBound(-32768, qRound(value), 32767));
}

void DlsArticulation::addModulator(SFModulator source, SFModulator amountSource, AttributeType destination, double amount)
{
    ModulatorData modulator;
    modulator.srcOper = source;
    modulator.amtSrcOper = amountSource;
    modulator.destOper = static_cast<uint16_t>(destination);
    modulator.transOper = SFTransform::linear;
    modulator.amount = static_cast<int16_t>(qBound(-32768, qRound(amount), 32767));
    mergeModulator(_modulators, modulator);
}

void DlsArticulation::mergeModulator(QList<ModulatorData> &modulators, const ModulatorData &modulator)
{
    for (int i = 0; i < modulators.count(); i++)
    {
        if (modulators[i] == modulator)
        {
            modulators[i].amount = static_cast<int16_t>(qBound(-32768, modulators[i].amount + modulator.amount, 32767));
            return;
        }
    }
    modulators << modulator;
}

QList<ModulatorData> DlsArticulation::getDefaultModulatorCompensation()
{
    QList<ModulatorData> modulators;
    ModulatorData modulator;
    modulator.transOper = SFTransform::linear;

    // No velocity to filter cutoff
    modulator.srcOper = SFModulator(GC_noteOnVelocity, typeLinear, true, false);
    modulator.destOper = champ_initialFilterFc;
    modulator.amount = 0;
    modulators << modulator;

    // No vibrato with the modulation wheel and the channel pressure
    modulator.srcOper = SFModulator(static_cast<uint8_t>(1), typeLinear, false, false);
    modulator.destOper = champ_vibLfoToPitch;
    modulator.amount = 0;
    modulators << modulator;
    modulator.srcOper = SFModulator(GC_channelPressure, typeLinear, false, false);
    modulators << modulator;

    // Reverb and chorus sends up to 100%
    modulator.srcOper = SFModulator(static_cast<uint8_t>(91), typeLinear, false, false);
    modulator.destOper = champ_reverbEffectsSend;
    modulator.amount = 1000;
    modulators << modulator;
    modulator.srcOper = SFModulator(static_cast<uint8_t>(93), typeLinear, false, false);
    modulator.destOper = champ_chorusEffectsSend;
    modulators << modulator;

    return modulators;
}

bool DlsArticulation::isDefaultModulator(const ModulatorData &modulator)
{
    // Modulators compensating a default modulator are always kept
    foreach (ModulatorData compensation, getDefaultModulatorCompensation())
        if (compensation == modulator)
            return false;

    int number;
    const ModulatorData * defaultModulators = ModulatorData::getDefaultModulators(1, number);
    for (int i = 0; i < number; i++)
        if (defaultModulators[i] == modulator && defaultModulators[i].amount == modulator.amount)
            return true;
    return false;
}

AttributeType DlsArticulation::getCombinedGenerator(quint16 source, quint16 destination)
{
    switch (source)
    {
    case Dls::SRC_LFO:
        if (destination == Dls::DST_PITCH)
            return champ_modLfoToPitch;
        if (destination == Dls::DST_GAIN)
            return champ_modLfoToVolume;
        if (destination == Dls::DST_FILTER_CUTOFF)
            return champ_modLfoToFilterFc;
        break;
    case Dls::SRC_VIBRATO:
        if (destination == Dls::DST_PITCH)
            return champ_vibLfoToPitch;
        break;
    case Dls::SRC_EG2:
        if (destination == Dls::DST_PITCH)
            return champ_modEnvToPitch;
        if (destination == Dls::DST_FILTER_CUTOFF)
            return champ_modEnvToFilterFc;
        break;
    default:
        break;
    }
    return champ_unknown;
}

AttributeType DlsArticulation::getModulatorDestination(quint16 destination, double &amount)
{
    switch (destination)
    {
    case Dls::DST_GAIN:
        amount = -amount; // Gain => attenuation
        return champ_initialAttenuation;
    case Dls::DST_PITCH:
        return champ_fineTune;
    case Dls::DST_PAN:
        return champ_pan;
    case Dls::DST_CHORUS:
        return champ_chorusEffectsSend;
    case Dls::DST_REVERB:
        return champ_reverbEffectsSend;
    case Dls::DST_LFO_FREQUENCY:
        return champ_freqModLFO;
    case Dls::DST_LFO_STARTDELAY:
        return champ_delayModLFO;
    case Dls::DST_VIB_FREQUENCY:
        return champ_freqVibLFO;
    case Dls::DST_VIB_STARTDELAY:
        return champ_delayVibLFO;
    case Dls::DST_EG1_DELAYTIME:
        return champ_delayVolEnv;
    case Dls::DST_EG1_ATTACKTIME:
        return champ_attackVolEnv;
    case Dls::DST_EG1_HOLDTIME:
        return champ_holdVolEnv;
    case Dls::DST_EG1_DECAYTIME:
        return champ_decayVolEnv;
    case Dls::DST_EG1_SUSTAINLEVEL:
        amount = -amount * Dls::EG1_DYNAMIC_RANGE / 100.0; // Level => attenuation
        return champ_sustainVolEnv;
    case Dls::DST_EG1_RELEASETIME:
        return champ_releaseVolEnv;
    case Dls::DST_EG2_DELAYTIME:
        return champ_delayModEnv;
    case Dls::DST_EG2_ATTACKTIME:
        return champ_attackModEnv;
    case Dls::DST_EG2_HOLDTIME:
        return champ_holdModEnv;
    case Dls::DST_EG2_DECAYTIME:
        return champ_decayModEnv;
    case Dls::DST_EG2_SUSTAINLEVEL:
        amount = -amount; // Level => decrease
        return champ_sustainModEnv;
    case Dls::DST_EG2_RELEASETIME:
        return champ_releaseModEnv;
    case Dls::DST_FILTER_CUTOFF:
        return champ_initialFilterFc;
    case Dls::DST_FILTER_Q:
        return champ_initialFilterQ;
    default:
        break;
    }
    return champ_unknown;
}

bool DlsArticulation::getModulatorSource(quint16 source, quint16 curve, bool isBipolar, bool isInverted, SFModulator &result)
{
    ModType type;
    switch (curve)
    {
    case Dls::TRN_CONCAVE: type = typeConcave; break;
    case Dls::TRN_CONVEX: type = typeConvex; break;
    case Dls::TRN_SWITCH: type = typeSwitch; break;
    default: type = typeLinear; break;
    }

    switch (source)
    {
    case Dls::SRC_NONE:
        result = SFModulator();
        return true;
    case Dls::SRC_KEYONVELOCITY:
        result = SFModulator(GC_noteOnVelocity, type, isInverted, isBipolar);
        return true;
    case Dls::SRC_KEYNUMBER:
        result = SFModulator(GC_noteOnKeyNumber, type, isInverted, isBipolar);
        return true;
    case Dls::SRC_PITCHWHEEL:
        result = SFModulator(GC_pitchWheel, type, isInverted, true);
        return true;
    case Dls::SRC_POLYPRESSURE:
        result = SFModulator(GC_polypressure, type, isInverted, isBipolar);
        return true;
    case Dls::SRC_CHANNELPRESSURE:
        result = SFModulator(GC_channelPressure, type, isInverted, isBipolar);
        return true;
    case Dls::SRC_RPN0:
        result = SFModulator(GC_pitchWheelSensitivity, type, isInverted, isBipolar);
        return true;
    default:
        break;
    }

    // MIDI controllers, except those that cannot be used as a source in the sf2 format
    if (source > Dls::SRC_CC_FIRST && source < Dls::SRC_CC_FIRST + 120)
    {
        int cc = source - Dls::SRC_CC_FIRST;
        if (cc == 6 || cc == 32 || cc == 38 || (cc >= 98 && cc <= 101))
            return false;
        result = SFModulator(static_cast<uint8_t>(cc), type, isInverted, isBipolar);
        return true;
    }

    // Internal sources (LFOs, envelopes) or RPN1 / RPN2
    return false;
}

qint16 DlsArticulation::toTimecents(qint32 scale)
{
    if (scale == Dls::ABSOLUTE_ZERO_TIME)
        return -12000; // Minimum value, no modulation is possible
    return static_cast<qint16>(qBound(-32768, qRound(scale / Dls::UNIT_SCALE), 32767));
}

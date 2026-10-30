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

#include "conversion_dls.h"
#include "soundfontmanager.h"
#include "dls/dlsarticulation.h"
#include <QFile>

ConversionDls::ConversionDls() : QObject(),
    _sm(SoundfontManager::getInstance()),
    _sf2Index(-1)
{}

QString ConversionDls::convert(QString fileName, int sf2Index)
{
    _sf2Index = sf2Index;
    _waves.clear();
    _listPositions.clear();

    // All samples are in the wave pool, the parts of samples used by the regions are added later
    EltID idSmpl(elementSmpl, sf2Index);
    foreach (int i, _sm->getSiblings(idSmpl))
    {
        idSmpl.indexElt = i;
        getWaveIndex(i, 0, _sm->get(idSmpl, champ_dwLength).dwValue);
    }

    // One instrument per preset
    QList<Instrument> instruments;
    EltID idPrst(elementPrst, sf2Index);
    foreach (int i, _sm->getSiblings(idPrst))
        instruments << getInstrument(i);

    // Write the file
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
        return tr("cannot create file \"%1\"").arg(fileName);

    beginList(file, "RIFF", "DLS ");

    QByteArray data;
    appendUInt32(data, static_cast<quint32>(instruments.count()));
    writeChunk(file, "colh", data);

    beginList(file, "LIST", "lins");
    foreach (const Instrument &instrument, instruments)
        writeInstrument(file, instrument);
    endList(file);

    // Pool table, completed after the waves are written
    qint64 poolTablePosition = file.pos();
    data.clear();
    appendUInt32(data, 8);
    appendUInt32(data, static_cast<quint32>(_waves.count()));
    for (int i = 0; i < _waves.count(); i++)
        appendUInt32(data, 0);
    writeChunk(file, "ptbl", data);

    beginList(file, "LIST", "wvpl");
    qint64 poolStart = file.pos();
    QByteArray offsets;
    foreach (const Wave &wave, _waves)
    {
        appendUInt32(offsets, static_cast<quint32>(file.pos() - poolStart));
        writeWave(file, wave);
    }
    endList(file);

    EltID idSf2(elementSf2, sf2Index);
    QMap<QByteArray, QString> info;
    info["INAM"] = _sm->getQstr(idSf2, champ_name);
    info["ICOP"] = _sm->getQstr(idSf2, champ_ICOP);
    info["ICMT"] = _sm->getQstr(idSf2, champ_ICMT);
    info["IENG"] = _sm->getQstr(idSf2, champ_IENG);
    info["ICRD"] = _sm->getQstr(idSf2, champ_ICRD);
    info["IPRD"] = _sm->getQstr(idSf2, champ_IPRD);
    info["ISFT"] = "Polyphone";
    writeInfo(file, info);

    endList(file);

    // Offsets of the waves
    file.seek(poolTablePosition + 16);
    file.write(offsets);

    bool isOk = (file.error() == QFileDevice::NoError);
    file.close();
    return isOk ? "" : tr("cannot write file \"%1\"").arg(fileName);
}

ConversionDls::Instrument ConversionDls::getInstrument(int presetIndex)
{
    EltID idPrst(elementPrst, _sf2Index, presetIndex);
    Instrument instrument;
    instrument.name = _sm->getQstr(idPrst, champ_name);
    int bank = _sm->get(idPrst, champ_wBank).wValue;
    instrument.bank = (bank == 128 ? Dls::F_INSTRUMENT_DRUMS : static_cast<quint32>((bank & 0x7F) << 8));
    instrument.program = _sm->get(idPrst, champ_wPreset).wValue & 0x7F;

    // Regions: combinations of the preset and instrument divisions
    EltID idPrstInst(elementPrstInst, _sf2Index, presetIndex);
    quint16 layer = 1;
    foreach (int i, _sm->getSiblings(idPrstInst))
    {
        idPrstInst.indexElt2 = i;
        addRegions(idPrstInst, instrument, layer++);
    }

    // Exclusive classes => key groups from 1 to 15
    QList<int> exclusiveClasses;
    for (int i = 0; i < instrument.regions.count(); i++)
    {
        Region &region = instrument.regions[i];
        if (region.exclusiveClass == 0)
            continue;
        int index = exclusiveClasses.indexOf(region.exclusiveClass);
        if (index == -1)
        {
            exclusiveClasses << region.exclusiveClass;
            index = exclusiveClasses.count() - 1;
        }
        region.keyGroup = (index < 15 ? static_cast<quint16>(index + 1) : 0);
    }

    linkStereoRegions(instrument);
    moveCommonArticulation(instrument);
    return instrument;
}

void ConversionDls::addRegions(EltID idPrstInst, Instrument &instrument, quint16 layer)
{
    EltID idInstSmpl(elementInstSmpl, _sf2Index, _sm->get(idPrstInst, champ_instrument).wValue);
    foreach (int i, _sm->getSiblings(idInstSmpl))
    {
        idInstSmpl.indexElt2 = i;
        Region region;
        if (getRegion(idPrstInst, idInstSmpl, region))
        {
            region.layer = layer;
            instrument.regions << region;
        }
    }
}

bool ConversionDls::getRegion(EltID idPrstInst, EltID idInstSmpl, Region &region)
{
    EltID idPrst = idPrstInst;
    idPrst.typeElement = elementPrst;
    EltID idInst = idInstSmpl;
    idInst.typeElement = elementInst;

    // Sample
    EltID idSmpl(elementSmpl, _sf2Index, _sm->get(idInstSmpl, champ_sampleID).wValue);
    if (!_sm->isValid(idSmpl))
        return false;
    region.sampleId = idSmpl.indexElt;

    // Ranges: intersection of the preset and instrument ranges
    AttributeType rangeChamps[2] = { champ_keyRange, champ_velRange };
    RangesType ranges[2];
    for (int i = 0; i < 2; i++)
    {
        RangesType instRange = _sm->isSet(idInstSmpl, rangeChamps[i]) ? _sm->get(idInstSmpl, rangeChamps[i]).rValue :
                                   (_sm->isSet(idInst, rangeChamps[i]) ? _sm->get(idInst, rangeChamps[i]).rValue :
                                        AttributeBase::getDefaultStoredValue(rangeChamps[i], false).rValue);
        RangesType prstRange = _sm->isSet(idPrstInst, rangeChamps[i]) ? _sm->get(idPrstInst, rangeChamps[i]).rValue :
                                   (_sm->isSet(idPrst, rangeChamps[i]) ? _sm->get(idPrst, rangeChamps[i]).rValue :
                                        AttributeBase::getDefaultStoredValue(rangeChamps[i], true).rValue);
        ranges[i].byLo = qMax(instRange.byLo, prstRange.byLo);
        ranges[i].byHi = qMin(qMin(instRange.byHi, prstRange.byHi), static_cast<quint8>(127));
        if (ranges[i].byLo > ranges[i].byHi)
            return false;
    }
    region.keyMin = ranges[0].byLo;
    region.keyMax = ranges[0].byHi;
    region.velMin = ranges[1].byLo;
    region.velMax = ranges[1].byHi;

    // Generators: instrument values (division or global division), plus the preset values
    QVector<int> generators(champ_endOper, 0);
    for (int i = 0; i < champ_endOper; i++)
    {
        AttributeType champ = static_cast<AttributeType>(i);
        switch (champ)
        {
        case champ_keyRange: case champ_velRange: case champ_sampleID: case champ_instrument:
        case champ_unused1: case champ_unused2: case champ_unused3: case champ_unused4: case champ_unused5:
        case champ_reserved1: case champ_reserved2: case champ_reserved3:
            continue;
        default:
            break;
        }

        AttributeValue value = _sm->isSet(idInstSmpl, champ) ? _sm->get(idInstSmpl, champ) :
                                   (_sm->isSet(idInst, champ) ? _sm->get(idInst, champ) :
                                        AttributeBase::getDefaultStoredValue(champ, false));
        int sum = value.shValue;

        switch (champ)
        {
        case champ_startAddrsOffset: case champ_endAddrsOffset: case champ_startloopAddrsOffset: case champ_endloopAddrsOffset:
        case champ_startAddrsCoarseOffset: case champ_endAddrsCoarseOffset: case champ_startloopAddrsCoarseOffset:
        case champ_endloopAddrsCoarseOffset: case champ_keynum: case champ_velocity: case champ_sampleModes:
        case champ_exclusiveClass: case champ_overridingRootKey:
            // Instrument level only
            break;
        default:
            value = _sm->isSet(idPrstInst, champ) ? _sm->get(idPrstInst, champ) :
                        (_sm->isSet(idPrst, champ) ? _sm->get(idPrst, champ) :
                             AttributeBase::getDefaultStoredValue(champ, true));
            sum += value.shValue;
            break;
        }

        // Not limited yet: as in a synthesizer, the limits apply once the modulations are added
        generators[i] = qBound(-32768, sum, 32767);
    }

    // No equivalent for the samples played when the keys are released
    if (generators[champ_sampleModes] == 2)
        return false;

    // Part of the sample that is played
    quint32 length = _sm->get(idSmpl, champ_dwLength).dwValue;
    qint64 start = generators[champ_startAddrsOffset] + 32768LL * generators[champ_startAddrsCoarseOffset];
    qint64 end = length + generators[champ_endAddrsOffset] + 32768LL * generators[champ_endAddrsCoarseOffset];
    start = qBound(0LL, start, static_cast<qint64>(length));
    end = qBound(0LL, end, static_cast<qint64>(length));
    if (end <= start)
    {
        start = 0;
        end = length;
    }
    region.waveIndex = getWaveIndex(idSmpl.indexElt, static_cast<quint32>(start), static_cast<quint32>(end));

    // Root key and tuning
    int rootKey = generators[champ_overridingRootKey] >= 0 ? generators[champ_overridingRootKey] :
                      _sm->get(idSmpl, champ_byOriginalPitch).bValue;
    int tuning = 100 * generators[champ_coarseTune] + generators[champ_fineTune] + _sm->get(idSmpl, champ_chPitchCorrection).cValue;
    if (generators[champ_scaleTuning] != 100 && region.keyMin == region.keyMax)
    {
        // Only one key: the tuning replaces the scale tuning (not supported by all DLS players)
        tuning += (region.keyMin - rootKey) * generators[champ_scaleTuning];
        rootKey = region.keyMin;
        generators[champ_scaleTuning] = 100;
    }
    region.unityNote = static_cast<quint16>(qBound(0, rootKey, 127));
    region.fineTune = static_cast<qint16>(qBound(-32768, tuning, 32767));

    // Gain (the sf2 attenuations are multiplied by 0.4 in the synthesizers)
    region.gain = static_cast<qint32>(qRound(-generators[champ_initialAttenuation] * DB_SF2_TO_REAL_DB * Dls::UNIT_SCALE));

    // Loop
    qint64 loopStart = static_cast<qint64>(_sm->get(idSmpl, champ_dwStartLoop).dwValue) - start +
                       generators[champ_startloopAddrsOffset] + 32768LL * generators[champ_startloopAddrsCoarseOffset];
    qint64 loopEnd = static_cast<qint64>(_sm->get(idSmpl, champ_dwEndLoop).dwValue) - start +
                     generators[champ_endloopAddrsOffset] + 32768LL * generators[champ_endloopAddrsCoarseOffset];
    int loopMode = generators[champ_sampleModes];
    region.hasLoop = (loopMode == 1 || loopMode == 3) && loopStart >= 0 && loopEnd > loopStart && loopEnd <= end - start;
    region.loopType = (loopMode == 3 ? Dls::WLOOP_TYPE_RELEASE : Dls::WLOOP_TYPE_FORWARD);
    region.loopStart = region.hasLoop ? static_cast<quint32>(loopStart) : 0;
    region.loopLength = region.hasLoop ? static_cast<quint32>(loopEnd - loopStart) : 0;

    region.exclusiveClass = generators[champ_exclusiveClass];
    region.keyGroup = 0;
    region.layer = 0;
    region.linkOptions = 0;
    region.phaseGroup = 0;
    region.channel = Dls::WAVELINK_CHANNEL_LEFT;
    region.hasArticulation = true;
    region.connections = getConnections(generators, getModulators(idPrstInst, idInstSmpl));

    return true;
}

QList<ModulatorData> ConversionDls::getModulators(EltID idPrstInst, EltID idInstSmpl)
{
    // Instrument level: default modulators, overridden by the global modulators and then by the modulators of the division
    QList<ModulatorData> modulators;
    int number;
    const ModulatorData * defaultModulators = ModulatorData::getDefaultModulators(1, number);
    for (int i = 0; i < number; i++)
        modulators << defaultModulators[i];
    EltID idInst = idInstSmpl;
    idInst.typeElement = elementInst;
    overrideModulators(modulators, readModulators(idInst));
    overrideModulators(modulators, readModulators(idInstSmpl));

    // Preset level, added to the instrument level
    QList<ModulatorData> prstModulators;
    EltID idPrst = idPrstInst;
    idPrst.typeElement = elementPrst;
    overrideModulators(prstModulators, readModulators(idPrst));
    overrideModulators(prstModulators, readModulators(idPrstInst));
    foreach (const ModulatorData &modulator, prstModulators)
        DlsArticulation::mergeModulator(modulators, modulator);

    return modulators;
}

QList<ModulatorData> ConversionDls::readModulators(EltID id)
{
    switch (id.typeElement)
    {
    case elementInst: id.typeElement = elementInstMod; break;
    case elementInstSmpl: id.typeElement = elementInstSmplMod; break;
    case elementPrst: id.typeElement = elementPrstMod; break;
    case elementPrstInst: id.typeElement = elementPrstInstMod; break;
    default: return QList<ModulatorData>();
    }

    QList<ModulatorData> modulators;
    foreach (int i, _sm->getSiblings(id))
    {
        id.indexMod = i;
        ModulatorData modulator;
        modulator.srcOper = _sm->get(id, champ_sfModSrcOper).sfModValue;
        modulator.amtSrcOper = _sm->get(id, champ_sfModAmtSrcOper).sfModValue;
        modulator.destOper = _sm->get(id, champ_sfModDestOper).wValue;
        modulator.transOper = _sm->get(id, champ_sfModTransOper).sfTransValue;
        modulator.amount = _sm->get(id, champ_modAmount).shValue;
        modulator.index = _sm->get(id, champ_indexMod).wValue;
        modulators << modulator;
    }
    return modulators;
}

void ConversionDls::overrideModulators(QList<ModulatorData> &modulators, const QList<ModulatorData> &newModulators)
{
    foreach (const ModulatorData &newModulator, newModulators)
    {
        int index = modulators.indexOf(newModulator);
        if (index == -1)
            modulators << newModulator;
        else
            modulators[index] = newModulator;
    }
}

QList<Dls::Connection> ConversionDls::getConnections(const QVector<int> &generators, const QList<ModulatorData> &modulators)
{
    QList<Dls::Connection> connections;

    // Modulated parameters
    QSet<int> modulated;
    foreach (const ModulatorData &modulator, modulators)
        if (modulator.amount != 0 && modulator.destOper < 32768)
            modulated << modulator.destOper;
    auto isUsed = [&](AttributeType champ) { return generators[champ] != 0 || modulated.contains(champ); };
    bool isModLfoUsed = isUsed(champ_modLfoToPitch) || isUsed(champ_modLfoToFilterFc) || isUsed(champ_modLfoToVolume);
    bool isVibLfoUsed = isUsed(champ_vibLfoToPitch);
    bool isModEnvUsed = isUsed(champ_modEnvToPitch) || isUsed(champ_modEnvToFilterFc);
    bool isFilterUsed = generators[champ_initialFilterFc] < 13500 || modulated.contains(champ_initialFilterFc) ||
                        isUsed(champ_modLfoToFilterFc) || isUsed(champ_modEnvToFilterFc);

    auto addStatic = [&](quint16 destination, double value) {
        addConnection(connections, Dls::SRC_NONE, Dls::SRC_NONE, destination, 0, value);
    };
    auto addTime = [&](quint16 destination, int timecents, bool isRequired) {
        // 0 second by default, very close to the minimum sf2 value
        if (timecents > -12000 || isRequired)
            addStatic(destination, timecents);
    };

    // Output
    if (generators[champ_pan] != 0)
        addStatic(Dls::DST_PAN, generators[champ_pan]);
    if (generators[champ_reverbEffectsSend] != 0)
        addStatic(Dls::DST_REVERB, generators[champ_reverbEffectsSend]);
    if (generators[champ_chorusEffectsSend] != 0)
        addStatic(Dls::DST_CHORUS, generators[champ_chorusEffectsSend]);

    // Filter (disabled by default)
    if (isFilterUsed)
    {
        addStatic(Dls::DST_FILTER_CUTOFF, generators[champ_initialFilterFc]);
        if (generators[champ_initialFilterQ] != 0)
            addStatic(Dls::DST_FILTER_Q, generators[champ_initialFilterQ]);
    }

    // LFOs (5 Hz and 10 ms by default)
    if (isModLfoUsed)
    {
        addStatic(Dls::DST_LFO_FREQUENCY, generators[champ_freqModLFO]);
        addStatic(Dls::DST_LFO_STARTDELAY, generators[champ_delayModLFO]);
        if (generators[champ_modLfoToPitch] != 0)
            addConnection(connections, Dls::SRC_LFO, Dls::SRC_NONE, Dls::DST_PITCH, 0, generators[champ_modLfoToPitch]);
        if (generators[champ_modLfoToFilterFc] != 0)
            addConnection(connections, Dls::SRC_LFO, Dls::SRC_NONE, Dls::DST_FILTER_CUTOFF, 0, generators[champ_modLfoToFilterFc]);
        if (generators[champ_modLfoToVolume] != 0)
            addConnection(connections, Dls::SRC_LFO, Dls::SRC_NONE, Dls::DST_GAIN, 0, generators[champ_modLfoToVolume]);
    }
    if (isVibLfoUsed)
    {
        addStatic(Dls::DST_VIB_FREQUENCY, generators[champ_freqVibLFO]);
        addStatic(Dls::DST_VIB_STARTDELAY, generators[champ_delayVibLFO]);
        if (generators[champ_vibLfoToPitch] != 0)
            addConnection(connections, Dls::SRC_VIBRATO, Dls::SRC_NONE, Dls::DST_PITCH, 0, generators[champ_vibLfoToPitch]);
    }

    // Envelopes
    // DLS: time = base time + scale * key / 128
    // sf2: time = base time + keynumTo * (60 - key)
    // => scale = -128 * keynumTo and the base time is corrected by 60 * keynumTo
    const AttributeType volChamps[6] = { champ_delayVolEnv, champ_attackVolEnv, champ_holdVolEnv, champ_decayVolEnv, champ_releaseVolEnv, champ_sustainVolEnv };
    const AttributeType modChamps[6] = { champ_delayModEnv, champ_attackModEnv, champ_holdModEnv, champ_decayModEnv, champ_releaseModEnv, champ_sustainModEnv };
    const quint16 volDestinations[6] = { Dls::DST_EG1_DELAYTIME, Dls::DST_EG1_ATTACKTIME, Dls::DST_EG1_HOLDTIME, Dls::DST_EG1_DECAYTIME, Dls::DST_EG1_RELEASETIME, Dls::DST_EG1_SUSTAINLEVEL };
    const quint16 modDestinations[6] = { Dls::DST_EG2_DELAYTIME, Dls::DST_EG2_ATTACKTIME, Dls::DST_EG2_HOLDTIME, Dls::DST_EG2_DECAYTIME, Dls::DST_EG2_RELEASETIME, Dls::DST_EG2_SUSTAINLEVEL };
    const AttributeType volKeyChamps[2] = { champ_keynumToVolEnvHold, champ_keynumToVolEnvDecay };
    const AttributeType modKeyChamps[2] = { champ_keynumToModEnvHold, champ_keynumToModEnvDecay };
    for (int envelope = 0; envelope < 2; envelope++)
    {
        if (envelope == 1 && !isModEnvUsed)
            break;
        const AttributeType * champs = (envelope == 0 ? volChamps : modChamps);
        const quint16 * destinations = (envelope == 0 ? volDestinations : modDestinations);
        const AttributeType * keyChamps = (envelope == 0 ? volKeyChamps : modKeyChamps);

        addTime(destinations[0], generators[champs[0]], modulated.contains(champs[0]));
        addTime(destinations[1], generators[champs[1]], modulated.contains(champs[1]));
        for (int i = 0; i < 2; i++)
        {
            int keyScaling = generators[keyChamps[i]];
            addTime(destinations[2 + i], generators[champs[2 + i]] + 60 * keyScaling, modulated.contains(champs[2 + i]) || keyScaling != 0);
            if (keyScaling != 0)
                addConnection(connections, Dls::SRC_KEYNUMBER, Dls::SRC_NONE, destinations[2 + i], 0, -128.0 * keyScaling);
        }
        addTime(destinations[4], generators[champs[4]], modulated.contains(champs[4]));

        // Sustain: attenuation in cB (volume) or decrease in 0.1% (modulation) => level in 0.1%
        if (generators[champs[5]] != 0)
            addStatic(destinations[5], qMax(0.0, 1000.0 - generators[champs[5]] * (envelope == 0 ? 100.0 / Dls::EG1_DYNAMIC_RANGE : 1.0)));
    }
    if (isModEnvUsed)
    {
        if (generators[champ_modEnvToPitch] != 0)
            addConnection(connections, Dls::SRC_EG2, Dls::SRC_NONE, Dls::DST_PITCH, 0, generators[champ_modEnvToPitch]);
        if (generators[champ_modEnvToFilterFc] != 0)
            addConnection(connections, Dls::SRC_EG2, Dls::SRC_NONE, Dls::DST_FILTER_CUTOFF, 0, generators[champ_modEnvToFilterFc]);
    }

    // Scale tuning (12800 cents for the 128 keys by default)
    if (generators[champ_scaleTuning] != 100)
        addConnection(connections, Dls::SRC_KEYNUMBER, Dls::SRC_NONE, Dls::DST_PITCH, 0, 128.0 * generators[champ_scaleTuning]);

    // Modulators, including the sf2 default modulators that have no equivalent in DLS
    foreach (const ModulatorData &modulator, modulators)
        addModulatorConnection(connections, modulator);

    // Remove the modulations without effect and the connections implied by the DLS format
    // (a modulation without effect replacing an implied connection is kept, so that it disables it)
    for (int i = connections.count() - 1; i >= 0; i--)
    {
        const Dls::Connection &connection = connections[i];
        if ((connection.scale == 0 && (connection.source != Dls::SRC_NONE || connection.control != Dls::SRC_NONE) &&
             !isImpliedConnection(connection, false)) || isImpliedConnection(connection, true))
            connections.removeAt(i);
    }

    return connections;
}

void ConversionDls::addModulatorConnection(QList<Dls::Connection> &connections, const ModulatorData &modulator)
{
    // Not supported: absolute value, links between modulators
    if (modulator.transOper != SFTransform::linear || modulator.destOper >= 32768)
        return;

    quint16 internalSource, destination;
    double factor;
    if (!getDlsDestination(modulator.destOper, internalSource, destination, factor))
        return;

    quint16 source, sourceTransform, control, controlTransform;
    bool isSourceBipolar, isSourceInverted, isControlBipolar, isControlInverted;
    if (!getDlsSource(modulator.srcOper, source, sourceTransform, isSourceBipolar, isSourceInverted) ||
        !getDlsSource(modulator.amtSrcOper, control, controlTransform, isControlBipolar, isControlInverted))
        return;
    if (source == Dls::SRC_NONE)
    {
        source = control;
        sourceTransform = controlTransform;
        isSourceBipolar = isControlBipolar;
        isSourceInverted = isControlInverted;
        control = Dls::SRC_NONE;
        controlTransform = 0;
        isControlBipolar = isControlInverted = false;
    }
    double value = modulator.amount * factor;

    if (internalSource != Dls::SRC_NONE)
    {
        // The LFO or the envelope is the source, the modulator source changes the depth
        if (control != Dls::SRC_NONE)
            return; // Not possible with 2 sources
        addConnection(connections, internalSource, source, destination,
                      Dls::Connection::buildTransform(0, false, false, sourceTransform, isSourceBipolar, isSourceInverted, 0), value);
        return;
    }

    // Range of the pitch wheel: 127 semitones in sf2, 128 in DLS
    if (destination == Dls::DST_PITCH && source == Dls::SRC_PITCHWHEEL && control == Dls::SRC_RPN0)
        value *= 128.0 / 127.0;

    addConnection(connections, source, control, destination,
                  Dls::Connection::buildTransform(sourceTransform, isSourceBipolar, isSourceInverted,
                                                  controlTransform, isControlBipolar, isControlInverted, 0), value);
}

void ConversionDls::linkStereoRegions(Instrument &instrument)
{
    // Regions playing both samples of a stereo sample with the same ranges are phase locked
    quint16 phaseGroup = 0;
    for (int i = 0; i < instrument.regions.count(); i++)
    {
        Region &left = instrument.regions[i];
        if (left.phaseGroup != 0)
            continue;
        EltID idSmpl(elementSmpl, _sf2Index, left.sampleId);
        SFSampleLink type = _sm->get(idSmpl, champ_sfSampleType).sfLinkValue;
        if (type != leftSample && type != RomLeftSample)
            continue;
        int linkedSampleId = _sm->get(idSmpl, champ_wSampleLink).wValue;

        for (int j = 0; j < instrument.regions.count(); j++)
        {
            Region &right = instrument.regions[j];
            if (j == i || right.phaseGroup != 0 || right.sampleId != linkedSampleId ||
                right.keyMin != left.keyMin || right.keyMax != left.keyMax ||
                right.velMin != left.velMin || right.velMax != left.velMax)
                continue;

            phaseGroup++;
            left.phaseGroup = right.phaseGroup = phaseGroup;
            left.linkOptions = Dls::F_WAVELINK_PHASE_MASTER;
            left.channel = Dls::WAVELINK_CHANNEL_LEFT;
            right.channel = Dls::WAVELINK_CHANNEL_RIGHT;
            break;
        }
    }
}

void ConversionDls::moveCommonArticulation(Instrument &instrument)
{
    // The most frequent articulation becomes the global articulation
    int bestIndex = -1;
    int bestCount = 0;
    for (int i = 0; i < instrument.regions.count(); i++)
    {
        int count = 0;
        for (int j = 0; j < instrument.regions.count(); j++)
            if (isEqual(instrument.regions[i].connections, instrument.regions[j].connections))
                count++;
        if (count > bestCount)
        {
            bestCount = count;
            bestIndex = i;
        }
    }
    if (bestIndex == -1)
        return;

    // A local articulation replaces the global articulation
    instrument.connections = instrument.regions[bestIndex].connections;
    for (int i = 0; i < instrument.regions.count(); i++)
    {
        Region &region = instrument.regions[i];
        if (isEqual(region.connections, instrument.connections))
        {
            region.hasArticulation = false;
            region.connections.clear();
        }
    }
}

int ConversionDls::getWaveIndex(int sampleId, quint32 start, quint32 end)
{
    for (int i = 0; i < _waves.count(); i++)
        if (_waves[i].sampleId == sampleId && _waves[i].start == start && _waves[i].end == end)
            return i;

    Wave wave;
    wave.sampleId = sampleId;
    wave.start = start;
    wave.end = end;
    _waves << wave;
    return _waves.count() - 1;
}

void ConversionDls::writeInstrument(QFile &file, const Instrument &instrument)
{
    beginList(file, "LIST", "ins ");

    QByteArray data;
    appendUInt32(data, static_cast<quint32>(instrument.regions.count()));
    appendUInt32(data, instrument.bank);
    appendUInt32(data, instrument.program);
    writeChunk(file, "insh", data);

    beginList(file, "LIST", "lrgn");
    foreach (const Region &region, instrument.regions)
    {
        beginList(file, "LIST", "rgn2");

        // Notes played several times are not stopped, as in a sf2 synthesizer
        data.clear();
        appendUInt16(data, region.keyMin);
        appendUInt16(data, region.keyMax);
        appendUInt16(data, region.velMin);
        appendUInt16(data, region.velMax);
        appendUInt16(data, Dls::F_RGN_OPTION_SELFNONEXCLUSIVE);
        appendUInt16(data, region.keyGroup);
        appendUInt16(data, region.layer);
        writeChunk(file, "rgnh", data);

        writeChunk(file, "wsmp", getWaveSampleData(region.unityNote, region.fineTune, region.gain, region.hasLoop,
                                                   region.loopType, region.loopStart, region.loopLength));

        data.clear();
        appendUInt16(data, region.linkOptions);
        appendUInt16(data, region.phaseGroup);
        appendUInt32(data, region.channel);
        appendUInt32(data, static_cast<quint32>(region.waveIndex));
        writeChunk(file, "wlnk", data);

        if (region.hasArticulation)
        {
            beginList(file, "LIST", "lar2");
            writeChunk(file, "art2", getArticulationData(region.connections));
            endList(file);
        }

        endList(file);
    }
    endList(file);

    if (!instrument.connections.isEmpty())
    {
        beginList(file, "LIST", "lar2");
        writeChunk(file, "art2", getArticulationData(instrument.connections));
        endList(file);
    }

    QMap<QByteArray, QString> info;
    info["INAM"] = instrument.name;
    writeInfo(file, info);

    endList(file);
}

void ConversionDls::writeWave(QFile &file, const Wave &wave)
{
    EltID idSmpl(elementSmpl, _sf2Index, wave.sampleId);
    beginList(file, "LIST", "wave");

    // Format: mono 16-bit PCM
    quint32 sampleRate = _sm->get(idSmpl, champ_dwSampleRate).dwValue;
    QByteArray data;
    appendUInt16(data, Dls::WAVE_FORMAT_PCM);
    appendUInt16(data, 1);
    appendUInt32(data, sampleRate);
    appendUInt32(data, 2 * sampleRate);
    appendUInt16(data, 2);
    appendUInt16(data, 16);
    writeChunk(file, "fmt ", data);

    // Pitch and loop of the sample (the regions have their own parameters)
    quint32 loopStart = _sm->get(idSmpl, champ_dwStartLoop).dwValue;
    quint32 loopEnd = _sm->get(idSmpl, champ_dwEndLoop).dwValue;
    bool hasLoop = loopEnd > loopStart && loopStart >= wave.start && loopEnd <= wave.end;
    writeChunk(file, "wsmp", getWaveSampleData(_sm->get(idSmpl, champ_byOriginalPitch).bValue,
                                               _sm->get(idSmpl, champ_chPitchCorrection).cValue, 0, hasLoop,
                                               Dls::WLOOP_TYPE_FORWARD, hasLoop ? loopStart - wave.start : 0,
                                               hasLoop ? loopEnd - loopStart : 0));

    // Data, 24-bit values being rounded to 16 bits
    quint32 length;
    qint16 * data16;
    quint8 * data24;
    _sm->getSound(idSmpl)->getData(length, data16, data24, false, false);
    data.clear();
    data.reserve(2 * static_cast<int>(wave.end - wave.start));
    for (quint32 i = wave.start; i < wave.end; i++)
    {
        qint16 value = 0;
        if (data16 != nullptr && i < length)
        {
            value = data16[i];
            if (data24 != nullptr && data24[i] >= 128 && value < 32767)
                value++;
        }
        appendUInt16(data, static_cast<quint16>(value));
    }
    writeChunk(file, "data", data);

    QMap<QByteArray, QString> info;
    info["INAM"] = _sm->getQstr(idSmpl, champ_name);
    writeInfo(file, info);

    endList(file);
}

void ConversionDls::writeInfo(QFile &file, const QMap<QByteArray, QString> &info)
{
    bool isEmpty = true;
    foreach (QString value, info.values())
        if (!value.isEmpty())
            isEmpty = false;
    if (isEmpty)
        return;

    beginList(file, "LIST", "INFO");
    foreach (QByteArray key, info.keys())
    {
        QString value = info[key];
        if (value.isEmpty())
            continue;

        // Text ending with '\0', encoded as Latin1 if possible
        QByteArray data = value.toLatin1();
        if (QString::fromLatin1(data) != value)
            data = value.toUtf8();
        data.append('\0');
        writeChunk(file, key.constData(), data);
    }
    endList(file);
}

QByteArray ConversionDls::getWaveSampleData(quint16 unityNote, qint16 fineTune, qint32 gain, bool hasLoop,
                                            quint32 loopType, quint32 loopStart, quint32 loopLength)
{
    QByteArray data;
    appendUInt32(data, 20);
    appendUInt16(data, unityNote);
    appendUInt16(data, static_cast<quint16>(fineTune));
    appendUInt32(data, static_cast<quint32>(gain));
    appendUInt32(data, Dls::F_WSMP_NO_TRUNCATION | Dls::F_WSMP_NO_COMPRESSION);
    appendUInt32(data, hasLoop ? 1 : 0);
    if (hasLoop)
    {
        appendUInt32(data, 16);
        appendUInt32(data, loopType);
        appendUInt32(data, loopStart);
        appendUInt32(data, loopLength);
    }
    return data;
}

QByteArray ConversionDls::getArticulationData(const QList<Dls::Connection> &connections)
{
    QByteArray data;
    appendUInt32(data, 8);
    appendUInt32(data, static_cast<quint32>(connections.count()));
    foreach (const Dls::Connection &connection, connections)
    {
        appendUInt16(data, connection.source);
        appendUInt16(data, connection.control);
        appendUInt16(data, connection.destination);
        appendUInt16(data, connection.transform);
        appendUInt32(data, static_cast<quint32>(connection.scale));
    }
    return data;
}

void ConversionDls::beginList(QFile &file, const char * id, const char * type)
{
    _listPositions << file.pos();
    QByteArray data(id, 4);
    appendUInt32(data, 0); // Size written at the end of the list
    data.append(type, 4);
    file.write(data);
}

void ConversionDls::endList(QFile &file)
{
    qint64 start = _listPositions.takeLast();
    qint64 end = file.pos();
    QByteArray data;
    appendUInt32(data, static_cast<quint32>(end - start - 8));
    file.seek(start + 4);
    file.write(data);
    file.seek(end);
}

void ConversionDls::writeChunk(QFile &file, const char * id, const QByteArray &data)
{
    QByteArray header(id, 4);
    appendUInt32(header, static_cast<quint32>(data.size()));
    file.write(header);
    file.write(data);
    if (data.size() % 2)
        file.write("\0", 1);
}

void ConversionDls::addConnection(QList<Dls::Connection> &connections, quint16 source, quint16 control, quint16 destination,
                                  quint16 transform, double value)
{
    qint64 scale = qRound64(value * Dls::UNIT_SCALE);

    // The combination source / control / destination must be unique
    for (int i = 0; i < connections.count(); i++)
    {
        Dls::Connection &connection = connections[i];
        if (connection.source == source && connection.control == control && connection.destination == destination)
        {
            if (connection.transform != transform && connection.scale == 0)
            {
                // A connection without effect is replaced
                connection.transform = transform;
                connection.scale = 0;
            }
            else if (connection.transform != transform && scale == 0)
                return;

            // Sum (the transform of the first connection is kept if they are different)
            connection.scale = static_cast<qint32>(qBound(static_cast<qint64>(INT32_MIN), connection.scale + scale, static_cast<qint64>(INT32_MAX)));
            return;
        }
    }

    Dls::Connection connection;
    connection.source = source;
    connection.control = control;
    connection.destination = destination;
    connection.transform = transform;
    connection.scale = static_cast<qint32>(qBound(static_cast<qint64>(INT32_MIN), scale, static_cast<qint64>(INT32_MAX)));
    connections << connection;
}

bool ConversionDls::getDlsSource(SFModulator source, quint16 &dlsSource, quint16 &transform, bool &isBipolar, bool &isInverted)
{
    switch (source.Type)
    {
    case typeLinear: transform = Dls::TRN_NONE; break;
    case typeConcave: transform = Dls::TRN_CONCAVE; break;
    case typeConvex: transform = Dls::TRN_CONVEX; break;
    case typeSwitch: transform = Dls::TRN_SWITCH; break;
    default: return false;
    }
    isBipolar = source.isBipolar;
    isInverted = source.isDescending;

    if (source.CC)
    {
        dlsSource = static_cast<quint16>(Dls::SRC_CC_FIRST + source.Index);
        return true;
    }

    switch (source.Index)
    {
    case GC_noController:
        dlsSource = Dls::SRC_NONE;
        transform = Dls::TRN_NONE;
        isBipolar = isInverted = false;
        return true;
    case GC_noteOnVelocity: dlsSource = Dls::SRC_KEYONVELOCITY; return true;
    case GC_noteOnKeyNumber: dlsSource = Dls::SRC_KEYNUMBER; return true;
    case GC_polypressure: dlsSource = Dls::SRC_POLYPRESSURE; return true;
    case GC_channelPressure: dlsSource = Dls::SRC_CHANNELPRESSURE; return true;
    case GC_pitchWheel: dlsSource = Dls::SRC_PITCHWHEEL; return true;
    case GC_pitchWheelSensitivity: dlsSource = Dls::SRC_RPN0; return true;
    default: break;
    }

    // Link from another modulator
    return false;
}

bool ConversionDls::getDlsDestination(quint16 champ, quint16 &internalSource, quint16 &destination, double &factor)
{
    internalSource = Dls::SRC_NONE;
    factor = 1.0;
    switch (champ)
    {
    // Generators combining a LFO or an envelope with a destination
    case champ_modLfoToPitch: internalSource = Dls::SRC_LFO; destination = Dls::DST_PITCH; break;
    case champ_modLfoToFilterFc: internalSource = Dls::SRC_LFO; destination = Dls::DST_FILTER_CUTOFF; break;
    case champ_modLfoToVolume: internalSource = Dls::SRC_LFO; destination = Dls::DST_GAIN; break;
    case champ_vibLfoToPitch: internalSource = Dls::SRC_VIBRATO; destination = Dls::DST_PITCH; break;
    case champ_modEnvToPitch: internalSource = Dls::SRC_EG2; destination = Dls::DST_PITCH; break;
    case champ_modEnvToFilterFc: internalSource = Dls::SRC_EG2; destination = Dls::DST_FILTER_CUTOFF; break;

    // Other generators
    case champ_initialAttenuation: destination = Dls::DST_GAIN; factor = -1.0; break;
    case champ_fineTune: destination = Dls::DST_PITCH; break;
    case champ_coarseTune: destination = Dls::DST_PITCH; factor = 100.0; break;
    case champ_pan: destination = Dls::DST_PAN; break;
    case champ_reverbEffectsSend: destination = Dls::DST_REVERB; break;
    case champ_chorusEffectsSend: destination = Dls::DST_CHORUS; break;
    case champ_initialFilterFc: destination = Dls::DST_FILTER_CUTOFF; break;
    case champ_initialFilterQ: destination = Dls::DST_FILTER_Q; break;
    case champ_freqModLFO: destination = Dls::DST_LFO_FREQUENCY; break;
    case champ_delayModLFO: destination = Dls::DST_LFO_STARTDELAY; break;
    case champ_freqVibLFO: destination = Dls::DST_VIB_FREQUENCY; break;
    case champ_delayVibLFO: destination = Dls::DST_VIB_STARTDELAY; break;
    case champ_delayVolEnv: destination = Dls::DST_EG1_DELAYTIME; break;
    case champ_attackVolEnv: destination = Dls::DST_EG1_ATTACKTIME; break;
    case champ_holdVolEnv: destination = Dls::DST_EG1_HOLDTIME; break;
    case champ_decayVolEnv: destination = Dls::DST_EG1_DECAYTIME; break;
    case champ_sustainVolEnv: destination = Dls::DST_EG1_SUSTAINLEVEL; factor = -100.0 / Dls::EG1_DYNAMIC_RANGE; break;
    case champ_releaseVolEnv: destination = Dls::DST_EG1_RELEASETIME; break;
    case champ_delayModEnv: destination = Dls::DST_EG2_DELAYTIME; break;
    case champ_attackModEnv: destination = Dls::DST_EG2_ATTACKTIME; break;
    case champ_holdModEnv: destination = Dls::DST_EG2_HOLDTIME; break;
    case champ_decayModEnv: destination = Dls::DST_EG2_DECAYTIME; break;
    case champ_sustainModEnv: destination = Dls::DST_EG2_SUSTAINLEVEL; factor = -1.0; break;
    case champ_releaseModEnv: destination = Dls::DST_EG2_RELEASETIME; break;

    default:
        // No equivalent (offsets, key scaling of the envelopes, ...)
        return false;
    }
    return true;
}

bool ConversionDls::isImpliedConnection(const Dls::Connection &connection, bool withDefaultValue)
{
    double value = connection.scale / Dls::UNIT_SCALE;

    // Velocity, volume and expression to gain
    if ((connection.source == Dls::SRC_KEYONVELOCITY || connection.source == Dls::SRC_CC7 || connection.source == Dls::SRC_CC11) &&
        connection.control == Dls::SRC_NONE && connection.destination == Dls::DST_GAIN &&
        connection.transform == Dls::Connection::buildTransform(Dls::TRN_CONCAVE, false, true, 0, false, false, 0))
        return !withDefaultValue || qAbs(value + 960) < 0.5;

    // Pitch wheel to pitch
    if (connection.source == Dls::SRC_PITCHWHEEL && connection.control == Dls::SRC_RPN0 && connection.destination == Dls::DST_PITCH &&
        connection.transform == Dls::Connection::buildTransform(0, true, false, 0, false, false, 0))
        return !withDefaultValue || qAbs(value - 12800) < 0.5;

    // Pan (50.8% in DLS, 100% in sf2 which is then limited to 50%)
    if (connection.source == Dls::SRC_CC10 && connection.control == Dls::SRC_NONE && connection.destination == Dls::DST_PAN &&
        connection.transform == Dls::Connection::buildTransform(0, true, false, 0, false, false, 0))
        return !withDefaultValue || (value >= 500 && value <= 1000);

    // Reverb and chorus
    if (((connection.source == Dls::SRC_CC91 && connection.destination == Dls::DST_REVERB) ||
         (connection.source == Dls::SRC_CC93 && connection.destination == Dls::DST_CHORUS)) &&
        connection.control == Dls::SRC_NONE && connection.transform == 0)
        return !withDefaultValue || qAbs(value - 1000) < 0.5;

    return false;
}

bool ConversionDls::isEqual(const QList<Dls::Connection> &list1, const QList<Dls::Connection> &list2)
{
    if (list1.count() != list2.count())
        return false;
    for (int i = 0; i < list1.count(); i++)
    {
        if (list1[i].source != list2[i].source || list1[i].control != list2[i].control ||
            list1[i].destination != list2[i].destination || list1[i].transform != list2[i].transform ||
            list1[i].scale != list2[i].scale)
            return false;
    }
    return true;
}

void ConversionDls::appendUInt16(QByteArray &data, quint16 value)
{
    data.append(static_cast<char>(value & 0xFF));
    data.append(static_cast<char>((value >> 8) & 0xFF));
}

void ConversionDls::appendUInt32(QByteArray &data, quint32 value)
{
    appendUInt16(data, static_cast<quint16>(value & 0xFFFF));
    appendUInt16(data, static_cast<quint16>(value >> 16));
}

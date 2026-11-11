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


#include "timbremodel.h"
#include "soundfontmanager.h"
#include "soundfonts.h"
#include "soundfont.h"
#include "instprst.h"
#include "attribute.h"
#include "voiceparam.h"
#include "enginevoiceparameters.h"
#include "auto_distribution/toolautodistribution.h"
#include "missing_samples/missingsamplecreator.h"
#include "sampleutils.h"
#include <cmath>
#include <algorithm>

const double TimbreModel::WEIGHT_EXPONENT = 0.3;
const double TimbreModel::WEIGHT_FLOOR = 0.05;

static double toDb(double power)
{
    return power > 0 ? 10 * std::log10(power) : -std::numeric_limits<double>::infinity();
}

// Value of a division, otherwise of the instrument
static bool getValue(SoundfontManager * sm, EltID idDivision, AttributeType champ, AttributeValue &value)
{
    if (sm->isSet(idDivision, champ))
    {
        value = sm->get(idDivision, champ);
        return true;
    }
    EltID idInst(elementInst, idDivision.indexSf2, idDivision.indexElt);
    if (sm->isSet(idInst, champ))
    {
        value = sm->get(idInst, champ);
        return true;
    }
    return false;
}

static RangesType getRange(SoundfontManager * sm, EltID idDivision, AttributeType champ)
{
    AttributeValue value;
    RangesType range;
    range.byLo = 0;
    range.byHi = 127;
    if (getValue(sm, idDivision, champ, value))
    {
        range = value.rValue;
        if (range.byLo > range.byHi)
            std::swap(range.byLo, range.byHi);
    }
    return range;
}

// Gain (dB) of the low-pass filter of the sound engine (same coefficients as Voice::biQuadCoefficients)
static double filterGainDb(double cutoff, double q, double frequency, double sampleRate)
{
    double theta = 2 * cutoff / sampleRate;
    if (theta > 0.98)
        theta = 0.98;
    double invQ = std::pow(10.0, -0.05 * (q - 3.01));
    if (invQ <= 0)
        return 0;
    double dTmp = invQ * std::sin(M_PI * theta) * 0.5;
    if (dTmp <= -1)
        return 0;
    double beta = 0.5 * (1 - dTmp) / (1 + dTmp);
    double gamma = (0.5 + beta) * std::cos(M_PI * theta);
    double c0 = (0.5 + beta - gamma) * 0.5;
    double c1 = 2 * gamma;
    double c2 = -2 * beta;

    // H(z) = c0 (1 + 2 z^-1 + z^-2) / (1 - c1 z^-1 - c2 z^-2)
    double w = 2 * M_PI * frequency / sampleRate;
    double numerator = c0 * (2 + 2 * std::cos(w));
    double re = 1 - c1 * std::cos(w) - c2 * std::cos(2 * w);
    double im = c1 * std::sin(w) + c2 * std::sin(2 * w);
    double denominator = std::sqrt(re * re + im * im);
    if (denominator <= 0 || numerator <= 0)
        return -200;
    return 20 * std::log10(numerator / denominator);
}

void TimbreSpectrum::add(const TimbreSpectrum &other)
{
    if (other.power.size() > power.size())
    {
        int previous = count();
        power.resize(other.power.size());
        state.resize(other.power.size());
        for (int i = previous; i < count(); i++)
        {
            power[i] = 0;
            state[i] = ABSENT;
        }
    }
    for (int i = 0; i < other.count(); i++)
    {
        if (other.state[i] == ABSENT)
            continue;
        power[i] += other.power[i];
        state[i] = qMax(state[i], other.state[i]);
    }
}

double TimbreSpectrum::strongest() const
{
    double result = 0;
    for (int i = 0; i < count(); i++)
        if (state[i] != ABSENT)
            result = qMax(result, power[i]);
    return result;
}

double TimbreSpectrum::totalDb() const
{
    double sum = 0;
    for (int i = 0; i < count(); i++)
        if (state[i] != ABSENT)
            sum += power[i];
    return sum > 0 ? 10 * std::log10(sum) : std::nan("");
}

TimbreModel::TimbreModel() :
    _minKey(-1),
    _maxKey(-1),
    _maxRank(0),
    _missingAnalyses(0),
    _engine(nullptr)
{}

TimbreModel::~TimbreModel()
{
    delete _engine;
}

void TimbreModel::clear()
{
    _rows.clear();
    _divisions.clear();
    _positions.clear();
    for (int i = 0; i < 128; i++)
        _keys[i] = TimbreKey();
    _minKey = _maxKey = -1;
    _maxRank = 0;
    _missingAnalyses = 0;
    _analyses.clear();
    _voiceCache.clear();
    delete _engine;
    _engine = nullptr;
}

bool TimbreModel::isInSubset(SoundfontManager * sm, EltID idDivision, int velocity, bool includeMuted)
{
    return MissingSampleCreator::isInSubset(sm, idDivision, velocity, includeMuted);
}

void TimbreModel::readSubset(SoundfontManager * sm)
{
    EltID idDivision(elementInstSmpl, _idInst.indexSf2, _idInst.indexElt);
    foreach (int index, sm->getSiblings(idDivision))
    {
        idDivision.indexElt2 = index;
        if (!isInSubset(sm, idDivision, _settings.velocity, _settings.includeMuted))
            continue;
        EltID idSmpl(elementSmpl, idDivision.indexSf2, sm->get(idDivision, champ_sampleID).wValue);

        // Root key: overriding root key of the division or of the instrument, otherwise original pitch of the sample
        int rootKey = sm->get(idSmpl, champ_byOriginalPitch).bValue;
        AttributeValue value;
        if (getValue(sm, idDivision, champ_overridingRootKey, value) && value.shValue >= 0)
            rootKey = value.shValue;
        if (rootKey > 127)
            continue;

        TimbreDivision division;
        division.index = index;
        division.sampleIndex = idSmpl.indexElt;
        division.rootKey = rootKey;
        RangesType range = getRange(sm, idDivision, champ_keyRange);
        division.keyMin = range.byLo;
        division.keyMax = range.byHi;
        range = getRange(sm, idDivision, champ_velRange);
        division.velMin = range.byLo;
        division.velMax = range.byHi;
        _divisions << division;
    }
}

void TimbreModel::compute(EltID idInst, const TimbreSettings &settings, bool analyzeNow)
{
    clear();
    SoundfontManager * sm = SoundfontManager::getInstance();
    _idInst = idInst;
    _idInst.typeElement = elementInst;
    _settings = settings;
    if (!sm->isValid(_idInst))
        return;
    readSubset(sm);

    // Analyses of the samples
    QList<int> sampleIndexes;
    foreach (TimbreDivision division, _divisions)
        if (!sampleIndexes.contains(division.sampleIndex))
            sampleIndexes << division.sampleIndex;
    TimbreAnalysisCache * cache = TimbreAnalysisCache::getInstance();
    if (analyzeNow)
        _analyses = cache->getNow(_idInst.indexSf2, sampleIndexes);
    else
    {
        foreach (int index, sampleIndexes)
        {
            QSharedPointer<const TimbreSampleAnalysis> analysis = cache->get(EltID(elementSmpl, _idInst.indexSf2, index));
            if (!analysis.isNull())
                _analyses[index] = analysis;
        }
    }
    _missingAnalyses = static_cast<int>(sampleIndexes.size() - _analyses.size());

    Soundfont * soundfont = sm->getSoundfonts()->getSoundfont(_idInst.indexSf2);
    _engine = new EngineVoiceParameters(_idInst.indexSf2, soundfont);

    // What is heard at each key
    for (int k = 0; k < 128; k++)
    {
        TimbreKey &key = _keys[k];
        key.analyzed = true;
        double strongest = -1;
        for (int i = 0; i < _divisions.count(); i++)
        {
            if (_divisions[i].keyMin > k || _divisions[i].keyMax < k)
                continue;
            key.sources << i;
            TimbreVoice voice = getVoice(i, k);
            key.voices << voice;
            TimbreSpectrum spectrum = getSpectrum(voice);
            key.spectrum.add(spectrum);
            key.analyzed &= _analyses.contains(_divisions[i].sampleIndex);
            double total = 0;
            for (int r = 0; r < spectrum.count(); r++)
                if (spectrum.state[r] != TimbreSpectrum::ABSENT)
                    total += spectrum.power[r];
            if (total > strongest)
            {
                strongest = total;
                key.position = _divisions[i].rootKey;
            }
        }
        if (!key.sources.isEmpty())
        {
            if (_minKey == -1)
                _minKey = k;
            _maxKey = k;
        }
        // Highest rank within the dynamic range
        double threshold = key.spectrum.strongest() * std::pow(10.0, -_settings.dynamicRange / 10);
        for (int r = key.spectrum.count() - 1; r >= 0; r--)
        {
            if (key.spectrum.state[r] != TimbreSpectrum::ABSENT && key.spectrum.power[r] >= threshold)
            {
                _maxRank = qMax(_maxRank, r + 1);
                break;
            }
        }
    }
    _rows = computeRows(_settings.detailedRanks, _settings.octaveBands, _maxRank);

    // Descriptors of the keys and jumps
    for (int k = 0; k < 128; k++)
    {
        TimbreKey &key = _keys[k];
        if (key.sources.isEmpty())
            continue;
        computeKeyDescriptors(key, k);
        if (k > 0 && !_keys[k - 1].sources.isEmpty())
        {
            const TimbreKey &previous = _keys[k - 1];
            key.jump = compare(key.spectrum, previous.spectrum);
            QList<int> divisions, previousDivisions, samples, previousSamples;
            foreach (int i, key.sources)
            {
                divisions << _divisions[i].index;
                if (!samples.contains(_divisions[i].sampleIndex))
                    samples << _divisions[i].sampleIndex;
            }
            foreach (int i, previous.sources)
            {
                previousDivisions << _divisions[i].index;
                if (!previousSamples.contains(_divisions[i].sampleIndex))
                    previousSamples << _divisions[i].sampleIndex;
            }
            std::sort(divisions.begin(), divisions.end());
            std::sort(previousDivisions.begin(), previousDivisions.end());
            std::sort(samples.begin(), samples.end());
            std::sort(previousSamples.begin(), previousSamples.end());
            key.junction = (divisions != previousDivisions);
            key.sampleChange = (samples != previousSamples);
        }
    }

    // Positions
    for (int i = 0; i < _divisions.count(); i++)
    {
        int rootKey = _divisions[i].rootKey;
        if (!_positions.contains(rootKey))
        {
            TimbrePosition position;
            position.key = rootKey;
            position.keyMin = _divisions[i].keyMin;
            position.keyMax = _divisions[i].keyMax;
            _positions[rootKey] = position;
        }
        TimbrePosition &position = _positions[rootKey];
        position.divisions << i;
        position.keyMin = qMin(position.keyMin, _divisions[i].keyMin);
        position.keyMax = qMax(position.keyMax, _divisions[i].keyMax);
    }
    QList<int> positionKeys = _positions.keys();
    for (int i = 0; i < positionKeys.count(); i++)
    {
        TimbrePosition &position = _positions[positionKeys[i]];
        position.previous = i > 0 ? positionKeys[i - 1] : -1;
        position.next = i < positionKeys.count() - 1 ? positionKeys[i + 1] : -1;
        position.spectrum = playPosition(position.key, position.key);
        position.analyzed = true;
        foreach (int index, position.divisions)
            position.analyzed &= _analyses.contains(_divisions[index].sampleIndex);
    }

    // References, deviations and redundancies
    if (positionKeys.count() >= 2)
    {
        for (int i = 0; i < positionKeys.count(); i++)
        {
            TimbrePosition &position = _positions[positionKeys[i]];
            position.reference = computeReference(position.key, position.referenceAtEdge);
            position.deviation = compare(position.spectrum, position.reference);
            computeRedundancy(position);
        }
    }

    applyThresholds(_settings);
}

TimbreVoice TimbreModel::getVoice(int divisionIndex, int key)
{
    qint64 cacheKey = static_cast<qint64>(divisionIndex) * 128 + key;
    if (_voiceCache.contains(cacheKey))
        return _voiceCache[cacheKey];

    TimbreVoice voice;
    voice.divisionIndex = divisionIndex;
    voice.sampleIndex = _divisions[divisionIndex].sampleIndex;
    SoundfontManager * sm = SoundfontManager::getInstance();
    Soundfont * soundfont = sm->getSoundfonts()->getSoundfont(_idInst.indexSf2);
    InstPrst * inst = soundfont != nullptr ? soundfont->getInstrument(_idInst.indexElt) : nullptr;
    VoiceParam * param = _engine->compute(nullptr, -1, inst, _divisions[divisionIndex].index, key, _settings.velocity);
    if (param != nullptr)
    {
        voice.valid = true;
        voice.attenuation = param->getCurrentFloat(champ_initialAttenuation);
        voice.filterQ = param->getCurrentFloat(champ_initialFilterQ);

        // Steady state: modulation envelope at its sustain level, LFOs at their mean value (0)
        double modSustain = 1.0 - 0.01 * param->getCurrentFloat(champ_sustainModEnv);
        voice.filterFrequency = param->getCurrentFloat(champ_initialFilterFc) *
                std::pow(2.0, param->getCurrentInteger(champ_modEnvToFilterFc) * modSustain / 1200);
        voice.gainDb = -voice.attenuation - param->getCurrentFloat(champ_sustainVolEnv) - 0.5 * voice.filterQ;
        voice.transposition = EngineVoiceParameters::getStaticPitchShift(param);
        QSharedPointer<const TimbreSampleAnalysis> analysis = _analyses.value(voice.sampleIndex);
        if (!analysis.isNull() && analysis->pitchFound)
            voice.fundamental = analysis->fundamental * std::pow(2.0, voice.transposition / 1200);
    }
    _voiceCache[cacheKey] = voice;
    return voice;
}

TimbreSpectrum TimbreModel::getSpectrum(const TimbreVoice &voice)
{
    TimbreSpectrum spectrum;
    if (!voice.valid)
        return spectrum;
    QSharedPointer<const TimbreSampleAnalysis> analysis = _analyses.value(voice.sampleIndex);
    if (analysis.isNull() || !analysis->pitchFound)
        return spectrum;

    double ratio = std::pow(2.0, voice.transposition / 1200);
    double nyquist = 0.5 * _settings.outputSampleRate;
    double upperLimit = _settings.upperLimit > 0 ? qMin(_settings.upperLimit, nyquist) : nyquist;
    int count = static_cast<int>(analysis->levels.size());
    spectrum.power.fill(0, count);
    spectrum.state.fill(TimbreSpectrum::ABSENT, count);
    for (int i = 0; i < count; i++)
    {
        double frequency = analysis->frequencies[i] > 0 ? analysis->frequencies[i] : (i + 1) * analysis->fundamental;
        frequency *= ratio;
        if (frequency >= upperLimit || !std::isfinite(analysis->levels[i]))
            continue;
        double level = analysis->levels[i] + voice.gainDb +
                filterGainDb(voice.filterFrequency, voice.filterQ, frequency, _settings.outputSampleRate);
        spectrum.power[i] = std::pow(10.0, level / 10);
        spectrum.state[i] = analysis->found[i] ? TimbreSpectrum::MEASURED : TimbreSpectrum::DOUBTFUL;
    }
    return spectrum;
}

QList<int> TimbreModel::getClosestDivisions(int positionKey, int key) const
{
    QList<int> result;
    if (!_positions.contains(positionKey))
        return result;
    int bestDistance = 1000;
    foreach (int index, _positions[positionKey].divisions)
    {
        const TimbreDivision &division = _divisions[index];
        int distance = key < division.keyMin ? division.keyMin - key : (key > division.keyMax ? key - division.keyMax : 0);
        if (distance < bestDistance)
        {
            bestDistance = distance;
            result.clear();
        }
        if (distance == bestDistance)
            result << index;
    }
    return result;
}

TimbreSpectrum TimbreModel::playPosition(int positionKey, int key, QList<TimbreVoice> * voices)
{
    TimbreSpectrum result;
    foreach (int index, getClosestDivisions(positionKey, key))
    {
        TimbreVoice voice = getVoice(index, key);
        if (voices != nullptr)
            *voices << voice;
        result.add(getSpectrum(voice));
    }
    return result;
}

TimbreSpectrum TimbreModel::computeReference(int positionKey, bool &atEdge, int excludedPosition)
{
    TimbreSpectrum result;
    atEdge = false;
    QList<int> below, above; // Closest first
    foreach (int key, _positions.keys())
    {
        if (key == positionKey || key == excludedPosition)
            continue;
        if (key < positionKey)
            below.prepend(key);
        else
            above << key;
    }
    if (below.isEmpty() && above.isEmpty())
        return result;
    atEdge = below.isEmpty() || above.isEmpty();

    // Neighbors played at the key, with their weights per key
    QList<int> neighbors;
    if (_settings.reference == TimbreSettings::REFERENCE_REGRESSION)
    {
        int n = qMax(1, _settings.regressionCount);
        neighbors << below.mid(0, n) << above.mid(0, n);
    }
    else if (!atEdge)
        neighbors << below.first() << above.first();
    else
        neighbors << (below.isEmpty() ? above : below).mid(0, 2);
    QList<TimbreSpectrum> spectra;
    int count = 0;
    foreach (int key, neighbors)
    {
        spectra << playPosition(key, positionKey);
        count = qMax(count, spectra.last().count());
    }
    result.power.fill(0, count);
    result.state.fill(TimbreSpectrum::ABSENT, count);

    for (int r = 0; r < count; r++)
    {
        // Levels of the neighbors at this rank
        QVector<double> x, y;
        bool allMeasured = true;
        for (int i = 0; i < neighbors.count(); i++)
        {
            if (r < spectra[i].count() && spectra[i].state[r] != TimbreSpectrum::ABSENT && spectra[i].power[r] > 0)
            {
                x << neighbors[i] - positionKey;
                y << toDb(spectra[i].power[r]);
                allMeasured &= (spectra[i].state[r] == TimbreSpectrum::MEASURED);
            }
            else if (_settings.reference != TimbreSettings::REFERENCE_REGRESSION)
            {
                x.clear();
                break; // Interpolation: the rank must be in both neighbors
            }
        }
        if (x.isEmpty())
            continue;

        double value;
        if (x.size() == 1)
            value = y[0];
        else if (_settings.reference != TimbreSettings::REFERENCE_REGRESSION)
        {
            // Interpolation or extrapolation in dB through the two neighbors (x relative to the key)
            value = y[0] + (y[1] - y[0]) * (0 - x[0]) / (x[1] - x[0]);
        }
        else
        {
            // Weighted linear regression (closer neighbors weigh more), then robust weights against outliers
            double maxDistance = 0;
            for (int i = 0; i < x.size(); i++)
                maxDistance = qMax(maxDistance, std::fabs(x[i]));
            QVector<double> distanceWeights(x.size());
            for (int i = 0; i < x.size(); i++)
            {
                double d = std::fabs(x[i]) / (maxDistance + 1);
                distanceWeights[i] = std::pow(1 - d * d * d, 3);
            }
            QVector<double> weights = distanceWeights;
            double slope = 0, intercept = 0;
            for (int iteration = 0; iteration < 2; iteration++)
            {
                if (!SampleUtils::linearRegression(x, y, weights, slope, intercept))
                {
                    // Not enough distinct points: weighted mean
                    double sum = 0, sumWeights = 0;
                    for (int i = 0; i < x.size(); i++)
                    {
                        sum += weights[i] * y[i];
                        sumWeights += weights[i];
                    }
                    slope = 0;
                    intercept = sumWeights > 0 ? sum / sumWeights : y[0];
                    break;
                }
                if (iteration == 1 || x.size() < 4)
                    break;

                // Bisquare weights from the residuals
                QVector<double> residuals(x.size());
                for (int i = 0; i < x.size(); i++)
                    residuals[i] = std::fabs(y[i] - slope * x[i] - intercept);
                QVector<double> sorted = residuals;
                std::sort(sorted.begin(), sorted.end());
                double scale = qMax(0.5, 1.4826 * sorted[sorted.size() / 2]) * 4.685;
                for (int i = 0; i < x.size(); i++)
                {
                    double u = residuals[i] / scale;
                    weights[i] = distanceWeights[i] * (u < 1 ? (1 - u * u) * (1 - u * u) : 0);
                }
            }
            value = intercept; // At the key
        }
        result.power[r] = std::pow(10.0, value / 10);
        result.state[r] = (allMeasured && x.size() >= 2) ? TimbreSpectrum::MEASURED : TimbreSpectrum::DOUBTFUL;
    }
    return result;
}

TimbreSpectrum TimbreModel::predict(int key, int excludedPosition, bool * atEdge)
{
    bool edge = false;
    TimbreSpectrum result = computeReference(key, edge, excludedPosition);
    if (atEdge != nullptr)
        *atEdge = edge;
    return result;
}

TimbreSpectrum TimbreModel::computeReferenceWith(int key, TimbreSettings::Reference mode, int regressionCount,
                                                 bool &atEdge, int excludedPosition)
{
    TimbreSettings::Reference previousMode = _settings.reference;
    int previousCount = _settings.regressionCount;
    _settings.reference = mode;
    _settings.regressionCount = regressionCount;
    TimbreSpectrum result = computeReference(key, atEdge, excludedPosition);
    _settings.reference = previousMode;
    _settings.regressionCount = previousCount;
    return result;
}

bool TimbreModel::isValidRank(const TimbreSpectrum &spectrum, int index, double threshold) const
{
    return index < spectrum.count() && spectrum.state[index] == TimbreSpectrum::MEASURED &&
            spectrum.power[index] > 0 && spectrum.power[index] >= threshold;
}

TimbreComparison TimbreModel::compare(const TimbreSpectrum &test, const TimbreSpectrum &reference) const
{
    TimbreComparison result;
    int rowCount = static_cast<int>(_rows.size());
    result.rowDifferences.fill(std::nan(""), rowCount);
    result.rowWeights.fill(0, rowCount);
    if (test.isEmpty() || reference.isEmpty())
        return result;

    double dynamic = std::pow(10.0, -_settings.dynamicRange / 10);
    double thresholdTest = test.strongest() * dynamic;
    double thresholdReference = reference.strongest() * dynamic;
    QVector<double> testPower(rowCount, 0), referencePower(rowCount, 0);
    double totalTest = 0, totalReference = 0, referencePresent = 0;
    for (int i = 0; i < reference.count(); i++)
        if (reference.state[i] != TimbreSpectrum::ABSENT)
            referencePresent += reference.power[i];
    for (int row = 0; row < rowCount; row++)
    {
        for (int r = _rows[row].firstRank; r <= _rows[row].lastRank; r++)
        {
            int index = r - 1;
            if (isValidRank(test, index, thresholdTest) && isValidRank(reference, index, thresholdReference))
            {
                testPower[row] += test.power[index];
                referencePower[row] += reference.power[index];
            }
        }
        totalTest += testPower[row];
        totalReference += referencePower[row];
    }
    if (totalTest <= 0 || totalReference <= 0)
        return result;

    result.valid = true;
    result.levelDifference = 10 * std::log10(totalTest / totalReference);
    result.confidence = referencePresent > 0 ? totalReference / referencePresent : 0;
    double maxPower = 0;
    for (int row = 0; row < rowCount; row++)
        maxPower = qMax(maxPower, referencePower[row]);
    double sum = 0, sumWeights = 0;
    for (int row = 0; row < rowCount; row++)
    {
        if (testPower[row] <= 0 || referencePower[row] <= 0)
            continue;
        double difference = 10 * std::log10(testPower[row] / referencePower[row]);
        double weight = qMax(std::pow(referencePower[row] / maxPower, WEIGHT_EXPONENT), WEIGHT_FLOOR);
        result.rowDifferences[row] = difference;
        result.rowWeights[row] = weight;
        sum += weight * (difference - result.levelDifference) * (difference - result.levelDifference);
        sumWeights += weight;
    }
    result.timbreDifference = sumWeights > 0 ? std::sqrt(sum / sumWeights) : 0;
    return result;
}

QVector<double> TimbreModel::getRowLevels(const TimbreSpectrum &spectrum) const
{
    QVector<double> result(_rows.size(), std::nan(""));
    for (int row = 0; row < _rows.size(); row++)
    {
        double sum = 0;
        bool present = false;
        for (int r = _rows[row].firstRank; r <= _rows[row].lastRank && r <= spectrum.count(); r++)
        {
            if (spectrum.state[r - 1] != TimbreSpectrum::ABSENT)
            {
                sum += spectrum.power[r - 1];
                present = true;
            }
        }
        if (present)
            result[row] = toDb(sum);
    }
    return result;
}

QVector<int> TimbreModel::getRowStates(const TimbreSpectrum &spectrum) const
{
    // 0: absent, 1: doubtful, 2: measured, 3: partly absent
    QVector<int> result(_rows.size(), 0);
    for (int row = 0; row < _rows.size(); row++)
    {
        bool anyAbsent = false, anyPresent = false, anyDoubtful = false;
        for (int r = _rows[row].firstRank; r <= qMin(_rows[row].lastRank, _maxRank); r++)
        {
            int state = r <= spectrum.count() ? static_cast<int>(spectrum.state[r - 1]) : static_cast<int>(TimbreSpectrum::ABSENT);
            if (state == TimbreSpectrum::ABSENT)
                anyAbsent = true;
            else
            {
                anyPresent = true;
                if (state == TimbreSpectrum::DOUBTFUL)
                    anyDoubtful = true;
            }
        }
        result[row] = !anyPresent ? 0 : (anyAbsent ? 3 : (anyDoubtful ? 1 : 2));
    }
    return result;
}

QVector<double> TimbreModel::getRowConfidence(const TimbreSpectrum &spectrum) const
{
    QVector<double> result(_rows.size(), std::nan(""));
    for (int row = 0; row < _rows.size(); row++)
    {
        double present = 0, measured = 0;
        for (int r = _rows[row].firstRank; r <= _rows[row].lastRank && r <= spectrum.count(); r++)
        {
            if (spectrum.state[r - 1] == TimbreSpectrum::ABSENT)
                continue;
            present += spectrum.power[r - 1];
            if (spectrum.state[r - 1] == TimbreSpectrum::MEASURED)
                measured += spectrum.power[r - 1];
        }
        if (present > 0)
            result[row] = measured / present;
    }
    return result;
}

void TimbreModel::computeKeyDescriptors(TimbreKey &key, int keyNumber)
{
    const TimbreSpectrum &spectrum = key.spectrum;
    key.totalLevel = spectrum.totalDb();
    double threshold = spectrum.strongest() * std::pow(10.0, -_settings.dynamicRange / 10);
    double sum = 0, weightedSum = 0, odd = 0, even = 0;
    QVector<double> x, y;
    for (int i = 0; i < spectrum.count(); i++)
    {
        if (!isValidRank(spectrum, i, threshold))
            continue;
        int rank = i + 1;
        sum += spectrum.power[i];
        weightedSum += rank * spectrum.power[i];
        x << std::log2(rank);
        y << toDb(spectrum.power[i]);
        if (rank >= 2 && rank <= qMax(2, _settings.detailedRanks))
            (rank % 2 == 1 ? odd : even) += spectrum.power[i];
    }
    key.centroid = sum > 0 ? weightedSum / sum : std::nan("");
    double slope, intercept;
    key.slope = (x.size() >= 3 && SampleUtils::linearRegression(x, y, QVector<double>(), slope, intercept)) ? slope : std::nan("");
    key.oddEven = (odd > 0 && even > 0) ? 10 * std::log10(odd / even) : std::nan("");

    // Transposition (mean weighted by the levels), tuning and quality of the main source
    double transpositionSum = 0, transpositionWeights = 0, mainPower = -1;
    key.tuning = key.hnr = key.loopCorrelation = std::nan("");
    for (int i = 0; i < key.voices.count(); i++)
    {
        const TimbreVoice &voice = key.voices[i];
        if (!voice.valid)
            continue;
        double weight = std::pow(10.0, voice.gainDb / 10);
        transpositionSum += weight * voice.transposition;
        transpositionWeights += weight;
        if (weight > mainPower)
        {
            mainPower = weight;
            QSharedPointer<const TimbreSampleAnalysis> analysis = _analyses.value(voice.sampleIndex);
            if (!analysis.isNull())
            {
                key.hnr = analysis->hnr;
                key.loopCorrelation = analysis->loopCorrelation;
            }
            if (voice.fundamental > 0)
                key.tuning = 1200 * std::log2(voice.fundamental / SampleUtils::keyToFrequency(keyNumber));
        }
    }
    double transposition = transpositionWeights > 0 ? transpositionSum / transpositionWeights : 0;
    key.transposition = transposition / 100;
    key.durationFactor = std::pow(2.0, -transposition / 1200);
}

QMap<int, int> TimbreModel::getKeysAfterRemoval(int positionKey) const
{
    QMap<int, int> result;
    if (!_positions.contains(positionKey))
        return result;
    const TimbrePosition &position = _positions[positionKey];
    int previous = position.previous, next = position.next;
    int limit = (previous >= 0 && next >= 0) ? ToolAutoDistribution::getMaxKey(previous, next) : -1;
    for (int k = position.keyMin; k <= position.keyMax; k++)
    {
        // Keys only played by the position
        const TimbreKey &key = _keys[k];
        bool covered = false, other = false;
        foreach (int index, key.sources)
        {
            if (_divisions[index].rootKey == positionKey)
                covered = true;
            else
                other = true;
        }
        if (!covered || other)
            continue;
        if (previous >= 0 && next >= 0)
            result[k] = (k <= limit ? previous : next);
        else
            result[k] = previous >= 0 ? previous : next;
    }
    return result;
}

void TimbreModel::computeRedundancy(TimbrePosition &position)
{
    position.redundancyComputed = false;
    if (position.previous < 0 && position.next < 0)
        return;
    QMap<int, int> keysAfterRemoval = getKeysAfterRemoval(position.key);
    double maxTimbre = 0, maxLevel = 0;
    bool valid = false;
    int lastPrevious = -1;
    for (int k = position.keyMin; k <= position.keyMax; k++)
    {
        const TimbreKey &key = _keys[k];
        if (key.sources.isEmpty())
            continue;

        // Sound after the removal: the other sources, and the neighbor taking the key
        TimbreSpectrum after;
        bool covered = false;
        foreach (int index, key.sources)
        {
            if (_divisions[index].rootKey != position.key)
                after.add(getSpectrum(getVoice(index, k)));
            else
                covered = true;
        }
        if (!covered)
            continue;
        if (keysAfterRemoval.contains(k) && keysAfterRemoval[k] >= 0)
        {
            after.add(playPosition(keysAfterRemoval[k], k));
            if (keysAfterRemoval[k] == position.previous)
                lastPrevious = k;
        }
        TimbreComparison comparison = compare(after, key.spectrum);
        if (comparison.valid)
        {
            valid = true;
            maxTimbre = qMax(maxTimbre, comparison.timbreDifference);
            maxLevel = qMax(maxLevel, std::fabs(comparison.levelDifference));
        }
    }
    position.redundancyComputed = valid;
    position.redundancy = maxTimbre;
    position.redundancyLevel = maxLevel;

    // New junction between the neighbors
    if (position.previous >= 0 && position.next >= 0)
    {
        int limit = lastPrevious >= 0 ? lastPrevious : ToolAutoDistribution::getMaxKey(position.previous, position.next);
        position.newJunction = compare(playPosition(position.next, limit + 1), playPosition(position.previous, limit));
    }
}

void TimbreModel::applyThresholds(const TimbreSettings &settings)
{
    _settings.tolerance = settings.tolerance;
    _settings.maximum = settings.maximum;
    _settings.redundancy = settings.redundancy;
    _settings.jump = settings.jump;
    _settings.level = settings.level;

    for (QMap<int, TimbrePosition>::iterator it = _positions.begin(); it != _positions.end(); ++it)
    {
        TimbrePosition &position = it.value();
        position.label = TimbrePosition::LABEL_NONE;
        position.levelLabel = false;
        if (!position.deviation.valid || !position.analyzed)
            continue;
        double timbre = position.deviation.timbreDifference;
        if (timbre <= _settings.tolerance)
        {
            bool redundant = position.redundancyComputed && position.redundancy <= _settings.redundancy &&
                    (!position.newJunction.valid || position.newJunction.timbreDifference <= _settings.jump);
            position.label = redundant ? TimbrePosition::LABEL_REDUNDANT : TimbrePosition::LABEL_CONFORM;
        }
        else if (timbre <= _settings.maximum)
            position.label = TimbrePosition::LABEL_TO_CORRECT;
        else
            position.label = TimbrePosition::LABEL_TOO_DIFFERENT;
        position.levelLabel = std::fabs(position.deviation.levelDifference) > _settings.level;
    }

    for (int k = 1; k < 128; k++)
    {
        TimbreKey &key = _keys[k];
        key.breakLabel = false;
        if (!key.sampleChange || !key.jump.valid || !key.analyzed || !_keys[k - 1].analyzed ||
            key.jump.timbreDifference <= _settings.jump)
            continue;

        // A free key between the positions on both sides
        int low = qMin(_keys[k - 1].position, key.position);
        int high = qMax(_keys[k - 1].position, key.position);
        for (int j = low + 1; j < high; j++)
        {
            if (!_positions.contains(j))
            {
                key.breakLabel = true;
                break;
            }
        }
    }
}

QList<TimbreRow> TimbreModel::computeRows(int detailedRanks, bool octaveBands, int maxRank)
{
    QList<TimbreRow> rows;
    int detailed = qMin(detailedRanks, maxRank);
    for (int r = 1; r <= detailed; r++)
        rows << TimbreRow { r, r, "H" + QString::number(r) };
    int previous = detailed;
    for (int i = 1; previous < maxRank && i < 200; i++)
    {
        int bound = octaveBands ? (1 << qMin(i, 30)) / 1 : static_cast<int>(std::floor(std::pow(2.0, i / 3.0) + 1e-9));
        if (octaveBands && i > 30)
            break;
        if (bound <= previous)
            continue;
        TimbreRow row;
        row.firstRank = previous + 1;
        row.lastRank = bound;
        row.label = row.firstRank == row.lastRank ? QString::number(row.firstRank) :
                                                    QString("%1-%2").arg(row.firstRank).arg(row.lastRank);
        rows << row;
        previous = bound;
    }
    return rows;
}

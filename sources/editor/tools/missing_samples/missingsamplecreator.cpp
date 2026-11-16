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

#include "missingsamplecreator.h"
#include "soundfontmanager.h"
#include "sampleutils.h"
#include "soundanalyzer.h"
#include "gaincurve.h"
#include "attribute.h"
#include "contextmanager.h"
#include "auto_distribution/toolautodistribution.h"
#include <QRegularExpression>
#include <cmath>
#include <utility>
#include <algorithm>

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
    if (getValue(sm, idDivision, champ, value))
    {
        RangesType range = value.rValue;
        if (range.byLo > range.byHi)
            std::swap(range.byLo, range.byHi);
        return range;
    }
    RangesType range;
    range.byLo = 0;
    range.byHi = 127;
    return range;
}

static bool isLoopPlayback(SoundfontManager * sm, EltID idDivision)
{
    AttributeValue value;
    if (!getValue(sm, idDivision, champ_sampleModes, value))
        return false;
    return value.wValue == 1 || value.wValue == 3;
}

static bool isMono(SFSampleLink type)
{
    return type == monoSample || type == RomMonoSample;
}

bool MissingSamplePosition::isLooped() const
{
    if (sources.isEmpty())
        return false;
    foreach (MissingSampleSource source, sources)
        if (!source.looped)
            return false;
    return true;
}

QList<MissingSampleSource> MissingSamplePosition::getMainSources() const
{
    QList<MissingSampleSource> result;
    QList<int> distances;
    foreach (MissingSampleSource source, sources)
    {
        int distance = key < source.keyMin ? source.keyMin - key : (key > source.keyMax ? key - source.keyMax : 0);
        int found = -1;
        for (int i = 0; i < result.count(); i++)
            if (result[i].idSample == source.idSample)
                found = i;
        if (found == -1)
        {
            result << source;
            distances << distance;
        }
        else if (distance < distances[found])
        {
            result[found] = source;
            distances[found] = distance;
        }
    }
    return result;
}

QSharedPointer<MissingSampleAnalysis> MissingSampleAnalysisCache::get(EltID idInst, const MissingSamplePosition &position)
{
    QString key = QString("%1:%2:%3").arg(idInst.indexSf2).arg(idInst.indexElt).arg(position.key);
    _mutex.lock();
    QSharedPointer<MissingSampleAnalysis> result = _analyses.value(key);
    _mutex.unlock();
    if (!result.isNull())
        return result;

    // Computed outside the lock (another thread may compute the same analysis, the first one is kept)
    result = QSharedPointer<MissingSampleAnalysis>(new MissingSampleAnalysis(MissingSampleCreator::analyze(position)));
    _mutex.lock();
    if (_analyses.contains(key))
        result = _analyses[key];
    else
        _analyses[key] = result;
    _mutex.unlock();
    return result;
}

bool MissingSampleCreator::isInSubset(SoundfontManager * sm, EltID idDivision, int velocity, bool includeMuted)
{
    if (!sm->isValid(idDivision)) // Hidden divisions excluded
        return false;
    if (!includeMuted && sm->get(idDivision, champ_mute).bValue > 0)
        return false;
    RangesType range = getRange(sm, idDivision, champ_velRange);
    if (velocity < range.byLo || velocity > range.byHi)
        return false;
    AttributeValue value;
    if (getValue(sm, idDivision, champ_sampleModes, value) && value.wValue == 2)
        return false; // Played at the release only
    EltID idSmpl(elementSmpl, idDivision.indexSf2, sm->get(idDivision, champ_sampleID).wValue);
    return sm->isValid(idSmpl);
}

QMap<int, MissingSamplePosition> MissingSampleCreator::getPositions(SoundfontManager * sm, EltID idInst, int velocity,
                                                                    bool includeMuted)
{
    QMap<int, MissingSamplePosition> positions;
    EltID idDivision(elementInstSmpl, idInst.indexSf2, idInst.indexElt);
    foreach (int index, sm->getSiblings(idDivision))
    {
        idDivision.indexElt2 = index;
        if (velocity >= 0 && !isInSubset(sm, idDivision, velocity, includeMuted))
            continue;
        EltID idSample(elementSmpl, idInst.indexSf2, sm->get(idDivision, champ_sampleID).wValue);
        if (!sm->isValid(idSample))
            continue;

        // Root key
        int key = sm->get(idSample, champ_byOriginalPitch).bValue;
        AttributeValue value;
        if (getValue(sm, idDivision, champ_overridingRootKey, value) && value.shValue >= 0)
            key = value.shValue;
        if (key > 127)
            continue;

        MissingSampleSource source;
        source.idDivision = idDivision;
        source.idSample = idSample;
        source.attenuation = getValue(sm, idDivision, champ_initialAttenuation, value) ?
                    Attribute::toRealValue(champ_initialAttenuation, false, value) : 0;
        source.looped = isLoopPlayback(sm, idDivision) &&
                SampleUtils::isLooped(sm->get(idSample, champ_dwStartLoop).dwValue, sm->get(idSample, champ_dwEndLoop).dwValue,
                                      sm->get(idSample, champ_dwLength).dwValue);
        RangesType keyRange = getRange(sm, idDivision, champ_keyRange);
        source.keyMin = keyRange.byLo;
        source.keyMax = keyRange.byHi;
        positions[key].key = key;
        positions[key].sources << source;
    }
    return positions;
}

bool MissingSampleCreator::hasSingleVelocityRange(SoundfontManager * sm, EltID idInst)
{
    EltID idDivision(elementInstSmpl, idInst.indexSf2, idInst.indexElt);
    bool first = true;
    RangesType reference;
    foreach (int index, sm->getSiblings(idDivision))
    {
        idDivision.indexElt2 = index;
        RangesType range = getRange(sm, idDivision, champ_velRange);
        if (first)
        {
            reference = range;
            first = false;
        }
        else if (range.byLo != reference.byLo || range.byHi != reference.byHi)
            return false;
    }
    return true;
}

QList<int> MissingSampleCreator::getKeysToFill(const QList<int> &rootKeys, int maxSpacing)
{
    QList<int> keys;
    if (maxSpacing < 1)
        return keys;
    for (int i = 1; i < rootKeys.count(); i++)
    {
        int gap = rootKeys[i] - rootKeys[i - 1];
        int count = (gap + maxSpacing - 1) / maxSpacing - 1; // Each interval of gap / (count + 1) is at most maxSpacing
        for (int j = 1; j <= count; j++)
            keys << rootKeys[i - 1] + qRound(static_cast<double>(j) * gap / (count + 1));
    }
    return keys;
}

int MissingSampleCreator::getAnchor(int rootKey, const QList<QPair<int, int> > &keyRanges)
{
    int anchor = -1;
    for (int i = 0; i < keyRanges.count(); i++)
    {
        int key = qBound(keyRanges[i].first, rootKey, keyRanges[i].second);
        int distance = std::abs(key - rootKey);
        int bestDistance = std::abs(anchor - rootKey);
        if (anchor == -1 || distance < bestDistance || (distance == bestDistance && key < anchor))
            anchor = key;
    }
    return anchor >= 0 ? anchor : rootKey;
}

void MissingSampleCreator::setAnchors(QMap<int, MissingSamplePosition> &positions)
{
    for (QMap<int, MissingSamplePosition>::iterator it = positions.begin(); it != positions.end(); ++it)
    {
        QList<QPair<int, int> > keyRanges;
        foreach (MissingSampleSource source, it.value().sources)
            keyRanges << qMakePair(source.keyMin, source.keyMax);
        it.value().anchor = getAnchor(it.key(), keyRanges);
    }
}

MissingSampleJob MissingSampleCreator::createJob(EltID idInst, int key, const QMap<int, MissingSamplePosition> &positions,
                                                 int anchor)
{
    MissingSampleJob job;
    job.idInst = idInst;
    job.key = key;
    job.anchor = anchor;

    // Keys where the root keys are heard (the root keys themselves without extension), in ascending order
    int target = job.heardKey();
    QList<QPair<int, int> > order; // Heard key, root key
    for (QMap<int, MissingSamplePosition>::const_iterator it = positions.constBegin(); it != positions.constEnd(); ++it)
        order << qMakePair(it.value().heardKey(), it.key());
    std::sort(order.begin(), order.end());
    QList<int> rootKeys, heardKeys;
    for (int i = 0; i < order.count(); i++)
    {
        heardKeys << order[i].first;
        rootKeys << order[i].second;
    }

    int below = -1, above = -1;
    for (int i = 0; i < heardKeys.count(); i++)
    {
        if (heardKeys[i] < target)
            below = i;
        else if (heardKeys[i] > target && above == -1)
            above = i;
    }

    int closest, second;
    if (below >= 0 && above >= 0)
    {
        // Between two root keys
        if (target - heardKeys[below] < heardKeys[above] - target)
        {
            closest = below;
            second = above;
        }
        else
        {
            closest = above;
            second = below;
        }
    }
    else if (below >= 0)
    {
        // Above all root keys
        closest = below;
        second = below - 1;
    }
    else
    {
        // Below all root keys
        closest = above;
        second = above + 1;
    }

    // Nothing computed if a root key is missing (the closest one being empty)
    if (closest < 0 || closest >= rootKeys.count())
        return job;
    job.closest = positions[rootKeys[closest]];
    job.second = (second >= 0 && second < rootKeys.count()) ? positions[rootKeys[second]] : job.closest;
    return job;
}

MissingSampleAnalysis MissingSampleCreator::analyze(const MissingSamplePosition &position)
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    MissingSampleAnalysis result;
    QVector<int> counts;
    double loudnessPower = 0;
    int loudnessCount = 0;
    foreach (MissingSampleSource source, position.getMainSources())
    {
        QVector<float> data = sm->getDataFloat(source.idSample);
        quint32 sampleRate = sm->get(source.idSample, champ_dwSampleRate).dwValue;
        quint32 loopStart = sm->get(source.idSample, champ_dwStartLoop).dwValue;
        quint32 loopEnd = sm->get(source.idSample, champ_dwEndLoop).dwValue;
        if (!SampleUtils::isLooped(loopStart, loopEnd, static_cast<quint32>(data.size())))
            loopStart = loopEnd = 0;
        int correction = sm->get(source.idSample, champ_chPitchCorrection).cValue;
        if (data.isEmpty() || sampleRate == 0)
            continue;

        // Harmonics, the expected frequency avoiding octave errors (the sound may be higher than the root key, as for
        // the organ stops of 4' or the mutations)
        double expected = SoundAnalyzer::adjustExpectedFrequency(data, sampleRate, loopStart, loopEnd,
                                                                 GainCurve::getFundamental(position.key, correction));
        SoundAnalyzer analyzer(data, sampleRate, loopStart, loopEnd, expected);
        double attenuation = std::pow(10.0, -source.attenuation / 10);
        foreach (SoundAnalyzer::Harmonic harmonic, analyzer.getHarmonicList())
        {
            int index = harmonic.rank - 1;
            if (index < 0)
                continue;
            if (index >= result.power.size())
            {
                result.power.resize(index + 1);
                result.found.resize(index + 1);
                counts.resize(index + 1);
            }
            result.power[index] += harmonic.amplitude * harmonic.amplitude * attenuation;
            result.found[index] = result.found[index] || harmonic.found;
            counts[index]++;
        }
        if (!result.fundamentals.contains(source.idSample.indexElt))
            result.fundamentals[source.idSample.indexElt] = analyzer.getFundamental();

        // Loudness, used if no harmonics are found
        double loudness = analyzer.getSteadyLoudness();
        if (std::isfinite(loudness))
        {
            loudnessPower += std::pow(10.0, (loudness - source.attenuation) / 10);
            loudnessCount++;
        }
    }

    for (int i = 0; i < result.power.size(); i++)
        if (counts[i] > 0)
            result.power[i] /= counts[i];
    result.loudness = loudnessCount > 0 ? 10 * std::log10(loudnessPower / loudnessCount) : std::nan("");
    return result;
}

QList<QPointF> MissingSampleCreator::getGains(const MissingSampleAnalysis &closest, const MissingSampleAnalysis &second, double ratio)
{
    QList<QPointF> points;
    int count = qMin(closest.power.size(), second.power.size());
    for (int i = 0; i < count; i++)
        if (closest.found[i] && second.found[i] && closest.power[i] > 0 && second.power[i] > 0)
            points << QPointF(i + 1, ratio * 10 * std::log10(second.power[i] / closest.power[i]));
    return points;
}

void MissingSampleCreator::compute(MissingSampleJob &job, MissingSampleAnalysisCache &cache)
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    job.samples.clear();
    job.levelCompensation = 0;
    job.loopFound = false;
    job.loopRequired = job.closest.isLooped() && job.second.isLooped();
    if (job.closest.sources.isEmpty() || job.second.key == job.closest.key)
        return;

    // Correction of the harmonics, or of the level only if no harmonics are found
    QSharedPointer<MissingSampleAnalysis> closest = cache.get(job.idInst, job.closest);
    QSharedPointer<MissingSampleAnalysis> second = cache.get(job.idInst, job.second);
    // Position of the new note between the keys where both root keys are heard
    int heardDistance = job.second.heardKey() - job.closest.heardKey();
    double ratio = heardDistance != 0 ?
                static_cast<double>(job.heardKey() - job.closest.heardKey()) / heardDistance :
                static_cast<double>(job.key - job.closest.key) / (job.second.key - job.closest.key);
    QList<QPointF> gains = getGains(*closest, *second, ratio);
    job.harmonicsFound = !gains.isEmpty();
    if (gains.isEmpty())
    {
        double difference = second->loudness - closest->loudness;
        gains << QPointF(1, std::isfinite(difference) ? ratio * difference : 0);
    }
    GainCurve curve(GainCurve::MODE_GAIN, true, gains);

    // Transposition then correction of each distinct sample of the closest root key
    double transposition = std::pow(2.0, (job.key - job.closest.key) / 12.0);
    QList<double> peaks, scales; // Peak of the result without limitation, scale applied
    foreach (MissingSampleSource source, job.closest.getMainSources())
    {
        bool alreadyProcessed = false;
        foreach (MissingSampleJob::NewSample sample, job.samples)
            alreadyProcessed |= (sample.idSource == source.idSample);
        if (alreadyProcessed)
            continue;

        MissingSampleJob::NewSample sample;
        sample.idSource = source.idSample;
        sample.sampleRate = sm->get(source.idSample, champ_dwSampleRate).dwValue;
        sample.correction = sm->get(source.idSample, champ_chPitchCorrection).cValue;
        QVector<float> data = sm->getDataFloat(source.idSample);
        if (data.isEmpty() || sample.sampleRate == 0)
            continue;
        data = SampleUtils::resampleMono(data, sample.sampleRate, sample.sampleRate / transposition);

        double fundamental = closest->fundamentals.value(source.idSample.indexElt, 0);
        if (fundamental <= 0)
            fundamental = GainCurve::getFundamental(job.closest.key, sample.correction);
        double peak = SampleUtils::peak(data.constData(), static_cast<quint32>(data.size()));
        if (!curve.isNeutral())
            data = curve.apply(data, sample.sampleRate, fundamental * transposition, 0, 0, nullptr, &peak);
        peaks << peak;
        scales << (curve.isNeutral() ? 1.0 : 1.0 / qMax(1.0, peak)); // The gain curve limits to the full scale
        sample.data = data;
        job.samples << sample;
    }

    // Same attenuation for all samples (stereo balance kept) if one of them is above the full scale
    double maxPeak = 1;
    foreach (double peak, peaks)
        maxPeak = qMax(maxPeak, peak);
    for (int i = 0; i < job.samples.count(); i++)
    {
        double factor = 1.0 / maxPeak / scales[i];
        if (std::fabs(factor - 1) > 1e-9)
        {
            float * data = job.samples[i].data.data();
            for (int j = 0; j < job.samples[i].data.size(); j++)
                data[j] = static_cast<float>(data[j] * factor);
        }
    }
    job.levelCompensation = 20 * std::log10(maxPeak);

    if (job.loopRequired)
        loop(job);
}

void MissingSampleCreator::loop(MissingSampleJob &job)
{
    SoundfontManager * sm = SoundfontManager::getInstance();

    // Groups of samples sharing a loop: linked samples with the same sample rate
    QList<QList<int> > groups;
    QList<int> grouped;
    for (int i = 0; i < job.samples.count(); i++)
    {
        if (grouped.contains(i))
            continue;
        QList<int> group;
        group << i;
        grouped << i;
        EltID idSource = job.samples[i].idSource;
        if (!isMono(sm->get(idSource, champ_sfSampleType).sfLinkValue))
        {
            int link = sm->get(idSource, champ_wSampleLink).wValue;
            for (int j = i + 1; j < job.samples.count(); j++)
            {
                if (!grouped.contains(j) && job.samples[j].idSource.indexElt == link &&
                    job.samples[j].sampleRate == job.samples[i].sampleRate)
                {
                    group << j;
                    grouped << j;
                    break;
                }
            }
        }
        groups << group;
    }

    job.loopFound = true;
    foreach (QList<int> group, groups)
    {
        // Loop of each sample of the group, the longest one fitting all samples being kept
        quint32 bestStart = 0, bestEnd = 0, bestCrossfade = 0;
        bool found = false;
        foreach (int index, group)
        {
            quint32 loopStart = 0, loopEnd = 0, crossfade = 0;
            if (!SampleUtils::loopStep1(job.samples[index].data, job.samples[index].sampleRate, loopStart, loopEnd, crossfade))
                continue;
            bool fits = true;
            foreach (int other, group)
                fits &= (loopEnd + 8 < static_cast<quint32>(job.samples[other].data.size()));
            if (fits && (!found || loopEnd - loopStart > bestEnd - bestStart))
            {
                bestStart = loopStart;
                bestEnd = loopEnd;
                bestCrossfade = crossfade;
                found = true;
            }
        }

        if (found)
        {
            foreach (int index, group)
            {
                job.samples[index].data = SampleUtils::loopStep2(std::move(job.samples[index].data), bestStart, bestEnd,
                                                                 bestCrossfade, true);
                job.samples[index].loopStart = bestStart;
                job.samples[index].loopEnd = bestEnd;
            }
        }
        else
            job.loopFound = false;
    }
}

QList<EltID> MissingSampleCreator::apply(SoundfontManager * sm, const MissingSampleJob &job, QList<EltID> &newSamples,
                                         QStringList &notLooped)
{
    QList<EltID> newDivisions;
    newSamples.clear();
    if (job.samples.isEmpty())
        return newDivisions;

    // Samples
    QMap<int, EltID> sampleBySource; // Index of the source => new sample
    AttributeValue value;
    foreach (MissingSampleJob::NewSample sample, job.samples)
    {
        EltID idSmpl(elementSmpl, job.idInst.indexSf2);
        idSmpl.indexElt = sm->add(idSmpl);
        sm->setData(idSmpl, sample.data);
        value.dwValue = static_cast<quint32>(sample.data.size());
        sm->set(idSmpl, champ_dwLength, value);
        value.dwValue = sample.sampleRate;
        sm->set(idSmpl, champ_dwSampleRate, value);
        // Pitch of the source transposed (the root key of the new note without extension; otherwise the root keys
        // of the divisions differ from the pitches of their samples, by an octave for instance)
        int sourcePitch = sm->get(sample.idSource, champ_byOriginalPitch).bValue;
        int pitch = qBound(0, sourcePitch + job.key - job.closest.key, 127);
        value.bValue = static_cast<quint8>(pitch);
        sm->set(idSmpl, champ_byOriginalPitch, value);
        value.cValue = static_cast<qint8>(sample.correction);
        sm->set(idSmpl, champ_chPitchCorrection, value);
        value.dwValue = sample.loopStart;
        sm->set(idSmpl, champ_dwStartLoop, value);
        value.dwValue = sample.loopEnd;
        sm->set(idSmpl, champ_dwEndLoop, value);
        value.sfLinkValue = monoSample;
        sm->set(idSmpl, champ_sfSampleType, value);
        sm->set(idSmpl, champ_name, getSampleName(sm->getQstr(sample.idSource, champ_name), sourcePitch, pitch));
        sampleBySource[sample.idSource.indexElt] = idSmpl;
        newSamples << idSmpl;
        if (job.loopRequired && !SampleUtils::isLooped(sample.loopStart, sample.loopEnd, static_cast<quint32>(sample.data.size())))
            notLooped << sm->getQstr(idSmpl, champ_name);
    }

    // Links between the new samples, as between their sources
    foreach (MissingSampleJob::NewSample sample, job.samples)
    {
        SFSampleLink type = sm->get(sample.idSource, champ_sfSampleType).sfLinkValue;
        int link = sm->get(sample.idSource, champ_wSampleLink).wValue;
        if (isMono(type) || !sampleBySource.contains(link))
            continue;
        switch (type)
        {
        case leftSample: case RomLeftSample: value.sfLinkValue = leftSample; break;
        case rightSample: case RomRightSample: value.sfLinkValue = rightSample; break;
        default: value.sfLinkValue = linkedSample; break;
        }
        EltID idSmpl = sampleBySource[sample.idSource.indexElt];
        sm->set(idSmpl, champ_sfSampleType, value);
        value.wValue = static_cast<quint16>(sampleBySource[link].indexElt);
        sm->set(idSmpl, champ_wSampleLink, value);
    }

    // Divisions, copying the divisions of the closest root key
    EltID idInst = job.idInst;
    idInst.typeElement = elementInst;
    bool rootKeyOverridden = sm->isSet(idInst, champ_overridingRootKey);
    foreach (MissingSampleSource source, job.closest.getMainSources())
    {
        if (!sampleBySource.contains(source.idSample.indexElt))
            continue;

        EltID idDivision = copyDivision(sm, source.idDivision);

        // Sample and root key
        value.wValue = static_cast<quint16>(sampleBySource[source.idSample.indexElt].indexElt);
        sm->set(idDivision, champ_sampleID, value);
        if (rootKeyOverridden || sm->isSet(source.idDivision, champ_overridingRootKey))
        {
            value.shValue = static_cast<qint16>(job.key);
            sm->set(idDivision, champ_overridingRootKey, value);
        }

        // Level removed from the samples given back by the attenuation, as far as possible
        if (job.levelCompensation > 0)
        {
            double attenuation = qMax(0.0, source.attenuation - job.levelCompensation);
            sm->set(idDivision, champ_initialAttenuation, Attribute::fromRealValue(champ_initialAttenuation, false, attenuation));
        }

        // No loop playback if the new sample is not looped
        EltID idSmpl = sampleBySource[source.idSample.indexElt];
        if (isLoopPlayback(sm, source.idDivision) &&
            !SampleUtils::isLooped(sm->get(idSmpl, champ_dwStartLoop).dwValue, sm->get(idSmpl, champ_dwEndLoop).dwValue,
                                   sm->get(idSmpl, champ_dwLength).dwValue))
        {
            value.wValue = 0;
            sm->set(idDivision, champ_sampleModes, value);
        }

        newDivisions << idDivision;
    }

    return newDivisions;
}

EltID MissingSampleCreator::copyDivision(SoundfontManager * sm, EltID idDivision)
{
    EltID idCopy(elementInstSmpl, idDivision.indexSf2, idDivision.indexElt);
    idCopy.indexElt2 = sm->add(idCopy);

    // Parameters
    EltID idFrom = idDivision;
    idFrom.typeElement = elementInstSmplGen;
    foreach (int champ, sm->getSiblings(idFrom))
        sm->set(idCopy, static_cast<AttributeType>(champ), sm->get(idDivision, static_cast<AttributeType>(champ)));

    // Modulators
    EltID idModFrom = idDivision;
    idModFrom.typeElement = elementInstSmplMod;
    EltID idModTo = idCopy;
    idModTo.typeElement = elementInstSmplMod;
    foreach (int index, sm->getSiblings(idModFrom))
    {
        idModFrom.indexMod = index;
        idModTo.indexMod = sm->add(idModTo);
        sm->set(idModTo, champ_modAmount, sm->get(idModFrom, champ_modAmount));
        sm->set(idModTo, champ_sfModSrcOper, sm->get(idModFrom, champ_sfModSrcOper));
        sm->set(idModTo, champ_sfModAmtSrcOper, sm->get(idModFrom, champ_sfModAmtSrcOper));
        sm->set(idModTo, champ_sfModDestOper, sm->get(idModFrom, champ_sfModDestOper));
        sm->set(idModTo, champ_sfModTransOper, sm->get(idModFrom, champ_sfModTransOper));
    }
    return idCopy;
}

void MissingSampleCreator::adjustKeyRanges(SoundfontManager * sm, const QMap<int, QList<EltID> > &existingDivisions,
                                           const QMap<int, QList<EltID> > &newDivisions, const QMap<int, int> &heardKeys)
{
    if (newDivisions.isEmpty())
        return;

    // Lowest and highest keys of the existing divisions
    int minKey = 127, maxKey = 0;
    foreach (QList<EltID> divisions, existingDivisions.values())
    {
        foreach (EltID idDivision, divisions)
        {
            RangesType range = getRange(sm, idDivision, champ_keyRange);
            minKey = qMin(minKey, static_cast<int>(range.byLo));
            maxKey = qMax(maxKey, static_cast<int>(range.byHi));
        }
    }

    // All root keys, in the order of the keys where they are heard (the root keys themselves without extension)
    QMap<int, QList<EltID> > allDivisions = existingDivisions;
    foreach (int key, newDivisions.keys())
        allDivisions[key] << newDivisions[key];
    QList<QPair<int, int> > order; // Heard key, root key
    foreach (int rootKey, allDivisions.keys())
        order << qMakePair(heardKeys.value(rootKey, rootKey), rootKey);
    std::sort(order.begin(), order.end());

    AttributeValue value;
    for (int i = 0; i < order.count(); i++)
    {
        int key = order[i].first;
        int rootKey = order[i].second;
        if (!newDivisions.contains(rootKey))
            continue;

        // Lowest key, the previous existing divisions being possibly shortened (those below the new note only, a
        // root key possibly playing other keys elsewhere)
        int lowKey;
        if (i == 0)
            lowKey = qMin(minKey, key);
        else
        {
            int previous = order[i - 1].second;
            int limit = ToolAutoDistribution::getMaxKey(order[i - 1].first, key);
            lowKey = limit + 1;
            if (!newDivisions.contains(previous))
            {
                int previousHigh = -1;
                foreach (EltID idDivision, allDivisions[previous])
                {
                    RangesType range = getRange(sm, idDivision, champ_keyRange);
                    if (range.byLo > key)
                        continue;
                    if (range.byHi > limit)
                    {
                        range.byHi = static_cast<quint8>(qMax(limit, static_cast<int>(range.byLo)));
                        value.rValue = range;
                        sm->set(idDivision, champ_keyRange, value);
                    }
                    previousHigh = qMax(previousHigh, static_cast<int>(range.byHi));
                }
                if (previousHigh >= 0)
                    lowKey = previousHigh + 1;
            }
        }

        // Highest key, the next existing divisions being possibly shortened (those above the new note only)
        int highKey;
        if (i == order.count() - 1)
            highKey = qMax(maxKey, key);
        else
        {
            int next = order[i + 1].second;
            int limit = ToolAutoDistribution::getMaxKey(key, order[i + 1].first);
            highKey = limit;
            if (!newDivisions.contains(next))
            {
                int nextLow = 128;
                foreach (EltID idDivision, allDivisions[next])
                {
                    RangesType range = getRange(sm, idDivision, champ_keyRange);
                    if (range.byHi < key)
                        continue;
                    if (range.byLo <= limit)
                    {
                        range.byLo = static_cast<quint8>(qMin(limit + 1, static_cast<int>(range.byHi)));
                        value.rValue = range;
                        sm->set(idDivision, champ_keyRange, value);
                    }
                    nextLow = qMin(nextLow, static_cast<int>(range.byLo));
                }
                if (nextLow < 128)
                    highKey = nextLow - 1;
            }
        }

        value.rValue.byLo = static_cast<quint8>(qBound(0, qMin(lowKey, key), 127));
        value.rValue.byHi = static_cast<quint8>(qBound(0, qMax(highKey, key), 127));
        foreach (EltID idDivision, newDivisions[rootKey])
            sm->set(idDivision, champ_keyRange, value);
    }
}

void MissingSampleCreator::removeCoveredDivisions(SoundfontManager * sm, const QMap<int, QList<EltID> > &existingDivisions,
                                                  const QMap<int, QList<EltID> > &newDivisions)
{
    QList<RangesType> newRanges;
    foreach (QList<EltID> divisions, newDivisions.values())
        if (!divisions.isEmpty())
            newRanges << getRange(sm, divisions.first(), champ_keyRange);
    foreach (QList<EltID> divisions, existingDivisions.values())
    {
        foreach (EltID idDivision, divisions)
        {
            if (!sm->isValid(idDivision))
                continue;
            RangesType range = getRange(sm, idDivision, champ_keyRange);
            foreach (RangesType newRange, newRanges)
            {
                if (range.byLo >= newRange.byLo && range.byHi <= newRange.byHi)
                {
                    sm->remove(idDivision);
                    break;
                }
            }
        }
    }
}

QString MissingSampleCreator::getSampleName(QString sourceName, int sourceKey, int key)
{
    static const char * names[12] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    auto englishName = [](int k) -> QString {
        return QString(names[k % 12]) + QString::number(k / 12 - 1);
    };

    // Forms of the key that may be written in the name (a number not within other digits, a note name not within a
    // word), and the same forms for the new key
    struct Form
    {
        QString source;
        QString target;
        bool isNumber;
    };
    QList<Form> forms;
    forms << Form { QString("%1").arg(sourceKey, 3, 10, QChar('0')), QString("%1").arg(key, 3, 10, QChar('0')), true };
    forms << Form { ContextManager::keyName()->getKeyName(static_cast<unsigned int>(sourceKey), true),
                    ContextManager::keyName()->getKeyName(static_cast<unsigned int>(key), true), false };
    forms << Form { englishName(sourceKey), englishName(key), false };
    forms << Form { QString::number(sourceKey), QString::number(key), true };

    int position = -1;
    QString found, replacement;
    for (int i = 0; i < forms.count() && position < 0; i++)
    {
        if (forms[i].source.isEmpty())
            continue;
        QRegularExpression expression((forms[i].isNumber ? "(?<![0-9])" : "(?<![0-9A-Za-z#])") +
                                      QRegularExpression::escape(forms[i].source) + "(?![0-9#])",
                                      QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatchIterator iterator = expression.globalMatch(sourceName);
        while (iterator.hasNext())
        {
            QRegularExpressionMatch match = iterator.next();
            position = static_cast<int>(match.capturedStart());
            found = match.captured();
            replacement = forms[i].target;
        }
    }

    QString prefix, suffix;
    if (position >= 0)
    {
        if (found == found.toLower())
            replacement = replacement.toLower();
        prefix = sourceName.left(position);
        suffix = sourceName.mid(position + found.size());
    }
    else
    {
        prefix = sourceName.trimmed() + " ";
        replacement = QString("%1").arg(key, 3, 10, QChar('0'));
    }

    // 20 characters at most, the prefix being shortened if needed
    int prefixLength = qMax(0, 20 - static_cast<int>(replacement.size() + suffix.size()));
    return (prefix.left(prefixLength) + replacement + suffix).left(20);
}

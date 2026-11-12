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


#include "timbreactions.h"
#include "soundfontmanager.h"
#include "attribute.h"
#include "gaincurve.h"
#include "duplicator.h"
#include "sampleutils.h"
#include "contextmanager.h"
#include <cmath>

int TimbreActions::s_sharedChoice = TimbreActions::SHARED_ASK;

static RangesType getKeyRange(SoundfontManager * sm, EltID idDivision)
{
    RangesType range;
    range.byLo = 0;
    range.byHi = 127;
    EltID idInst(elementInst, idDivision.indexSf2, idDivision.indexElt);
    if (sm->isSet(idDivision, champ_keyRange))
        range = sm->get(idDivision, champ_keyRange).rValue;
    else if (sm->isSet(idInst, champ_keyRange))
        range = sm->get(idInst, champ_keyRange).rValue;
    if (range.byLo > range.byHi)
        std::swap(range.byLo, range.byHi);
    return range;
}

bool TimbreActions::prepareCreation(SoundfontManager * sm, EltID idInst, const TimbreSettings &settings, int key,
                                    int closestKey, int secondKey, int excludedPosition, TimbreCreation &creation,
                                    QString &error)
{
    idInst.typeElement = elementInst;
    QMap<int, MissingSamplePosition> positions = MissingSampleCreator::getPositions(sm, idInst, settings.velocity,
                                                                                    settings.includeMuted);
    positions.remove(excludedPosition);
    if (positions.contains(key))
    {
        error = tr("The key %1 already has a sample.").arg(ContextManager::keyName()->getKeyName(static_cast<unsigned int>(key)));
        return false;
    }
    if (positions.count() < 2)
    {
        error = tr("At least two root keys are needed for computing a note.");
        return false;
    }

    if (closestKey >= 0 && secondKey >= 0 && closestKey != secondKey && positions.contains(closestKey) &&
        positions.contains(secondKey))
    {
        creation.job = MissingSampleJob();
        creation.job.idInst = idInst;
        creation.job.key = key;
        creation.job.closest = positions[closestKey];
        creation.job.second = positions[secondKey];
    }
    else
        creation.job = MissingSampleCreator::createJob(idInst, key, positions);

    creation.existingDivisions.clear();
    foreach (MissingSamplePosition position, positions.values())
        foreach (MissingSampleSource source, position.sources)
            creation.existingDivisions[position.key] << source.idDivision;

    // Extrapolation far from the root keys used
    int closest = creation.job.closest.key, second = creation.job.second.key;
    bool between = (key > qMin(closest, second) && key < qMax(closest, second));
    creation.extrapolationWarning = !between && std::abs(key - closest) > 2 * std::abs(second - closest);
    return true;
}

void TimbreActions::computeCreation(TimbreCreation &creation)
{
    MissingSampleAnalysisCache cache;
    MissingSampleCreator::compute(creation.job, cache);
}

QList<EltID> TimbreActions::applyCreation(SoundfontManager * sm, const TimbreCreation &creation, QStringList &warnings)
{
    QList<EltID> newSamples;
    QStringList notLooped;
    QList<EltID> newDivisions = MissingSampleCreator::apply(sm, creation.job, newSamples, notLooped);
    if (newDivisions.isEmpty())
    {
        warnings << tr("The note couldn't be computed.");
        return newDivisions;
    }
    QMap<int, QList<EltID> > created;
    created[creation.job.key] = newDivisions;
    MissingSampleCreator::adjustKeyRanges(sm, creation.existingDivisions, created);

    MissingSampleCreator::removeCoveredDivisions(sm, creation.existingDivisions, created);

    if (!creation.job.harmonicsFound)
        warnings << tr("No harmonics found, only the level was interpolated.");
    if (!notLooped.isEmpty())
        warnings << tr("The following samples couldn't be looped:") + " " + notLooped.join(", ");
    return newDivisions;
}

void TimbreActions::removePosition(SoundfontManager * sm, const TimbreModel &model, int positionKey, bool removeUnusedSamples)
{
    if (!model.getPositions().contains(positionKey))
        return;
    const TimbrePosition &position = model.getPositions()[positionKey];
    EltID idInst = model.getInstrument();
    EltID idDivision(elementInstSmpl, idInst.indexSf2, idInst.indexElt);

    // Ranges of the neighbors extended
    QMap<int, int> keysAfterRemoval = model.getKeysAfterRemoval(positionKey);
    int highestForPrevious = -1, lowestForNext = 128;
    foreach (int key, keysAfterRemoval.keys())
    {
        if (keysAfterRemoval[key] == position.previous && position.previous >= 0)
            highestForPrevious = qMax(highestForPrevious, key);
        else if (keysAfterRemoval[key] == position.next && position.next >= 0)
            lowestForNext = qMin(lowestForNext, key);
    }
    AttributeValue value;
    if (highestForPrevious >= 0)
    {
        foreach (int index, model.getClosestDivisions(position.previous, highestForPrevious))
        {
            idDivision.indexElt2 = model.getDivisions()[index].index;
            RangesType range = getKeyRange(sm, idDivision);
            if (range.byHi < highestForPrevious)
            {
                range.byHi = static_cast<quint8>(highestForPrevious);
                value.rValue = range;
                sm->set(idDivision, champ_keyRange, value);
            }
        }
    }
    if (lowestForNext < 128)
    {
        foreach (int index, model.getClosestDivisions(position.next, lowestForNext))
        {
            idDivision.indexElt2 = model.getDivisions()[index].index;
            RangesType range = getKeyRange(sm, idDivision);
            if (range.byLo > lowestForNext)
            {
                range.byLo = static_cast<quint8>(lowestForNext);
                value.rValue = range;
                sm->set(idDivision, champ_keyRange, value);
            }
        }
    }

    // Divisions removed, then the samples not used anymore
    QList<int> samples;
    foreach (int index, position.divisions)
    {
        idDivision.indexElt2 = model.getDivisions()[index].index;
        sm->remove(idDivision);
        int sampleIndex = model.getDivisions()[index].sampleIndex;
        if (!samples.contains(sampleIndex))
            samples << sampleIndex;
    }
    if (removeUnusedSamples)
    {
        foreach (int sampleIndex, samples)
        {
            int message = 1;
            sm->remove(EltID(elementSmpl, idInst.indexSf2, sampleIndex), &message); // Nothing done if still used
        }
    }
}

double TimbreActions::changeAttenuation(SoundfontManager * sm, EltID idInst, const QList<int> &divisions, double delta)
{
    double applied = std::fabs(delta);
    EltID idGlobal(elementInst, idInst.indexSf2, idInst.indexElt);
    EltID idDivision(elementInstSmpl, idInst.indexSf2, idInst.indexElt);
    foreach (int index, divisions)
    {
        idDivision.indexElt2 = index;
        double attenuation = 0;
        if (sm->isSet(idDivision, champ_initialAttenuation))
            attenuation = Attribute::toRealValue(champ_initialAttenuation, false, sm->get(idDivision, champ_initialAttenuation));
        else if (sm->isSet(idGlobal, champ_initialAttenuation))
            attenuation = Attribute::toRealValue(champ_initialAttenuation, false, sm->get(idGlobal, champ_initialAttenuation));
        double newValue = qBound(0.0, attenuation + delta, 144.0);
        AttributeValue value = Attribute::fromRealValue(champ_initialAttenuation, false, newValue);
        sm->set(idDivision, champ_initialAttenuation, value);
        applied = qMin(applied, std::fabs(Attribute::toRealValue(champ_initialAttenuation, false, value) - attenuation));
    }
    return applied;
}

TimbreCorrectionPlan TimbreActions::planCorrection(TimbreModel &model, int positionKey,
                                                   const TimbreCorrectionParameters &parameters)
{
    TimbreCorrectionPlan plan;
    plan.idInst = model.getInstrument();
    plan.positionKey = positionKey;
    if (!model.getPositions().contains(positionKey))
        return plan;
    const TimbrePosition position = model.getPositions()[positionKey];
    foreach (int index, position.divisions)
    {
        const TimbreDivision &division = model.getDivisions()[index];
        plan.divisions << division.index;
        if (!plan.samples.contains(division.sampleIndex))
            plan.samples << division.sampleIndex;
    }
    QMap<int, QSharedPointer<const TimbreSampleAnalysis> > analyses =
            TimbreAnalysisCache::getInstance()->getNow(plan.idInst.indexSf2, plan.samples);
    foreach (int sampleIndex, plan.samples)
    {
        if (!analyses.contains(sampleIndex) || !analyses[sampleIndex]->pitchFound)
            return plan;
        plan.fundamentals[sampleIndex] = analyses[sampleIndex]->fundamental;
    }

    // Target
    bool atEdge;
    switch (parameters.reference)
    {
    case TimbreCorrectionParameters::REFERENCE_POSITION:
        if (!model.getPositions().contains(parameters.alignedPosition) || parameters.alignedPosition == positionKey)
            return plan;
        plan.target = model.playPosition(parameters.alignedPosition, positionKey);
        break;
    case TimbreCorrectionParameters::REFERENCE_REGRESSION:
        plan.target = model.computeReferenceWith(positionKey, TimbreSettings::REFERENCE_REGRESSION,
                                                 parameters.regressionCount, atEdge);
        break;
    default:
        plan.target = model.computeReferenceWith(positionKey, TimbreSettings::REFERENCE_INTERPOLATION, 0, atEdge);
        break;
    }
    if (plan.target.isEmpty())
        return plan;

    // Macros
    for (int i = 0; i < plan.target.count(); i++)
    {
        int rank = i + 1;
        double gain = parameters.brightness * std::log2(rank) + ((rank >= 3 && rank % 2 == 1) ? parameters.oddEven : 0);
        plan.target.power[i] *= std::pow(10.0, gain / 10);
    }

    // Gains of the rows (timbre part, the level being removed): what exceeds the tolerance, with the strength, bounded
    const TimbreSpectrum &heard = position.spectrum;
    plan.before = model.compare(heard, plan.target);
    if (!plan.before.valid)
        return plan;
    const QList<TimbreRow> &rows = model.getRows();
    QVector<double> timbreGains(rows.size(), 0);
    QVector<bool> corrected(rows.size(), false);
    if (parameters.correctTimbre)
    {
        for (int row = 0; row < rows.size(); row++)
        {
            if (parameters.maxRank > 0 && rows[row].lastRank > parameters.maxRank)
                continue;
            double difference = plan.before.rowDifferences[row];
            if (std::isnan(difference))
                continue;
            corrected[row] = true;
            difference -= plan.before.levelDifference;
            double excess = std::fabs(difference) - parameters.tolerance;
            if (excess > 0)
                timbreGains[row] = qBound(-parameters.maxCorrection, -std::copysign(excess, difference) * parameters.strength,
                                          parameters.maxCorrection);
        }
    }

    // Points of the gain curve (abscissa: rank), constant within the bands
    bool neutral = true;
    int lastRank = 0;
    for (int row = 0; row < rows.size(); row++)
    {
        if (!corrected[row])
            continue;
        if (timbreGains[row] != 0)
            neutral = false;
        plan.gains << QPointF(rows[row].firstRank, timbreGains[row]);
        if (rows[row].lastRank > rows[row].firstRank)
            plan.gains << QPointF(rows[row].lastRank, timbreGains[row]);
        lastRank = rows[row].lastRank;
    }
    if (parameters.maxRank > 0 && lastRank > 0)
        plan.gains << QPointF(lastRank + 0.5, 0); // Nothing changed above
    if (neutral)
        plan.gains.clear();

    // Expected result, then the level
    GainCurve curve(GainCurve::MODE_GAIN, true, plan.gains);
    plan.after = heard;
    if (!plan.gains.isEmpty())
        for (int i = 0; i < plan.after.count(); i++)
            plan.after.power[i] *= std::pow(10.0, curve.getDb(i + 1) / 10);
    if (parameters.correctLevel)
    {
        TimbreComparison comparison = model.compare(plan.after, plan.target);
        double excess = std::fabs(comparison.levelDifference) - parameters.tolerance;
        if (comparison.valid && excess > 0)
            plan.levelGain = qBound(-parameters.maxCorrection,
                                    -std::copysign(excess, comparison.levelDifference) * parameters.strength,
                                    parameters.maxCorrection);
        for (int i = 0; i < plan.after.count(); i++)
            plan.after.power[i] *= std::pow(10.0, plan.levelGain / 10);

        // A level increase beyond what the attenuation allows (it cannot be negative) goes in the data, within the full
        // scale
        SoundfontManager * sm = SoundfontManager::getInstance();
        double available = 144;
        EltID idGlobal = plan.idInst;
        EltID idDivision(elementInstSmpl, plan.idInst.indexSf2, plan.idInst.indexElt);
        foreach (int index, plan.divisions)
        {
            idDivision.indexElt2 = index;
            double attenuation = 0;
            if (sm->isSet(idDivision, champ_initialAttenuation))
                attenuation = Attribute::toRealValue(champ_initialAttenuation, false, sm->get(idDivision, champ_initialAttenuation));
            else if (sm->isSet(idGlobal, champ_initialAttenuation))
                attenuation = Attribute::toRealValue(champ_initialAttenuation, false, sm->get(idGlobal, champ_initialAttenuation));
            available = qMin(available, attenuation);
        }
        if (plan.levelGain > available + 0.01)
        {
            double excess = plan.levelGain - available;
            plan.levelGain = available;
            if (plan.gains.isEmpty())
                plan.gains << QPointF(1, excess);
            else
                for (int i = 0; i < plan.gains.count(); i++)
                    plan.gains[i].setY(plan.gains[i].y() + excess);
        }
    }
    plan.expected = model.compare(plan.after, plan.target);

    // Gains per row, for the display
    QVector<double> levelsBefore = model.getRowLevels(heard), levelsAfter = model.getRowLevels(plan.after);
    plan.rowGains.fill(0, rows.size());
    for (int row = 0; row < rows.size(); row++)
        if (!std::isnan(levelsBefore[row]) && !std::isnan(levelsAfter[row]))
            plan.rowGains[row] = levelsAfter[row] - levelsBefore[row];
    plan.valid = !plan.gains.isEmpty() || plan.levelGain != 0;
    return plan;
}

QList<TimbreCorrectedSample> TimbreActions::computeCorrection(int indexSf2, const TimbreCorrectionPlan &plan,
                                                              double &levelCompensation)
{
    QList<TimbreCorrectedSample> result;
    levelCompensation = 0;
    if (plan.gains.isEmpty())
        return result;
    SoundfontManager * sm = SoundfontManager::getInstance();
    GainCurve curve(GainCurve::MODE_GAIN, true, plan.gains);

    // Same scale for all samples (stereo balance kept) if one of them goes above the full scale
    QList<double> peaks;
    foreach (int sampleIndex, plan.samples)
    {
        EltID idSmpl(elementSmpl, indexSf2, sampleIndex);
        QVector<float> data;
        quint32 sampleRate, loopStart, loopEnd;
        {
            QMutexLocker locker(sm->getMutex());
            data = sm->getDataFloat(idSmpl);
            sampleRate = sm->get(idSmpl, champ_dwSampleRate).dwValue;
            loopStart = sm->get(idSmpl, champ_dwStartLoop).dwValue;
            loopEnd = sm->get(idSmpl, champ_dwEndLoop).dwValue;
        }
        if (data.isEmpty() || sampleRate == 0)
            continue;
        if (!SampleUtils::isLooped(loopStart, loopEnd, static_cast<quint32>(data.size())))
            loopStart = loopEnd = 0;
        double peakAfter = 1;
        TimbreCorrectedSample sample;
        sample.sampleIndex = sampleIndex;
        sample.data = curve.apply(data, sampleRate, plan.fundamentals.value(sampleIndex, 0), loopStart, loopEnd,
                                  nullptr, &peakAfter);
        peaks << peakAfter;
        result << sample;
    }
    double maxPeak = 1;
    foreach (double peak, peaks)
        maxPeak = qMax(maxPeak, peak);
    for (int i = 0; i < result.count(); i++)
    {
        double factor = qMax(1.0, peaks[i]) / maxPeak; // The gain curve already scaled each sample to the full scale
        if (std::fabs(factor - 1) > 1e-9)
        {
            float * data = result[i].data.data();
            for (int j = 0; j < result[i].data.size(); j++)
                data[j] = static_cast<float>(data[j] * factor);
        }
    }
    levelCompensation = 20 * std::log10(maxPeak);
    return result;
}

QList<int> TimbreActions::getSharedSamples(SoundfontManager * sm, EltID idInst, const QList<int> &divisions,
                                           const QList<int> &samples)
{
    QList<int> shared;
    EltID id(elementInst, idInst.indexSf2);
    foreach (int instIndex, sm->getSiblings(id))
    {
        EltID idDivision(elementInstSmpl, idInst.indexSf2, instIndex);
        foreach (int divIndex, sm->getSiblings(idDivision))
        {
            idDivision.indexElt2 = divIndex;
            if (instIndex == idInst.indexElt && divisions.contains(divIndex))
                continue;
            int sampleIndex = sm->get(idDivision, champ_sampleID).wValue;
            if (samples.contains(sampleIndex) && !shared.contains(sampleIndex))
                shared << sampleIndex;
        }
    }
    return shared;
}

void TimbreActions::applyCorrection(SoundfontManager * sm, const TimbreCorrectionPlan &plan,
                                    const QList<TimbreCorrectedSample> &samples, double levelCompensation,
                                    bool duplicateShared, QStringList &warnings)
{
    EltID idInst = plan.idInst;
    QMap<int, int> replacements; // Sample => sample written
    foreach (int sampleIndex, plan.samples)
        replacements[sampleIndex] = sampleIndex;

    if (!samples.isEmpty() && duplicateShared)
    {
        // A stereo pair is duplicated together
        QList<int> shared = getSharedSamples(sm, idInst, plan.divisions, plan.samples);
        foreach (int sampleIndex, plan.samples)
        {
            EltID idSmpl(elementSmpl, idInst.indexSf2, sampleIndex);
            SFSampleLink type = sm->get(idSmpl, champ_sfSampleType).sfLinkValue;
            int link = sm->get(idSmpl, champ_wSampleLink).wValue;
            if (type != monoSample && type != RomMonoSample && shared.contains(link) && plan.samples.contains(link) &&
                !shared.contains(sampleIndex))
                shared << sampleIndex;
        }
        if (!shared.isEmpty())
        {
            Duplicator duplicator;
            duplicator.setNonInteractive();
            foreach (int sampleIndex, plan.samples)
            {
                if (!shared.contains(sampleIndex))
                    continue;
                EltID idNew = duplicator.duplicate(EltID(elementSmpl, idInst.indexSf2, sampleIndex));
                if (idNew.typeElement == elementSmpl)
                    replacements[sampleIndex] = idNew.indexElt;
            }

            // The divisions of the position use the copies
            EltID idDivision(elementInstSmpl, idInst.indexSf2, idInst.indexElt);
            AttributeValue value;
            foreach (int divIndex, plan.divisions)
            {
                idDivision.indexElt2 = divIndex;
                int sampleIndex = sm->get(idDivision, champ_sampleID).wValue;
                if (replacements.contains(sampleIndex) && replacements[sampleIndex] != sampleIndex)
                {
                    value.wValue = static_cast<quint16>(replacements[sampleIndex]);
                    sm->set(idDivision, champ_sampleID, value);
                }
            }
        }
    }

    // New data
    foreach (TimbreCorrectedSample sample, samples)
        sm->setData(EltID(elementSmpl, idInst.indexSf2, replacements.value(sample.sampleIndex, sample.sampleIndex)),
                    sample.data);

    // Level: the correction, and what was removed from the data for staying below the full scale
    double delta = -plan.levelGain - levelCompensation;
    if (std::fabs(delta) > 0.001)
    {
        double applied = changeAttenuation(sm, idInst, plan.divisions, delta);
        if (applied < std::fabs(delta) - 0.05)
            warnings << tr("The attenuation reached its limit: the level differs by %1 dB from the expected one.")
                        .arg(QLocale::system().toString(std::fabs(delta) - applied, 'f', 1));
    }
}

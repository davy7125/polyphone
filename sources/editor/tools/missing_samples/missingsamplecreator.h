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

#ifndef MISSINGSAMPLECREATOR_H
#define MISSINGSAMPLECREATOR_H

#include "basetypes.h"
#include <QVector>
#include <QList>
#include <QMap>
#include <QMutex>
#include <QPointF>
#include <QAtomicInt>
#include <QSharedPointer>
#include <vector>
class SoundfontManager;

// Division of an instrument used for creating a missing sample
struct MissingSampleSource
{
    EltID idDivision; // elementInstSmpl
    EltID idSample;
    double attenuation; // dB (value of the division or of the instrument)
    bool looped; // Loop playback with loop points defined in the sample
    int keyMin, keyMax; // Key range of the division
};

// Divisions of an instrument sharing a root key
struct MissingSamplePosition
{
    int key = -1;
    QList<MissingSampleSource> sources;

    bool isLooped() const; // True if all divisions are looped

    // One division per distinct sample, the closest to the root key (several divisions may play the same sample on
    // different keys): their sound is the one of the root key, and the new divisions copy them
    QList<MissingSampleSource> getMainSources() const;
};

// Sample to create at a key of an instrument: the closest root key ("closest") is transposed and the levels of its
// harmonics are corrected according to a second root key ("second"), by interpolation if the key is between them,
// otherwise by extrapolation
struct MissingSampleJob
{
    EltID idInst;
    int key = -1;
    MissingSamplePosition closest;
    MissingSamplePosition second;

    // Result of the computation
    struct NewSample
    {
        EltID idSource; // Sample of the closest root key that is transposed
        QVector<float> data;
        quint32 sampleRate = 0;
        int correction = 0; // Pitch correction (cents) of the source, kept
        quint32 loopStart = 0;
        quint32 loopEnd = 0;
    };
    QList<NewSample> samples; // One per distinct sample of the closest root key
    double levelCompensation = 0; // dB removed from all samples for staying below the full scale
    bool harmonicsFound = false; // False if only the level could be interpolated
    bool loopRequired = false; // Both root keys are looped
    bool loopFound = false;
};

// Harmonic levels of the divisions of a root key
struct MissingSampleAnalysis
{
    QVector<double> power; // Mean power of the divisions per rank (index = rank - 1), attenuation included
    QVector<bool> found; // The harmonic stands out in at least one division
    double loudness = 0; // Mean steady loudness of the divisions (LUFS), attenuation included, NaN if not measured
    QMap<int, double> fundamentals; // Index of a sample => fundamental frequency measured (Hz, 0 if not found)
};

// Analyses shared by the jobs of a run (thread safe)
class MissingSampleAnalysisCache
{
public:
    QSharedPointer<MissingSampleAnalysis> get(EltID idInst, const MissingSamplePosition &position);

private:
    QMutex _mutex;
    QMap<QString, QSharedPointer<MissingSampleAnalysis> > _analyses;
};

// Jobs of a run, computed by several threads (each one writing only its job)
class MissingSampleRun
{
public:
    std::vector<MissingSampleJob> jobs;
    MissingSampleAnalysisCache cache;
    QAtomicInt canceled;
};

// Creation of the samples missing in an instrument
class MissingSampleCreator
{
public:
    // Divisions of an instrument by root key (overriding root key of the division or of the instrument, otherwise
    // original pitch of the sample), root keys above 127 being ignored. With a velocity (0 to 127), only the divisions
    // of the subset playing at this velocity (see isInSubset).
    static QMap<int, MissingSamplePosition> getPositions(SoundfontManager * sm, EltID idInst, int velocity = -1,
                                                         bool includeMuted = false);

    // True if a division is in the subset of an instrument playing at a velocity: playable (not hidden, not muted
    // unless includeMuted), its velocity range containing the velocity, not played only at the release (sample mode 2),
    // with a sample
    static bool isInSubset(SoundfontManager * sm, EltID idDivision, int velocity, bool includeMuted);

    // True if all divisions of an instrument have the same velocity range
    static bool hasSingleVelocityRange(SoundfontManager * sm, EltID idInst);

    // Keys to add so that two consecutive root keys are at most maxSpacing semitones apart, evenly spread
    static QList<int> getKeysToFill(const QList<int> &rootKeys, int maxSpacing);

    // Job for a key without sample (at least two positions): the closest root key is transposed (the higher one if
    // two root keys are at the same distance), the second one is on the other side of the key if there is one,
    // otherwise the next one on the same side
    static MissingSampleJob createJob(EltID idInst, int key, const QMap<int, MissingSamplePosition> &positions);

    // Harmonic levels of the divisions of a root key (the soundfont is only read)
    static MissingSampleAnalysis analyze(const MissingSamplePosition &position);

    // Compute the samples of a job (the soundfont is only read)
    static void compute(MissingSampleJob &job, MissingSampleAnalysisCache &cache);

    // Gains (dB) per harmonic rank (x = rank) for the closest root key: difference of level with the second root key
    // multiplied by the ratio (key - closest key) / (second key - closest key), for the ranks found in both
    static QList<QPointF> getGains(const MissingSampleAnalysis &closest, const MissingSampleAnalysis &second, double ratio);

    // Create the samples and the divisions of a computed job, return the new divisions; newSamples receives the samples
    // created and notLooped the names of the samples that could not be looped
    static QList<EltID> apply(SoundfontManager * sm, const MissingSampleJob &job, QList<EltID> &newSamples,
                              QStringList &notLooped);

    // Adjust the key ranges around the new root keys: the ranges of the existing divisions can only shrink (limits as
    // in the tool "auto distribution") and the new divisions take the keys up to the neighbors, the lowest and highest
    // keys of the instrument being kept
    static void adjustKeyRanges(SoundfontManager * sm, const QMap<int, QList<EltID> > &existingDivisions,
                                const QMap<int, QList<EltID> > &newDivisions);

    // Remove the existing divisions whose key range is entirely within the range of a new division (one division per
    // key for instance: they could not shrink and would be played with the new note)
    static void removeCoveredDivisions(SoundfontManager * sm, const QMap<int, QList<EltID> > &existingDivisions,
                                       const QMap<int, QList<EltID> > &newDivisions);

    // Name of a new sample: the key written in the name of the source (number, possibly with 3 digits, or note name) is
    // replaced, otherwise the key is added at the end
    static QString getSampleName(QString sourceName, int sourceKey, int key);

private:
    static void loop(MissingSampleJob &job);
};

#endif // MISSINGSAMPLECREATOR_H

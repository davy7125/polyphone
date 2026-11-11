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


#ifndef TIMBREMODEL_H
#define TIMBREMODEL_H

#include "basetypes.h"
#include "timbreanalysis.h"
#include <QVector>
#include <QList>
#include <QMap>
#include <QHash>
#include <QString>
class SoundfontManager;
class EngineVoiceParameters;

// Settings of the timbre view, shared by the display, the deviations and the corrections
struct TimbreSettings
{
    int velocity = 100;

    // Partials: ranks displayed one by one, then bands grouping the next ranks (third of octave or octave in rank)
    // up to an upper frequency (0: half of the sample rate), the ranks too far below the strongest one being ignored
    int detailedRanks = 10;
    bool octaveBands = false;
    double upperLimit = 16000;
    double dynamicRange = 60;

    // Reference of a position: interpolation between the neighbors, or robust local regression on n neighbors on each side
    enum Reference
    {
        REFERENCE_INTERPOLATION = 0,
        REFERENCE_REGRESSION = 1
    };
    Reference reference = REFERENCE_INTERPOLATION;
    int regressionCount = 3;

    // Thresholds (dB) of the labels, calibrated on organ sample sets (natural deviation of the root keys from their
    // reference: median 3.7 dB, 70% below 4.5 dB, 95% below 8 dB; redundancy below 2.5 dB for 15% of them; timbre
    // jumps at the changes of samples below 6 dB for 80% of them; level deviations below 2.5 dB for 78% of them)
    double tolerance = 4.5;
    double maximum = 8;
    double redundancy = 2.5;
    double jump = 6;
    double level = 2.5;

    // Muted divisions are included (the mute follows the selection with the solo on selection)
    bool includeMuted = false;

    // Sample rate of the output
    double outputSampleRate = 48000;
};

// Power of each rank of a sound heard at a key (index = rank - 1)
struct TimbreSpectrum
{
    enum State
    {
        ABSENT = 0, // Not in the band (sample rate of the sample or of the output, upper limit) or not measured
        DOUBTFUL = 1, // The peak doesn't stand out
        MEASURED = 2
    };

    QVector<double> power; // Linear (amplitude²)
    QVector<char> state;

    bool isEmpty() const { return power.isEmpty(); }
    int count() const { return static_cast<int>(power.size()); }
    void add(const TimbreSpectrum &other);
    double strongest() const; // Largest power of the present ranks
    double totalDb() const; // NaN if empty
};

// Row of the map: a rank, or a band of ranks
struct TimbreRow
{
    int firstRank;
    int lastRank;
    QString label;
};

// Difference between a sound and a reference
struct TimbreComparison
{
    bool valid = false;
    double levelDifference = 0; // ΔL, dB
    double timbreDifference = 0; // ΔT, dB (weighted quadratic mean of the differences by row, level removed)
    double confidence = 0; // Part of the power of the reference in the ranks compared
    QVector<double> rowDifferences; // dB per row, level included (NaN if not compared)
    QVector<double> rowWeights;
};

// Division of the active subset
struct TimbreDivision
{
    int index; // indexElt2 of the division
    int sampleIndex;
    int rootKey;
    int keyMin, keyMax;
    int velMin, velMax;
};

// Parameters of a division played at a key, steady state (envelopes at their sustain level, LFOs at their mean value)
struct TimbreVoice
{
    bool valid = false;
    int divisionIndex = -1;
    int sampleIndex = -1;
    double gainDb = 0; // Attenuation, sustain of the volume envelope and compensation of the resonance, negative
    double attenuation = 0; // dB (real value of the engine)
    double filterFrequency = 0; // Hz
    double filterQ = 0; // dB
    double transposition = 0; // Cents
    double fundamental = 0; // Hz, played
};

// Root key of the subset, with all the divisions having this root key
struct TimbrePosition
{
    int key;
    QList<int> divisions; // Indexes in TimbreModel::getDivisions()
    int keyMin, keyMax; // Keys covered by its divisions
    TimbreSpectrum spectrum; // Played at its root key
    TimbreSpectrum reference; // Predicted by the neighbors, played at the root key
    bool referenceAtEdge = false; // Extrapolation from the neighbors on one side
    TimbreComparison deviation; // spectrum against reference
    bool redundancyComputed = false;
    double redundancy = 0; // D: largest ΔT over the keys of the position if it were removed
    double redundancyLevel = 0; // Largest |ΔL| over the same keys
    TimbreComparison newJunction; // Jump that would appear between the neighbors
    int previous = -1, next = -1; // Keys of the neighbors
    bool analyzed = false; // All analyses of its samples are available

    enum Label
    {
        LABEL_NONE,
        LABEL_CONFORM,
        LABEL_REDUNDANT,
        LABEL_TO_CORRECT,
        LABEL_TOO_DIFFERENT
    };
    Label label = LABEL_NONE;
    bool levelLabel = false;
};

// What is heard at a key
struct TimbreKey
{
    QList<int> sources; // Indexes in TimbreModel::getDivisions()
    QList<TimbreVoice> voices;
    TimbreSpectrum spectrum;
    bool analyzed = false; // All analyses of its sources are available
    int position = -1; // Root key of the position of the main source
    double totalLevel = 0; // dB, NaN if nothing
    double centroid = 0; // Brightness, in ranks
    double slope = 0; // dB per octave (in rank)
    double oddEven = 0; // dB, odd ranks (3, 5...) against even ranks (2, 4...) among the detailed ranks
    double transposition = 0; // Semitones
    double durationFactor = 1;
    double tuning = 0; // Cents from the equal temperament
    double hnr = 0; // dB
    double loopCorrelation = 0;
    TimbreComparison jump; // With the previous key, if both are covered
    bool junction = false; // The sources differ from the previous key
    bool sampleChange = false; // The samples differ from the previous key
    bool breakLabel = false; // Junction with a break that a computed note could fix
};

// Timbre of an instrument along the keyboard, as heard at a velocity (see SPECIFICATION_VUE_TIMBRE)
class TimbreModel
{
public:
    TimbreModel();
    ~TimbreModel();

    static const double WEIGHT_EXPONENT;
    static const double WEIGHT_FLOOR;

    // True if a division of an instrument is in the active subset: playable (not hidden, not muted unless
    // includeMuted), its velocity range containing the velocity, not played at the release only, with a sample
    static bool isInSubset(SoundfontManager * sm, EltID idDivision, int velocity, bool includeMuted);

    // Compute everything (the analyses not available are started in the background if analyzeNow is false, otherwise
    // computed before)
    void compute(EltID idInst, const TimbreSettings &settings, bool analyzeNow = false);

    // Compute again the labels only, after a change of the thresholds
    void applyThresholds(const TimbreSettings &settings);

    EltID getInstrument() const { return _idInst; }
    const TimbreSettings &getSettings() const { return _settings; }
    const QList<TimbreRow> &getRows() const { return _rows; }
    const QList<TimbreDivision> &getDivisions() const { return _divisions; }
    const QMap<int, TimbrePosition> &getPositions() const { return _positions; }
    const TimbreKey &getKey(int key) const { return _keys[qBound(0, key, 127)]; }
    int getMinKey() const { return _minKey; } // Lowest and highest keys covered, -1 if nothing
    int getMaxKey() const { return _maxKey; }
    bool isComplete() const { return _missingAnalyses == 0; }

    // Values per row of a spectrum: level (dB, NaN if absent) and state (worst state of the ranks of the row)
    QVector<double> getRowLevels(const TimbreSpectrum &spectrum) const;
    QVector<int> getRowStates(const TimbreSpectrum &spectrum) const;
    QVector<double> getRowConfidence(const TimbreSpectrum &spectrum) const; // Part of the power well measured, NaN if absent

    // Comparison of a sound with a reference, by rows
    TimbreComparison compare(const TimbreSpectrum &test, const TimbreSpectrum &reference) const;

    // Sound predicted at a key by the positions around it (for a computed note), excluding a position (-1: none)
    TimbreSpectrum predict(int key, int excludedPosition = -1, bool * atEdge = nullptr);

    // Reference of a position (or sound predicted at a free key) with another mode than the one of the settings
    TimbreSpectrum computeReferenceWith(int key, TimbreSettings::Reference mode, int regressionCount, bool &atEdge,
                                        int excludedPosition = -1);

    // Sound of divisions of a position played at a key: the divisions covering the key, otherwise the closest ones
    TimbreSpectrum playPosition(int positionKey, int key, QList<TimbreVoice> * voices = nullptr);

    // Keys that a removed position would leave to each neighbor: key => key of the neighbor, -1 if none
    QMap<int, int> getKeysAfterRemoval(int positionKey) const;

    // Divisions of a position closest to a key (all divisions covering the key, otherwise the closest ones)
    QList<int> getClosestDivisions(int positionKey, int key) const;

    // Name of a row: "H1" for the rank 1, "11-12" for a band
    static QList<TimbreRow> computeRows(int detailedRanks, bool octaveBands, int maxRank);

private:
    void clear();
    void readSubset(SoundfontManager * sm);
    TimbreVoice getVoice(int divisionIndex, int key);
    TimbreSpectrum getSpectrum(const TimbreVoice &voice);
    TimbreSpectrum computeReference(int positionKey, bool &atEdge, int excludedPosition = -1);
    void computeKeyDescriptors(TimbreKey &key, int keyNumber);
    void computeRedundancy(TimbrePosition &position);
    bool isValidRank(const TimbreSpectrum &spectrum, int index, double threshold) const;

    EltID _idInst;
    TimbreSettings _settings;
    QList<TimbreRow> _rows;
    QList<TimbreDivision> _divisions;
    QMap<int, TimbrePosition> _positions;
    TimbreKey _keys[128];
    int _minKey, _maxKey;
    int _maxRank;
    int _missingAnalyses;
    QMap<int, QSharedPointer<const TimbreSampleAnalysis> > _analyses; // By sample index
    QHash<qint64, TimbreVoice> _voiceCache; // (division, key) => voice
    EngineVoiceParameters * _engine;
};

#endif // TIMBREMODEL_H

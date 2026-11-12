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


#ifndef TIMBREACTIONS_H
#define TIMBREACTIONS_H

#include "basetypes.h"
#include "timbremodel.h"
#include "missing_samples/missingsamplecreator.h"
#include <QCoreApplication>
#include <QPointF>
class SoundfontManager;

// Note computed at a key of the subset, from the closest root key and a second one
struct TimbreCreation
{
    MissingSampleJob job;
    QMap<int, QList<EltID> > existingDivisions; // Divisions of the subset by root key (without the position replaced)
    bool extrapolationWarning = false; // The key is far from the root keys used
};

// Correction of the timbre and of the level of a position
struct TimbreCorrectionParameters
{
    enum Reference
    {
        REFERENCE_INTERPOLATION = 0,
        REFERENCE_REGRESSION = 1,
        REFERENCE_POSITION = 2 // Aligned on another position played at the root key
    };
    Reference reference = REFERENCE_INTERPOLATION;
    int regressionCount = 3;
    int alignedPosition = -1;
    bool correctTimbre = true;
    bool correctLevel = true;
    double tolerance = 1.5; // dB: dead zone
    double strength = 1.0; // 0 to 1
    double maxCorrection = 12; // dB on each side
    int maxRank = 0; // 0: all rows (detailed ranks and bands), otherwise the ranks 1 to maxRank
    double brightness = 0; // dB per octave (in rank) added to the target
    double oddEven = 0; // dB added to the odd ranks (3, 5...) of the target
};

struct TimbreCorrectionPlan
{
    bool valid = false;
    EltID idInst;
    int positionKey = -1;
    QList<int> divisions; // indexElt2
    QList<int> samples;
    QMap<int, double> fundamentals; // By sample: measured fundamental (Hz)
    QList<QPointF> gains; // Rank => dB, for GainCurve (note tracking), empty if the timbre is not changed
    double levelGain = 0; // dB, through the attenuation
    TimbreSpectrum target, after;
    QVector<double> rowGains; // dB per row, level included
    TimbreComparison before, expected; // Heard and expected result against the target
};

struct TimbreCorrectedSample
{
    int sampleIndex;
    QVector<float> data;
};

class TimbreActions
{
    Q_DECLARE_TR_FUNCTIONS(TimbreActions)

public:
    // Choice for the samples also used outside of a corrected position: -1 to ask, otherwise SHARED_DUPLICATE or
    // SHARED_MODIFY_ALL (remembered until Polyphone is closed if the user asked not to be asked again)
    enum SharedChoice
    {
        SHARED_ASK = -1,
        SHARED_DUPLICATE = 0,
        SHARED_MODIFY_ALL = 1
    };
    static int s_sharedChoice;

    // Creation of a note at a key of the subset (closest and second root keys chosen automatically if -1), a position
    // being possibly excluded (replacement); computeCreation reads the soundfont only and can be called by a thread
    static bool prepareCreation(SoundfontManager * sm, EltID idInst, const TimbreSettings &settings, int key, int closestKey,
                                int secondKey, int excludedPosition, TimbreCreation &creation, QString &error);
    static void computeCreation(TimbreCreation &creation);
    static QList<EltID> applyCreation(SoundfontManager * sm, const TimbreCreation &creation, QStringList &warnings);

    // Removal of the divisions of a position, the neighbors taking its keys (getKeysAfterRemoval); the samples not used
    // anymore are possibly removed too
    static void removePosition(SoundfontManager * sm, const TimbreModel &model, int positionKey, bool removeUnusedSamples);

    // Attenuation of divisions changed by a delta (dB, positive: quieter), within 0 and 144 dB; return the smallest
    // absolute change applied
    static double changeAttenuation(SoundfontManager * sm, EltID idInst, const QList<int> &divisions, double delta);

    // Correction of a position: plan (gains), computation of the new data (reads only, any thread), application
    static TimbreCorrectionPlan planCorrection(TimbreModel &model, int positionKey, const TimbreCorrectionParameters &parameters);
    static QList<TimbreCorrectedSample> computeCorrection(int indexSf2, const TimbreCorrectionPlan &plan,
                                                          double &levelCompensation);
    static void applyCorrection(SoundfontManager * sm, const TimbreCorrectionPlan &plan,
                                const QList<TimbreCorrectedSample> &samples, double levelCompensation,
                                bool duplicateShared, QStringList &warnings);

    // Samples of divisions of an instrument also used by other divisions (of any instrument)
    static QList<int> getSharedSamples(SoundfontManager * sm, EltID idInst, const QList<int> &divisions,
                                       const QList<int> &samples);
};

#endif // TIMBREACTIONS_H

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


#ifndef TIMBREANALYSIS_H
#define TIMBREANALYSIS_H

#include "basetypes.h"
#include <QObject>
#include <QMap>
#include <QPair>
#include <QVector>
#include <QSharedPointer>
#include <QThreadPool>
#include <QMutex>

// Measures of the steady part of a sample (the loop if there is one) used by the timbre view
struct TimbreSampleAnalysis
{
    bool pitchFound = false;
    double fundamental = 0; // Hz, measured
    double sampleRate = 0;
    QVector<double> levels; // dBFS of the sine of each rank (index = rank - 1), -inf if no peak
    QVector<double> frequencies; // Hz of each rank (partials possibly stretched)
    QVector<bool> found; // The peak stands out from its neighborhood
    double hnr = 0; // Harmonics to noise ratio (dB), NaN if not measured
    double inharmonicity = 0;
    double loopCorrelation = 0; // 1 = perfect seam, NaN if not looped
};

// Analyses of the samples, computed once in the background by several threads and kept until the data, the loop, the
// root key, the pitch correction or the sample rate of a sample change (they survive the changes of page)
class TimbreAnalysisCache : public QObject
{
    Q_OBJECT

public:
    static TimbreAnalysisCache * getInstance();

    // Highest rank and frequency measured (more than the default limits of the analyzer, for the low keys)
    static const int MAX_RANK_COUNT;
    static const double MAX_FREQUENCY;

    // Analysis of a sample for its current data, null if not computed yet: the computation is then started in the
    // background and analysisReady is emitted when it's done (main thread only)
    QSharedPointer<const TimbreSampleAnalysis> get(EltID idSmpl);

    // Same, the missing analyses being computed in parallel before returning (main thread only)
    QMap<int, QSharedPointer<const TimbreSampleAnalysis> > getNow(int indexSf2, const QList<int> &sampleIndexes);

    // Number of analyses being computed in the background
    int pendingCount();

    // Analysis of data (the soundfont is not read): can be called from any thread
    static TimbreSampleAnalysis analyze(const QVector<float> &data, quint32 sampleRate, quint32 loopStart,
                                        quint32 loopEnd, double expectedFrequency);

signals:
    void analysisReady(int indexSf2);

private slots:
    void onSoundfontClosed(int indexSf2);
    void onAnalysisFinished(int indexSf2, int indexSmpl, QByteArray fingerprint,
                            QSharedPointer<const TimbreSampleAnalysis> analysis);

private:
    TimbreAnalysisCache();
    QByteArray getFingerprint(EltID idSmpl);

    struct Entry
    {
        QByteArray fingerprint;
        QSharedPointer<const TimbreSampleAnalysis> analysis; // Null while it is computed
        bool pending = false;
    };
    QMap<QPair<int, int>, Entry> _entries;
    QThreadPool _pool;
    static TimbreAnalysisCache * s_instance;
};

#endif // TIMBREANALYSIS_H

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


#include "timbreanalysis.h"
#include "soundfontmanager.h"
#include "soundanalyzer.h"
#include "sampleutils.h"
#include "gaincurve.h"
#include <QRunnable>
#include <QDataStream>
#include <QCoreApplication>
#include <cmath>

const int TimbreAnalysisCache::MAX_RANK_COUNT = 400;
const double TimbreAnalysisCache::MAX_FREQUENCY = 20000;
TimbreAnalysisCache * TimbreAnalysisCache::s_instance = nullptr;

TimbreAnalysisCache * TimbreAnalysisCache::getInstance()
{
    if (s_instance == nullptr)
        s_instance = new TimbreAnalysisCache();
    return s_instance;
}

TimbreAnalysisCache::TimbreAnalysisCache() : QObject()
{
    qRegisterMetaType<QSharedPointer<const TimbreSampleAnalysis> >("QSharedPointer<const TimbreSampleAnalysis>");
    connect(SoundfontManager::getInstance(), SIGNAL(soundfontClosed(int)), this, SLOT(onSoundfontClosed(int)));

    // One thread left to the interface and the sound
    _pool.setMaxThreadCount(qMax(1, QThread::idealThreadCount() - 1));

    // The analyses read the soundfonts, deleted when the application quits
    connect(qApp, &QCoreApplication::aboutToQuit, this, [this]() {
        _pool.clear();
        _pool.waitForDone();
    });
}

TimbreSampleAnalysis TimbreAnalysisCache::analyze(const QVector<float> &data, quint32 sampleRate, quint32 loopStart,
                                                  quint32 loopEnd, double expectedFrequency)
{
    TimbreSampleAnalysis result;
    result.sampleRate = sampleRate;
    result.hnr = std::nan("");
    result.loopCorrelation = std::nan("");
    if (data.isEmpty() || sampleRate == 0)
        return result;
    if (!SampleUtils::isLooped(loopStart, loopEnd, static_cast<quint32>(data.size())))
        loopStart = loopEnd = 0;

    // The root key may not give the pitch (organ stops of 4', mutations, root key overridden by the divisions)
    expectedFrequency = SoundAnalyzer::adjustExpectedFrequency(data, sampleRate, loopStart, loopEnd, expectedFrequency);
    SoundAnalyzer analyzer(data, sampleRate, loopStart, loopEnd, expectedFrequency);
    analyzer.setHarmonicLimits(MAX_RANK_COUNT, MAX_FREQUENCY);
    QList<SoundAnalyzer::Harmonic> harmonics = analyzer.getHarmonicList();
    if (harmonics.isEmpty())
        return result;

    result.pitchFound = true;
    result.fundamental = analyzer.getFundamental();
    int count = 0;
    foreach (SoundAnalyzer::Harmonic harmonic, harmonics)
        count = qMax(count, harmonic.rank);
    result.levels.fill(-std::numeric_limits<double>::infinity(), count);
    result.frequencies.fill(0, count);
    result.found.fill(false, count);
    foreach (SoundAnalyzer::Harmonic harmonic, harmonics)
    {
        int index = harmonic.rank - 1;
        if (index < 0)
            continue;
        result.frequencies[index] = harmonic.frequency;
        if (harmonic.amplitude > 0)
            result.levels[index] = 20 * std::log10(harmonic.amplitude);
        result.found[index] = harmonic.found;
    }
    result.hnr = analyzer.getHnr();
    result.inharmonicity = analyzer.getInharmonicity();
    if (loopEnd > loopStart)
        result.loopCorrelation = analyzer.getLoopCorrelation();
    return result;
}

// Computation of an analysis in a thread of the pool
class RunnableTimbreAnalysis: public QRunnable
{
public:
    RunnableTimbreAnalysis(TimbreAnalysisCache * cache, EltID idSmpl, QByteArray fingerprint) :
        _cache(cache),
        _idSmpl(idSmpl),
        _fingerprint(fingerprint)
    {}

    void run() override
    {
        SoundfontManager * sm = SoundfontManager::getInstance();
        QVector<float> data;
        quint32 sampleRate = 0, loopStart = 0, loopEnd = 0;
        int rootKey = 60, correction = 0;
        {
            QMutexLocker locker(sm->getMutex());
            if (sm->isValid(_idSmpl))
            {
                data = sm->getDataFloat(_idSmpl);
                sampleRate = sm->get(_idSmpl, champ_dwSampleRate).dwValue;
                loopStart = sm->get(_idSmpl, champ_dwStartLoop).dwValue;
                loopEnd = sm->get(_idSmpl, champ_dwEndLoop).dwValue;
                rootKey = sm->get(_idSmpl, champ_byOriginalPitch).bValue;
                correction = sm->get(_idSmpl, champ_chPitchCorrection).cValue;
            }
        }

        // The expected frequency avoids octave errors
        QSharedPointer<const TimbreSampleAnalysis> analysis(new TimbreSampleAnalysis(TimbreAnalysisCache::analyze(
            data, sampleRate, loopStart, loopEnd, GainCurve::getFundamental(rootKey, correction))));
        QMetaObject::invokeMethod(_cache, "onAnalysisFinished", Qt::QueuedConnection,
                                  Q_ARG(int, _idSmpl.indexSf2), Q_ARG(int, _idSmpl.indexElt), Q_ARG(QByteArray, _fingerprint),
                                  Q_ARG(QSharedPointer<const TimbreSampleAnalysis>, analysis));
    }

private:
    TimbreAnalysisCache * _cache;
    EltID _idSmpl;
    QByteArray _fingerprint;
};

QByteArray TimbreAnalysisCache::getFingerprint(EltID idSmpl)
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    QMutexLocker locker(sm->getMutex()); // The data may be loaded by another thread at the same time
    if (!sm->isValid(idSmpl))
        return QByteArray();
    QByteArray result;
    QDataStream stream(&result, QIODevice::WriteOnly);
    stream << sm->get(idSmpl, champ_dwLength).dwValue << sm->get(idSmpl, champ_dwSampleRate).dwValue
           << sm->get(idSmpl, champ_dwStartLoop).dwValue << sm->get(idSmpl, champ_dwEndLoop).dwValue
           << static_cast<int>(sm->get(idSmpl, champ_byOriginalPitch).bValue)
           << static_cast<int>(sm->get(idSmpl, champ_chPitchCorrection).cValue);
    Sound * sound = sm->getSound(idSmpl);
    if (sound != nullptr)
        stream << sound->getDataHash();
    return result;
}

QSharedPointer<const TimbreSampleAnalysis> TimbreAnalysisCache::get(EltID idSmpl)
{
    QPair<int, int> key(idSmpl.indexSf2, idSmpl.indexElt);
    QByteArray fingerprint = getFingerprint(idSmpl);
    if (fingerprint.isEmpty())
        return QSharedPointer<const TimbreSampleAnalysis>();

    Entry &entry = _entries[key];
    if (entry.fingerprint == fingerprint)
        return entry.analysis; // Possibly null if pending

    // New computation (a pending one for previous data will be ignored)
    entry.fingerprint = fingerprint;
    entry.analysis.clear();
    entry.pending = true;
    _pool.start(new RunnableTimbreAnalysis(this, idSmpl, fingerprint));
    return QSharedPointer<const TimbreSampleAnalysis>();
}

QMap<int, QSharedPointer<const TimbreSampleAnalysis> > TimbreAnalysisCache::getNow(int indexSf2, const QList<int> &sampleIndexes)
{
    // Start the missing analyses, then wait for them (the results arrive through the event queue)
    QMap<int, QSharedPointer<const TimbreSampleAnalysis> > result;
    foreach (int index, sampleIndexes)
        get(EltID(elementSmpl, indexSf2, index));
    _pool.waitForDone();
    QCoreApplication::sendPostedEvents(this);

    foreach (int index, sampleIndexes)
    {
        QSharedPointer<const TimbreSampleAnalysis> analysis = get(EltID(elementSmpl, indexSf2, index));
        if (analysis.isNull())
        {
            // Data changed in the meantime: computed here
            EltID idSmpl(elementSmpl, indexSf2, index);
            QByteArray fingerprint = getFingerprint(idSmpl);
            RunnableTimbreAnalysis runnable(this, idSmpl, fingerprint);
            runnable.run();
            QCoreApplication::sendPostedEvents(this);
            analysis = get(idSmpl);
        }
        if (!analysis.isNull())
            result[index] = analysis;
    }
    return result;
}

int TimbreAnalysisCache::pendingCount()
{
    int count = 0;
    foreach (Entry entry, _entries.values())
        if (entry.pending)
            count++;
    return count;
}

void TimbreAnalysisCache::onAnalysisFinished(int indexSf2, int indexSmpl, QByteArray fingerprint,
                                             QSharedPointer<const TimbreSampleAnalysis> analysis)
{
    QPair<int, int> key(indexSf2, indexSmpl);
    if (!_entries.contains(key))
        return;
    Entry &entry = _entries[key];
    if (entry.fingerprint != fingerprint)
        return; // Result for previous data, another computation is running
    entry.analysis = analysis;
    entry.pending = false;
    emit analysisReady(indexSf2);
}

void TimbreAnalysisCache::onSoundfontClosed(int indexSf2)
{
    // The indexes of the soundfont can be used again by another one
    QList<QPair<int, int> > keys = _entries.keys();
    for (int i = 0; i < keys.count(); i++)
        if (keys[i].first == indexSf2)
            _entries.remove(keys[i]);
}

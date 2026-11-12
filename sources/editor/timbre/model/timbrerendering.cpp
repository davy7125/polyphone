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


#include "timbrerendering.h"
#include "offlinerenderer.h"
#include "soundanalyzer.h"
#include <cmath>

const double TimbreRendering::SAMPLE_RATE = 48000;

TimbreSpectrum TimbreRendering::render(EltID idInst, int key, int velocity, double expectedFrequency, QString &error)
{
    TimbreSpectrum result;
    QStringList lines;
    lines << QString("samplerate %1").arg(static_cast<int>(SAMPLE_RATE)) << "block 256" << "duration 3.0"
          << QString("0.0 instrument %1 %2 %3").arg(idInst.indexElt).arg(key).arg(velocity)
          << QString("2.6 instrument %1 %2 0").arg(idInst.indexElt).arg(key);
    OfflineRenderer renderer;
    QVector<float> data;
    quint32 frameCount = 0;
    if (!renderer.applyDefaultControllerValues(idInst.indexSf2) || !renderer.readEventLines(lines) ||
        !renderer.render(idInst.indexSf2, data, frameCount, OfflineRenderer::SYNTH_CORE))
    {
        error = renderer.getError();
        return result;
    }

    // Sustained part, after the attack
    int start = static_cast<int>(1.2 * SAMPLE_RATE);
    int end = qMin(static_cast<int>(frameCount), static_cast<int>(2.5 * SAMPLE_RATE));
    if (end - start < 4096)
        return result;
    QVector<float> left(end - start), right(end - start);
    for (int i = start; i < end; i++)
    {
        left[i - start] = data[2 * i];
        right[i - start] = data[2 * i + 1];
    }
    SoundAnalyzer analyzer(left, static_cast<quint32>(SAMPLE_RATE), 0, 0, expectedFrequency);
    analyzer.setSecondChannel(right);
    analyzer.setHarmonicLimits(TimbreAnalysisCache::MAX_RANK_COUNT, TimbreAnalysisCache::MAX_FREQUENCY);
    QList<SoundAnalyzer::Harmonic> harmonics = analyzer.getHarmonicList();
    int count = 0;
    foreach (SoundAnalyzer::Harmonic harmonic, harmonics)
        count = qMax(count, harmonic.rank);
    result.power.fill(0, count);
    result.state.fill(TimbreSpectrum::ABSENT, count);
    foreach (SoundAnalyzer::Harmonic harmonic, harmonics)
    {
        if (harmonic.rank < 1 || harmonic.amplitude <= 0)
            continue;
        result.power[harmonic.rank - 1] = 2 * harmonic.amplitude * harmonic.amplitude; // Mean power of the channels => sum
        result.state[harmonic.rank - 1] = harmonic.found ? TimbreSpectrum::MEASURED : TimbreSpectrum::DOUBTFUL;
    }
    return result;
}

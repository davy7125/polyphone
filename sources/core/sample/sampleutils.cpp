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

#include "sampleutils.h"
#include "fastmaths.h"
#include "synthmath.h"
#include "utils.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <vector>
#include <vorbis/vorbisenc.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

FourierTransform::FourierTransform(int size) :
    _size(size),
    _twiddles(static_cast<size_t>(qMax(1, size / 2)))
{
    // Computed directly (no accumulation of rounding errors as with a recurrence)
    for (int k = 0; k < size / 2; k++)
        _twiddles[static_cast<size_t>(k)] = std::polar(1.0, -2.0 * M_PI * k / size);
}

void FourierTransform::transform(std::vector<std::complex<double> > &data, bool inverse) const
{
    // Stockham algorithm (no bit reversal: the data are read and written sequentially, which is faster for long signals),
    // alternating between the data and a buffer, the inverse transform using the conjugated twiddle factors.
    // Real arithmetic for the butterflies: the complex product of the standard library checks for infinities.
    Q_ASSERT(static_cast<int>(data.size()) == _size);
    int n = _size;
    if (n < 2)
        return;
    std::vector<std::complex<double> > buffer(static_cast<size_t>(n));
    std::complex<double> * source = data.data();
    std::complex<double> * destination = buffer.data();
    double sign = inverse ? -1.0 : 1.0;

    // Butterfly of a stage: x1 = e + w * d, x2 = e - w * d
    auto butterfly = [](const std::complex<double> &e, const std::complex<double> &d, double wr, double wi,
                        std::complex<double> &x1, std::complex<double> &x2) {
        double dr = d.real() * wr - d.imag() * wi;
        double di = d.real() * wi + d.imag() * wr;
        x1 = std::complex<double>(e.real() + dr, e.imag() + di);
        x2 = std::complex<double>(e.real() - dr, e.imag() - di);
    };

    // Stages processed two by two (the same operations, the data being read and written half as many times). For each
    // stage, E and D have a length of half, their elements being separated by 2 * skip: E starts from source[0] to
    // source[skip - 1] and D from source[skip] to source[2 * skip - 1]
    int half = 1;
    for (; 4 * half <= n; half *= 4)
    {
        int skip = n / (2 * half);
        int nextSkip = skip / 2; // Of the next stage, whose half is 2 * half
        for (int k = 0; k < half; k++)
        {
            const std::complex<double> &w = _twiddles[static_cast<size_t>(k * skip)];
            const std::complex<double> &w1 = _twiddles[static_cast<size_t>(k * nextSkip)];
            const std::complex<double> &w2 = _twiddles[static_cast<size_t>((k + half) * nextSkip)];
            double wr = w.real(), wi = sign * w.imag();
            double w1r = w1.real(), w1i = sign * w1.imag();
            double w2r = w2.real(), w2i = sign * w2.imag();
            const std::complex<double> * e = source + 2 * k * skip;
            std::complex<double> * x = destination + k * nextSkip;
            for (int m = 0; m < nextSkip; m++)
            {
                // First stage: k, at m and m + nextSkip, the results going to the positions k * skip + (m or
                // m + nextSkip) and n / 2 + the same positions
                std::complex<double> a0, a1, a2, a3;
                butterfly(e[m], e[skip + m], wr, wi, a0, a2);
                butterfly(e[nextSkip + m], e[skip + nextSkip + m], wr, wi, a1, a3);

                // Second stage: k (from a0 and a1) and k + half (from a2 and a3)
                butterfly(a0, a1, w1r, w1i, x[m], x[n / 2 + m]);
                butterfly(a2, a3, w2r, w2i, x[n / 4 + m], x[3 * n / 4 + m]);
            }
        }
        std::swap(source, destination);
    }

    // Last stage if their number is odd
    if (half < n)
    {
        int skip = n / (2 * half);
        const std::complex<double> * e = source;
        std::complex<double> * x1 = destination;
        std::complex<double> * x2 = destination + n / 2;
        for (int k = 0; k < half; k++)
        {
            const std::complex<double> &w = _twiddles[static_cast<size_t>(k * skip)];
            double wr = w.real(), wi = sign * w.imag();
            for (int m = 0; m < skip; m++)
                butterfly(e[m], e[skip + m], wr, wi, *x1++, *x2++);
            e += 2 * skip;
        }
        std::swap(source, destination);
    }
    if (source != data.data())
        std::copy(source, source + n, data.begin());

    if (inverse)
    {
        double scale = 1.0 / n;
        for (int i = 0; i < n; i++)
            data[static_cast<size_t>(i)] *= scale;
    }
}

int SampleUtils::nextPowerOfTwo(int n)
{
    int result = 1;
    while (result < n)
        result <<= 1;
    return result;
}

QVector<float> SampleUtils::int24ToFloat(const qint16 * data16, const quint8 * data24, quint32 length)
{
    QVector<float> vRet(static_cast<int>(length));
    float * result = vRet.data();
    if (data24 == nullptr)
    {
        for (quint32 i = 0; i < length; i++)
            result[i] = Utils::int24ToFloat(data16[i] << 8);
    }
    else
    {
        for (quint32 i = 0; i < length; i++)
            result[i] = Utils::int24ToFloat(data16[i] << 8 | data24[i]);
    }
    return vRet;
}

void SampleUtils::floatToInt24(const QVector<float> &data, qint16 *& data16, quint8 *& data24)
{
    int length = data.size();
    const float * values = data.constData();
    data16 = new qint16[length];
    data24 = new quint8[length];
    for (int i = 0; i < length; i++)
    {
        int iTmp = Utils::floatToInt24(values[i]);
        data16[i] = (iTmp >> 8);
        data24[i] = iTmp & 0xFF;
    }
}

QVector<float> SampleUtils::resampleMono(const QVector<float> &vData, double echInit, double echFinal)
{
    if (vData.isEmpty())
        return vData;

    // Parameters
    const double beta = 6;
    const quint32 order = 64; // Must be a multiple of 4
    const quint32 subDivisions = 1024;

    // Prepare the input signal, with order / 2 zeros before and after for the resampling
    double ratio = echInit / echFinal;
    double lowPassFilterFrequency = 0.9 * qMin(echFinal, echInit) * 0.5;
    QVector<float> padded;
    quint32 sizeInit;
    {
        QVector<float> filtered = SampleUtils::bandFilter(vData, echInit, lowPassFilterFrequency, 0, -1);
        sizeInit = static_cast<quint32>(filtered.size());
        padded.resize(static_cast<int>(sizeInit + order));
        std::copy(filtered.constBegin(), filtered.constEnd(), padded.begin() + order / 2);
    }

    // Initialize the output
    quint32 sizeFinal = 1 + static_cast<quint32>((double)(sizeInit - 1) / ratio);
    QVector<float> dataRet(static_cast<int>(sizeFinal));
    float * dataOut = dataRet.data();

    // SinC table, computed once
    static const std::vector<float> sincWindow = [&]() {
        std::vector<float> table(order * subDivisions);
        fillSincTable(table.data(), order, subDivisions, beta);
        return table;
    }();

    // Resample
    const float * dataF = padded.constData();
    double currentPosition = 0;
    quint32 currentPositionWhole = 0;
    quint32 currentPositionFraction = 0;
    for (quint32 i = 0; i < sizeFinal; i++)
    {
        const float * coeffs = &sincWindow[currentPositionFraction * order];
        float value = 0;
        for (quint32 j = 0; j < order; j += 4)
            value += FastMaths::multiply4(&coeffs[j], &dataF[currentPositionWhole + j]);
        dataOut[i] = value;

        currentPosition += ratio;
        currentPositionWhole += (int)currentPosition;
        currentPosition -= (int)currentPosition;
        currentPositionFraction = static_cast<int>(currentPosition * subDivisions) % subDivisions;
    }

    // Possible clipping
    limitToFullScale(dataRet);
    return dataRet;
}

QVector<float> SampleUtils::filterSpectrum(const QVector<float> &vData, double sampleRate,
                                          const std::function<double(double, double, double)> &gain)
{
    // Spectrum of the signal zero-padded to a power of 2
    std::vector<std::complex<double> > spectrum = toComplex(vData);
    int size = static_cast<int>(spectrum.size());
    FourierTransform fourierTransform(size);
    fourierTransform.transform(spectrum, false);

    // Gain of each bin from 0 to the Nyquist frequency, also applied to its symmetric bin (size - k): the spectrum of the
    // real signal stays symmetric
    double maxModule = 0;
    for (int k = 0; k <= size / 2; k++)
        maxModule = qMax(maxModule, std::norm(spectrum[static_cast<size_t>(k)]));
    maxModule = std::sqrt(maxModule);
    for (int k = 0; k <= size / 2; k++)
    {
        double value = gain(k * sampleRate / size, std::abs(spectrum[static_cast<size_t>(k)]), maxModule);
        spectrum[static_cast<size_t>(k)] *= value;
        if (k > 0 && k < size - k)
            spectrum[static_cast<size_t>(size - k)] *= value;
    }

    // Back to the time domain
    fourierTransform.transform(spectrum, true);
    return toReal(spectrum, vData.size());
}

QVector<float> SampleUtils::bandFilter(const QVector<float> &vData, double dwSmplRate, double lowPassFrequency,
                                       double highPassFrequency, int order)
{
    // Valid frequencies (lower than the Nyquist frequency)?
    if (dwSmplRate < 1 || (highPassFrequency <= 0 && lowPassFrequency <= 0) ||
        2 * highPassFrequency > dwSmplRate || 2 * lowPassFrequency > dwSmplRate)
        return vData;

    // Butterworth gains of the given order (applied without phase), or brick wall if the order is -1
    auto lowPass = [&](double frequency) -> double {
        if (order == -1)
            return frequency < lowPassFrequency ? 1.0 : 0.0;
        return 1.0 / (1.0 + pow(frequency / lowPassFrequency, 2 * order));
    };
    auto highPass = [&](double frequency) -> double {
        if (order == -1)
            return frequency > highPassFrequency ? 1.0 : 0.0;
        return 1.0 - 1.0 / (1.0 + pow(frequency / highPassFrequency, 2 * order));
    };
    return filterSpectrum(vData, dwSmplRate, [&](double frequency, double, double) -> double {
        if (highPassFrequency <= 0)
            return lowPass(frequency);
        if (lowPassFrequency <= 0)
            return highPass(frequency);
        return lowPass(frequency) * highPass(frequency);
    });
}

QVector<float> SampleUtils::cutFilter(const QVector<float> &vData, quint32 dwSmplRate, const QVector<float> &dValues, int maxFreq)
{
    // The modules are limited by dValues (fractions of the maximum module, from 0 to maxFreq), interpolated
    int nbValues = dValues.count();
    return filterSpectrum(vData, dwSmplRate, [&](double frequency, double module, double maxModule) -> double {
        double limit = maxModule;
        int index1 = static_cast<int>(frequency / maxFreq * nbValues);
        if (index1 >= nbValues - 1)
            limit *= dValues[nbValues - 1];
        else
        {
            double x1 = static_cast<double>(index1) / nbValues * maxFreq;
            double y1 = dValues[index1];
            double x2 = static_cast<double>(index1 + 1) / nbValues * maxFreq;
            double y2 = dValues[index1 + 1];
            limit *= ((frequency - x1) / (x2 - x1)) * (y2 - y1) + y1;
        }
        return module > limit ? limit / module : 1.0;
    });
}

QVector<float> SampleUtils::EQ(const QVector<float> &vData, quint32 dwSmplRate, const QVector<int> &eqGains)
{
    return filterSpectrum(vData, dwSmplRate, [&](double frequency, double, double) -> double {
        return gainEQ(frequency, eqGains);
    });
}

QVector<float> SampleUtils::getFourierTransform(const QVector<float> &input)
{
    // Hann window, the amplitude being compensated (the peak of a sinusoid keeps the same value): the sum of the
    // window over n values is (n - 1) / 2, hence a gain of 2 n / (n - 1)
    std::vector<std::complex<double> > data = toComplex(input);
    int length = input.size();
    if (length > 1)
    {
        double gain = length > 2 ? 2.0 * length / (length - 1) : 1.0;
        for (int i = 0; i < length; i++)
            data[static_cast<size_t>(i)] *= gain * (0.5 - 0.5 * cos(2.0 * M_PI * i / (length - 1)));
    }
    FourierTransform(static_cast<int>(data.size())).transform(data, false);

    // Module of the bins from 0 to the Nyquist frequency (the other half is symmetric for a real signal)
    int size = static_cast<int>(data.size()) / 2;
    QVector<float> vectFourier(size);
    float * result = vectFourier.data();
    for (int i = 0; i < size; i++)
        result[i] = static_cast<float>(std::abs(data[static_cast<size_t>(i)]));
    return vectFourier;
}

std::vector<std::complex<double> > SampleUtils::toComplex(const QVector<float> &fData)
{
    std::vector<std::complex<double> > result(static_cast<size_t>(nextPowerOfTwo(fData.size())));
    const float * data = fData.constData();
    for (int i = 0; i < fData.size(); i++)
        result[static_cast<size_t>(i)] = data[i];
    return result;
}

QVector<float> SampleUtils::toReal(const std::vector<std::complex<double> > &cpxData, int size)
{
    QVector<float> vRet(size);
    float * data = vRet.data();
    for (int i = 0; i < size; i++)
        data[i] = static_cast<float>(cpxData[static_cast<size_t>(i)].real());
    limitToFullScale(vRet);
    return vRet;
}

QVector<float> SampleUtils::normalize(QVector<float> vData, float dVal, float &db)
{
    // Get the maximum value
    float * data = vData.data();
    float valMax = peak(data, static_cast<quint32>(vData.size()));
    if (valMax <= 0)
    {
        // Silent sample, otherwise 0 * infinity would give NaN values
        db = 0;
        return vData;
    }

    // Compute the amplification
    float mult = dVal / valMax;
    db = 20.0f * log10(mult);

    // Amplify
    for (int i = 0; i < vData.size(); i++)
        data[i] *= mult;

    return vData;
}

QVector<float> SampleUtils::multiply(QVector<float> vData, float dMult, float &db)
{
    // Compute the amplification
    db = 20.0f * log10(dMult);

    // Amplify
    float * data = vData.data();
    for (int i = 0; i < vData.size(); i++)
        data[i] *= dMult;

    return vData;
}

bool SampleUtils::isLooped(quint32 loopStart, quint32 loopEnd, quint32 length)
{
    return loopEnd > loopStart + 2 && loopEnd <= length;
}

quint32 SampleUtils::findOnset(const QVector<float> &vData, quint32 dwSmplRate, bool looped, double &noiseFloorDb,
                               NoiseReference * noiseReference)
{
    noiseFloorDb = -200;
    if (noiseReference != nullptr)
        *noiseReference = NOISE_UNKNOWN;
    int length = vData.size();
    if (length == 0)
        return 0;
    const float * data = vData.constData();
    double peakValue = peak(data, static_cast<quint32>(length));

    // Envelope with frames of 20 ms every 10 ms
    QVector<double> cumSquares = cumulativeSquares(vData);
    int window = qMax(1, static_cast<int>(0.02 * dwSmplRate));
    int hop = qMax(1, static_cast<int>(0.01 * dwSmplRate));
    QVector<double> envelope = rmsEnvelope(cumSquares, window, hop);
    double maxEnvelope = 0;
    foreach (double value, envelope)
        maxEnvelope = qMax(maxEnvelope, value);
    double maxDb = toDb(maxEnvelope);

    // Tentative onset: first frame 40 dB below the loudest frame
    int onsetFrame = 0;
    while (onsetFrame < envelope.size() - 1 && toDb(envelope[onsetFrame]) < maxDb - 40)
        onsetFrame++;

    // Noise floor: before the onset, otherwise at the end of a decaying sound
    QVector<double> noiseFrames;
    NoiseReference reference = NOISE_UNKNOWN;
    int preOnsetFrames = onsetFrame - 2;
    if (preOnsetFrames * hop > 0.05 * dwSmplRate)
    {
        for (int i = 0; i < preOnsetFrames; i++)
            if (envelope[i] > 0)
                noiseFrames << toDb(envelope[i]);
        reference = NOISE_BEFORE_ONSET;
    }
    if (noiseFrames.isEmpty() && !looped && envelope.size() > 20)
    {
        int count = qMax(1, static_cast<int>(0.1 * dwSmplRate / hop));
        QVector<double> tail;
        for (int i = qMax(0, static_cast<int>(envelope.size()) - count); i < envelope.size(); i++)
            if (envelope[i] > 0)
                tail << toDb(envelope[i]);
        if (!tail.isEmpty() && median(tail) < maxDb - 40)
        {
            noiseFrames = tail;
            reference = NOISE_AT_END;
        }
    }
    if (noiseFrames.isEmpty())
        reference = NOISE_UNKNOWN;
    else
        noiseFloorDb = median(noiseFrames);
    if (noiseReference != nullptr)
        *noiseReference = reference;

    // Onset at the sample level: 50 dB below the peak, and 10 dB above the noise peaks if the noise is known
    double threshold = peakValue * std::pow(10.0, -50.0 / 20);
    if (noiseFloorDb > -199)
        threshold = qMax(threshold, 3.16 * 1.41 * std::pow(10.0, noiseFloorDb / 20));
    int onset = 0;
    while (onset < length - 1 && std::fabs(data[onset]) < threshold)
        onset++;
    if (onset >= length - 1)
        onset = 0;
    return static_cast<quint32>(onset);
}

quint32 SampleUtils::findTrimStart(const QVector<float> &vData, quint32 dwSmplRate, bool looped)
{
    double noiseFloorDb;
    quint32 onset = findOnset(vData, dwSmplRate, looped, noiseFloorDb);

    // Zero crossing before the onset (a value of 0 or a change of sign)
    const float * data = vData.constData();
    quint32 limit = onset > dwSmplRate / 100 ? onset - dwSmplRate / 100 : 0;
    for (quint32 pos = onset; pos > limit; pos--)
    {
        if (data[pos] == 0.0f)
            return pos;
        if ((data[pos - 1] < 0) != (data[pos] < 0))
            return qAbs(data[pos - 1]) < qAbs(data[pos]) ? pos - 1 : pos;
    }
    return onset;
}

QVector<float> SampleUtils::trimStart(QVector<float> vData, quint32 pos)
{
    // Skip the first points
    if (static_cast<quint32>(vData.size()) > pos + 8)
        return vData.mid(static_cast<int>(pos));
    return vData;
}

bool SampleUtils::findSteadyPart(const QVector<float> &fData, quint32 dwSmplRate, quint32 &posStart, quint32 &posEnd)
{
    // Envelope in dB, with at most 400 values (5 ms minimum between them)
    int length = fData.size();
    posStart = 0;
    posEnd = (length == 0 ? 0 : length - 1);
    int hop = qMax(static_cast<int>(dwSmplRate / 200), length / 400);
    int window = 2 * hop;
    QVector<double> cumSquares = cumulativeSquares(fData);
    QVector<double> y = rmsEnvelope(cumSquares, window, hop);
    int n = y.size();
    if (n < 10)
        return false;
    for (int i = 0; i < n; i++)
        y[i] = toDb(y[i]);
    double maxDb = *std::max_element(y.begin(), y.end());
    for (int i = 0; i < n; i++)
        y[i] = qMax(y[i], maxDb - 80);
    double dt = static_cast<double>(hop) / dwSmplRate;

    // Squared error of a linear fit on a segment [a, b) computed with cumulative sums
    QVector<double> sx(n + 1, 0), sy(n + 1, 0), sxx(n + 1, 0), sxy(n + 1, 0), syy(n + 1, 0);
    for (int i = 0; i < n; i++)
    {
        double x = i * dt;
        sx[i + 1] = sx[i] + x;
        sy[i + 1] = sy[i] + y[i];
        sxx[i + 1] = sxx[i] + x * x;
        sxy[i + 1] = sxy[i] + x * y[i];
        syy[i + 1] = syy[i] + y[i] * y[i];
    }
    auto fit = [&](int a, int b, double &slope, double &intercept) -> double {
        double m = b - a, X = sx[b] - sx[a], Y = sy[b] - sy[a], XX = sxx[b] - sxx[a], XY = sxy[b] - sxy[a], YY = syy[b] - syy[a];
        double d = m * XX - X * X;
        slope = d > 1e-15 ? (m * XY - X * Y) / d : 0;
        intercept = (Y - slope * X) / m;
        return qMax(0.0, YY - 2 * slope * XY - 2 * intercept * Y + slope * slope * XX + 2 * slope * intercept * X + m * intercept * intercept);
    };

    // Natural fluctuation of the envelope (tremolo, noise): median error of linear fits over 0.2 s
    int span = qMax(5, static_cast<int>(0.2 / dt));
    QVector<double> fluctuations;
    double slope, intercept;
    for (int a = 0; a + span <= n; a += span / 2)
        if ((sy[a + span] - sy[a]) / span >= maxDb - 40)
            fluctuations << fit(a, a + span, slope, intercept) / span;
    std::sort(fluctuations.begin(), fluctuations.end());
    double variance = fluctuations.isEmpty() ? 0.01 : qMax(0.01, fluctuations[fluctuations.size() / 2]);

    // Segmentation in straight lines (optimal partitioning), a new segment costing 12 times the fluctuation x ln(n)
    double penalty = 12 * variance * std::log(static_cast<double>(n));
    int minLength = qMax(3, static_cast<int>(0.04 / dt));
    QVector<double> best(n + 1, 1e300);
    QVector<int> from(n + 1, -1);
    best[0] = 0;
    for (int j = minLength; j <= n; j++)
    {
        for (int i = 0; i <= j - minLength; i++)
        {
            if (best[i] >= 1e299)
                continue;
            double cost = best[i] + fit(i, j, slope, intercept) + penalty;
            if (cost < best[j])
            {
                best[j] = cost;
                from[j] = i;
            }
        }
    }
    QVector<int> bounds;
    for (int j = n; j > 0; j = from[j])
    {
        if (from[j] < 0)
            return false;
        bounds.prepend(j);
    }
    bounds.prepend(0);

    // Merge of adjacent segments while their common straight line is almost flat (6 dB/s maximum) and close to the
    // envelope (within its natural fluctuation): the slow undulations of a sustained sound are kept together
    double tolerance = qMax(1.5, 2 * std::sqrt(variance));
    bool merged = true;
    while (merged)
    {
        merged = false;
        for (int k = 1; k < bounds.size() - 1; k++)
        {
            double error = fit(bounds[k - 1], bounds[k + 1], slope, intercept);
            if (qAbs(slope) <= 6 && std::sqrt(error / (bounds[k + 1] - bounds[k - 1])) <= tolerance)
            {
                bounds.removeAt(k);
                merged = true;
                break;
            }
        }
    }

    // Steady part: the longest run of segments after the attack (at least 0.1 s, less than 20 dB below the maximum)
    // whose common straight line is flat (6 dB/s maximum) and close to the envelope (RMS deviation of 3.5 dB, or twice
    // the tolerance): slow undulations deeper than the natural fluctuation (beats of a celeste, swell box...) split a
    // sustained sound into short sloping segments that are kept together this way
    int segmentCount = bounds.size() - 1;
    int firstSegment = 0;
    if (segmentCount > 1)
    {
        fit(bounds[0], bounds[1], slope, intercept);
        if (slope > 10)
            firstSegment = 1; // Attack
    }
    double maxDeviation = qMax(3.5, 2 * tolerance);
    int steadyFirst = -1, steadyLast = -1;
    double steadyDuration = 0;
    for (int i = firstSegment; i < segmentCount; i++)
    {
        for (int j = i; j < segmentCount; j++)
        {
            int a = bounds[i], b = bounds[j + 1];
            double duration = (b - a) * dt;
            if (duration < 0.1 || duration <= steadyDuration)
                continue;
            double error = fit(a, b, slope, intercept);
            if (qAbs(slope) <= 6 && (sy[b] - sy[a]) / (b - a) >= maxDb - 20 && std::sqrt(error / (b - a)) <= maxDeviation)
            {
                steadyDuration = duration;
                steadyFirst = i;
                steadyLast = j;
            }
        }
    }

    // Otherwise the flattest segment after the attack, not steady
    bool isSteady = (steadyFirst >= 0);
    if (!isSteady)
    {
        double flattest = 1e9;
        for (int k = firstSegment; k < segmentCount; k++)
        {
            int a = bounds[k], b = bounds[k + 1];
            fit(a, b, slope, intercept);
            if ((sy[b] - sy[a]) / (b - a) >= maxDb - 30 && qAbs(slope) < flattest) // Not the end of the sound
            {
                flattest = qAbs(slope);
                steadyFirst = steadyLast = k;
            }
        }
        if (steadyFirst < 0)
            return false;
    }
    int a = bounds[steadyFirst] + (steadyFirst > 0 ? 1 : 0); // One value of margin after a boundary
    int b = bounds[steadyLast + 1];
    double medianLevel = 0;
    if (isSteady)
    {
        // The beginning of a merged part may be the end of the decay, and its end the beginning of the release or a
        // final fade: keep the values from the first to the last one near the median level
        QVector<double> levels = y.mid(a, b - a);
        std::sort(levels.begin(), levels.end());
        medianLevel = levels[levels.size() / 2];
        while (a < b - minLength && qAbs(y[a] - medianLevel) > tolerance)
            a++;
        while (b > a + minLength && qAbs(y[b - 1] - medianLevel) > tolerance)
            b--;
    }
    posStart = static_cast<quint32>(qMin(length - 1, a * hop));
    posEnd = static_cast<quint32>(b == n ? length - 1 : qMin(length - 1, (b - 1) * hop + window));

    // A fade or a silence at the end, shorter than the resolution of the envelope, is also removed (5 ms steps)
    if (isSteady)
    {
        int step = qMax(1, static_cast<int>(dwSmplRate / 200));
        while (posEnd > posStart + static_cast<quint32>(window) + step)
        {
            double level = toDb(rms(cumSquares, static_cast<int>(posEnd) - step, static_cast<int>(posEnd)));
            if (level >= medianLevel - tolerance)
                break;
            posEnd -= step;
        }
    }
    return isSteady;
}

namespace
{
    // Maximum number of offsets before and after the loop points used by the filter of the loop search
    const int LOOP_FILTER_MAX_BEFORE = 11;
    const int LOOP_FILTER_MAX_AFTER = 12;

    // Difference of the waveforms at two positions (on the value, the slope and the slope difference)
    inline float loopPointDifference(const float * data, quint32 pos1, quint32 pos2)
    {
        // Difference on the loop points
        float diff0 = data[pos1] - data[pos2];

        // Difference on the slope
        float diff1 = data[pos1 - 1] - data[pos2 - 1] - diff0;

        // Difference on the slope difference
        float diff2 = data[pos1 - 2] - data[pos2 - 2] - 2 * diff1 + diff0;

        // Combine the values
        return 0.45f * qAbs(diff0) + 0.33f * qAbs(diff1) + 0.22f * qAbs(diff2);
    }

    // Number of offsets that can be used before a loop start (the offsets being increasing, the first one out of the
    // data ends the count)
    int loopOffsetsBefore(quint32 loopStart, const QVector<quint32> &offsets)
    {
        int count = 0;
        while (count < offsets.size() && loopStart > 3 + offsets[count])
            count++;
        return count;
    }

    // Number of offsets that can be used after a loop end
    int loopOffsetsAfter(quint32 loopEnd, quint32 length, const QVector<quint32> &offsets)
    {
        int count = 0;
        while (count < offsets.size() && loopEnd + 3 + offsets[count] < length)
            count++;
        return count;
    }

    // Sum of the differences at the loop points, then at the first beforeCount offsets before them and the first
    // afterCount offsets after them (in this order), stopped as soon as it exceeds limit: the differences being positive,
    // the total is then above the limit too
    inline float loopDifferenceSum(const float * data, quint32 loopStart, quint32 loopEnd, const quint32 * offsets,
                                   int beforeCount, int afterCount, double limit)
    {
        float result = loopPointDifference(data, loopStart, loopEnd);
        for (int k = 0; k < beforeCount && result <= limit; k++)
            result += loopPointDifference(data, loopStart - offsets[k] - 1, loopEnd - offsets[k] - 1);
        for (int k = 0; k < afterCount && result <= limit; k++)
            result += loopPointDifference(data, loopStart + offsets[k] - 1, loopEnd + offsets[k] - 1);
        return result;
    }

    // Weighted features of the difference at a loop point (see loopPointDifference): the difference between
    // two positions is the sum of the absolute differences of their features, up to the rounding errors
    void loopFeatures(const float * data, quint32 pos, float * features)
    {
        features[0] = 0.45f * data[pos];
        features[1] = 0.33f * (data[pos - 1] - data[pos]);
        features[2] = 0.22f * (data[pos - 2] - 2 * data[pos - 1] + 3 * data[pos]);
    }

    // The filter of the loop search is compiled for the default instruction set and, on x86, also for AVX2 (chosen at run
    // time): its functions are then inlined in each version
#if defined(__GNUC__)
#  define LOOP_FILTER_INLINE inline __attribute__((always_inline))
#  if defined(__x86_64__) || defined(__i386__)
#    define LOOP_FILTER_AVX2
#  endif
#else
#  define LOOP_FILTER_INLINE inline
#endif

    // Add the differences of the features of terms (3 features per term) between a loop start and count loop ends
    // endFeatures: rows of features (stride values between them) for the loop ends
    template <int TERMS>
    LOOP_FILTER_INLINE void addFeatureDifferences(int count, const float * startFeatures, const float * endFeatures,
                                                  int stride, float * __restrict values)
    {
        for (int k = 0; k < count; k++)
        {
            float sum = 0;
            for (int f = 0; f < 3 * TERMS; f++)
                sum += std::fabs(startFeatures[f] - endFeatures[f * stride + k]);
            values[k] += sum;
        }
    }

    // Data of the filter of consecutive loop ends for a loop start
    struct LoopFilter
    {
        int count; // Number of loop ends
        const float * startFeatures; // 3 per term
        int termCount;
        const float * endFeatures; // Rows of features (stride values between them) for the loop ends
        int stride;
        const qint32 * endPositions;
        qint32 startPosition;
        const float * endDifferenceCounts; // Number of differences after the loop ends
        float startDifferenceCount; // Number of differences at and before the loop start
        float range; // Length of the search area
        float bestRank; // Including a margin
        float scale; // Maximum value / 200
        float margin; // Absolute margin of the sums
    };

    // Filter of the loop ends, written so that the compiler vectorizes it. It compares a lower bound of the sum of the
    // differences of each loop (sum of the feature differences on the first terms) with the sum above which the rank
    // cannot be better than the best one, with margins covering the rounding errors. Terms are added while loop ends
    // remain. The indices of the remaining loop ends (increasing) are written in candidates and their number is returned.
    // values: buffer of count values
    LOOP_FILTER_INLINE int filterLoopEnds(const LoopFilter &filter, float * __restrict values, int * __restrict candidates)
    {
        // Opposite of the sums above which the ranks cannot be better (see loopStep1)
        int count = filter.count;
        const qint32 * __restrict endPositions = filter.endPositions;
        const float * __restrict endDifferenceCounts = filter.endDifferenceCounts;
        for (int k = 0; k < count; k++)
        {
            float lengthScore = std::max(3.0f, filter.range / static_cast<float>(endPositions[k] - filter.startPosition));
            values[k] = (lengthScore - filter.bestRank) * (filter.startDifferenceCount + endDifferenceCounts[k]) *
                        filter.scale - filter.margin;
        }

        // Differences of the first 4 terms, then of the next ones one by one while loop ends remain
        int t = 0;
        if (filter.termCount >= 4)
        {
            addFeatureDifferences<4>(count, filter.startFeatures, filter.endFeatures, filter.stride, values);
            t = 4;
        }
        int remaining = count;
        while (true)
        {
            if (t >= 4)
            {
                remaining = 0;
                for (int k = 0; k < count; k++)
                    remaining += (values[k] > 0 ? 0 : 1);
            }
            if (t >= filter.termCount || remaining == 0)
                break;
            addFeatureDifferences<1>(count, filter.startFeatures + 3 * t, filter.endFeatures + 3 * t * filter.stride,
                                     filter.stride, values);
            t++;
        }
        if (remaining == 0)
            return 0;

        // Remaining loop ends (NaN values included)
        int candidateCount = 0;
        for (int k = 0; k < count; k++)
        {
            candidates[candidateCount] = k;
            candidateCount += (values[k] > 0 ? 0 : 1);
        }
        return candidateCount;
    }

    typedef int (*LoopFilterFunction)(const LoopFilter &, float *, int *);

    int filterLoopEndsDefault(const LoopFilter &filter, float * values, int * candidates)
    {
        return filterLoopEnds(filter, values, candidates);
    }

#ifdef LOOP_FILTER_AVX2
    __attribute__((target("avx2"))) int filterLoopEndsAvx2(const LoopFilter &filter, float * values, int * candidates)
    {
        return filterLoopEnds(filter, values, candidates);
    }
#endif

    LoopFilterFunction getLoopFilterFunction()
    {
#ifdef LOOP_FILTER_AVX2
        if (__builtin_cpu_supports("avx2"))
            return filterLoopEndsAvx2;
#endif
        return filterLoopEndsDefault;
    }
}

bool SampleUtils::loopStep1(const QVector<float> &vData, quint32 dwSmplRate, quint32 &loopStart, quint32 &loopEnd, quint32 &loopCrossfadeLength)
{
    const float * data = vData.constData();
    quint32 length = static_cast<quint32>(vData.size());
    if (length < 3)
        return false;

    // Search area
    quint32 posStart, posEnd;
    bool isSteady = findSteadyPart(vData, dwSmplRate, posStart, posEnd);
    quint32 steadyStart = posStart;
    if (!isSteady)
        posEnd = length - 1;
    if (length > 40000)
    {
        posStart = 4000;
        if (posEnd + 400 > length)
            posEnd = length - 400;
    }
    else if (length > 4000)
    {
        posStart = 400;
        if (posEnd + 400 > length)
            posEnd = length - 400;
    }
    else
    {
        posStart = 2;
        if (posEnd + 1 > length)
            posEnd = length - 1;
    }

    // No manual input (previously it was possible to manually adapt the left limit for finding a loop)
    // if (loopStart > posStart)
    //     posStart = loopStart;

    // Find all points crossing 0
    QVector<quint32> crossPositions;
    for (quint32 i = posStart; i < posEnd; i++)
        if (data[i - 1] < 0 && data[i] >= 0)
            crossPositions << i;
    if (crossPositions.count() < 2)
        return false;

    // Limit the area of the loop start, in the steady part if there is one (not in the attack)
    quint32 minLoopStart = posStart + 0.25 * (posEnd - posStart);
    if (minLoopStart > dwSmplRate)
        minLoopStart = dwSmplRate;
    if (isSteady && steadyStart > minLoopStart && steadyStart + dwSmplRate / 20 < posEnd)
        minLoopStart = steadyStart;
    int minLoopStartIndex = 0;
    for (int i = 0; i < crossPositions.count(); i++)
    {
        if (crossPositions[i] > minLoopStart)
        {
            minLoopStartIndex = i;
            break;
        }
    }

    // Limit the area of the loop end
    quint32 minLoopEnd = posStart + 0.25 * (posEnd - posStart);
    int minLoopEndIndex = 0;
    for (int i = 0; i < crossPositions.count(); i++)
    {
        if (crossPositions[i] > minLoopEnd)
        {
            minLoopEndIndex = i;
            break;
        }
    }

    // Number of positions per seconds
    float positionDensity = static_cast<float>(crossPositions.count()) / length * dwSmplRate;

    // Absolute maximum value of the signal
    float max = qMax(0.00045f, peak(data, length));

    // Compute the quality of the different possible loops and keep the best, minimizing the error and maximizing the loop length
    int checkNumber = 22 - positionDensity / 150;
    if (checkNumber < 8)
        checkNumber = 8;
    QVector<quint32> offsets = loopErrorOffsets(checkNumber);

    // Number of differences computed before and after each position (see loopError)
    int crossCount = crossPositions.count();
    QVector<int> beforeCounts(crossCount), afterCounts(crossCount);
    QVector<float> afterCountValues(crossCount);
    QVector<qint32> positions(crossCount);
    for (int i = 0; i < crossCount; i++)
    {
        beforeCounts[i] = loopOffsetsBefore(crossPositions[i], offsets);
        afterCounts[i] = loopOffsetsAfter(crossPositions[i], length, offsets);
        afterCountValues[i] = static_cast<float>(afterCounts[i]);
        positions[i] = static_cast<qint32>(crossPositions[i]);
    }

    // Features of the differences used by the filter below for each position, stored in rows (one per feature) so that
    // consecutive loop ends are read contiguously. Terms: the loop point, the first offsets before it, then the first
    // offsets after it (zero when out of the data)
    int filterBeforeCount = qMin(LOOP_FILTER_MAX_BEFORE, offsets.size());
    int filterAfterCount = qMin(LOOP_FILTER_MAX_AFTER, offsets.size());
    int filterTermCount = 1 + filterBeforeCount + filterAfterCount;
    QVector<float> filterFeatures(3 * filterTermCount * crossCount, 0.0f);
    for (int i = 0; i < crossCount; i++)
    {
        for (int t = 0; t < filterTermCount; t++)
        {
            quint32 position = crossPositions[i];
            if (t > filterBeforeCount)
            {
                int k = t - 1 - filterBeforeCount;
                if (k >= afterCounts[i])
                    break;
                position += offsets[k] - 1;
            }
            else if (t > 0)
            {
                if (t > beforeCounts[i])
                    continue;
                position -= offsets[t - 1] + 1;
            }

            float features[3];
            loopFeatures(data, position, features);
            for (int f = 0; f < 3; f++)
                filterFeatures[(3 * t + f) * crossCount + i] = features[f];
        }
    }

    float bestError = -1;
    float bestRank = 999999; // Lower is better
    quint32 bestLoopStart = 0;
    quint32 bestLoopEnd = 0;
    float range = static_cast<float>(posEnd - posStart);
    auto lengthScore = [&](quint32 start, quint32 end) {
        return qMax(3.0f, range / (end - start));
    };

    // The loops are tried from the longest one for each start: the length score only increases afterwards, and once it is
    // above the best rank no other loop from this start or the next ones (shorter) can be better
    LoopFilterFunction filterLoopEnds = getLoopFilterFunction();
    const int chunkSize = 256;
    float filterValues[chunkSize];
    int candidates[chunkSize];
    float startFeatures[3 * (1 + LOOP_FILTER_MAX_BEFORE + LOOP_FILTER_MAX_AFTER)];
    for (int i = minLoopStartIndex; i <= crossCount - 2; i++)
    {
        quint32 currentPosStart = crossPositions[i];
        if (lengthScore(currentPosStart, crossPositions[crossCount - 1]) > bestRank)
            break;

        // Features of the start used by the filter (the offsets being increasing, the valid ones are the first ones):
        // the terms after the loop points are used only if all terms before are valid
        bool allBefore = (beforeCounts[i] >= filterBeforeCount);
        int startTermCount = allBefore ? filterTermCount : 1 + beforeCounts[i];
        for (int f = 0; f < 3 * startTermCount; f++)
            startFeatures[f] = filterFeatures[f * crossCount + i];

        // Loop ends processed by chunks
        bool finished = false;
        int jMin = qMax(minLoopEndIndex, i + 1);
        for (int jHigh = crossCount - 1; jHigh >= jMin && !finished; jHigh -= chunkSize)
        {
            int jLow = qMax(jMin, jHigh - chunkSize + 1);
            if (lengthScore(currentPosStart, crossPositions[jHigh]) > bestRank)
                break;

            // Quick filter (the terms after the loop points being valid if they are valid for the last loop end), then
            // exact computation for the remaining loop ends (from the longest loop)
            int chunkTermCount = allBefore ? 1 + filterBeforeCount + qMin(filterAfterCount, afterCounts[jHigh]) :
                                             startTermCount;
            LoopFilter filter;
            filter.count = jHigh - jLow + 1;
            filter.startFeatures = startFeatures;
            filter.termCount = chunkTermCount;
            filter.endFeatures = filterFeatures.constData() + jLow;
            filter.stride = crossCount;
            filter.endPositions = positions.constData() + jLow;
            filter.startPosition = static_cast<qint32>(currentPosStart);
            filter.endDifferenceCounts = afterCountValues.constData() + jLow;
            filter.startDifferenceCount = static_cast<float>(1 + beforeCounts[i]);
            filter.range = range;
            filter.scale = max / 200;
            // Margins covering the rounding errors, the features and the sums being computed differently than in
            // loopDifferenceSum: the filter removes only loops that the exact computation would not keep
            filter.bestRank = bestRank * (1 + 1e-4f);
            filter.margin = 1e-3f * max;
            int candidateCount = filterLoopEnds(filter, filterValues, candidates);
            for (int c = candidateCount - 1; c >= 0; c--)
            {
                int j = jLow + candidates[c];
                quint32 currentPosEnd = crossPositions[j];
                float currentLengthScore = lengthScore(currentPosStart, currentPosEnd);
                if (currentLengthScore > bestRank)
                {
                    finished = true;
                    break;
                }

                // Sum of the differences above which the rank cannot be better than the best one: its computation stops
                // there (margin of 1e-5 covering the rounding errors, the result being the same as with a complete sum)
                int n = 1 + beforeCounts[i] + afterCounts[j];
                double limit = (static_cast<double>(bestRank) * (1 + 1e-5) - currentLengthScore) / 200 * max * n;
                float sum = loopDifferenceSum(data, currentPosStart, currentPosEnd, offsets.constData(),
                                              beforeCounts[i], afterCounts[j], limit);
                if (sum > limit)
                    continue;

                float currentError = sum / (max * n);
                float currentRank = 200.0f * currentError + currentLengthScore;
                if (currentRank < bestRank)
                {
                    bestError = currentError;
                    bestRank = currentRank;
                    bestLoopStart = currentPosStart;
                    bestLoopEnd = currentPosEnd;
                }
            }
        }
    }

    // Result, with a crossfade longer for a worse loop error, within the search area and at most half of the loop: a
    // longer crossfade mixes most of the loop with the signal one loop before, attenuating the harmonics whose phases
    // have drifted in between
    loopStart = bestLoopStart;
    loopEnd = bestLoopEnd;
    double crossfade = bestError * dwSmplRate / (5 + 0.75 * checkNumber);
    crossfade = qMin(crossfade * crossfade, static_cast<double>(qMin(loopStart - posStart, (loopEnd - loopStart) / 2)));
    loopCrossfadeLength = static_cast<quint32>(crossfade);
    return true;
}

QVector<float> SampleUtils::loopStep2(QVector<float> vData, quint32 loopStart, quint32 loopEnd, quint32 loopCrossfadeLength, bool withTrimEnd)
{
    // Loop with a crossfade (at least 2 values, otherwise the coefficient below is 0 / 0)
    if (loopCrossfadeLength >= 2)
    {
        float * fData = vData.data();
        for (quint32 i = 0; i < loopCrossfadeLength; i++)
        {
            float dTmp = static_cast<float>(i) / static_cast<float>(loopCrossfadeLength - 1);
            fData[loopEnd - loopCrossfadeLength + i] =
                (1.f - dTmp) * fData[loopEnd - loopCrossfadeLength + i] +
                dTmp * fData[loopStart - loopCrossfadeLength + i];
        }
    }

    if (withTrimEnd)
    {
        // Coupure et ajout de 8 valeurs (resize can move the data)
        vData.resize(loopEnd + 8);
        float * fData = vData.data();
        for (quint32 i = 0; i < 8; i++)
            fData[loopEnd + i] = fData[loopStart + i];
    }

    return vData;
}

QList<quint32> SampleUtils::findMax(const QVector<float> &vectData, int maxNb, float minFrac)
{
    if (vectData.isEmpty())
        return QList<quint32>();
    const float * data = vectData.constData();
    int size = vectData.size();

    // Calcul mini maxi
    float mini = data[0], maxi = data[0];
    for (int i = 1; i < size; i++)
    {
        if (data[i] < mini)
            mini = data[i];
        if (data[i] > maxi)
            maxi = data[i];
    }

    // Valeur à dépasser
    float valMin = mini + minFrac * (maxi - mini);

    // Recherche des indices de tous les pics
    std::vector<std::pair<float, quint32>> peaks;
    for (int i = 1; i < size - 1; i++)
        if (data[i - 1] < data[i] && data[i + 1] < data[i] && data[i] > valMin)
            peaks.push_back(std::make_pair(data[i], static_cast<quint32>(i)));

    // Sélection des plus grands pics, par valeur décroissante
    int count = qMax(0, qMin(maxNb, static_cast<int>(peaks.size())));
    std::partial_sort(peaks.begin(), peaks.begin() + count, peaks.end(),
                      [](const std::pair<float, quint32> &a, const std::pair<float, quint32> &b) {
                          return a.first > b.first || (a.first == b.first && a.second < b.second);
                      });
    QList<quint32> listRet;
    for (int i = 0; i < count; i++)
        listRet << peaks[i].second;

    return listRet;
}

// Reconnaissance de liens R - L dans les noms de samples
int SampleUtils::lastLettersToRemove(QString str1, QString str2)
{
    str1 = str1.toLower();
    str2 = str2.toLower();
    int nbLetters = 0;

    int size = 0;
    if (str1.size() == str2.size())
        size = str1.size();
    else return 0;

    if (str1.left(size - 2).compare(str2.left(size - 2)) != 0)
        return 0;

    QString fin1_3 = str1.right(3);
    QString fin2_3 = str2.right(3);
    QString fin1_2 = str1.right(2).left(1);
    QString fin2_2 = str2.right(2).left(1);
    QString fin1_1 = str1.right(1);
    QString fin2_1 = str2.right(1);

    if ((fin1_3.compare("(r)") == 0 && fin2_3.compare("(l)") == 0) ||
        (fin1_3.compare("(l)") == 0 && fin2_3.compare("(r)") == 0))
        nbLetters = 3;
    else if (((fin1_1.compare("r") == 0 && fin2_1.compare("l") == 0) ||
              (fin1_1.compare("l") == 0 && fin2_1.compare("r") == 0)) &&
             str1.left(size - 1).compare(str2.left(size - 1)) == 0)
    {
        nbLetters = 1;
        if ((fin1_2.compare("-") == 0 && fin2_2.compare("-") == 0) ||
            (fin1_2.compare("_") == 0 && fin2_2.compare("_") == 0) ||
            (fin1_2.compare(".") == 0 && fin2_2.compare(".") == 0) ||
            (fin1_2.compare(" ") == 0 && fin2_2.compare(" ") == 0))
            nbLetters = 2;
    }

    return nbLetters;
}

float SampleUtils::rms(const float * data, quint32 length)
{
    // Sum in double: in float the precision drops after a few hundred thousand values
    if (length == 0)
        return 0;
    double sum = 0;
    for (quint32 i = 0; i < length; i++)
        sum += static_cast<double>(data[i]) * data[i];
    return static_cast<float>(qSqrt(sum / length));
}

float SampleUtils::peak(const float * data, quint32 length)
{
    float result = 0;
    for (quint32 i = 0; i < length; i++)
        result = qMax(result, qAbs(data[i]));
    return result;
}

void SampleUtils::limitToFullScale(QVector<float> &vData)
{
    float valMax = peak(vData.constData(), static_cast<quint32>(vData.size()));
    if (valMax > 1.0f)
    {
        float coef = 1.0f / valMax;
        float * data = vData.data();
        for (int i = 0; i < vData.size(); i++)
            data[i] *= coef;
    }
}

QVector<double> SampleUtils::cumulativeSquares(const QVector<float> &vData)
{
    QVector<double> result(vData.size() + 1);
    result[0] = 0;
    const float * data = vData.constData();
    for (int i = 0; i < vData.size(); i++)
        result[i + 1] = result[i] + static_cast<double>(data[i]) * data[i];
    return result;
}

double SampleUtils::rms(const QVector<double> &cumulativeSquares, int start, int end)
{
    int length = cumulativeSquares.size() - 1;
    start = qBound(0, start, length);
    end = qBound(start, end, length);
    if (end <= start)
        return 0;
    return std::sqrt(qMax(0.0, cumulativeSquares[end] - cumulativeSquares[start]) / (end - start));
}

QVector<double> SampleUtils::rmsEnvelope(const QVector<double> &cumulativeSquares, int window, int hop, int start, int end)
{
    int length = cumulativeSquares.size() - 1;
    if (end < 0 || end > length)
        end = length;
    QVector<double> result;
    window = qMax(1, window);
    hop = qMax(1, hop);
    for (int position = start; position + window <= end; position += hop)
        result << rms(cumulativeSquares, position, position + window);
    if (result.isEmpty() && end > start)
        result << rms(cumulativeSquares, start, end);
    return result;
}

double SampleUtils::toDb(double amplitude, double floor)
{
    return amplitude > 0 ? qMax(floor, 20.0 * std::log10(amplitude)) : floor;
}

double SampleUtils::keyToFrequency(double key)
{
    return 440.0 * std::pow(2.0, (key - 69) / 12);
}

double SampleUtils::frequencyToKey(double frequency)
{
    return 69 + 12 * std::log2(frequency / 440.0);
}

void SampleUtils::toKeyAndCorrection(double key, int &closestKey, int &correction)
{
    closestKey = qRound(key);
    correction = -qRound(100 * (key - closestKey));
}

double SampleUtils::percentile(QVector<double> values, double fraction)
{
    if (values.isEmpty())
        return 0;
    std::sort(values.begin(), values.end());
    double position = fraction * (values.size() - 1);
    int index = static_cast<int>(position);
    if (index >= values.size() - 1)
        return values.last();
    return values[index] + (position - index) * (values[index + 1] - values[index]);
}

double SampleUtils::median(const QVector<double> &values)
{
    return percentile(values, 0.5);
}

bool SampleUtils::linearRegression(const QVector<double> &x, const QVector<double> &y, const QVector<double> &w,
                                   double &slope, double &intercept)
{
    double sw = 0, sx = 0, sy = 0, sxx = 0, sxy = 0;
    for (int i = 0; i < x.size(); i++)
    {
        double weight = w.isEmpty() ? 1.0 : w[i];
        sw += weight;
        sx += weight * x[i];
        sy += weight * y[i];
        sxx += weight * x[i] * x[i];
        sxy += weight * x[i] * y[i];
    }
    double denominator = sw * sxx - sx * sx;
    if (x.size() < 2 || sw <= 0 || std::fabs(denominator) < 1e-30)
        return false;
    slope = (sw * sxy - sx * sy) / denominator;
    intercept = (sy - slope * sx) / sw;
    return true;
}

void SampleUtils::add(QVector<float> &data, const QVector<float> &other, double gain)
{
    float * data1 = data.data();
    const float * data2 = other.constData();
    for (int i = 0; i < qMin(data.size(), other.size()); i++)
        data1[i] += gain * data2[i];
}

QVector<float> SampleUtils::readWithLoop(const QVector<float> &vData, quint32 loopStart, quint32 loopEnd, quint32 length)
{
    QVector<float> vDataRet(static_cast<int>(length));
    const float * data = vData.constData();
    float * dataRet = vDataRet.data();
    if (loopStart < loopEnd && loopEnd <= static_cast<quint32>(vData.size()))
    {
        // Loop
        quint32 pos = 0;
        quint32 total = 0;
        while (total < length)
        {
            const quint32 chunk = qMin(loopEnd - pos, length - total);
            memcpy(&dataRet[total], &data[pos], chunk * sizeof(float));
            pos += chunk;
            if (pos >= loopEnd)
                pos = loopStart;
            total += chunk;
        }
    }
    else if (static_cast<quint32>(vData.size()) < length)
    {
        // No loop, completed with zeros
        vDataRet.fill(0);
        memcpy(dataRet, data, vData.size() * sizeof(float));
    }
    else
        memcpy(dataRet, data, length * sizeof(float));
    return vDataRet;
}

double SampleUtils::gainEQ(double freq, const QVector<int> &eqGains)
{
    int x1 = 0;
    int x2 = 1;
    int y1 = 0;
    int y2 = 1;
    if (freq < 32)
    {
        x1 = 32; x2 = 64;
        y1 = qMin(eqGains[0], eqGains[1]); y2 = eqGains[1];
    }
    else if (freq < 64)
    {
        x1 = 32; x2 = 64;
        y1 = eqGains[0]; y2 = eqGains[1];
    }
    else if (freq < 125)
    {
        x1 = 64; x2 = 125;
        y1 = eqGains[1]; y2 = eqGains[2];
    }
    else if (freq < 250)
    {
        x1 = 125; x2 = 250;
        y1 = eqGains[2]; y2 = eqGains[3];
    }
    else if (freq < 500)
    {
        x1 = 250; x2 = 500;
        y1 = eqGains[3]; y2 = eqGains[4];
    }
    else if (freq < 1000)
    {
        x1 = 500; x2 = 1000;
        y1 = eqGains[4]; y2 = eqGains[5];
    }
    else if (freq < 2000)
    {
        x1 = 1000; x2 = 2000;
        y1 = eqGains[5]; y2 = eqGains[6];
    }
    else if (freq < 4000)
    {
        x1 = 2000; x2 = 4000;
        y1 = eqGains[6]; y2 = eqGains[7];
    }
    else if (freq < 8000)
    {
        x1 = 4000; x2 = 8000;
        y1 = eqGains[7]; y2 = eqGains[8];
    }
    else if (freq < 16000)
    {
        x1 = 8000; x2 = 16000;
        y1 = eqGains[8]; y2 = eqGains[9];
    }
    else
    {
        x1 = 8000; x2 = 16000;
        y1 = eqGains[8]; y2 = qMin(eqGains[8], eqGains[9]);
    }
    double a = static_cast<double>(y1 - y2) / (x1 - x2);
    double b = static_cast<double>(y2) - a * x2;

    // Gain en dB
    double val = a * freq + b;

    // Conversion
    return pow(10.0, 0.1 * val);
}

void SampleUtils::fillSincTable(float * table, int order, int subdivisions, double kaiserBesserBeta)
{
    // Shared with the synth
    SynthMath::fillSincTable(table, order, subdivisions, kaiserBesserBeta);
}

float SampleUtils::quickLoopError(const QVector<float> &vData, quint32 loopStart, quint32 loopEnd, quint32 checkNumber, bool bipolar, float maxValue)
{
    return loopError(vData.constData(), static_cast<quint32>(vData.size()), loopStart, loopEnd,
                     loopErrorOffsets(checkNumber), bipolar, maxValue);
}

QVector<quint32> SampleUtils::loopErrorOffsets(quint32 checkNumber)
{
    // Distances to the loop points where the differences are also computed (increasing)
    QVector<quint32> offsets;
    quint32 offset = 3;
    for (quint32 i = 1; i < checkNumber; i++)
    {
        offset += (M_PI_2 /* something irrational */ + i - 1) * i;
        offsets << offset;
    }
    return offsets;
}

float SampleUtils::loopError(const float * data, quint32 length, quint32 loopStart, quint32 loopEnd,
                             const QVector<quint32> &offsets, bool bipolar, float maxValue)
{
    if (loopStart < 2 || loopStart >= loopEnd || loopEnd >= length)
        return 0;

    // Difference at the loop points, a bit before and after if bipolar
    int beforeCount = loopOffsetsBefore(loopStart, offsets);
    int afterCount = bipolar ? loopOffsetsAfter(loopEnd, length, offsets) : 0;
    float result = loopDifferenceSum(data, loopStart, loopEnd, offsets.constData(), beforeCount, afterCount,
                                     std::numeric_limits<double>::infinity());

    // Maximum value inside the loop
    float max = maxValue;
    if (max < 0)
        max = qMax(0.00045f, peak(data + loopStart, loopEnd - loopStart + 1));

    // Differences are relative to the maximum
    return result / (max * (1 + beforeCount + afterCount));
}

QByteArray SampleUtils::compressSample(qint16* data16, quint32 sampleLength, quint32 sampleRate, double oggQuality)
{
    quint32 BLOCK_SIZE = 1024;
    ogg_stream_state os;
    ogg_page         og;
    ogg_packet       op;
    vorbis_info      vi;
    vorbis_dsp_state vd;
    vorbis_block     vb;
    vorbis_comment   vc;

    vorbis_info_init(&vi);
    if (vorbis_encode_init_vbr(&vi, 1, sampleRate, oggQuality) != 0)
        return QByteArray(); // Failed to initialize vorbis

    vorbis_comment_init(&vc);
    vorbis_analysis_init(&vd, &vi);
    vorbis_block_init(&vd, &vb);
    srand(time(nullptr));
    ogg_stream_init(&os, rand());

    ogg_packet header;
    ogg_packet header_comm;
    ogg_packet header_code;

    vorbis_analysis_headerout(&vd, &vc, &header, &header_comm, &header_code);
    ogg_stream_packetin(&os, &header);
    ogg_stream_packetin(&os, &header_comm);
    ogg_stream_packetin(&os, &header_code);

    QByteArray obuf;
    obuf.reserve(1024 * 1024);

    auto append_page = [&](const ogg_page &pg)
    {
        obuf.append(reinterpret_cast<const char*>(pg.header), pg.header_len);
        obuf.append(reinterpret_cast<const char*>(pg.body),   pg.body_len);
    };

    while (ogg_stream_flush(&os, &og) != 0)
        append_page(og);

    quint32 page = 0;
    quint32 max = 0;
    do
    {
        int bufflength = qMin(BLOCK_SIZE, sampleLength - page * BLOCK_SIZE);
        float **buffer = vorbis_analysis_buffer(&vd, bufflength);

        int j = 0;
        float coef = 1.0f / ((0.5f + 0x7fff) * 1.009f);
        max = qMin((page + 1) * BLOCK_SIZE, sampleLength);

        for (quint32 i = page * BLOCK_SIZE; i < max; i++)
            buffer[0][j++] = (0.5f + data16[i]) * coef;

        vorbis_analysis_wrote(&vd, bufflength);

        while (vorbis_analysis_blockout(&vd, &vb) == 1)
        {
            vorbis_analysis(&vb, nullptr);
            vorbis_bitrate_addblock(&vb);

            while (vorbis_bitrate_flushpacket(&vd, &op))
            {
                ogg_stream_packetin(&os, &op);
                while (ogg_stream_pageout(&os, &og) != 0)
                    append_page(og);
            }
        }

        page++;
    }
    while (max != sampleLength);

    vorbis_analysis_wrote(&vd, 0);

    while (vorbis_analysis_blockout(&vd, &vb) == 1)
    {
        vorbis_analysis(&vb, nullptr);
        vorbis_bitrate_addblock(&vb);

        while (vorbis_bitrate_flushpacket(&vd, &op))
        {
            ogg_stream_packetin(&os, &op);
            while (ogg_stream_pageout(&os, &og) != 0)
                append_page(og);
        }
    }

    ogg_stream_clear(&os);
    vorbis_block_clear(&vb);
    vorbis_dsp_clear(&vd);
    vorbis_comment_clear(&vc);
    vorbis_info_clear(&vi);

    return obuf;
}

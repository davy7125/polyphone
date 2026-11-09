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

#include "soundanalyzer.h"
#include "utils.h"
#include <QJsonArray>
#include <QPainter>
#include <complex>
#include <cmath>
#include <algorithm>

// References for the algorithms:
// - pitch: YIN (de Cheveigné & Kawahara 2002), difference function computed with FFTs
// - harmonics: quadratic interpolation of the peaks on a Blackman-Harris spectrum, harmonic search and inharmonicity
//   fit (f_h / h)^2 = f0^2 (1 + B h^2) as in sms-tools
// - attack: end of the fast rise of the envelope (the "weakest effort" method of the Timbre Toolbox, Peeters et al.
//   2011, stops at the maximum and is misled by a later swell or a tremolo)
// - loudness: ITU-R BS.1770 K-weighting with the coefficients of libebur128 (valid for all sample rates)
// - loop: seam correlation, LPC residual at the junction (click detection), level / spectrum / pitch matches,
//   phase commensurability of the harmonics

namespace
{
typedef std::complex<double> Complex;

const double PI = 3.14159265358979323846;

int previousPowerOfTwo(int n)
{
    int result = 1;
    while (result * 2 <= n)
        result <<= 1;
    return result;
}

double roundDecimals(double value, int decimals)
{
    if (!std::isfinite(value))
        return 0;
    double factor = std::pow(10.0, decimals);
    return std::round(value * factor) / factor;
}

// Vertex of the parabola going through (-1, a), (0, b) and (1, c): offset of the position (between -1 and 1 for a local
// maximum) and value at this position
double parabolicPeak(double a, double b, double c, double &value)
{
    double denominator = a - 2 * b + c;
    double p = std::fabs(denominator) > 1e-12 ? 0.5 * (a - c) / denominator : 0;
    value = b - 0.25 * (a - c) * p;
    return p;
}

// A-weighting gain (linear amplitude) at a frequency
double aWeighting(double f)
{
    double f2 = f * f;
    double ra = 12194.0 * 12194.0 * f2 * f2 /
                ((f2 + 20.6 * 20.6) * std::sqrt((f2 + 107.7 * 107.7) * (f2 + 737.9 * 737.9)) * (f2 + 12194.0 * 12194.0));
    return ra * 1.2589254; // +2 dB
}

// Amplitude of the component at a frequency (DFT of a non uniformly sampled track)
double componentAmplitude(const QVector<double> &times, const QVector<double> &values, double frequency)
{
    double re = 0, im = 0;
    for (int i = 0; i < times.size(); i++)
    {
        re += values[i] * std::cos(2 * PI * frequency * times[i]);
        im -= values[i] * std::sin(2 * PI * frequency * times[i]);
    }
    return 2 * std::sqrt(re * re + im * im) / times.size();
}

// Find a periodic modulation (vibrato, tremolo) in a track: rate and amplitude, false if none
bool findModulation(QVector<double> times, QVector<double> values, double minRate, double maxRate,
                    double &rate, double &amplitude)
{
    if (times.size() < 16 || times.last() - times.first() < 2.5 / minRate)
        return false;

    // Remove the trend
    double slope, intercept;
    if (SampleUtils::linearRegression(times, values, QVector<double>(), slope, intercept))
        for (int i = 0; i < values.size(); i++)
            values[i] -= slope * times[i] + intercept;

    QVector<double> amplitudes;
    double bestAmplitude = 0, bestRate = 0;
    for (double f = 0.5 * minRate; f <= 1.5 * maxRate; f += 0.05)
    {
        double a = componentAmplitude(times, values, f);
        amplitudes << a;
        if (f >= minRate && f <= maxRate && a > bestAmplitude)
        {
            bestAmplitude = a;
            bestRate = f;
        }
    }

    // The peak must stand out
    if (bestAmplitude <= 0 || bestAmplitude < 2.5 * SampleUtils::median(amplitudes))
        return false;
    rate = bestRate;
    amplitude = bestAmplitude;
    return true;
}
}

SoundAnalyzer::SoundAnalyzer(const QVector<float> &data, quint32 sampleRate, quint32 loopStart, quint32 loopEnd,
                             double expectedFrequency) :
    _sampleRate(sampleRate == 0 ? 44100 : sampleRate),
    _length(static_cast<int>(data.size())),
    _loopStart(static_cast<int>(loopStart)),
    _loopEnd(static_cast<int>(loopEnd)),
    _expectedFrequency(expectedFrequency),
    _maxPitchFrames(300),
    _maxSpectrumFrames(48),
    _regionsComputed(false),
    _pitchComputed(false),
    _harmonicsComputed(false),
    _spectrumComputed(false),
    _loudnessComputed(false),
    _loopComputed(false),
    _loopCorrelation(std::nan(""))
{
    _source = data;
    _data.resize(_length);
    for (int i = 0; i < _length; i++)
        _data[i] = static_cast<double>(data[i]);
    _cumSquares = SampleUtils::cumulativeSquares(data);
    _looped = SampleUtils::isLooped(loopStart, loopEnd, static_cast<quint32>(_length)); // Single-cycle loops included
}

void SoundAnalyzer::setSecondChannel(const QVector<float> &data)
{
    QVector<float> copy = data;
    copy.resize(_length); // Same length, zeros if shorter
    _data2.resize(_length);
    for (int i = 0; i < _length; i++)
        _data2[i] = static_cast<double>(copy[i]);
    _cumSquares2 = SampleUtils::cumulativeSquares(copy);
}

double SoundAnalyzer::meanSquare(int start, int end) const
{
    if (end <= start)
        return 0;
    double value = (_cumSquares[end] - _cumSquares[start]) / (end - start);
    if (!_cumSquares2.isEmpty())
        value = 0.5 * (value + (_cumSquares2[end] - _cumSquares2[start]) / (end - start));
    return value;
}

const FourierTransform &SoundAnalyzer::getFourierTransform(int size) const
{
    auto it = _fourierTransforms.find(size);
    if (it == _fourierTransforms.end())
        it = _fourierTransforms.emplace(size, FourierTransform(size)).first;
    return it->second;
}

QJsonValue SoundAnalyzer::jsonNumber(double value, int decimals)
{
    if (!std::isfinite(value))
        return QJsonValue();
    return roundDecimals(value, decimals);
}

//////////////////////////////////////////////////////////
/// REGIONS: onset, attack, steady part, noise floor  ///
//////////////////////////////////////////////////////////

void SoundAnalyzer::computeRegions()
{
    if (_regionsComputed)
        return;
    _regionsComputed = true;

    _peak = 0;
    _peakPosition = 0;
    for (int i = 0; i < _length; i++)
    {
        if (std::fabs(_data[i]) > _peak)
        {
            _peak = std::fabs(_data[i]);
            _peakPosition = i;
        }
    }

    // Onset and noise floor
    SampleUtils::NoiseReference noiseReference;
    _onset = static_cast<int>(SampleUtils::findOnset(_source, _sampleRate, _looped, _noiseFloorDb, &noiseReference));
    if (noiseReference == SampleUtils::NOISE_BEFORE_ONSET)
        _noiseReference = "before the onset";
    else if (noiseReference == SampleUtils::NOISE_AT_END)
        _noiseReference = "end of the sample";
    else
        _noiseReference = "";
    int hop = qMax(1, static_cast<int>(0.01 * _sampleRate));

    // Attack: end of the fast rise, i.e. the first point near the maximum (12 dB) where the level stops increasing
    // quickly (10 dB/s), robust to a later swell or a tremolo; start at 20 dB below the level reached
    int attackWindow = qMax(1, static_cast<int>(0.005 * _sampleRate));
    int attackHop = qMax(1, static_cast<int>(0.001 * _sampleRate));
    int attackEndLimit = qMin(_length, _onset + static_cast<int>(3 * _sampleRate));
    QVector<double> e = rmsEnvelope(attackWindow, attackHop, _onset, attackEndLimit);
    QVector<double> levels(e.size());
    for (int i = 0; i < e.size(); i++)
    {
        // Smoothing over 5 ms
        double sum = 0;
        int count = 0;
        for (int j = qMax(0, i - 2); j <= qMin(static_cast<int>(e.size()) - 1, i + 2); j++)
        {
            sum += e[j] * e[j];
            count++;
        }
        levels[i] = SampleUtils::toDb(std::sqrt(sum / count));
    }
    double maxLevel = -200;
    foreach (double level, levels)
        maxLevel = qMax(maxLevel, level);
    int slopeFrames = qMax(1, static_cast<int>(0.03 * _sampleRate / attackHop));
    double slopeDuration = static_cast<double>(slopeFrames * attackHop) / _sampleRate;
    int iEnd = static_cast<int>(levels.size()) - 1;
    for (int i = 0; i + slopeFrames < levels.size(); i++)
    {
        if (levels[i] >= maxLevel - 12 && (levels[i + slopeFrames] - levels[i]) / slopeDuration < 10)
        {
            iEnd = i;
            break;
        }
    }
    int iStart = 0;
    while (iStart < iEnd && levels[iStart] < levels[qMax(0, iEnd)] - 20)
        iStart++;
    _attackStart = qMin(_length - 1, _onset + iStart * attackHop + attackWindow / 2);
    _attackEnd = qMin(_length - 1, _onset + qMax(0, iEnd) * attackHop + attackWindow / 2);
    _attackEnd = qMax(_attackEnd, _attackStart);

    // Steady part: the loop, otherwise the steady part found by a segmentation of the envelope (the same as the loop
    // search of the editor), otherwise after the attack until the level drops by 15 dB (1.5 s max)
    quint32 steadyStart, steadyEnd;
    _steadyPartFound = false;
    if (_looped && _loopEnd - _loopStart >= 0.02 * _sampleRate)
    {
        _steadyStart = _loopStart;
        _steadyEnd = _loopEnd;
    }
    else if (SampleUtils::findSteadyPart(_source, _sampleRate, steadyStart, steadyEnd) && steadyEnd > steadyStart)
    {
        _steadyStart = static_cast<int>(steadyStart);
        _steadyEnd = static_cast<int>(steadyEnd);
        _steadyPartFound = true;
    }
    else
    {
        _steadyStart = qMin(_length, _attackEnd + static_cast<int>(0.02 * _sampleRate));
        int steadyWindow = qMax(1, static_cast<int>(0.05 * _sampleRate));
        QVector<double> env = rmsEnvelope(steadyWindow, hop, _steadyStart);
        double maxAfter = 0;
        int count300ms = static_cast<int>(0.3 * _sampleRate / hop);
        for (int i = 0; i < qMin(static_cast<int>(env.size()), count300ms); i++)
            maxAfter = qMax(maxAfter, env[i]);
        int endFrame = 0;
        while (endFrame < env.size() && SampleUtils::toDb(env[endFrame]) > SampleUtils::toDb(maxAfter) - 15)
            endFrame++;
        _steadyEnd = qMin(_length, qMin(_steadyStart + endFrame * hop + steadyWindow,
                                        _steadyStart + static_cast<int>(1.5 * _sampleRate)));
        if (_steadyEnd - _steadyStart < 0.05 * _sampleRate)
        {
            // Short sound: from the onset to the end
            _steadyStart = _onset;
            _steadyEnd = _length;
        }
    }
}

bool SoundAnalyzer::isLooped()
{
    return _looped;
}

QJsonObject SoundAnalyzer::getRegions()
{
    computeRegions();
    QJsonObject result;
    double sr = _sampleRate;
    result["duration_s"] = roundDecimals(_length / sr, 3);
    result["onset_ms"] = roundDecimals(1000 * _onset / sr, 1);
    result["attack_ms"] = QJsonArray({ roundDecimals(1000 * _attackStart / sr, 1), roundDecimals(1000 * _attackEnd / sr, 1) });
    result["peak_time_ms"] = roundDecimals(1000 * _peakPosition / sr, 1);
    result["analyzed_part_s"] = QJsonArray({ roundDecimals(_steadyStart / sr, 3), roundDecimals(_steadyEnd / sr, 3) });
    result["analyzed_part"] = (_looped && _steadyStart == _loopStart) ? "loop" : (_steadyPartFound ? "steady part" : "after the attack");
    if (_looped)
        result["loop_s"] = QJsonArray({ roundDecimals(_loopStart / sr, 4), roundDecimals(_loopEnd / sr, 4) });
    return result;
}

//////////////////////
/// LEVELS        ///
//////////////////////

QVector<double> SoundAnalyzer::kWeighted(const QVector<double> &data) const
{
    // Coefficients of libebur128 (analog prototypes giving the ITU-R BS.1770 filters at 48 kHz)
    double fs = _sampleRate;
    double f0 = 1681.974450955533;
    double G = 3.999843853973347;
    double Q = 0.7071752369554196;
    double K = std::tan(PI * f0 / fs);
    double Vh = std::pow(10.0, G / 20.0);
    double Vb = std::pow(Vh, 0.4996667741545416);
    double a0 = 1.0 + K / Q + K * K;
    double b[3] = { (Vh + Vb * K / Q + K * K) / a0, 2.0 * (K * K - Vh) / a0, (Vh - Vb * K / Q + K * K) / a0 };
    double a[3] = { 1.0, 2.0 * (K * K - 1.0) / a0, (1.0 - K / Q + K * K) / a0 };

    f0 = 38.13547087602444;
    Q = 0.5003270373238773;
    K = std::tan(PI * f0 / fs);
    double hb[3] = { 1.0, -2.0, 1.0 };
    double ha[3] = { 1.0, 2.0 * (K * K - 1.0) / (1.0 + K / Q + K * K), (1.0 - K / Q + K * K) / (1.0 + K / Q + K * K) };

    // First stage (high shelf)
    int length = static_cast<int>(data.size());
    QVector<double> result(length);
    double x1 = 0, x2 = 0, y1 = 0, y2 = 0;
    for (int i = 0; i < length; i++)
    {
        double x = data[i];
        double y = b[0] * x + b[1] * x1 + b[2] * x2 - a[1] * y1 - a[2] * y2;
        x2 = x1; x1 = x; y2 = y1; y1 = y;
        result[i] = y;
    }

    // Second stage (high-pass)
    double u1 = 0, u2 = 0, z1 = 0, z2 = 0;
    for (int i = 0; i < length; i++)
    {
        double u = result[i];
        double z = hb[0] * u + hb[1] * u1 + hb[2] * u2 - ha[1] * z1 - ha[2] * z2;
        u2 = u1; u1 = u; z2 = z1; z1 = z;
        result[i] = z;
    }
    return result;
}

void SoundAnalyzer::computeLoudness()
{
    if (_loudnessComputed)
        return;
    _loudnessComputed = true;
    computeRegions();

    // Energy of the K-weighted channels, summed for a stereo signal (ITU-R BS.1770)
    QVector<double> k = kWeighted(_data);
    QVector<double> k2 = _data2.isEmpty() ? QVector<double>() : kWeighted(_data2);
    QVector<double> cum(_length + 1);
    cum[0] = 0;
    for (int i = 0; i < _length; i++)
        cum[i + 1] = cum[i] + k[i] * k[i] + (k2.isEmpty() ? 0 : k2[i] * k2[i]);
    auto loudness = [&](int start, int end) -> double {
        if (end <= start)
            return -200;
        double meanSquare = (cum[end] - cum[start]) / (end - start);
        return meanSquare > 0 ? -0.691 + 10 * std::log10(meanSquare) : -200;
    };

    _steadyLoudness = loudness(_steadyStart, _steadyEnd);

    // Momentary loudness (400 ms) and gated integrated loudness, blocks every 100 ms
    int block = static_cast<int>(0.4 * _sampleRate);
    int step = static_cast<int>(0.1 * _sampleRate);
    QVector<double> blocks;
    if (_length < block)
        blocks << loudness(0, _length);
    else
        for (int start = 0; start + block <= _length; start += step)
            blocks << loudness(start, start + block);
    _momentaryMax = -200;
    foreach (double value, blocks)
        _momentaryMax = qMax(_momentaryMax, value);

    double sum = 0;
    int count = 0;
    foreach (double value, blocks)
    {
        if (value > -70)
        {
            sum += std::pow(10.0, (value + 0.691) / 10);
            count++;
        }
    }
    _integratedLoudness = -200;
    if (count > 0)
    {
        double relativeGate = -0.691 + 10 * std::log10(sum / count) - 10;
        sum = 0;
        count = 0;
        foreach (double value, blocks)
        {
            if (value > -70 && value > relativeGate)
            {
                sum += std::pow(10.0, (value + 0.691) / 10);
                count++;
            }
        }
        if (count > 0)
            _integratedLoudness = -0.691 + 10 * std::log10(sum / count);
    }
}

double SoundAnalyzer::truePeak(const QVector<double> &data) const
{
    // Oversampling with a windowed sinc of 49 taps (as libebur128)
    int factor = _sampleRate < 96000 ? 4 : (_sampleRate < 192000 ? 2 : 1);
    int length = static_cast<int>(data.size());
    double peak = 0;
    for (int i = 0; i < length; i++)
        peak = qMax(peak, std::fabs(data[i]));
    if (factor == 1)
        return peak;

    const int taps = 49, half = 24;
    QVector<double> filter(taps * factor);
    for (int phase = 0; phase < factor; phase++)
    {
        for (int j = 0; j < taps; j++)
        {
            double m = (j - half) - static_cast<double>(phase) / factor;
            double sinc = std::fabs(m) < 1e-12 ? 1.0 : std::sin(PI * m) / (PI * m);
            double window = 0.5 * (1 - std::cos(2 * PI * (j + static_cast<double>(phase) / factor) / (taps)));
            filter[phase * taps + j] = sinc * window;
        }
    }

    for (int i = half; i < length - half; i++)
    {
        for (int phase = 1; phase < factor; phase++)
        {
            double value = 0;
            const double * f = filter.constData() + phase * taps;
            for (int j = 0; j < taps; j++)
                value += f[j] * data[i + j - half];
            peak = qMax(peak, std::fabs(value));
        }
    }
    return peak;
}

QJsonObject SoundAnalyzer::getLevels()
{
    computeRegions();
    computeLoudness();
    computeSpectrum();

    // Second channel: largest peak, mean power of the channels
    QJsonObject result;
    double peak = _peak;
    double truePeakValue = truePeak(_data);
    if (!_data2.isEmpty())
    {
        for (int i = 0; i < _length; i++)
            peak = qMax(peak, std::fabs(_data2[i]));
        truePeakValue = qMax(truePeakValue, truePeak(_data2));
    }
    double steadyRms = std::sqrt(meanSquare(_steadyStart, _steadyEnd));
    result["peak_dbfs"] = jsonNumber(SampleUtils::toDb(peak), 2);
    result["true_peak_dbtp"] = jsonNumber(SampleUtils::toDb(truePeakValue), 2);
    result["rms_dbfs"] = jsonNumber(SampleUtils::toDb(std::sqrt(meanSquare(0, _length))), 2);
    result["steady_rms_dbfs"] = jsonNumber(SampleUtils::toDb(steadyRms), 2);
    result["crest_factor_db"] = jsonNumber(SampleUtils::toDb(peak) - SampleUtils::toDb(steadyRms), 2);
    result["steady_loudness_lufs"] = jsonNumber(_steadyLoudness, 2);
    result["momentary_max_lufs"] = jsonNumber(_momentaryMax, 2);
    if (_length >= 0.4 * _sampleRate)
        result["integrated_lufs"] = jsonNumber(_integratedLoudness, 2);

    // A-weighted level of the steady part (frequency balance close to the ear at moderate levels)
    if (!_power.isEmpty())
    {
        double sum = 0;
        for (int k = 1; k < _power.size(); k++)
        {
            double f = static_cast<double>(k) * _sampleRate / _fftSize;
            double gain = aWeighting(f);
            sum += _power[k] * gain * gain;
        }
        double meanSquare = 2 * sum / (static_cast<double>(_fftSize) * _windowSquareSum);
        result["steady_a_weighted_dbfs"] = jsonNumber(meanSquare > 0 ? 10 * std::log10(meanSquare) : -200, 2);
    }
    result["loudness_note"] = QString(_data2.isEmpty() ? "LUFS of the mono signal" : "LUFS of the stereo signal (sum of the "
                              "channels, rms and A-weighted levels being the mean power of the channels)") +
                              " (BS.1770 K-weighting): steady_loudness_lufs for the sustained part (the loop if any), "
                              "momentary_max_lufs with the attack";
    return result;
}

//////////////////////
/// PITCH         ///
//////////////////////

double SoundAnalyzer::yin(int start, int length, double fMin, double fMax, double &confidence) const
{
    confidence = 0;
    int tauMin = qMax(2, static_cast<int>(std::floor(_sampleRate / fMax)));
    int tauMax = static_cast<int>(std::ceil(_sampleRate / fMin));
    if (start < 0 || start + length + tauMax + 1 > _length || length < 16 || tauMax <= tauMin + 2)
        return 0;

    double energy0 = _cumSquares[start + length] - _cumSquares[start];
    if (energy0 < 1e-12 * length)
        return 0;

    // Cross-correlation of the window with the following signal
    int n = SampleUtils::nextPowerOfTwo(2 * length + tauMax + 1);
    std::vector<Complex> a(n, Complex(0)), b(n, Complex(0));
    for (int i = 0; i < length; i++)
        a[i] = _data[start + i];
    for (int i = 0; i < length + tauMax + 1; i++)
        b[i] = _data[start + i];
    const FourierTransform &fourierTransform = getFourierTransform(n);
    fourierTransform.transform(a, false);
    fourierTransform.transform(b, false);
    for (int i = 0; i < n; i++)
        a[i] = std::conj(a[i]) * b[i];
    fourierTransform.transform(a, true);

    // Cumulative mean normalized difference
    QVector<double> cmndf(tauMax + 1);
    cmndf[0] = 1;
    double runningSum = 0;
    for (int tau = 1; tau <= tauMax; tau++)
    {
        double energyTau = _cumSquares[start + tau + length] - _cumSquares[start + tau];
        double d = qMax(0.0, energy0 + energyTau - 2 * a[tau].real());
        runningSum += d;
        cmndf[tau] = runningSum > 0 ? d * tau / runningSum : 1;
    }

    // First dip below the threshold, then the local minimum, otherwise the global minimum
    int best = -1;
    for (int tau = tauMin; tau < tauMax; tau++)
    {
        if (cmndf[tau] < 0.15)
        {
            while (tau + 1 < tauMax && cmndf[tau + 1] < cmndf[tau])
                tau++;
            best = tau;
            break;
        }
    }
    if (best < 0)
    {
        best = tauMin;
        for (int tau = tauMin; tau < tauMax; tau++)
            if (cmndf[tau] < cmndf[best])
                best = tau;
    }

    // The first dip may be a harmonic (dominant third harmonic of a clarinet or a cromorne): a multiple of this lag
    // giving a much deeper dip is the period
    for (int k = 2; k <= 6 && best * k + k < tauMax; k++)
    {
        int multiple = best * k;
        for (int tau = multiple - k; tau <= multiple + k; tau++)
            if (cmndf[tau] < cmndf[multiple])
                multiple = tau;
        if (cmndf[multiple] < 0.5 * cmndf[best] && cmndf[multiple] < cmndf[best] - 0.05)
        {
            best = multiple;
            break;
        }
    }

    double shift = 0;
    if (best > 1 && best < tauMax)
    {
        double denominator = 2 * (cmndf[best - 1] - 2 * cmndf[best] + cmndf[best + 1]);
        if (std::fabs(denominator) > 1e-12)
            shift = qBound(-1.0, (cmndf[best - 1] - cmndf[best + 1]) / denominator, 1.0);
    }
    confidence = qBound(0.0, 1.0 - cmndf[best], 1.0);
    return _sampleRate / (best + shift);
}

void SoundAnalyzer::getPitchRange(double &fMin, double &fMax) const
{
    if (_expectedFrequency > 0)
    {
        fMin = qMax(15.0, _expectedFrequency / 1.5);
        fMax = qMin(_sampleRate / 4.0, _expectedFrequency * 1.5);
    }
    else
    {
        // Whole range of the instruments, from a 32' organ pipe to the highest ranks of a mixture
        fMin = 16;
        fMax = _sampleRate / 4.0;
    }
}

void SoundAnalyzer::computePitch()
{
    if (_pitchComputed)
        return;
    _pitchComputed = true;
    computeRegions();
    _f0 = 0;
    _f0Confidence = 0;
    _driftCentsPerSecond = std::nan("");
    _stabilityCents = std::nan("");
    _vibratoRate = 0;
    _vibratoDepth = 0;
    _pitchFrames = 0;

    double fMin, fMax;
    getPitchRange(fMin, fMax);
    int tauMax = static_cast<int>(std::ceil(_sampleRate / fMin));
    int window = qMax(static_cast<int>(0.03 * _sampleRate), 3 * tauMax);

    int start = _steadyStart, end = _steadyEnd;
    if (end - start < window + tauMax + 2)
    {
        start = _onset;
        end = _length;
    }
    if (end - start < window + tauMax + 2)
    {
        window = (end - start - tauMax - 2) / 2;
        if (window < 2 * static_cast<int>(_sampleRate / fMax))
            return;
    }
    int lastStart = qMin(_length, end + tauMax) - window - tauMax - 1;
    int hop = qMax(static_cast<int>(0.01 * _sampleRate), (lastStart - start) / _maxPitchFrames + 1);

    QVector<double> times, frequencies, confidences;
    for (int position = start; position <= lastStart; position += hop)
    {
        double confidence;
        double f = yin(position, window, fMin, fMax, confidence);
        if (f > 0 && confidence >= 0.6)
        {
            times << static_cast<double>(position + window / 2) / _sampleRate;
            frequencies << f;
            confidences << confidence;
        }
    }
    _pitchFrames = static_cast<int>(frequencies.size());
    if (frequencies.isEmpty())
        return;

    _f0 = SampleUtils::median(frequencies);
    _f0Confidence = SampleUtils::median(confidences);

    // Track in cents, without the outliers (octave errors)
    QVector<double> t, cents;
    for (int i = 0; i < frequencies.size(); i++)
    {
        double c = 1200 * std::log2(frequencies[i] / _f0);
        if (std::fabs(c) < 300)
        {
            t << times[i];
            cents << c;
        }
    }
    if (cents.size() >= 3)
        _stabilityCents = SampleUtils::percentile(cents, 0.95) - SampleUtils::percentile(cents, 0.05);
    double slope, intercept;
    if (cents.size() >= 5 && t.last() - t.first() >= 0.2 && SampleUtils::linearRegression(t, cents, QVector<double>(), slope, intercept))
        _driftCentsPerSecond = slope;

    double rate, amplitude;
    if (findModulation(t, cents, 3, 9, rate, amplitude) && amplitude >= 3)
    {
        _vibratoRate = rate;
        _vibratoDepth = amplitude;
    }
}

double SoundAnalyzer::estimatePitch(const QVector<float> &data, quint32 sampleRate, quint32 start, quint32 end, float &confidence)
{
    SoundAnalyzer analyzer(data, sampleRate, start, end, 0);
    analyzer.setMaxPitchFrames(8);
    analyzer.setMaxSpectrumFrames(8);
    double frequency = analyzer.getFundamental();
    confidence = frequency > 0 ? static_cast<float>(analyzer._f0Confidence) : 0.f;
    return frequency;
}

QList<Peak> SoundAnalyzer::computePeaks(const QVector<float> &data, quint32 sampleRate, quint32 start, quint32 end,
                                        QVector<float> &spectrum, int &spectrumMax, int * key, int * correction, float * reliance)
{
    spectrumMax = -1;
    if (reliance != nullptr)
        *reliance = -1.0f;
    quint32 length = static_cast<quint32>(data.size());
    if (length == 0 || sampleRate == 0)
        return QList<Peak>();

    // If there is no loop, the steady part (otherwise the most stable part) widened to 0.5 second if possible
    if (start == end || end > length)
    {
        SampleUtils::findSteadyPart(data, sampleRate, start, end);
        quint32 minimum = qMin(length, sampleRate / 2);
        if (end < start + minimum)
        {
            qint64 position = (static_cast<qint64>(start) + end) / 2 - minimum / 2;
            start = static_cast<quint32>(qBound(static_cast<qint64>(0), position, static_cast<qint64>(length - minimum)));
            end = start + minimum;
        }
    }
    if (end <= start)
    {
        start = 0;
        end = length;
    }

    // Pitch: YIN algorithm on the portion, refined with the harmonics
    float confidence;
    double frequency = estimatePitch(data, sampleRate, start, end, confidence);
    if (frequency > 0)
    {
        int closestKey, keyCorrection;
        SampleUtils::toKeyAndCorrection(SampleUtils::frequencyToKey(frequency), closestKey, keyCorrection);
        if (closestKey >= 0 && closestKey <= 127)
        {
            if (key != nullptr)
                *key = closestKey;
            if (correction != nullptr)
                *correction = keyCorrection;
            if (reliance != nullptr)
                *reliance = confidence;
        }
    }

    // Fourier transform of 0.5 second of the portion (repeated if shorter)
    if (end > sampleRate / 2 + start)
    {
        quint32 offset = (end - sampleRate / 2 - start) / 2;
        start += offset;
        end -= offset;
    }
    QVector<float> portion = data.mid(static_cast<int>(start), static_cast<int>(end - start));
    while (portion.size() < 0.5 * sampleRate)
        portion += data.mid(static_cast<int>(start), static_cast<int>(end - start));
    spectrum = SampleUtils::getFourierTransform(portion);
    quint32 fftSize = static_cast<quint32>(spectrum.size()) * 2;

    // Main peaks
    QList<quint32> maxima = SampleUtils::findMax(spectrum, 50, 0.05f);
    if (maxima.isEmpty())
        return QList<Peak>();
    spectrumMax = static_cast<int>(maxima[0]);

    // Frequency and level of the peaks interpolated between the bins (parabola on the levels in dB)
    QVector<double> frequencies, levels;
    double maxLevel = -1000;
    foreach (quint32 index, maxima)
    {
        auto db = [&](quint32 i) -> double { return 20 * std::log10(static_cast<double>(spectrum[static_cast<int>(i)]) + 1e-30); };
        double level;
        double offset = parabolicPeak(db(index - 1), db(index), db(index + 1), level); // findMax excludes the first and last bins
        frequencies << (index + offset) * sampleRate / fftSize;
        levels << level;
        maxLevel = qMax(maxLevel, level);
    }

    QList<Peak> result;
    for (int i = 0; i < maxima.size(); i++)
    {
        int peakKey, peakCorrection;
        double fractionalKey = qBound(0.0, SampleUtils::frequencyToKey(frequencies[i]), 128.0);
        SampleUtils::toKeyAndCorrection(fractionalKey, peakKey, peakCorrection);
        result << Peak(std::pow(10.0, (levels[i] - maxLevel) / 20), frequencies[i], peakKey, peakCorrection);
    }
    return result;
}

QJsonObject SoundAnalyzer::getRelease(const QVector<float> &data, quint32 sampleRate, int releasePosition)
{
    QJsonObject result;
    QVector<double> cumSquares = SampleUtils::cumulativeSquares(data);
    int window = qMax(1, static_cast<int>(0.02 * sampleRate));
    int hop = qMax(1, static_cast<int>(0.005 * sampleRate));
    auto rmsAt = [&](int start) -> double { return SampleUtils::rms(cumSquares, start, start + window); };
    double releaseLevel = rmsAt(qMax(0, releasePosition - window));
    double peakRms = 0;
    for (int i = 0; i + window < releasePosition; i += hop)
        peakRms = qMax(peakRms, rmsAt(i));
    if (releaseLevel > 0 && peakRms > 0)
    {
        result["level_at_release_db"] = jsonNumber(20 * std::log10(releaseLevel / peakRms), 1);
        QJsonObject releaseTimes;
        foreach (int drop, QList<int>({ 20, 40, 60 }))
        {
            double target = releaseLevel * std::pow(10.0, -drop / 20.0);
            for (int i = releasePosition; i + window <= data.size(); i += hop)
            {
                if (rmsAt(i) < target)
                {
                    releaseTimes[QString("minus_%1_db_s").arg(drop)] = jsonNumber(static_cast<double>(i - releasePosition) / sampleRate, 3);
                    break;
                }
            }
        }
        result["release"] = releaseTimes;
    }
    return result;
}

double SoundAnalyzer::getFundamental()
{
    computeHarmonics();
    return _refinedF0 > 0 ? _refinedF0 : _f0;
}

QJsonObject SoundAnalyzer::getPitch()
{
    computePitch();
    computeHarmonics();
    QJsonObject result;
    if (_f0 <= 0)
    {
        result["found"] = false;
        result["note"] = "no stable pitch found (noise, percussion, or a pitch far from the expected one)";
        return result;
    }

    double f = _refinedF0 > 0 ? _refinedF0 : _f0;
    double key = SampleUtils::frequencyToKey(f);
    int nearestKey = qRound(key);
    result["found"] = true;
    result["frequency_hz"] = Utils::roundSignificant(f, 6);
    result["nearest_key"] = nearestKey;
    result["cents_from_nearest_key"] = roundDecimals(100 * (key - nearestKey), 1);
    result["confidence"] = roundDecimals(_f0Confidence, 3);
    if (std::isfinite(_stabilityCents))
        result["stability_cents"] = roundDecimals(_stabilityCents, 1);
    if (std::isfinite(_driftCentsPerSecond))
        result["drift_cents_per_s"] = roundDecimals(_driftCentsPerSecond, 2);
    if (_vibratoDepth > 0)
    {
        result["vibrato_rate_hz"] = roundDecimals(_vibratoRate, 2);
        result["vibrato_depth_cents"] = roundDecimals(_vibratoDepth, 1);
    }
    if (_harmonicsComputed && _inharmonicity > 0)
        result["first_partial_hz"] = _harmonics.isEmpty() || !_harmonics[0].found ? QJsonValue() :
                                         QJsonValue(Utils::roundSignificant(_harmonics[0].frequency, 6));
    return result;
}

//////////////////////////
/// SPECTRUM          ///
//////////////////////////

QVector<double> SoundAnalyzer::averagePowerSpectrum(int start, int end, int frameLength, int fftSize,
                                                    bool blackmanHarris, double &windowSum, double &windowSquareSum) const
{
    QVector<double> window(frameLength);
    windowSum = 0;
    windowSquareSum = 0;
    for (int i = 0; i < frameLength; i++)
    {
        double x = 2 * PI * i / frameLength;
        window[i] = blackmanHarris ? 0.35875 - 0.48829 * std::cos(x) + 0.14128 * std::cos(2 * x) - 0.01168 * std::cos(3 * x) :
                                     0.5 - 0.5 * std::cos(x);
        windowSum += window[i];
        windowSquareSum += window[i] * window[i];
    }

    QVector<double> power(fftSize / 2 + 1, 0.0);
    start = qBound(0, start, _length);
    end = qBound(start, end, _length);
    if (end - start < frameLength)
        return power;

    int frameCount = qMin(_maxSpectrumFrames, (end - start - frameLength) / qMax(1, frameLength / 4) + 1);
    double step = frameCount > 1 ? static_cast<double>(end - start - frameLength) / (frameCount - 1) : 0;
    std::vector<Complex> buffer(fftSize);
    const FourierTransform &fourierTransform = getFourierTransform(fftSize);
    int channelCount = _data2.isEmpty() ? 1 : 2;
    for (int frame = 0; frame < frameCount; frame++)
    {
        int position = start + static_cast<int>(frame * step);
        for (int channel = 0; channel < channelCount; channel++)
        {
            const QVector<double> &data = channel == 0 ? _data : _data2;
            std::fill(buffer.begin(), buffer.end(), Complex(0));
            for (int i = 0; i < frameLength; i++)
                buffer[i] = data[position + i] * window[i];
            fourierTransform.transform(buffer, false);
            for (int k = 0; k <= fftSize / 2; k++)
                power[k] += std::norm(buffer[k]) / (frameCount * channelCount); // Mean power of the channels
        }
    }
    return power;
}

QVector<double> SoundAnalyzer::getThirdOctaveFrequencies()
{
    QVector<double> result;
    for (int i = -17; i <= 13; i++) // 20 Hz to 20 kHz
        result << 1000.0 * std::pow(2.0, i / 3.0);
    return result;
}

QVector<double> SoundAnalyzer::bandLevels(const QVector<double> &power, int fftSize, const QVector<double> &centers,
                                          double bandWidthOctave) const
{
    QVector<double> result;
    double binWidth = static_cast<double>(_sampleRate) / fftSize;
    foreach (double center, centers)
    {
        double low = center * std::pow(2.0, -bandWidthOctave / 2);
        double high = center * std::pow(2.0, bandWidthOctave / 2);
        if (center >= _sampleRate / 2.0 || power.isEmpty())
        {
            result << -200;
            continue;
        }
        int kLow = qMax(1, static_cast<int>(std::ceil(low / binWidth)));
        int kHigh = qMin(static_cast<int>(power.size()) - 1, static_cast<int>(std::floor(high / binWidth)));
        double sum = 0;
        if (kHigh < kLow)
        {
            // Band narrower than a bin: density of the closest bin
            int k = qBound(1, qRound(center / binWidth), static_cast<int>(power.size()) - 1);
            sum = power[k] * (high - low) / binWidth;
        }
        else
            for (int k = kLow; k <= kHigh; k++)
                sum += power[k];
        result << (sum > 0 ? 10 * std::log10(sum) : -200);
    }
    return result;
}

void SoundAnalyzer::computeSpectrum()
{
    if (_spectrumComputed)
        return;
    _spectrumComputed = true;
    computeRegions();
    _centroid = _spread = _skewness = _kurtosis = _rolloff85 = _rolloff95 = 0;
    _flatness = _flatnessHigh = _tilt = std::nan("");

    int start = _steadyStart, end = _steadyEnd;
    _fftSize = _sampleRate <= 48000 ? 2048 : (_sampleRate <= 96000 ? 4096 : 8192);
    if (end - start < _fftSize)
    {
        start = _onset;
        end = _length;
    }
    if (end - start < _fftSize)
        _fftSize = qMax(256, previousPowerOfTwo(end - start));
    if (end - start < _fftSize)
    {
        _power.clear();
        return;
    }
    _power = averagePowerSpectrum(start, end, _fftSize, _fftSize, false, _windowSum, _windowSquareSum);

    double binWidth = static_cast<double>(_sampleRate) / _fftSize;
    int kMin = qMax(1, static_cast<int>(std::ceil(20 / binWidth)));
    int kMax = qMin(static_cast<int>(_power.size()) - 1, static_cast<int>(20000 / binWidth));
    double total = 0;
    for (int k = kMin; k <= kMax; k++)
        total += _power[k];
    if (total <= 0)
        return;

    for (int k = kMin; k <= kMax; k++)
        _centroid += k * binWidth * _power[k] / total;
    for (int k = kMin; k <= kMax; k++)
    {
        double d = k * binWidth - _centroid;
        double p = _power[k] / total;
        _spread += d * d * p;
        _skewness += d * d * d * p;
        _kurtosis += d * d * d * d * p;
    }
    _spread = std::sqrt(_spread);
    if (_spread > 0)
    {
        _skewness /= _spread * _spread * _spread;
        _kurtosis /= _spread * _spread * _spread * _spread;
    }

    double cumulated = 0;
    for (int k = kMin; k <= kMax; k++)
    {
        cumulated += _power[k];
        if (_rolloff85 == 0 && cumulated >= 0.85 * total)
            _rolloff85 = k * binWidth;
        if (_rolloff95 == 0 && cumulated >= 0.95 * total)
        {
            _rolloff95 = k * binWidth;
            break;
        }
    }

    // Flatness (0 dB = white noise, very negative = tonal)
    auto flatness = [&](double fLow, double fHigh) -> double {
        int k1 = qMax(1, static_cast<int>(fLow / binWidth));
        int k2 = qMin(static_cast<int>(_power.size()) - 1, static_cast<int>(fHigh / binWidth));
        if (k2 - k1 < 8)
            return std::nan("");
        double logSum = 0, sum = 0;
        for (int k = k1; k <= k2; k++)
        {
            logSum += std::log(_power[k] + 1e-20);
            sum += _power[k];
        }
        int count = k2 - k1 + 1;
        return 10 * std::log10(std::exp(logSum / count) / (sum / count + 1e-20));
    };
    double fTop = qMin(16000.0, 0.95 * _sampleRate / 2);
    _flatness = flatness(20, fTop);
    if (fTop > 5000)
        _flatnessHigh = flatness(4000, fTop);

    // Tilt: regression of the third-octave levels between 100 Hz and 16 kHz
    QVector<double> centers, selected, x, y;
    foreach (double f, getThirdOctaveFrequencies())
        if (f >= 100 && f <= qMin(16000.0, 0.9 * _sampleRate / 2))
            centers << f;
    QVector<double> levels = bandLevels(_power, _fftSize, centers, 1.0 / 3);
    for (int i = 0; i < centers.size(); i++)
    {
        if (levels[i] > -150)
        {
            x << std::log2(centers[i]);
            y << levels[i];
        }
    }
    double slope, intercept;
    if (x.size() >= 4 && SampleUtils::linearRegression(x, y, QVector<double>(), slope, intercept))
        _tilt = slope;
}

double SoundAnalyzer::getSpectralCentroid()
{
    computeSpectrum();
    return _centroid;
}

QVector<double> SoundAnalyzer::getThirdOctaveLevels()
{
    computeSpectrum();
    return bandLevels(_power, _fftSize, getThirdOctaveFrequencies(), 1.0 / 3);
}

QJsonObject SoundAnalyzer::getSpectrum()
{
    computeSpectrum();
    computePitch();
    QJsonObject result;
    if (_power.isEmpty())
    {
        result["note"] = "sound too short for a spectral analysis";
        return result;
    }
    result["centroid_hz"] = Utils::roundSignificant(_centroid, 4);
    double f0 = getFundamental();
    if (f0 > 0)
        result["centroid_in_harmonics"] = roundDecimals(_centroid / f0, 2);
    result["spread_hz"] = Utils::roundSignificant(_spread, 4);
    result["skewness"] = roundDecimals(_skewness, 2);
    result["kurtosis"] = roundDecimals(_kurtosis, 2);
    result["rolloff_85_hz"] = Utils::roundSignificant(_rolloff85, 4);
    result["rolloff_95_hz"] = Utils::roundSignificant(_rolloff95, 4);
    result["flatness_db"] = jsonNumber(_flatness, 1);
    if (std::isfinite(_flatnessHigh))
        result["flatness_above_4khz_db"] = jsonNumber(_flatnessHigh, 1);
    result["tilt_db_per_octave"] = jsonNumber(_tilt, 2);

    // Octave bands relative to the strongest band
    QVector<double> centers = { 31.5, 63, 125, 250, 500, 1000, 2000, 4000, 8000, 16000 };
    QVector<double> levels = bandLevels(_power, _fftSize, centers, 1.0);
    double maxLevel = -200;
    foreach (double level, levels)
        maxLevel = qMax(maxLevel, level);
    QJsonObject bands;
    for (int i = 0; i < centers.size(); i++)
        if (levels[i] > -150)
            bands[QString::number(centers[i])] = roundDecimals(levels[i] - maxLevel, 1);
    result["octave_bands_db"] = bands;
    result["note"] = "steady part; flatness: 0 dB = noise, below -30 dB = tonal; tilt: slope of the spectrum "
                     "(more negative = darker); octave bands relative to the strongest one";
    return result;
}

QJsonObject SoundAnalyzer::getSpectrumCurves()
{
    computeSpectrum();
    computeHarmonics();
    QJsonObject result;
    QVector<double> centers = getThirdOctaveFrequencies();
    QVector<double> levels = getThirdOctaveLevels();
    double maxLevel = -200;
    foreach (double level, levels)
        maxLevel = qMax(maxLevel, level);
    QJsonArray thirds;
    for (int i = 0; i < centers.size(); i++)
        if (levels[i] > -150 && levels[i] - maxLevel > -100)
            thirds.append(QJsonArray({ Utils::roundSignificant(centers[i], 3), roundDecimals(levels[i] - maxLevel, 1) }));
    result["third_octave_db"] = thirds;

    // Spectral envelope: the harmonic peaks (frequency, dB relative to the strongest one)
    QJsonArray envelope;
    double maxAmplitude = 0;
    foreach (Harmonic harmonic, _harmonics)
        if (harmonic.found)
            maxAmplitude = qMax(maxAmplitude, harmonic.amplitude);
    foreach (Harmonic harmonic, _harmonics)
        if (harmonic.found && maxAmplitude > 0)
            envelope.append(QJsonArray({ Utils::roundSignificant(harmonic.frequency, 5), roundDecimals(SampleUtils::toDb(harmonic.amplitude / maxAmplitude), 1) }));
    if (!envelope.isEmpty())
        result["harmonic_envelope_db"] = envelope;
    result["note"] = "[frequency in Hz, level in dB relative to the maximum]";
    return result;
}

QList<SoundAnalyzer::Partial> SoundAnalyzer::getPartials(int maxCount, double &startSeconds, double &endSeconds)
{
    computeRegions();
    QList<Partial> result;
    double sr = _sampleRate;
    int start = _attackEnd;
    if (_length - start < static_cast<int>(0.1 * sr))
        start = _onset;
    int length = qMin(_length - start, qMin(1 << 20, static_cast<int>(10 * sr)));
    startSeconds = start / sr;
    endSeconds = (start + qMax(0, length)) / sr;
    if (length < 1024 || maxCount <= 0)
        return result;

    // Blackman-Harris window (sidelobes at -92 dB) on the whole part, zero-padded 4 times for the interpolation, mean
    // power of the channels
    int fftSize = SampleUtils::nextPowerOfTwo(4 * length);
    int bins = fftSize / 2;
    int channelCount = _data2.isEmpty() ? 1 : 2;
    QVector<double> blackmanHarris(length);
    double windowSum = 0;
    for (int i = 0; i < length; i++)
    {
        double x = 2 * PI * i / length;
        blackmanHarris[i] = 0.35875 - 0.48829 * std::cos(x) + 0.14128 * std::cos(2 * x) - 0.01168 * std::cos(3 * x);
        windowSum += blackmanHarris[i];
    }
    QVector<double> power(bins + 1, 0.0);
    {
        std::vector<Complex> buffer(fftSize);
        for (int channel = 0; channel < channelCount; channel++)
        {
            const QVector<double> &data = channel == 0 ? _data : _data2;
            std::fill(buffer.begin(), buffer.end(), Complex(0));
            for (int i = 0; i < length; i++)
                buffer[i] = data[start + i] * blackmanHarris[i];
            getFourierTransform(fftSize).transform(buffer, false);
            for (int k = 0; k <= bins; k++)
                power[k] += std::norm(buffer[k]) / channelCount;
        }
    }
    QVector<double> db(bins + 1);
    for (int k = 0; k <= bins; k++)
        db[k] = 10 * std::log10(power[k] + 1e-300);
    power.clear();

    // Local maxima over the main lobe of the window (4 bins of the window length on each side)
    double binWidth = sr / fftSize;
    int halfLobe = qMax(2, static_cast<int>(std::ceil(4.0 * fftSize / length)));
    int kMin = qMax(halfLobe + 1, static_cast<int>(std::ceil(10.0 / binWidth)));
    int kMax = qMin(bins - halfLobe - 1, static_cast<int>(0.95 * sr / 2 / binWidth));
    QList<QPair<double, int> > candidates; // Level, bin
    double maxDb = -1e300;
    for (int k = kMin; k <= kMax; k++)
    {
        if (db[k] < db[k - 1] || db[k] < db[k + 1])
            continue;
        bool isMax = true;
        for (int j = k - halfLobe; j <= k + halfLobe && isMax; j++)
            if (db[j] > db[k])
                isMax = false;
        if (isMax)
        {
            candidates << QPair<double, int>(db[k], k);
            maxDb = qMax(maxDb, db[k]);
        }
    }
    std::sort(candidates.begin(), candidates.end(), [](const QPair<double, int> &a, const QPair<double, int> &b) {
        return a.first > b.first;
    });

    // Strongest peaks standing out of their neighborhood (median of 10 main lobes on each side, the main lobe excluded)
    QList<int> selected;
    for (int i = 0; i < candidates.size() && selected.size() < maxCount; i++)
    {
        if (candidates[i].first < maxDb - 90)
            break;
        int k = candidates[i].second;
        QVector<double> around;
        for (int j = qMax(1, k - 10 * halfLobe); j <= qMin(bins, k + 10 * halfLobe); j++)
            if (std::abs(j - k) > 2 * halfLobe)
                around << db[j];
        if (around.isEmpty() || candidates[i].first - SampleUtils::median(around) >= 10)
            selected << k;
    }
    std::sort(selected.begin(), selected.end());

    foreach (int k, selected)
    {
        Partial partial;
        double peakDb;
        double p = parabolicPeak(db[k - 1], db[k], db[k + 1], peakDb);
        partial.frequency = (k + p) * binWidth;
        partial.levelDb = 20 * std::log10(2 * std::sqrt(std::pow(10.0, peakDb / 10)) / windowSum + 1e-300);
        partial.bandHz = 0;
        partial.variationDb = std::nan("");
        partial.beatRate = std::nan("");
        partial.beatDepthDb = 0;
        result << partial;
    }

    // Level of each partial over time: complex demodulation and Hann window whose bandwidth excludes the closest partials
    for (int i = 0; i < result.size(); i++)
    {
        double nearest = 1e9;
        for (int j = 0; j < result.size(); j++)
            if (j != i)
                nearest = qMin(nearest, std::fabs(result[i].frequency - result[j].frequency));
        double band = qBound(1.0, nearest / 2, 20.0);
        result[i].bandHz = band;
        int window = static_cast<int>(sr / band);
        int hop = qMax(1, window / 4);
        if (window < 16 || window > length / 2)
            continue;

        QVector<double> hann(window);
        double hannSum = 0;
        for (int j = 0; j < window; j++)
        {
            hann[j] = 0.5 - 0.5 * std::cos(2 * PI * j / window);
            hannSum += hann[j];
        }
        // Amplitude of the partial in each channel, then the quadratic mean of the channels
        QVector<double> squares;
        for (int channel = 0; channel < channelCount; channel++)
        {
            const QVector<double> &data = channel == 0 ? _data : _data2;
            std::vector<Complex> rotated(length);
            Complex step = std::polar(1.0, -2 * PI * result[i].frequency / sr);
            Complex z(1, 0);
            for (int n = 0; n < length; n++)
            {
                rotated[n] = data[start + n] * z;
                z *= step;
                if ((n & 4095) == 4095)
                    z /= std::abs(z);
            }
            int frame = 0;
            for (int m = 0; m + window <= length; m += hop, frame++)
            {
                Complex sum(0);
                for (int j = 0; j < window; j++)
                    sum += rotated[m + j] * hann[j];
                double amplitude = 2 * std::abs(sum) / hannSum;
                if (channel == 0)
                    squares << amplitude * amplitude / channelCount;
                else
                    squares[frame] += amplitude * amplitude / channelCount;
            }
        }
        QVector<double> times, amplitudes, levels;
        double meanAmplitude = 0;
        for (int frame = 0; frame < squares.size(); frame++)
        {
            double amplitude = std::sqrt(squares[frame]);
            times << (frame * hop + window / 2) / sr;
            amplitudes << amplitude;
            levels << 20 * std::log10(amplitude + 1e-12);
            meanAmplitude += amplitude;
        }
        if (levels.size() < 3 || meanAmplitude <= 0)
            continue;
        meanAmplitude /= amplitudes.size();
        result[i].variationDb = SampleUtils::percentile(levels, 0.95) - SampleUtils::percentile(levels, 0.05);

        // Periodic modulation of the amplitude (linear: the deep minima of a beat would give many harmonics in dB),
        // depth in dB between the maximum and the minimum of the modulation
        result[i].beatRate = 0;
        for (int j = 0; j < amplitudes.size(); j++)
            amplitudes[j] /= meanAmplitude;
        double minRate = qMax(0.1, 3.0 / qMax(1e-6, times.last() - times.first())); // 3 periods at least
        double maxRate = qMin(15.0, band / 2);
        double rate, amplitude;
        if (minRate < maxRate && findModulation(times, amplitudes, minRate, maxRate, rate, amplitude))
        {
            // Finer rate around the one found on a grid of 0.05 Hz, the search going below the lowest rate: a maximum
            // there is a slower variation than the analyzed duration can measure, not a modulation at the rate found
            QVector<double> detrended = amplitudes;
            double slope, intercept;
            if (SampleUtils::linearRegression(times, detrended, QVector<double>(), slope, intercept))
                for (int j = 0; j < detrended.size(); j++)
                    detrended[j] -= slope * times[j] + intercept;
            double resolution = 1.0 / (times.last() - times.first());
            double bestRate = rate;
            for (double f = qMax(0.02, rate - 0.05 - resolution); f <= qMin(maxRate, rate + 0.05); f += 0.001)
            {
                double a = componentAmplitude(times, detrended, f);
                if (a > amplitude)
                {
                    amplitude = a;
                    bestRate = f;
                }
            }
            // The modulation must also stand out clearly from the random fluctuations of the level: 4 times the median
            // amplitude of the rates searched
            QVector<double> others;
            for (double f = minRate; f <= maxRate; f += 0.5 * resolution)
                others << componentAmplitude(times, detrended, f);
            if (bestRate >= minRate && !others.isEmpty() && amplitude >= 4 * SampleUtils::median(others))
            {
                result[i].beatRate = bestRate;
                result[i].beatDepthDb = qMin(60.0, 20 * std::log10((1 + amplitude) / qMax(1e-3, 1 - amplitude)));
            }
        }
    }
    return result;
}

QVector<double> SoundAnalyzer::getPitchTrack(double hopSeconds, double &firstTime)
{
    // Windows of 2 periods of the lowest frequency searched (20 ms at least)
    QVector<double> track;
    double fMin, fMax;
    getPitchRange(fMin, fMax);
    int tauMax = static_cast<int>(std::ceil(_sampleRate / fMin));
    int window = qMax(static_cast<int>(0.02 * _sampleRate), 2 * tauMax);
    int hop = qMax(1, static_cast<int>(hopSeconds * _sampleRate));
    firstTime = static_cast<double>(window / 2) / _sampleRate;
    for (int position = 0; position + window + tauMax + 1 <= _length; position += hop)
    {
        double confidence;
        double f = yin(position, window, fMin, fMax, confidence);
        track << ((f > 0 && confidence >= 0.6) ? f : 0);
    }
    return track;
}

//////////////////////////
/// HARMONICS         ///
//////////////////////////

void SoundAnalyzer::computeHarmonics()
{
    if (_harmonicsComputed)
        return;
    _harmonicsComputed = true;
    computePitch();
    _harmonics.clear();
    _inharmonicity = 0;
    _refinedF0 = 0;
    _hnr = std::nan("");
    if (_f0 <= 0)
        return;

    // Window long enough to separate the harmonics (8 periods for Blackman-Harris)
    int start = _steadyStart, end = _steadyEnd;
    int frameLength = qBound(2048, SampleUtils::nextPowerOfTwo(static_cast<int>(8 * _sampleRate / _f0)), 65536);
    if (end - start < frameLength)
    {
        start = _onset;
        end = _length;
    }
    if (end - start < frameLength)
        frameLength = previousPowerOfTwo(end - start);
    if (frameLength < 512)
        return;
    _harmonicFftSize = 2 * frameLength;
    double windowSquareSum;
    _harmonicPower = averagePowerSpectrum(start, end, frameLength, _harmonicFftSize, true, _harmonicWindowSum, windowSquareSum);

    double binWidth = static_cast<double>(_sampleRate) / _harmonicFftSize;
    int lastBin = static_cast<int>(_harmonicPower.size()) - 2;
    auto db = [&](int k) -> double { return 10 * std::log10(_harmonicPower[qBound(0, k, lastBin + 1)] + 1e-30); };

    // Peak around a predicted frequency
    auto findPeak = [&](int rank, double predicted, double halfWidth, double f0) -> Harmonic {
        Harmonic harmonic;
        harmonic.rank = rank;
        harmonic.frequency = predicted;
        harmonic.amplitude = 0;
        harmonic.found = false;
        int k1 = qMax(2, static_cast<int>(std::floor((predicted - halfWidth) / binWidth)));
        int k2 = qMin(lastBin, static_cast<int>(std::ceil((predicted + halfWidth) / binWidth)));
        if (k2 <= k1)
            return harmonic;
        int best = k1;
        for (int k = k1; k <= k2; k++)
            if (_harmonicPower[k] > _harmonicPower[best])
                best = k;
        if (best == k1 || best == k2)
            return harmonic; // Not a local maximum

        double peakDb;
        double p = parabolicPeak(db(best - 1), db(best), db(best + 1), peakDb);
        harmonic.frequency = (best + p) * binWidth;
        harmonic.amplitude = 2 * std::sqrt(std::pow(10.0, peakDb / 10)) / _harmonicWindowSum;

        // Prominence over the minimum around (half the distance to the next harmonics)
        int m1 = qMax(1, static_cast<int>((harmonic.frequency - f0 / 2) / binWidth));
        int m2 = qMin(lastBin, static_cast<int>((harmonic.frequency + f0 / 2) / binWidth));
        double minDb = peakDb;
        for (int k = m1; k <= m2; k++)
            minDb = qMin(minDb, db(k));
        harmonic.found = (peakDb - minDb >= 6);
        return harmonic;
    };

    // First pass on the first harmonics, then fit of the inharmonicity
    QVector<double> x, y, w;
    double maxAmplitude = 0;
    QList<Harmonic> first;
    int firstCount = qMin(10, static_cast<int>(0.45 * _sampleRate / _f0));
    for (int h = 1; h <= firstCount; h++)
    {
        Harmonic harmonic = findPeak(h, h * _f0, _f0 / 3, _f0);
        first << harmonic;
        if (harmonic.found)
            maxAmplitude = qMax(maxAmplitude, harmonic.amplitude);
    }
    double weightedSum = 0, weights = 0;
    foreach (Harmonic harmonic, first)
    {
        if (harmonic.found && harmonic.amplitude > maxAmplitude * 0.003) // -50 dB
        {
            double ratio = harmonic.frequency / harmonic.rank;
            x << harmonic.rank * harmonic.rank;
            y << ratio * ratio;
            w << harmonic.amplitude * harmonic.amplitude;
            weightedSum += ratio * w.last();
            weights += w.last();
        }
    }
    double f0 = weights > 0 ? weightedSum / weights : _f0;
    double slope, intercept;
    if (x.size() >= 4 && SampleUtils::linearRegression(x, y, w, slope, intercept) && intercept > 0)
    {
        // Residuals with and without stretching of the partials
        double meanSquare = 0;
        for (int i = 0; i < y.size(); i++)
            meanSquare += w[i] * y[i] / weights;
        double rssHarmonic = 0, rssStretched = 0;
        for (int i = 0; i < y.size(); i++)
        {
            rssHarmonic += w[i] * (y[i] - meanSquare) * (y[i] - meanSquare);
            rssStretched += w[i] * (y[i] - slope * x[i] - intercept) * (y[i] - slope * x[i] - intercept);
        }
        double b = slope / intercept;
        if (b > 1e-6 && b < 0.05 && rssStretched < 0.25 * rssHarmonic)
        {
            _inharmonicity = b;
            f0 = std::sqrt(intercept);
        }
    }
    if (std::fabs(1200 * std::log2(f0 / _f0)) > 100)
        f0 = _f0; // Fit not reliable
    _refinedF0 = f0;

    // Second pass on all harmonics up to 16 kHz
    int count = qMin(60, static_cast<int>(qMin(16000.0, 0.95 * _sampleRate / 2) / f0));
    for (int h = 1; h <= count; h++)
    {
        double predicted = h * f0 * std::sqrt(1 + _inharmonicity * h * h);
        if (predicted >= 0.95 * _sampleRate / 2)
            break;
        double halfWidth = qMin(f0 / 3 + 0.01 * predicted, f0 / 2.2);
        _harmonics << findPeak(h, predicted, halfWidth, f0);
    }
    double strongest = 0;
    foreach (Harmonic harmonic, _harmonics)
        if (harmonic.found)
            strongest = qMax(strongest, harmonic.amplitude);
    for (int i = 0; i < _harmonics.size(); i++)
        if (_harmonics[i].amplitude < strongest * 3.16e-5) // -90 dB
            _harmonics[i].found = false;

    // Harmonic to noise ratio: energy around the harmonics (main lobes) against the rest
    double total = 0, harmonicEnergy = 0;
    int kLow = qMax(1, static_cast<int>(0.5 * f0 / binWidth));
    int kHigh = qMin(lastBin, static_cast<int>(qMin(16000.0, 0.95 * _sampleRate / 2) / binWidth));
    QVector<bool> inLobe(_harmonicPower.size(), false);
    foreach (Harmonic harmonic, _harmonics)
    {
        if (!harmonic.found)
            continue;
        int center = qRound(harmonic.frequency / binWidth);
        for (int k = qMax(0, center - 8); k <= qMin(lastBin, center + 8); k++)
            inLobe[k] = true;
    }
    for (int k = kLow; k <= kHigh; k++)
    {
        total += _harmonicPower[k];
        if (inLobe[k])
            harmonicEnergy += _harmonicPower[k];
    }
    if (total > 0 && harmonicEnergy > 0)
        _hnr = 10 * std::log10(harmonicEnergy / qMax(total - harmonicEnergy, total * 1e-9));
}

double SoundAnalyzer::getHarmonicCentroid()
{
    computeHarmonics();
    double sum = 0, weightedSum = 0;
    foreach (Harmonic harmonic, _harmonics)
    {
        if (harmonic.found)
        {
            sum += harmonic.amplitude * harmonic.amplitude;
            weightedSum += harmonic.rank * harmonic.amplitude * harmonic.amplitude;
        }
    }
    return sum > 0 ? weightedSum / sum : std::nan("");
}

double SoundAnalyzer::getHarmonicTilt()
{
    computeHarmonics();
    QVector<double> x, y;
    foreach (Harmonic harmonic, _harmonics)
    {
        if (harmonic.found)
        {
            x << std::log2(harmonic.rank);
            y << SampleUtils::toDb(harmonic.amplitude);
        }
    }
    double slope, intercept;
    if (x.size() >= 3 && SampleUtils::linearRegression(x, y, QVector<double>(), slope, intercept))
        return slope;
    return std::nan("");
}

double SoundAnalyzer::getHnr()
{
    computeHarmonics();
    return _hnr;
}

QJsonObject SoundAnalyzer::getHarmonics(int maxListed)
{
    computeHarmonics();
    QJsonObject result;
    if (_harmonics.isEmpty())
    {
        result["found"] = false;
        return result;
    }

    double maxAmplitude = 0;
    foreach (Harmonic harmonic, _harmonics)
        if (harmonic.found)
            maxAmplitude = qMax(maxAmplitude, harmonic.amplitude);

    QJsonArray levels;
    int audible = 0;
    double odd = 0, even = 0, oddWithoutFirst = 0;
    double sumAmplitudes = 0, t1 = 0, t2 = 0;
    for (int i = 0; i < _harmonics.size(); i++)
    {
        const Harmonic &harmonic = _harmonics[i];
        double level = harmonic.found ? SampleUtils::toDb(harmonic.amplitude / maxAmplitude) : std::nan("");
        if (i < maxListed)
            levels.append(harmonic.found ? QJsonValue(roundDecimals(level, 1)) : QJsonValue());
        if (!harmonic.found)
            continue;
        if (level > -60)
            audible++;
        double energy = harmonic.amplitude * harmonic.amplitude;
        if (harmonic.rank % 2 == 1)
        {
            odd += energy;
            if (harmonic.rank > 1)
                oddWithoutFirst += energy;
        }
        else
            even += energy;
        sumAmplitudes += harmonic.amplitude;
        if (harmonic.rank == 1)
            t1 += harmonic.amplitude;
        else if (harmonic.rank <= 4)
            t2 += harmonic.amplitude;
    }

    result["found"] = true;
    result["levels_db"] = levels;
    result["levels_note"] = "harmonics 1, 2, 3... in dB relative to the strongest one (null: not detected)";
    result["count_above_60db"] = audible;
    result["fundamental_level_db"] = _harmonics[0].found ? QJsonValue(roundDecimals(SampleUtils::toDb(_harmonics[0].amplitude / maxAmplitude), 1)) : QJsonValue();
    result["centroid_in_harmonics"] = jsonNumber(getHarmonicCentroid(), 2);
    result["tilt_db_per_octave"] = jsonNumber(getHarmonicTilt(), 2);
    if (even > 0)
    {
        result["odd_even_ratio_db"] = roundDecimals(10 * std::log10(odd / even), 1);
        if (oddWithoutFirst > 0)
            result["odd_even_ratio_without_fundamental_db"] = roundDecimals(10 * std::log10(oddWithoutFirst / even), 1);
    }
    if (sumAmplitudes > 0)
        result["tristimulus"] = QJsonArray({ roundDecimals(t1 / sumAmplitudes, 3), roundDecimals(t2 / sumAmplitudes, 3),
                                            roundDecimals(1 - (t1 + t2) / sumAmplitudes, 3) });
    result["inharmonicity_b"] = _inharmonicity > 0 ? QJsonValue(Utils::roundSignificant(_inharmonicity, 3)) : QJsonValue(0);
    result["harmonic_to_noise_db"] = jsonNumber(_hnr, 1);

    // Irregularity (Krimphoff): deviation from the local mean of the levels
    double irregularity = 0;
    int counted = 0;
    for (int i = 1; i + 1 < _harmonics.size() && i < 20; i++)
    {
        if (_harmonics[i - 1].found && _harmonics[i].found && _harmonics[i + 1].found)
        {
            double a = SampleUtils::toDb(_harmonics[i - 1].amplitude), b = SampleUtils::toDb(_harmonics[i].amplitude), c = SampleUtils::toDb(_harmonics[i + 1].amplitude);
            irregularity += std::fabs(b - (a + b + c) / 3);
            counted++;
        }
    }
    if (counted > 0)
        result["irregularity_db"] = roundDecimals(irregularity / counted, 2);
    result["note"] = "centroid in harmonics: brightness independent of the pitch; tilt: slope of the harmonic levels "
                     "(dB per octave of rank); tristimulus: weights of the fundamental, harmonics 2-4, harmonics 5+; "
                     "inharmonicity_b: stretching of the partials (piano), 0 for winds and bowed strings";
    return result;
}

//////////////////////////
/// ENVELOPE          ///
//////////////////////////

double SoundAnalyzer::getAttackTime()
{
    computeRegions();
    return static_cast<double>(_attackEnd - _attackStart) / _sampleRate;
}

QJsonObject SoundAnalyzer::getEnvelope()
{
    computeRegions();
    double sr = _sampleRate;
    QJsonObject result;

    double attackTime = qMax(1.0 / sr, (_attackEnd - _attackStart) / sr);
    result["attack_time_ms"] = roundDecimals(1000 * attackTime, 1);
    result["log_attack_time"] = roundDecimals(std::log10(attackTime), 2);

    // Rise time from 10% to 90% of the level reached at the end of the attack (5 ms envelope)
    int window = qMax(1, static_cast<int>(0.005 * sr));
    int hop = qMax(1, static_cast<int>(0.001 * sr));
    QVector<double> e = rmsEnvelope(window, hop, _onset, qMin(_length, _attackEnd + window));
    double reference = rms(_attackEnd - window / 2, _attackEnd + window / 2);
    int i10 = 0, i90 = 0;
    while (i10 < e.size() - 1 && e[i10] < 0.1 * reference)
        i10++;
    while (i90 < e.size() - 1 && e[i90] < 0.9 * reference)
        i90++;
    result["rise_10_90_ms"] = roundDecimals(1000.0 * (i90 - i10) * hop / sr, 1);

    // Envelope of 50 ms every 10 ms in dB
    int window50 = qMax(1, static_cast<int>(0.05 * sr));
    int hop10 = qMax(1, static_cast<int>(0.01 * sr));
    QVector<double> env = rmsEnvelope(window50, hop10);
    double maxEnv = 0;
    int maxIndex = 0;
    for (int i = 0; i < env.size(); i++)
    {
        if (env[i] > maxEnv)
        {
            maxEnv = env[i];
            maxIndex = i;
        }
    }
    double maxDb = SampleUtils::toDb(maxEnv);

    // Temporal centroid and effective duration
    double sumE = 0, sumTE = 0;
    int effective = 0;
    for (int i = 0; i < env.size(); i++)
    {
        if (env[i] > 0.15 * maxEnv)
        {
            double t = (i * hop10 + window50 / 2) / sr;
            sumE += env[i];
            sumTE += t * env[i];
        }
        if (env[i] > 0.4 * maxEnv)
            effective++;
    }
    if (sumE > 0)
        result["temporal_centroid_s"] = roundDecimals(sumTE / sumE, 3);
    result["effective_duration_s"] = roundDecimals(effective * hop10 / sr, 3);

    // Decay after the attack, until -40 dB or the noise floor + 10 dB
    double stopDb = qMax(maxDb - 40, _noiseFloorDb + 10);
    QVector<double> t, levels;
    int startFrame = qMax(maxIndex, static_cast<int>(_attackEnd / hop10));
    for (int i = startFrame; i < env.size(); i++)
    {
        double level = SampleUtils::toDb(env[i]);
        if (level < stopDb)
            break;
        if (_looped && i * hop10 > _loopStart)
            break;
        t << i * hop10 / sr;
        levels << level;
    }
    double slope = 0, intercept;
    bool decayFound = t.size() >= 5 && SampleUtils::linearRegression(t, levels, QVector<double>(), slope, intercept);
    if (decayFound)
    {
        result["decay_rate_db_per_s"] = roundDecimals(slope, 2);
        if (slope < -1)
        {
            result["t60_s"] = roundDecimals(-60 / slope, 3);
            result["equivalent_vol_env_decay_s"] = roundDecimals(-100 / slope, 3);
        }
    }

    // Kind of sound
    QString kind;
    if (_looped)
        kind = "sustained (looped)";
    else if (attackTime < 0.01 && effective * hop10 / sr < 0.3)
        kind = "percussive";
    else if (decayFound && slope < -3)
        kind = "decaying";
    else
        kind = "sustained";
    result["kind"] = kind;

    // Tremolo in the steady part
    int hop5 = qMax(1, static_cast<int>(0.005 * sr));
    int window10 = qMax(1, static_cast<int>(0.01 * sr));
    QVector<double> steadyEnv = rmsEnvelope(window10, hop5, _steadyStart, _steadyEnd);
    QVector<double> times, steadyDb;
    for (int i = 0; i < steadyEnv.size(); i++)
    {
        if (steadyEnv[i] > 0)
        {
            times << (i * hop5) / sr;
            steadyDb << SampleUtils::toDb(steadyEnv[i]);
        }
    }
    double rate, amplitude;
    if (findModulation(times, steadyDb, 1, 12, rate, amplitude) && 2 * amplitude >= 0.5)
    {
        result["tremolo_rate_hz"] = roundDecimals(rate, 2);
        result["tremolo_depth_db"] = roundDecimals(2 * amplitude, 2);
    }

    // End of the sample
    int tail = qMax(1, static_cast<int>(0.01 * sr));
    double tailDb = SampleUtils::toDb(rms(_length - tail, _length)) - SampleUtils::toDb(_peak);
    result["end_level_db"] = roundDecimals(qMax(-200.0, tailDb), 1);

    // Summary of the envelope: about 32 points (time in s, dB relative to the maximum)
    QJsonArray curve;
    int step = qMax(1, static_cast<int>(env.size()) / 32);
    for (int i = 0; i < env.size(); i += step)
        curve.append(QJsonArray({ roundDecimals((i * hop10 + window50 / 2) / sr, 3), roundDecimals(qMax(-120.0, SampleUtils::toDb(env[i]) - maxDb), 1) }));
    result["curve"] = curve;
    result["note"] = "curve: [time in s, RMS level in dB relative to the maximum], 50 ms windows; "
                     "equivalent_vol_env_decay_s: decay setting of the soundfont giving the same decrease (100 dB in this time)";
    return result;
}

//////////////////////////
/// LOOP              ///
//////////////////////////

double SoundAnalyzer::lpcClickScore() const
{
    // Linear prediction of order 16 learnt before the end of the loop, residual across the junction
    const int order = 16;
    int window = qMin(static_cast<int>(0.02 * _sampleRate), _loopEnd - _loopStart);
    if (window < 4 * order || _loopEnd - window < 0)
        return std::nan("");

    QVector<double> segment(window);
    for (int i = 0; i < window; i++)
        segment[i] = _data[_loopEnd - window + i] * (0.5 - 0.5 * std::cos(2 * PI * i / (window - 1)));
    double r[order + 1];
    for (int lag = 0; lag <= order; lag++)
    {
        r[lag] = 0;
        for (int i = lag; i < window; i++)
            r[lag] += segment[i] * segment[i - lag];
    }
    if (r[0] <= 0)
        return std::nan("");
    r[0] *= 1.0 + 1e-9;

    // Levinson-Durbin
    double a[order + 1] = { 1 };
    double errorPower = r[0];
    for (int i = 1; i <= order; i++)
    {
        double acc = r[i];
        for (int j = 1; j < i; j++)
            acc += a[j] * r[i - j];
        double k = -acc / errorPower;
        double previous[order + 1];
        std::copy(a, a + order + 1, previous);
        for (int j = 1; j < i; j++)
            a[j] = previous[j] + k * previous[i - j];
        a[i] = k;
        errorPower *= (1 - k * k);
        if (errorPower <= 0)
            return std::nan("");
    }

    // Signal heard: end of the loop then its beginning
    int after = qMin(window, _loopEnd - _loopStart);
    QVector<double> y;
    for (int i = _loopEnd - window; i < _loopEnd; i++)
        y << _data[i];
    for (int i = _loopStart; i < _loopStart + after; i++)
        y << _data[i];
    QVector<double> residual, before;
    double seamMax = 0;
    for (int n = order; n < y.size(); n++)
    {
        double e = y[n];
        for (int j = 1; j <= order; j++)
            e += a[j] * y[n - j];
        if (n < window - 3)
            before << std::fabs(e);
        else if (n <= window + 3)
            seamMax = qMax(seamMax, std::fabs(e));
    }
    double signalRms = std::sqrt(r[0] / window);
    double sigma = qMax(1.4826 * SampleUtils::median(before), 3.16e-3 * signalRms); // Residuals below -50 dB are inaudible
    if (sigma <= 0)
        return std::nan("");
    return 20 * std::log10(qMax(seamMax, 1e-12) / sigma);
}

void SoundAnalyzer::computeLoop()
{
    if (_loopComputed)
        return;
    _loopComputed = true;
    computeRegions();
    computeHarmonics();
    QJsonObject result;
    result["looped"] = _looped;
    if (!_looped)
    {
        _loopResult = result;
        return;
    }

    double sr = _sampleRate;
    int length = _loopEnd - _loopStart;
    double f0 = getFundamental();
    result["start"] = _loopStart;
    result["end"] = _loopEnd;
    result["duration_ms"] = roundDecimals(1000 * length / sr, 2);
    result["starts_after_attack_end_ms"] = roundDecimals(1000 * (_loopStart - _attackEnd) / sr, 1);

    // Number of periods, meaningless for several fundamentals (celeste) and imprecise for long loops (error of the
    // fundamental)
    if (f0 > 0)
    {
        double periods = length * f0 / sr;
        double mismatch = periods - std::round(periods);
        result["periods"] = roundDecimals(periods, 3);
        result["period_mismatch"] = roundDecimals(std::fabs(mismatch), 3);

        // Phase commensurability of the harmonics: 0 = all harmonics continue in phase
        double sum = 0, weighted = 0;
        foreach (Harmonic harmonic, _harmonics)
        {
            if (harmonic.found)
            {
                double energy = harmonic.amplitude * harmonic.amplitude;
                sum += energy;
                weighted += energy * (1 - std::cos(2 * PI * harmonic.frequency * length / sr));
            }
        }
        if (sum > 0)
            result["harmonic_phase_mismatch"] = roundDecimals(weighted / (2 * sum), 3);
    }

    // Seam correlation: what precedes the loop start compared to what precedes the loop end
    int n = f0 > 0 ? static_cast<int>(2 * sr / f0) : 0;
    n = qBound(static_cast<int>(0.005 * sr), n, qMax(8, qMin(static_cast<int>(0.05 * sr), length / 2)));
    double correlation = std::nan(""), errorDb = std::nan("");
    auto compareWindows = [&](int a, int b, int count) {
        double sab = 0, saa = 0, sbb = 0, sdd = 0;
        for (int i = 0; i < count; i++)
        {
            double va = _data[a + i], vb = _data[b + i];
            sab += va * vb;
            saa += va * va;
            sbb += vb * vb;
            sdd += (va - vb) * (va - vb);
        }
        if (saa > 0 && sbb > 0)
        {
            correlation = sab / std::sqrt(saa * sbb);
            errorDb = qMax(-120.0, 10 * std::log10(qMax(sdd, 1e-30) / sbb));
        }
    };
    if (_loopStart >= n)
        compareWindows(_loopEnd - n, _loopStart - n, n);
    else if (_loopEnd + n <= _length)
        compareWindows(_loopStart, _loopEnd, n);
    else if (_loopStart >= 8)
        compareWindows(_loopEnd - _loopStart, 0, _loopStart);
    _loopCorrelation = correlation;
    result["seam_correlation"] = jsonNumber(correlation, 4);
    result["seam_error_db"] = jsonNumber(errorDb, 1);

    // Junction: jump compared to the typical second difference
    if (length > 8 && _loopEnd >= 3)
    {
        double predicted = 2 * _data[_loopEnd - 1] - _data[_loopEnd - 2];
        double jump = _data[_loopStart] - predicted;
        double sum = 0;
        int count = 0;
        for (int i = qMax(_loopStart + 2, _loopEnd - static_cast<int>(0.05 * sr)); i < _loopEnd; i++)
        {
            double d2 = _data[i] - 2 * _data[i - 1] + _data[i - 2];
            sum += d2 * d2;
            count++;
        }
        double reference = count > 0 ? std::sqrt(sum / count) : 0;
        if (reference > 0)
            result["junction_jump_db"] = roundDecimals(SampleUtils::toDb(std::fabs(jump) / reference, -60), 1);
    }
    result["click_score_db"] = jsonNumber(lpcClickScore(), 1);

    // Level and spectrum: what precedes the loop end compared with what precedes the loop start, that is with what the
    // original sound does at the same point (a seamless loop gives the same signal whatever the slow modulations of the
    // sound: beats of a celeste, tremulant...), also further before the loop points for covering a crossfade (until half
    // of the loop, both windows being after the attack). Without signal between the attack and the loop start: the end
    // of the loop is compared with its beginning and judged against the natural variations inside the loop
    auto referenceDistances = [&](int window) -> QList<int> {
        QList<int> distances;
        int maxDistance = qMin(_loopStart - qMax(_attackEnd, 0) - window, length / 2 - window);
        if (maxDistance >= 0)
            for (int k = 0; k < 8; k++)
                distances << static_cast<int>(static_cast<double>(k) / 7 * maxDistance);
        return distances;
    };
    int w = qMax(16, qMin(static_cast<int>(0.03 * sr), length / 4));
    QVector<double> naturalLevel;
    for (int k = 1; k <= 8; k++)
    {
        int p = _loopStart + w + static_cast<int>(static_cast<double>(k) / 9 * (length - 2 * w));
        if (p - w >= _loopStart && p + w <= _loopEnd)
            naturalLevel << std::fabs(SampleUtils::toDb(rms(p, p + w)) - SampleUtils::toDb(rms(p - w, p)));
    }
    double naturalLevelVariation = naturalLevel.isEmpty() ? 0 : SampleUtils::median(naturalLevel);
    QList<int> levelDistances = referenceDistances(w);
    double levelJump = 0, maxLevelDifference = 0;
    if (levelDistances.isEmpty())
        levelJump = maxLevelDifference = SampleUtils::toDb(rms(_loopStart, _loopStart + w)) - SampleUtils::toDb(rms(_loopEnd - w, _loopEnd));
    else
    {
        foreach (int d, levelDistances)
        {
            double difference = SampleUtils::toDb(rms(_loopStart - d - w, _loopStart - d)) - SampleUtils::toDb(rms(_loopEnd - d - w, _loopEnd - d));
            if (d == 0)
                levelJump = difference;
            if (std::fabs(difference) > std::fabs(maxLevelDifference))
                maxLevelDifference = difference;
        }
        result["max_level_difference_db"] = roundDecimals(maxLevelDifference, 2);
    }
    result["level_jump_db"] = roundDecimals(levelJump, 2);
    result["natural_level_variation_db"] = roundDecimals(naturalLevelVariation, 2);
    int quarter = length / 4;
    if (quarter > 16)
        result["level_change_in_loop_db"] = roundDecimals(SampleUtils::toDb(rms(_loopEnd - quarter, _loopEnd)) - SampleUtils::toDb(rms(_loopStart, _loopStart + quarter)), 2);

    int spectrumLength = previousPowerOfTwo(qMin(4096, length / 3));
    if (spectrumLength >= 256)
    {
        double ws, wss;
        auto spectrumAt = [&](int start) -> QVector<double> {
            return averagePowerSpectrum(start, start + spectrumLength, spectrumLength, spectrumLength, false, ws, wss);
        };
        QList<int> timbreDistances = referenceDistances(spectrumLength);
        bool withReference = !timbreDistances.isEmpty();

        // Spectra before and after the loop point (with a reference: what precedes the loop end and what precedes the loop start)
        QVector<double> endSpectrum = spectrumAt(_loopEnd - spectrumLength);
        QVector<double> startSpectrum = withReference ? spectrumAt(_loopStart - spectrumLength) : spectrumAt(_loopStart);
        double binWidth = sr / spectrumLength;
        double c1 = 0, c2 = 0, p1 = 0, p2 = 0;
        for (int k = qMax(1, static_cast<int>(50 / binWidth)); k < qMin(static_cast<int>(endSpectrum.size()), static_cast<int>(16000 / binWidth)); k++)
        {
            c1 += k * binWidth * endSpectrum[k];
            p1 += endSpectrum[k];
            c2 += k * binWidth * startSpectrum[k];
            p2 += startSpectrum[k];
        }

        // Spectral distance on the significant third-octave bands
        QVector<double> centers;
        foreach (double f, getThirdOctaveFrequencies())
            if (f >= 50 && f <= qMin(16000.0, 0.9 * sr / 2) && f > 2 * binWidth)
                centers << f;
        // Distance between the spectrum of a window ending at a position and a window starting at another one
        auto distance = [&](const QVector<double> &before, const QVector<double> &after) -> double {
            QVector<double> beforeBands = bandLevels(before, spectrumLength, centers, 1.0 / 3);
            QVector<double> afterBands = bandLevels(after, spectrumLength, centers, 1.0 / 3);
            double maxBand = -200;
            foreach (double level, beforeBands)
                maxBand = qMax(maxBand, level);
            QVector<double> diff;
            for (int i = 0; i < centers.size(); i++)
                if (qMax(beforeBands[i], afterBands[i]) > maxBand - 50 && beforeBands[i] > -150 && afterBands[i] > -150)
                    diff << afterBands[i] - beforeBands[i];
            if (diff.size() < 4)
                return std::nan("");
            double mean = 0;
            foreach (double value, diff)
                mean += value / diff.size();
            double variance = 0;
            foreach (double value, diff)
                variance += (value - mean) * (value - mean) / diff.size();
            return std::sqrt(variance);
        };
        QVector<double> naturalTimbre;
        for (int k = 1; k <= 3; k++)
        {
            int p = _loopStart + spectrumLength + k * (length - 2 * spectrumLength) / 4;
            if (p - spectrumLength < _loopStart || p + spectrumLength > _loopEnd)
                continue;
            double d = distance(spectrumAt(p - spectrumLength), spectrumAt(p));
            if (std::isfinite(d))
                naturalTimbre << d;
        }
        double naturalTimbreVariation = naturalTimbre.isEmpty() ? 0 : SampleUtils::median(naturalTimbre);
        double timbreJump = withReference ? distance(startSpectrum, endSpectrum) : distance(endSpectrum, startSpectrum);
        double maxTimbreDifference = timbreJump;
        foreach (int d, timbreDistances)
        {
            if (d == 0)
                continue;
            double difference = distance(spectrumAt(_loopStart - d - spectrumLength), spectrumAt(_loopEnd - d - spectrumLength));
            if (std::isfinite(difference) && !(difference <= maxTimbreDifference)) // Also if maxTimbreDifference is NaN
                maxTimbreDifference = difference;
        }
        if (std::isfinite(timbreJump))
        {
            result["timbre_jump_db"] = roundDecimals(timbreJump, 2);
            result["natural_timbre_variation_db"] = roundDecimals(naturalTimbreVariation, 2);
        }
        if (withReference && std::isfinite(maxTimbreDifference))
            result["max_timbre_difference_db"] = roundDecimals(maxTimbreDifference, 2);
        if (p1 > 0 && p2 > 0)
            result["brightness_jump_percent"] = roundDecimals(100 * (c2 / p2 - c1 / p1) / (c1 / p1), 1);
    }

    if (f0 > 0)
    {
        double fMin, fMax;
        getPitchRange(fMin, fMax);
        int tauMax = static_cast<int>(std::ceil(sr / fMin));
        int window = qMax(static_cast<int>(0.02 * sr), 3 * tauMax);
        if (window + tauMax + 1 < length / 2)
        {
            double c1, c2;
            double fStart = yin(_loopStart, window, fMin, fMax, c1);
            double fEnd = yin(_loopEnd - window - tauMax - 1, window, fMin, fMax, c2);
            if (fStart > 0 && fEnd > 0 && c1 > 0.6 && c2 > 0.6)
                result["pitch_jump_cents"] = roundDecimals(1200 * std::log2(fStart / fEnd), 1);
        }
    }

    result["note"] = "seam_correlation: correlation between the waveforms preceding the loop end and the loop start "
                     "(1 = identical); seam_error_db: energy of their difference relative to the signal; "
                     "junction_jump_db: jump at the loop point relative to the typical second difference of the signal "
                     "before it; click_score_db: residual of a linear prediction across the junction relative to its "
                     "level before; level_jump_db and timbre_jump_db compare what precedes the loop end with what "
                     "precedes the loop start (the original sound at the same point), max_level_difference_db and "
                     "max_timbre_difference_db the same further before the loop points; natural_*_variation_db: the "
                     "same measures between consecutive windows inside the loop; periods and harmonic_phase_mismatch "
                     "(0 = harmonics continue in phase, 1 = opposition) assume a single fundamental";
    _loopResult = result;
}

QJsonObject SoundAnalyzer::getLoop()
{
    computeLoop();
    return _loopResult;
}

double SoundAnalyzer::getLoopCorrelation()
{
    computeLoop();
    return _loopCorrelation;
}

//////////////////////////
/// QUALITY           ///
//////////////////////////

QJsonObject SoundAnalyzer::getQuality()
{
    computeRegions();
    computeSpectrum();
    computeHarmonics();
    double sr = _sampleRate;
    QJsonObject result;

    // Clipping: runs of 3 samples or more at the full scale (or at the maximum of a normalized clipped sample)
    int runs = 0, longest = 0, current = 0;
    QJsonArray positions;
    double limit = _peak >= 0.5 ? qMin(0.999, _peak - 1.0 / 32768) : 0.999;
    for (int i = 0; i <= _length; i++)
    {
        if (i < _length && std::fabs(_data[i]) >= limit)
            current++;
        else
        {
            if (current >= 3)
            {
                runs++;
                longest = qMax(longest, current);
                if (positions.size() < 5)
                    positions.append(roundDecimals((i - current) / sr, 4));
            }
            current = 0;
        }
    }
    QJsonObject clipping;
    clipping["runs"] = runs;
    clipping["longest_run"] = longest;
    if (runs > 0)
        clipping["first_positions_s"] = positions;
    result["clipping"] = clipping;

    // DC offset
    double mean = 0, steadyMean = 0;
    for (int i = 0; i < _length; i++)
        mean += _data[i] / _length;
    int steadyLength = _steadyEnd - _steadyStart;
    double fundamental = getFundamental();
    if (fundamental > 0)
    {
        // Whole number of periods, otherwise the mean of an asymmetric waveform is not 0
        int periods = static_cast<int>(steadyLength * fundamental / sr);
        if (periods > 0)
            steadyLength = qMax(1, qRound(periods * sr / fundamental));
    }
    for (int i = _steadyStart; i < _steadyStart + steadyLength && i < _length; i++)
        steadyMean += _data[i] / qMax(1, steadyLength);
    result["dc_offset_dbfs"] = roundDecimals(SampleUtils::toDb(std::fabs(mean)), 1);
    result["dc_offset_steady_dbfs"] = roundDecimals(SampleUtils::toDb(std::fabs(steadyMean)), 1);

    // Noise
    double steadyDb = SampleUtils::toDb(rms(_steadyStart, _steadyEnd));
    if (_noiseFloorDb > -199)
    {
        result["noise_floor_dbfs"] = roundDecimals(_noiseFloorDb, 1);
        result["noise_measured"] = _noiseReference;
        result["signal_to_noise_db"] = roundDecimals(steadyDb - _noiseFloorDb, 1);
    }
    else
        result["noise_floor_dbfs"] = QJsonValue(); // No silent part at the beginning or at the end

    // Hum (50 / 60 Hz and harmonics not explained by the fundamental) and rumble
    double f0 = getFundamental();
    if (!_power.isEmpty())
    {
        double binWidth = sr / _fftSize;
        QJsonArray hum;
        foreach (double base, QList<double>({ 50, 60 }))
        {
            for (int m = 1; m <= 3; m++)
            {
                double f = base * m;
                if (f0 > 0)
                {
                    double ratio = f / f0;
                    if (std::fabs(ratio - std::round(ratio)) * f0 < 0.03 * f || ratio < 0.5)
                        continue;
                }
                int k = qRound(f / binWidth);
                if (k < 2 || k + 3 >= _power.size())
                    continue;
                QVector<double> around;
                for (int j = qMax(1, k - static_cast<int>(10 / binWidth) - 2); j <= k + static_cast<int>(10 / binWidth) + 2; j++)
                    if (std::abs(j - k) > 1)
                        around << _power[qMin(j, static_cast<int>(_power.size()) - 1)];
                double peak = qMax(_power[k], qMax(_power[k - 1], _power[k + 1]));
                if (!around.isEmpty() && SampleUtils::median(around) > 0 && 10 * std::log10(peak / SampleUtils::median(around)) > 10)
                    hum.append(f);
            }
        }
        if (!hum.isEmpty())
            result["hum_hz"] = hum;

        double low = 0, total = 0;
        for (int k = 1; k < _power.size(); k++)
        {
            total += _power[k];
            if (k * binWidth < 20)
                low += _power[k];
        }
        if (total > 0)
            result["energy_below_20hz_db"] = roundDecimals(SampleUtils::toDb(std::sqrt(low / total), -120), 1);
    }

    // Resolution actually used (a 24-bit sample may only contain 16 bits)
    int zeroBits = 8;
    bool allZero = true;
    for (int i = 0; i < _length && zeroBits > 0; i++)
    {
        qint64 value = qRound64(_data[i] * 8388608.0);
        if (value == 0)
            continue;
        allZero = false;
        int bits = 0;
        while (bits < zeroBits && (value & (1LL << bits)) == 0)
            bits++;
        zeroBits = qMin(zeroBits, bits);
    }
    result["effective_bits"] = allZero ? 0 : 24 - zeroBits;

    // Silences and end (level of the last 10 ms relative to the peak)
    result["leading_silence_ms"] = roundDecimals(1000 * _onset / sr, 1);
    int tail = qMax(1, static_cast<int>(0.01 * sr));
    result["end_level_db"] = _peak > 0 ? QJsonValue(roundDecimals(qMax(-200.0, SampleUtils::toDb(rms(_length - tail, _length)) - SampleUtils::toDb(_peak)), 1)) :
                                         QJsonValue();
    result["peak_dbfs"] = jsonNumber(SampleUtils::toDb(_peak), 2);
    result["note"] = "hum_hz: peaks at 50 / 60 Hz and their first multiples standing 10 dB above their neighborhood, "
                     "the harmonics of the fundamental excluded; end_level_db: level of the last 10 ms relative to the "
                     "peak (null if the sample is silent)";
    return result;
}

//////////////////////////
/// COMPARISON        ///
//////////////////////////

QJsonObject SoundAnalyzer::compare(SoundAnalyzer &reference, SoundAnalyzer &other)
{
    QJsonObject result;
    double f1 = reference.getFundamental(), f2 = other.getFundamental();
    if (f1 > 0 && f2 > 0)
        result["interval_semitones"] = roundDecimals(12 * std::log2(f2 / f1), 2);

    // Spectral balance after removing the level difference: on the envelopes of the harmonics if both sounds are
    // harmonic (the third-octave bands of two notes contain different numbers of harmonics), otherwise on the bands
    QVector<double> centers = getThirdOctaveFrequencies();
    QVector<double> diff, usedCenters;
    double fHigh = qMin(16000.0, 0.9 * qMin(reference._sampleRate, other._sampleRate) / 2);
    bool useEnvelopes = reference.harmonicCount() >= 3 && other.harmonicCount() >= 3;
    if (useEnvelopes)
    {
        foreach (double center, centers)
        {
            double a = reference.harmonicEnvelopeDb(center), b = other.harmonicEnvelopeDb(center);
            if (center <= fHigh && std::isfinite(a) && std::isfinite(b))
            {
                diff << b - a;
                usedCenters << center;
            }
        }
        if (diff.size() < 3)
            useEnvelopes = false;
    }
    if (!useEnvelopes)
    {
        diff.clear();
        usedCenters.clear();
        QVector<double> l1 = reference.getThirdOctaveLevels(), l2 = other.getThirdOctaveLevels();
        double fLow = qMax(qMax(f1, f2) * 0.9, 40.0);
        for (int i = 0; i < centers.size(); i++)
        {
            if (centers[i] >= fLow && centers[i] <= fHigh && l1[i] > -150 && l2[i] > -150)
            {
                diff << l2[i] - l1[i];
                usedCenters << centers[i];
            }
        }
    }
    result["compared_on"] = useEnvelopes ? "envelopes of the harmonic peaks" : "third-octave bands";
    if (diff.size() >= 3)
    {
        double mean = 0;
        foreach (double value, diff)
            mean += value / diff.size();
        double variance = 0;
        foreach (double value, diff)
            variance += (value - mean) * (value - mean) / diff.size();
        result["spectral_distance_db"] = roundDecimals(std::sqrt(variance), 2);

        // Mean differences per octave
        QJsonObject octaves;
        foreach (double center, QList<double>({ 125, 250, 500, 1000, 2000, 4000, 8000, 16000 }))
        {
            double sum = 0;
            int count = 0;
            for (int i = 0; i < usedCenters.size(); i++)
            {
                if (usedCenters[i] >= center / 1.42 && usedCenters[i] < center * 1.42)
                {
                    sum += diff[i] - mean;
                    count++;
                }
            }
            if (count > 0)
                octaves[QString::number(center)] = roundDecimals(sum / count, 1);
        }
        result["octave_differences_db"] = octaves;
    }

    auto difference = [&](QString name, double a, double b, int decimals) {
        if (std::isfinite(a) && std::isfinite(b))
            result[name] = roundDecimals(b - a, decimals);
    };
    difference("steady_loudness_difference_db", reference.getSteadyLoudness(), other.getSteadyLoudness(), 2);
    difference("momentary_max_difference_db", reference.getMomentaryMaxLoudness(), other.getMomentaryMaxLoudness(), 2);
    double c1 = reference.getSpectralCentroid(), c2 = other.getSpectralCentroid();
    difference("centroid_difference_hz", c1, c2, 0);
    if (c1 > 0)
        result["centroid_difference_percent"] = roundDecimals(100 * (c2 - c1) / c1, 1);
    difference("centroid_in_harmonics_difference", reference.getHarmonicCentroid(), other.getHarmonicCentroid(), 2);
    difference("harmonic_tilt_difference_db_per_octave", reference.getHarmonicTilt(), other.getHarmonicTilt(), 2);
    difference("harmonic_to_noise_difference_db", reference.getHnr(), other.getHnr(), 1);
    difference("attack_time_difference_ms", 1000 * reference.getAttackTime(), 1000 * other.getAttackTime(), 1);

    // First harmonics, levels relative to the strongest one
    QJsonArray harmonicDifferences;
    QJsonArray h1 = reference.getHarmonics(10).value("levels_db").toArray();
    QJsonArray h2 = other.getHarmonics(10).value("levels_db").toArray();
    for (int i = 0; i < qMin(h1.size(), h2.size()); i++)
        harmonicDifferences.append(h1[i].isDouble() && h2[i].isDouble() ? QJsonValue(roundDecimals(h2[i].toDouble() - h1[i].toDouble(), 1)) : QJsonValue());
    if (!harmonicDifferences.isEmpty())
        result["harmonic_level_differences_db"] = harmonicDifferences;
    result["note"] = "differences are other minus reference; spectral_distance_db (standard deviation of the "
                     "differences, 0 = same spectral balance) and octave_differences_db compare the spectral balance "
                     "after removing the mean level difference";
    return result;
}

int SoundAnalyzer::harmonicCount()
{
    computeHarmonics();
    int count = 0;
    foreach (Harmonic harmonic, _harmonics)
        if (harmonic.found)
            count++;
    return count;
}

double SoundAnalyzer::harmonicEnvelopeDb(double frequency)
{
    // Levels of the harmonic peaks relative to the strongest one, interpolated on a logarithmic frequency scale
    computeHarmonics();
    double strongest = 0;
    foreach (Harmonic harmonic, _harmonics)
        if (harmonic.found)
            strongest = qMax(strongest, harmonic.amplitude);
    int previous = -1;
    for (int i = 0; i < _harmonics.size(); i++)
    {
        if (!_harmonics[i].found)
            continue;
        if (_harmonics[i].frequency >= frequency)
        {
            double level = SampleUtils::toDb(_harmonics[i].amplitude / strongest);
            if (previous < 0)
                return std::fabs(_harmonics[i].frequency - frequency) < 1e-9 ? level : std::nan("");
            double previousLevel = SampleUtils::toDb(_harmonics[previous].amplitude / strongest);
            double ratio = std::log(frequency / _harmonics[previous].frequency) /
                           std::log(_harmonics[i].frequency / _harmonics[previous].frequency);
            return previousLevel + ratio * (level - previousLevel);
        }
        previous = i;
    }
    return std::nan("");
}

double SoundAnalyzer::getSteadyLoudness()
{
    computeLoudness();
    return _steadyLoudness;
}

double SoundAnalyzer::getMomentaryMaxLoudness()
{
    computeLoudness();
    return _momentaryMax;
}

//////////////////////////
/// STEREO            ///
//////////////////////////

QJsonObject SoundAnalyzer::getStereo(const QVector<float> &left, const QVector<float> &right, quint32 sampleRate,
                                     quint32 start, quint32 end)
{
    QJsonObject result;
    int length = static_cast<int>(qMin(left.size(), right.size()));
    int s = qBound(0, static_cast<int>(start), length);
    int e = qBound(s, static_cast<int>(end), length);
    if (e - s < 64)
    {
        s = 0;
        e = length;
    }
    if (e - s < 64)
        return result;

    double ll = 0, rr = 0, lr = 0, mm = 0, ss = 0;
    for (int i = s; i < e; i++)
    {
        double l = left[i], r = right[i];
        ll += l * l;
        rr += r * r;
        lr += l * r;
        double m = (l + r) / 2, d = (l - r) / 2;
        mm += m * m;
        ss += d * d;
    }
    if (ll <= 0 || rr <= 0)
    {
        result["note"] = "one channel is silent";
        return result;
    }
    double correlation = lr / std::sqrt(ll * rr);
    result["correlation"] = roundDecimals(correlation, 3);
    result["balance_db"] = roundDecimals(10 * std::log10(ll / rr), 2);
    result["width_db"] = mm > 0 ? QJsonValue(roundDecimals(qMax(-120.0, 10 * std::log10(qMax(ss, 1e-30) / mm)), 1)) : QJsonValue();
    result["mono_sum_loss_db"] = roundDecimals(10 * std::log10(qMax(mm, 1e-30) / ((ll + rr) / 2)), 2);

    // Delay between the channels (maximum of the cross-correlation within 2 ms)
    int maxLag = static_cast<int>(0.002 * sampleRate);
    int center = (s + e) / 2;
    int half = qMin(4096, (e - s) / 2 - maxLag - 1);
    if (half > 64)
    {
        double best = 0;
        int bestLag = 0;
        for (int lag = -maxLag; lag <= maxLag; lag++)
        {
            double sum = 0, sl = 0, sr = 0;
            for (int i = center - half; i < center + half; i++)
            {
                double l = left[i], r = right[i + lag];
                sum += l * r;
                sl += l * l;
                sr += r * r;
            }
            double value = (sl > 0 && sr > 0) ? sum / std::sqrt(sl * sr) : 0;
            if (std::fabs(value) > std::fabs(best))
            {
                best = value;
                bestLag = lag;
            }
        }
        result["delay_ms"] = roundDecimals(1000.0 * bestLag / sampleRate, 3);
        result["correlation_at_delay"] = roundDecimals(best, 3);
    }

    result["note"] = "correlation: 1 = identical channels, 0 = uncorrelated, -1 = opposite; width_db: side / mid energy; "
                     "balance_db: left / right energy; mono_sum_loss_db: energy of (L + R) / 2 relative to the mean "
                     "energy of the channels; delay_ms: lag of the right channel maximizing the absolute "
                     "cross-correlation within 2 ms (positive = right channel later), correlation_at_delay: its value";
    return result;
}

//////////////////////////
/// IMAGES            ///
//////////////////////////

static QColor colorMap(double value)
{
    // Black, blue, purple, orange, light yellow
    static const double stops[5][3] = { { 0, 0, 0 }, { 30, 30, 120 }, { 170, 40, 100 }, { 245, 140, 30 }, { 255, 250, 200 } };
    value = qBound(0.0, value, 1.0) * 4;
    int index = qMin(3, static_cast<int>(value));
    double t = value - index;
    return QColor(static_cast<int>(stops[index][0] + t * (stops[index + 1][0] - stops[index][0])),
                  static_cast<int>(stops[index][1] + t * (stops[index + 1][1] - stops[index][1])),
                  static_cast<int>(stops[index][2] + t * (stops[index + 1][2] - stops[index][2])));
}

QImage SoundAnalyzer::getSpectrogram(int width, int height)
{
    QImage image(width, height, QImage::Format_RGB32);
    image.fill(Qt::black);
    int frameLength = _sampleRate <= 48000 ? 2048 : 4096;
    if (_length < 256)
        return image;
    if (_length < frameLength)
        frameLength = previousPowerOfTwo(_length);

    const int marginLeft = 40, marginBottom = 16;
    int plotWidth = width - marginLeft, plotHeight = height - marginBottom;
    double nyquist = _sampleRate / 2.0;
    double fMin = 20, fMax = nyquist;

    // Spectra of the columns
    QVector<double> window(frameLength);
    for (int i = 0; i < frameLength; i++)
        window[i] = 0.5 - 0.5 * std::cos(2 * PI * i / frameLength);
    QVector<QVector<double> > columns(plotWidth);
    double maxDb = -200;
    std::vector<Complex> buffer(frameLength);
    const FourierTransform &fourierTransform = getFourierTransform(frameLength);
    for (int c = 0; c < plotWidth; c++)
    {
        int position = plotWidth > 1 ? static_cast<int>(static_cast<double>(c) * (_length - frameLength) / (plotWidth - 1)) : 0;
        for (int i = 0; i < frameLength; i++)
            buffer[i] = _data[position + i] * window[i];
        fourierTransform.transform(buffer, false);
        QVector<double> column(plotHeight);
        for (int r = 0; r < plotHeight; r++)
        {
            double f = fMin * std::pow(fMax / fMin, static_cast<double>(plotHeight - 1 - r) / (plotHeight - 1));
            double bin = f * frameLength / _sampleRate;
            int k = qBound(0, static_cast<int>(bin), frameLength / 2 - 1);
            double t = bin - k;
            double p = (1 - t) * std::norm(buffer[k]) + t * std::norm(buffer[k + 1]);
            column[r] = p > 0 ? 10 * std::log10(p) : -200;
            maxDb = qMax(maxDb, column[r]);
        }
        columns[c] = column;
    }
    for (int c = 0; c < plotWidth; c++)
        for (int r = 0; r < plotHeight; r++)
            image.setPixelColor(marginLeft + c, r, colorMap((columns[c][r] - (maxDb - 90)) / 90));

    // Frequency grid, time axis and loop
    QPainter painter(&image);
    painter.setPen(QColor(200, 200, 200));
    QFont font = painter.font();
    font.setPixelSize(10);
    painter.setFont(font);
    foreach (double f, QList<double>({ 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000 }))
    {
        if (f >= fMax)
            continue;
        int r = static_cast<int>((plotHeight - 1) * (1 - std::log(f / fMin) / std::log(fMax / fMin)));
        painter.drawLine(marginLeft - 4, r, marginLeft, r);
        painter.drawText(QRect(0, r - 6, marginLeft - 6, 12), Qt::AlignRight | Qt::AlignVCenter,
                         f >= 1000 ? QString("%1k").arg(f / 1000) : QString::number(f));
    }
    double duration = static_cast<double>(_length) / _sampleRate;
    painter.drawText(QRect(marginLeft, plotHeight, plotWidth, marginBottom), Qt::AlignLeft | Qt::AlignVCenter, "0 s");
    painter.drawText(QRect(marginLeft, plotHeight, plotWidth, marginBottom), Qt::AlignRight | Qt::AlignVCenter,
                     QString("%1 s").arg(roundDecimals(duration, 2)));
    if (_looped)
    {
        painter.setPen(QPen(QColor(0, 230, 230), 1, Qt::DashLine));
        foreach (int position, QList<int>({ _loopStart, _loopEnd }))
        {
            int x = marginLeft + static_cast<int>(static_cast<double>(position) / _length * plotWidth);
            painter.drawLine(x, 0, x, plotHeight - 1);
        }
        painter.drawText(QRect(marginLeft, plotHeight, plotWidth, marginBottom), Qt::AlignHCenter | Qt::AlignVCenter, "loop: dashed lines");
    }
    return image;
}

QImage SoundAnalyzer::getLoopSeamImage(int width, int height)
{
    QImage image(width, height, QImage::Format_RGB32);
    image.fill(QColor(255, 255, 255));
    if (!_looped)
        return image;

    // End of the loop then its beginning, as heard
    double f0 = getFundamental();
    int count = f0 > 0 ? static_cast<int>(3 * _sampleRate / f0) : static_cast<int>(0.005 * _sampleRate);
    count = qBound(static_cast<int>(0.002 * _sampleRate), count, static_cast<int>(0.02 * _sampleRate));
    count = qMin(count, qMin(_loopEnd - _loopStart, _loopEnd));
    QVector<double> heard, original;
    for (int i = _loopEnd - count; i < _loopEnd; i++)
        heard << _data[i];
    for (int i = _loopStart; i < _loopStart + count; i++)
        heard << _data[i];
    for (int i = _loopEnd; i < qMin(_length, _loopEnd + count); i++)
        original << _data[i]; // Natural continuation, if the sample was not trimmed

    double maxValue = 1e-9;
    foreach (double value, heard)
        maxValue = qMax(maxValue, std::fabs(value));
    foreach (double value, original)
        maxValue = qMax(maxValue, std::fabs(value));

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    int top = 16;
    int plotHeight = height - top - 4;
    auto pointAt = [&](int index, double value) -> QPointF {
        return QPointF(static_cast<double>(index) * (width - 1) / (heard.size() - 1),
                       top + plotHeight / 2.0 - value / maxValue * (plotHeight / 2.0 - 2));
    };
    painter.setPen(QColor(220, 220, 220));
    painter.drawLine(0, top + plotHeight / 2, width, top + plotHeight / 2);
    if (original.size() > 1)
    {
        painter.setPen(QPen(QColor(150, 150, 150), 1, Qt::DashLine));
        QPolygonF polygon;
        for (int i = 0; i < original.size(); i++)
            polygon << pointAt(count + i, original[i]);
        painter.drawPolyline(polygon);
    }
    painter.setPen(QPen(QColor(20, 60, 160), 1.5));
    QPolygonF polygon;
    for (int i = 0; i < heard.size(); i++)
        polygon << pointAt(i, heard[i]);
    painter.drawPolyline(polygon);
    int seam = static_cast<int>(pointAt(count, 0).x());
    painter.setPen(QPen(QColor(220, 30, 30), 1));
    painter.drawLine(seam, top, seam, height);
    QFont font = painter.font();
    font.setPixelSize(10);
    painter.setFont(font);
    painter.setPen(Qt::black);
    painter.drawText(QRect(0, 0, seam - 4, top), Qt::AlignRight | Qt::AlignVCenter, "end of loop");
    painter.drawText(QRect(seam + 4, 0, width - seam - 4, top), Qt::AlignLeft | Qt::AlignVCenter,
                     original.size() > 1 ? "start of loop (dashed: original continuation)" : "start of loop");
    return image;
}

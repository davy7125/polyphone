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

#include "gaincurve.h"
#include "sampleutils.h"
#include <algorithm>
#include <cmath>
#include <complex>

const double GainCurve::MIN_GAIN = -60;
const double GainCurve::MAX_GAIN = 24;
const double GainCurve::CEILING_REMOVED = -80;

GainCurve::GainCurve() :
    _mode(MODE_GAIN),
    _noteTracking(false)
{}

GainCurve::GainCurve(Mode mode, bool noteTracking, const QList<QPointF> &points) :
    _mode(mode),
    _noteTracking(noteTracking)
{
    for (const QPointF &point : points)
        if (point.x() > 0 && std::isfinite(point.x()) && std::isfinite(point.y()))
            _points << point;
    std::stable_sort(_points.begin(), _points.end(), [](const QPointF &a, const QPointF &b) {
        return a.x() < b.x();
    });
}

double GainCurve::getDb(double x) const
{
    double db = 0;
    if (!_points.isEmpty())
    {
        if (x <= _points.first().x())
            db = _points.first().y();
        else if (x >= _points.last().x())
            db = _points.last().y();
        else
        {
            // First point after x, the previous one being at x or before
            auto next = std::upper_bound(_points.constBegin(), _points.constEnd(), x, [](double value, const QPointF &point) {
                return value < point.x();
            });
            const QPointF &p1 = *(next - 1);
            const QPointF &p2 = *next;
            db = p1.y() + (p2.y() - p1.y()) * std::log(x / p1.x()) / std::log(p2.x() / p1.x());
        }
    }
    return _mode == MODE_GAIN ? qBound(MIN_GAIN, db, MAX_GAIN) : qMin(db, 0.0);
}

bool GainCurve::isNeutral() const
{
    for (const QPointF &point : _points)
        if (getDb(point.x()) != 0.0)
            return false;
    return true;
}

double GainCurve::getFundamental(int rootKey, int pitchCorrection)
{
    int key = (rootKey >= 0 && rootKey <= 127) ? rootKey : 60;
    return SampleUtils::keyToFrequency(key - 0.01 * pitchCorrection);
}

QVector<float> GainCurve::apply(const QVector<float> &data, double sampleRate, double fundamental, quint32 loopStart,
                                quint32 loopEnd, double * peakBefore, double * peakAfter) const
{
    double initialPeak = SampleUtils::peak(data.constData(), static_cast<quint32>(data.size()));
    if (peakBefore != nullptr)
        *peakBefore = initialPeak;
    if (data.isEmpty() || sampleRate <= 0 || isNeutral())
    {
        if (peakAfter != nullptr)
            *peakAfter = initialPeak;
        return data;
    }

    // Abscissa of a frequency
    double xPerHz = (_noteTracking && fundamental > 0) ? 1.0 / fundamental : 1.0;

    QVector<float> result = (_mode == MODE_GAIN) ? applyGain(data, sampleRate, xPerHz, loopStart, loopEnd) :
                                                   applyCeiling(data, sampleRate, xPerHz);
    if (peakAfter != nullptr)
        *peakAfter = SampleUtils::peak(result.constData(), static_cast<quint32>(result.size()));
    SampleUtils::limitToFullScale(result);
    return result;
}

QVector<float> GainCurve::applyCeiling(const QVector<float> &data, double sampleRate, double xPerHz) const
{
    // Spectrum of the whole sample, zero-padded to a power of 2
    int size = SampleUtils::nextPowerOfTwo(data.size());
    std::vector<std::complex<double> > spectrum(static_cast<size_t>(size));
    for (int i = 0; i < data.size(); i++)
        spectrum[static_cast<size_t>(i)] = data[i];
    FourierTransform transform(size);
    transform.transform(spectrum, false);

    // Modules limited relative to the strongest one, the symmetric bin (size - k) being multiplied by the same gain so
    // that the signal stays real
    double maxModule = 0;
    for (int k = 0; k <= size / 2; k++)
        maxModule = qMax(maxModule, std::abs(spectrum[static_cast<size_t>(k)]));
    for (int k = 0; k <= size / 2; k++)
    {
        double db = getDb(k * sampleRate / size * xPerHz);
        double limit = (db <= CEILING_REMOVED) ? 0.0 : maxModule * std::pow(10.0, db / 20);
        double module = std::abs(spectrum[static_cast<size_t>(k)]);
        double gain = module > limit ? limit / module : 1.0;
        spectrum[static_cast<size_t>(k)] *= gain;
        if (k > 0 && k < size - k)
            spectrum[static_cast<size_t>(size - k)] *= gain;
    }

    transform.transform(spectrum, true);
    QVector<float> result(data.size());
    for (int i = 0; i < data.size(); i++)
        result[i] = static_cast<float>(spectrum[static_cast<size_t>(i)].real());
    return result;
}

QVector<float> GainCurve::applyGain(const QVector<float> &data, double sampleRate, double xPerHz, quint32 loopStart,
                                    quint32 loopEnd) const
{
    std::vector<double> response = minimumPhaseResponse(sampleRate, xPerHz);
    size_t length = static_cast<size_t>(data.size());
    std::vector<double> result;

    if (!SampleUtils::isLooped(loopStart, loopEnd, static_cast<quint32>(length)))
    {
        std::vector<double> signal(data.constBegin(), data.constEnd());
        result = convolve(signal, response, length);
    }
    else
    {
        size_t start = loopStart;
        size_t end = loopEnd;
        size_t loopLength = end - start;

        // Before the loop: the filter only depends on the past
        std::vector<double> attack;
        if (start > 0)
        {
            attack.assign(data.constBegin(), data.constBegin() + static_cast<int>(start));
            attack = convolve(attack, response, start);
        }

        // Loop played endlessly: once the response covers repetitions only, the result is periodic. The data after the
        // loop follows these repetitions (sample looped until the key is released).
        size_t repetitions = (response.size() + loopLength - 1) / loopLength + 1;
        std::vector<double> repeated(repetitions * loopLength + (length - end));
        for (size_t r = 0; r < repetitions; r++)
            std::copy(data.constBegin() + static_cast<int>(start), data.constBegin() + static_cast<int>(end),
                      repeated.begin() + static_cast<std::ptrdiff_t>(r * loopLength));
        std::copy(data.constBegin() + static_cast<int>(end), data.constEnd(),
                  repeated.begin() + static_cast<std::ptrdiff_t>(repetitions * loopLength));
        std::vector<double> steady = convolve(repeated, response, repeated.size());
        size_t steadyStart = (repetitions - 1) * loopLength; // Result of the loop start, periodic from there

        result.resize(length);
        std::copy(attack.begin(), attack.end(), result.begin());
        std::copy(steady.begin() + static_cast<std::ptrdiff_t>(steadyStart), steady.end(),
                  result.begin() + static_cast<std::ptrdiff_t>(start));

        // The transient of the filter after the attack cannot be stored in a periodic loop: the result of the attack
        // fades into the periodic result (the end of the loop) before the loop start, over the length of the response
        size_t fade = std::min(std::min(start, loopLength), response.size());
        for (size_t j = 0; j < fade; j++)
        {
            size_t i = start - fade + j;
            double weight = static_cast<double>(j + 1) / static_cast<double>(fade + 1);
            result[i] = (1.0 - weight) * attack[i] + weight * steady[steadyStart + loopLength - (start - i)];
        }
    }

    QVector<float> output(static_cast<int>(length));
    for (size_t i = 0; i < length; i++)
        output[static_cast<int>(i)] = static_cast<float>(result[i]);
    return output;
}

std::vector<double> GainCurve::minimumPhaseResponse(double sampleRate, double xPerHz) const
{
    // Frequency grid of about 0.5 Hz, also leaving room for the decay of the cepstrum and of the response
    int size = qMax(4096, SampleUtils::nextPowerOfTwo(static_cast<int>(2 * sampleRate)));
    int half = size / 2;
    FourierTransform transform(size);

    // Real cepstrum: inverse transform of the logarithm of the magnitude (real and symmetric)
    std::vector<std::complex<double> > values(static_cast<size_t>(size));
    const double dbToNeper = std::log(10.0) / 20;
    for (int k = 0; k <= half; k++)
    {
        double logMagnitude = getDb(k * sampleRate / size * xPerHz) * dbToNeper;
        values[static_cast<size_t>(k)] = logMagnitude;
        if (k > 0 && k < half)
            values[static_cast<size_t>(size - k)] = logMagnitude;
    }
    transform.transform(values, true);

    // Folded cepstrum (causal part counted twice): its exponential has the same magnitude with a minimum phase
    values[0] = values[0].real();
    for (int n = 1; n < half; n++)
        values[static_cast<size_t>(n)] = 2.0 * values[static_cast<size_t>(n)].real();
    values[static_cast<size_t>(half)] = values[static_cast<size_t>(half)].real();
    for (int n = half + 1; n < size; n++)
        values[static_cast<size_t>(n)] = 0.0;
    transform.transform(values, false);
    for (std::complex<double> &value : values)
        value = std::exp(value);

    // Impulse response, cut where the remaining energy is negligible (-120 dB)
    transform.transform(values, true);
    std::vector<double> response(static_cast<size_t>(size));
    double energy = 0;
    for (int i = 0; i < size; i++)
    {
        response[static_cast<size_t>(i)] = values[static_cast<size_t>(i)].real();
        energy += response[static_cast<size_t>(i)] * response[static_cast<size_t>(i)];
    }
    double tail = 0;
    size_t length = response.size();
    while (length > 1)
    {
        double value = response[length - 1];
        if (tail + value * value > 1e-12 * energy)
            break;
        tail += value * value;
        length--;
    }
    response.resize(length);
    return response;
}

std::vector<double> GainCurve::convolve(const std::vector<double> &signal, const std::vector<double> &response,
                                        size_t outputLength)
{
    // Linear convolution by FFT, the size avoiding any circular overlap
    std::vector<double> result(outputLength, 0.0);
    if (signal.empty() || response.empty())
        return result;
    int size = SampleUtils::nextPowerOfTwo(static_cast<int>(signal.size() + response.size() - 1));
    std::vector<std::complex<double> > a(static_cast<size_t>(size)), b(static_cast<size_t>(size));
    std::copy(signal.begin(), signal.end(), a.begin());
    std::copy(response.begin(), response.end(), b.begin());
    FourierTransform transform(size);
    transform.transform(a, false);
    transform.transform(b, false);
    for (size_t k = 0; k < a.size(); k++)
        a[k] *= b[k];
    transform.transform(a, true);
    size_t count = std::min(outputLength, a.size());
    for (size_t i = 0; i < count; i++)
        result[i] = a[i].real();
    return result;
}

QVariantList GainCurve::pointsToVariant() const
{
    QVariantList list;
    for (const QPointF &point : _points)
        list << point.x() << point.y();
    return list;
}

QList<QPointF> GainCurve::pointsFromVariant(const QVariant &value)
{
    QList<QPointF> points;
    const QVariantList list = value.toList();
    for (int i = 0; i + 1 < list.size(); i += 2)
        points << QPointF(list[i].toDouble(), list[i + 1].toDouble());
    return points;
}

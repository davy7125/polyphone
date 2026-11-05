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

#ifndef SAMPLEUTILS_H
#define SAMPLEUTILS_H

#include "basetypes.h"
#include <complex>
#include <functional>
#include <vector>

// Fast Fourier transform (radix 2, double precision), the twiddle factors being computed once for the repeated
// transforms of a same size
class FourierTransform
{
public:
    explicit FourierTransform(int size); // Power of 2
    int getSize() const { return _size; }

    // In-place transform of getSize() values, the inverse transform being divided by the size
    void transform(std::vector<std::complex<double> > &data, bool inverse) const;

private:
    int _size;
    std::vector<std::complex<double> > _twiddles; // exp(-2 i pi k / size) for k < size / 2
};

class SampleUtils
{
public:
    enum NoiseReference
    {
        NOISE_UNKNOWN,
        NOISE_BEFORE_ONSET,
        NOISE_AT_END
    };

    static QVector<float> int24ToFloat(const qint16 * data16, const quint8 * data24, quint32 length);
    static void floatToInt24(const QVector<float> &data, qint16 *& data16, quint8 *& data24);
    static QVector<float> resampleMono(const QVector<float> &vData, double echInit, double echFinal);
    static void fillSincTable(float * table, int order, int subdivisions, double kaiserBesserBeta);
    // Low pass and / or high pass filter (frequency <= 0 if not used), Butterworth of the given order or brick wall if -1
    static QVector<float> bandFilter(const QVector<float> &vData, double dwSmplRate, double lowPassFrequency,
                                     double highPassFrequency, int order);
    static QVector<float> cutFilter(const QVector<float> &vData, quint32 dwSmplRate, const QVector<float> &dValues, int maxFreq);
    static QVector<float> EQ(const QVector<float> &vData, quint32 dwSmplRate, const QVector<int> &eqGains);
    static QVector<float> getFourierTransform(const QVector<float> &input);

    // These functions modify vData and return it: call them with std::move(vData) when the result replaces it, so that
    // the data is not copied
    static QVector<float> normalize(QVector<float> vData, float dVal, float &db);
    static QVector<float> multiply(QVector<float> vData, float dMult, float &db);
    static QVector<float> loopStep2(QVector<float> vData, quint32 loopStart, quint32 loopEnd, quint32 loopCrossfadeLength, bool withTrimEnd);
    static QVector<float> trimStart(QVector<float> vData, quint32 pos); // Unchanged if less than 8 values would remain

    // Add other multiplied by gain to data (on the length of the shortest)
    static void add(QVector<float> &data, const QVector<float> &other, double gain);

    // Length values read from the beginning of the data, repeating the loop if any (otherwise completed with zeros)
    static QVector<float> readWithLoop(const QVector<float> &vData, quint32 loopStart, quint32 loopEnd, quint32 length);

    // Same rule as the analysis (SoundAnalyzer): loop of at least 3 values inside the data
    static bool isLooped(quint32 loopStart, quint32 loopEnd, quint32 length);

    // Start of the sound: first value above -50 dB relative to the peak, or 10 dB above the peaks of the noise when the
    // noise floor can be measured (before the sound, otherwise at the end of a sound that is not looped). 0 if not found
    // noiseFloorDb: RMS level of the noise in dBFS, -200 if unknown
    static quint32 findOnset(const QVector<float> &vData, quint32 dwSmplRate, bool looped, double &noiseFloorDb,
                             NoiseReference * noiseReference = nullptr);

    // Position for removing the beginning of a sample: its onset moved back to the zero crossing before (10 ms maximum), so
    // that the remaining data does not start with a jump
    static quint32 findTrimStart(const QVector<float> &vData, quint32 dwSmplRate, bool looped);

    // Steady part of a sound (found by a segmentation of its envelope in straight lines), return false if there is
    // none (decaying or percussive sound): the positions are then the most stable part after the attack
    static bool findSteadyPart(const QVector<float> &fData, quint32 dwSmplRate, quint32 &posStart, quint32 &posEnd);
    static bool loopStep1(const QVector<float> &vData, quint32 dwSmplRate, quint32 &loopStart, quint32 &loopEnd, quint32 &loopCrossfadeLength);
    static QList<quint32> findMax(const QVector<float> &vectData, int maxNb, float minFrac = 0);
    static int lastLettersToRemove(QString str1, QString str2);

    // Quick and rough estimation of the error at a loop point, comparing the waveforms on a few values before (and after
    // if bipolar) the loop points. Blind to the level or timbre changes left by a crossfade: see SoundAnalyzer::getLoop
    // for a full analysis.
    // - if the result is < 0.05 it can be considered as OK
    // - if > 0.150 => you will probably hear the loop point
    // if maxValue is -1, the algorithm will compute it
    // if checknumber is high, the accuracy is best but it needs more samples to compute
    static float quickLoopError(const QVector<float> &vData, quint32 loopStart, quint32 loopEnd, quint32 checkNumber, bool bipolar, float maxValue = -1);

    // Levels
    static float peak(const float * data, quint32 length); // Maximum absolute value
    static void limitToFullScale(QVector<float> &vData); // Attenuation if the peak is above 1
    static float rms(const float * data, quint32 length);
    // Cumulative sums of the squares (size + 1 values): the RMS of any part is then computed in constant time
    static QVector<double> cumulativeSquares(const QVector<float> &vData);
    static double rms(const QVector<double> &cumulativeSquares, int start, int end); // end excluded
    static QVector<double> rmsEnvelope(const QVector<double> &cumulativeSquares, int window, int hop, int start = 0, int end = -1);
    static double toDb(double amplitude, double floor = -200);

    // Pitch: key 69 = A4 = 440 Hz
    static double keyToFrequency(double key);
    static double frequencyToKey(double frequency);
    // Closest key of a fractional key, with the pitch correction in cents compensating the difference (as the correction
    // of a sample: a sound a little sharp needs a negative correction)
    static void toKeyAndCorrection(double key, int &closestKey, int &correction);

    // Statistics
    static double percentile(QVector<double> values, double fraction); // Interpolated
    static double median(const QVector<double> &values);
    // Weighted least squares y = slope * x + intercept (not weighted if w is empty), return false if not enough points
    static bool linearRegression(const QVector<double> &x, const QVector<double> &y, const QVector<double> &w,
                                 double &slope, double &intercept);

    static int nextPowerOfTwo(int n); // Power of 2 greater than or equal to n (at least 1)

    static QByteArray compressSample(qint16* data16, quint32 sampleLength, quint32 sampleRate, double oggQuality);

private:
    // Values completed with zeros up to a power of 2
    static std::vector<std::complex<double> > toComplex(const QVector<float> &fData);
    // Real part of the first values, attenuated if the peak is above 1
    static QVector<float> toReal(const std::vector<std::complex<double> > &cpxData, int size);
    // Signal transformed in the frequency domain, each component multiplied by gain(frequency, module, maximum module of
    // the spectrum), and transformed back
    static QVector<float> filterSpectrum(const QVector<float> &vData, double sampleRate,
                                         const std::function<double(double, double, double)> &gain);
    static double gainEQ(double freq, const QVector<int> &eqGains);
    static QVector<quint32> loopErrorOffsets(quint32 checkNumber);
    static float loopError(const float * data, quint32 length, quint32 loopStart, quint32 loopEnd,
                           const QVector<quint32> &offsets, bool bipolar, float maxValue);
};

#endif // SAMPLEUTILS_H

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

#ifndef SOUNDANALYZER_H
#define SOUNDANALYZER_H

#include <QVector>
#include <QJsonObject>
#include <QImage>
#include <QMetaType>
#include <map>
#include "sampleutils.h"

// Peak of a spectrum: level relative to the strongest peak, frequency, closest key and pitch correction (cents)
class Peak
{
public:
    Peak() :
        _factor(0),
        _frequency(0),
        _key(-1),
        _correction(0)
    {}

    Peak(double factor, double frequency, int key, int correction) :
        _factor(factor),
        _frequency(frequency),
        _key(key),
        _correction(correction)
    {}

    double _factor;
    double _frequency;
    int _key;
    int _correction;
};
Q_DECLARE_METATYPE(Peak)

// Measures on a mono signal (sample, rendered note), giving an AI objective data about a sound: regions, levels,
// loudness, pitch, harmonics, partials, spectrum, envelope, loop and quality measures.
// Conventions: dBFS with 0 dBFS = full scale peak (a full scale sine has an RMS of -3 dBFS), keys with 60 = C4,
// loop end = first frame after the loop (as in the soundfont format)
class SoundAnalyzer
{
public:
    // The expected frequency (0 if unknown) restricts the pitch search around it (one fifth), avoiding octave errors
    SoundAnalyzer(const QVector<float> &data, quint32 sampleRate, quint32 loopStart = 0, quint32 loopEnd = 0,
                  double expectedFrequency = 0);

    // Second channel of a stereo signal (same length), to give before any measure: its energy is added to the
    // spectra, the harmonics, the partials, the levels (mean power of the channels) and the loudness (ITU-R BS.1770: sum
    // of the channels), whereas the regions, the pitch and the envelope are measured on the first channel only
    void setSecondChannel(const QVector<float> &data);

    // Groups of descriptors
    QJsonObject getRegions();
    QJsonObject getLevels();
    QJsonObject getPitch();
    QJsonObject getHarmonics(int maxListed = 16);
    QJsonObject getSpectrum();
    QJsonObject getEnvelope();
    QJsonObject getLoop();
    QJsonObject getQuality();

    // Spectrum curves: third-octave levels and spectral envelope of the steady part
    QJsonObject getSpectrumCurves();

    // Partial of a sound (peak of its spectrum)
    struct Partial
    {
        double frequency; // Hz
        double levelDb; // dBFS, amplitude of the sine
        double bandHz; // Half-width of the band in which the level is followed over time
        double variationDb; // Range of the level over time (5th to 95th percentile), NaN if not measured
        double beatRate; // Hz of the periodic modulation of the level standing out, 0 if none, NaN if not measured
        double beatDepthDb; // Peak to peak
    };

    // Partials of the sustained part (from the end of the attack to the end of the data, 10 s at most): the maxCount
    // strongest peaks standing 10 dB above their neighborhood and at most 90 dB below the strongest one, sorted by
    // frequency; the part analyzed is returned in seconds
    QList<Partial> getPartials(int maxCount, double &startSeconds, double &endSeconds);

    // Harmonic of the steady part
    struct Harmonic
    {
        int rank; // 1 = fundamental
        double frequency; // Hz
        double amplitude; // Linear, relative to full scale (0 if there is no peak at this rank)
        bool found; // The peak stands out from its neighborhood
    };

    // Harmonics of the steady part (mean power of the channels if there are two), from the rank 1 to the last one
    // below 16 kHz (60 at most, see setHarmonicLimits), empty if no pitch is found
    QList<Harmonic> getHarmonicList();

    // Limits of the harmonics measured, to give before any measure: maximum number of ranks (60 by default) and highest
    // frequency (16 kHz by default), the harmonics staying below 95% of the half of the sample rate
    void setHarmonicLimits(int maxCount, double maxFrequency)
    {
        _maxHarmonicCount = qMax(1, maxCount);
        _maxHarmonicFrequency = maxFrequency;
    }

    // Pitch over the whole signal (first channel), searched as for getPitch: a frequency in Hz every hop, 0 where no
    // pitch is found; firstTime is the time of the first value (center of its window)
    QVector<double> getPitchTrack(double hopSeconds, double &firstTime);

    // Expected frequency to give to the analyzer of a sample whose root key gives the frequency "expected": this one,
    // unless the harmonics found show that the sound is n times higher (only the ranks multiple of n stand out, as for
    // the organ stops of 4', 2' or the mutations whose root key is the key played), or the pitch searched without prior
    // if none is found around it
    static double adjustExpectedFrequency(const QVector<float> &data, quint32 sampleRate, quint32 loopStart,
                                          quint32 loopEnd, double expected);

    // Fast estimation of the pitch (Hz, 0 if not found) without prior, on a region analyzed as a loop, with the
    // confidence of the estimation between 0 and 1
    static double estimatePitch(const QVector<float> &data, quint32 sampleRate, quint32 start, quint32 end, float &confidence);

    // Main peaks of the spectrum of the loop (start / end), otherwise of the steady part (0.5 s), with the estimation of
    // the root key and of its correction (pitch correction of the sample, in cents); spectrum is the magnitude of the
    // Fourier transform and spectrumMax the index of its maximum (-1 if no peak)
    static QList<Peak> computePeaks(const QVector<float> &data, quint32 sampleRate, quint32 start, quint32 end,
                                    QVector<float> &spectrum, int &spectrumMax,
                                    int * key = nullptr, int * correction = nullptr, float * reliance = nullptr);

    // Release of a rendered note: level at the release relative to the loudest part before (RMS on 20 ms windows) and
    // times for the level to drop by 20, 40 and 60 dB after the release
    static QJsonObject getRelease(const QVector<float> &data, quint32 sampleRate, int releasePosition);

    // Rounded number for a JSON result, null if not finite
    static QJsonValue jsonNumber(double value, int decimals);

    // Comparison of a sound with a reference (differences are "other - reference")
    static QJsonObject compare(SoundAnalyzer &reference, SoundAnalyzer &other);

    // Stereo image of two channels, measured between two frames
    static QJsonObject getStereo(const QVector<float> &left, const QVector<float> &right, quint32 sampleRate,
                                 quint32 start, quint32 end);

    // Images: spectrogram (logarithmic frequencies) and waveform around the loop point
    QImage getSpectrogram(int width, int height);
    QImage getLoopSeamImage(int width, int height);

    // Single values (keyboard consistency)
    // Maximum number of frames for the pitch track (300 by default) and the averaged spectra (48 by default), fewer for
    // a fast estimation
    void setMaxPitchFrames(int count) { _maxPitchFrames = qMax(1, count); }
    void setMaxSpectrumFrames(int count) { _maxSpectrumFrames = qMax(1, count); }

    bool isLooped();
    double getFundamental(); // Hz, 0 if not found
    double getSteadyLoudness(); // LUFS (mono)
    double getMomentaryMaxLoudness(); // LUFS (mono)
    double getSpectralCentroid(); // Hz
    double getHarmonicCentroid(); // In harmonic ranks
    double getHarmonicTilt(); // dB per octave
    double getHnr(); // dB
    double getInharmonicity(); // Coefficient B of the stretched partials (f_n = n f0 sqrt(1 + B n^2)), 0 if harmonic
    double getAttackTime(); // s
    double getLoopCorrelation(); // 1 = perfect seam, NaN if not looped
    int harmonicCount();
    double harmonicEnvelopeDb(double frequency); // dB relative to the strongest harmonic, NaN outside the harmonics
    double getDuration() { return static_cast<double>(_length) / _sampleRate; }
    QVector<double> getThirdOctaveLevels(); // dB, bands of getThirdOctaveFrequencies()
    static QVector<double> getThirdOctaveFrequencies();

private:
    // Regions and envelopes
    void computeRegions();
    double rms(int start, int end) const { return SampleUtils::rms(_cumSquares, start, end); }
    QVector<double> rmsEnvelope(int windowLength, int hop, int start = 0, int end = -1) const
    {
        return SampleUtils::rmsEnvelope(_cumSquares, windowLength, hop, start, end);
    }

    // Spectra
    const FourierTransform &getFourierTransform(int size) const; // Kept for the next transforms of the same size
    QVector<double> averagePowerSpectrum(int start, int end, int frameLength, int fftSize, bool blackmanHarris,
                                         double &windowSum, double &windowSquareSum) const;
    void computeSpectrum();
    void computeHarmonics();
    QVector<double> bandLevels(const QVector<double> &power, int fftSize, const QVector<double> &centers, double bandWidthOctave) const;

    // Pitch
    double yin(int start, int length, double fMin, double fMax, double &confidence) const;
    void getPitchRange(double &fMin, double &fMax) const;
    void computePitch();

    // Levels
    double truePeak(const QVector<double> &data) const;
    QVector<double> kWeighted(const QVector<double> &data) const;
    void computeLoudness();
    double meanSquare(int start, int end) const; // Mean of the channels

    // Loop
    void computeLoop();
    double lpcClickScore() const;

    QVector<float> _source; // Shared with the caller (implicit sharing)
    QVector<double> _data;
    QVector<double> _cumSquares; // Cumulative sum of the squares
    QVector<double> _data2, _cumSquares2; // Possible second channel
    quint32 _sampleRate;
    int _length;
    int _loopStart, _loopEnd;
    bool _looped;
    double _expectedFrequency;
    int _maxPitchFrames;
    int _maxSpectrumFrames;
    int _maxHarmonicCount;
    double _maxHarmonicFrequency;
    mutable std::map<int, FourierTransform> _fourierTransforms;

    // Regions (frames)
    bool _regionsComputed;
    double _peak;
    int _peakPosition;
    int _onset, _attackStart, _attackEnd, _steadyStart, _steadyEnd;
    bool _steadyPartFound;
    double _noiseFloorDb;
    QString _noiseReference;

    // Pitch
    bool _pitchComputed;
    double _f0, _f0Confidence, _driftCentsPerSecond, _stabilityCents, _vibratoRate, _vibratoDepth;
    int _pitchFrames;

    // Harmonics
    bool _harmonicsComputed;
    QList<Harmonic> _harmonics;
    double _inharmonicity, _refinedF0, _hnr;
    QVector<double> _harmonicPower;
    int _harmonicFftSize;
    double _harmonicWindowSum;

    // Spectrum (Welch, steady part)
    bool _spectrumComputed;
    QVector<double> _power;
    int _fftSize;
    double _windowSum, _windowSquareSum;
    double _centroid, _spread, _skewness, _kurtosis, _rolloff85, _rolloff95, _flatness, _flatnessHigh, _tilt;

    // Loudness
    bool _loudnessComputed;
    double _steadyLoudness, _momentaryMax, _integratedLoudness;

    // Loop
    bool _loopComputed;
    QJsonObject _loopResult;
    double _loopCorrelation;
};

#endif // SOUNDANALYZER_H

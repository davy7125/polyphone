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

// Measures on a mono signal (sample, rendered note), giving an AI objective data about a sound: regions, levels,
// loudness, pitch, harmonics, spectrum, envelope, loop quality and defects.
// Conventions: dBFS with 0 dBFS = full scale peak (a full scale sine has an RMS of -3 dBFS), keys with 60 = C4,
// loop end = first frame after the loop (as in the soundfont format)
class SoundAnalyzer
{
public:
    // The expected frequency (0 if unknown) restricts the pitch search around it (one fifth), avoiding octave errors
    SoundAnalyzer(const QVector<float> &data, quint32 sampleRate, quint32 loopStart = 0, quint32 loopEnd = 0,
                  double expectedFrequency = 0);

    // Groups of descriptors
    QJsonObject getRegions();
    QJsonObject getLevels();
    QJsonObject getPitch();
    QJsonObject getHarmonics(int maxListed = 16);
    QJsonObject getSpectrum();
    QJsonObject getEnvelope();
    QJsonObject getLoop();
    QJsonObject getQuality();

    // Spectrum curves for equalization decisions: third-octave levels and spectral envelope of the steady part
    QJsonObject getSpectrumCurves();

    // Comparison of a sound with a reference (differences are "other - reference")
    static QJsonObject compare(SoundAnalyzer &reference, SoundAnalyzer &other);

    // Stereo image of two channels, measured between two frames
    static QJsonObject getStereo(const QVector<float> &left, const QVector<float> &right, quint32 sampleRate,
                                 quint32 start, quint32 end);

    // Images: spectrogram (logarithmic frequencies) and waveform around the loop point
    QImage getSpectrogram(int width, int height);
    QImage getLoopSeamImage(int width, int height);

    // Single values (keyboard consistency)
    bool isLooped();
    double getFundamental(); // Hz, 0 if not found
    double getSteadyLoudness(); // LUFS (mono)
    double getMomentaryMaxLoudness(); // LUFS (mono)
    double getSpectralCentroid(); // Hz
    double getHarmonicCentroid(); // In harmonic ranks
    double getHarmonicTilt(); // dB per octave
    double getHnr(); // dB
    double getAttackTime(); // s
    double getLoopCorrelation(); // 1 = perfect seam, NaN if not looped
    int harmonicCount();
    double harmonicEnvelopeDb(double frequency); // dB relative to the strongest harmonic, NaN outside the harmonics
    double getDuration() { return static_cast<double>(_length) / _sampleRate; }
    QVector<double> getThirdOctaveLevels(); // dB, bands of getThirdOctaveFrequencies()
    static QVector<double> getThirdOctaveFrequencies();

private:
    struct Harmonic
    {
        int rank;
        double frequency;
        double amplitude; // Linear, relative to full scale
        bool found;
    };

    // Regions and envelopes
    void computeRegions();
    double rms(int start, int end) const;
    QVector<double> rmsEnvelope(int windowLength, int hop, int start = 0, int end = -1) const;
    static double toDb(double value, double floor = -200);

    // Spectra
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
    double truePeak() const;
    QVector<double> kWeighted() const;
    void computeLoudness();

    // Loop
    void computeLoop();
    double lpcClickScore() const;

    QVector<double> _data;
    QVector<double> _cumSquares; // Cumulative sum of the squares
    quint32 _sampleRate;
    int _length;
    int _loopStart, _loopEnd;
    bool _looped;
    double _expectedFrequency;

    // Regions (frames)
    bool _regionsComputed;
    double _peak;
    int _peakPosition;
    int _onset, _attackStart, _attackEnd, _steadyStart, _steadyEnd;
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

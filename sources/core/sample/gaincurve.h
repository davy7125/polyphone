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

#ifndef GAINCURVE_H
#define GAINCURVE_H

#include <QList>
#include <QPointF>
#include <QVector>
#include <QVariantList>
#include <vector>

// Level in dB as a function of the frequency, given by points interpolated on a logarithmic scale of the abscissa and
// constant beyond the first and the last points. The abscissa is a frequency in Hz or, with the note tracking, a ratio
// to the fundamental frequency of each sample (1: fundamental, 2: octave, 3: twelfth...).
// Shared by the tool "filter frequencies" and the equalizer of the sample page, so that the preview of the equalizer
// is the data that will be written.
class GainCurve
{
public:
    enum Mode
    {
        // Maximum level of the components relative to the strongest frequency of the sample (0 dB: nothing removed,
        // CEILING_REMOVED or less: removed). Zero phase, applied on the whole spectrum of the sample.
        MODE_CEILING = 0,

        // Gain in dB (MIN_GAIN to MAX_GAIN), applied with a minimum phase response (causal as an analog or acoustic
        // filter: no pre-echo before the attacks), the loop being processed as if it were played endlessly
        MODE_GAIN = 1
    };

    static const double MIN_GAIN;        // -60 dB
    static const double MAX_GAIN;        // +24 dB
    static const double CEILING_REMOVED; // -80 dB

    GainCurve(); // Gain of 0 dB, in Hz
    GainCurve(Mode mode, bool noteTracking, const QList<QPointF> &points);

    Mode getMode() const { return _mode; }
    bool isNoteTracking() const { return _noteTracking; }

    // Points with a positive abscissa, sorted (several points at the same abscissa: step)
    const QList<QPointF> &getPoints() const { return _points; }

    // Level in dB at an abscissa (Hz or ratio), within the limits of the mode
    double getDb(double x) const;

    // True if the processing changes nothing (every point at 0 dB)
    bool isNeutral() const;

    // Fundamental frequency of a sample: its root key (60 if not a key) shifted by its pitch correction in cents, the
    // correction compensating the pitch (a sound a little sharp has a negative correction)
    static double getFundamental(int rootKey, int pitchCorrection);

    // Process the data of a sample. The fundamental is used with the note tracking. The loop (if loopStart < loopEnd
    // <= size, end excluded) stays seamless in gain mode. If the peak of the result is above the full scale, the
    // result is attenuated to the full scale. peakBefore and peakAfter (if not null) receive the peaks of the data
    // and of the result before this attenuation (1 = full scale).
    QVector<float> apply(const QVector<float> &data, double sampleRate, double fundamental, quint32 loopStart,
                         quint32 loopEnd, double * peakBefore = nullptr, double * peakAfter = nullptr) const;

    // Storage of the points in the configuration: x1, dB1, x2, dB2...
    QVariantList pointsToVariant() const;
    static QList<QPointF> pointsFromVariant(const QVariant &value);

private:
    QVector<float> applyCeiling(const QVector<float> &data, double sampleRate, double xPerHz) const;
    QVector<float> applyGain(const QVector<float> &data, double sampleRate, double xPerHz, quint32 loopStart,
                             quint32 loopEnd) const;
    std::vector<double> minimumPhaseResponse(double sampleRate, double xPerHz) const;
    static std::vector<double> convolve(const std::vector<double> &signal, const std::vector<double> &response,
                                        size_t outputLength);

    Mode _mode;
    bool _noteTracking;
    QList<QPointF> _points;
};

#endif // GAINCURVE_H

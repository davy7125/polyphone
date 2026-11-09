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

#ifndef GRAPHFILTERFREQUENCIES_H
#define GRAPHFILTERFREQUENCIES_H

#include <QWidget>
#include <QPen>
#include <QPolygonF>
#include "gaincurve.h"

// Curve of the tool "filter frequencies", drawn with the mouse on a logarithmic scale of frequencies (20 Hz to 20 kHz)
// or, with the note tracking, of ratios to the fundamental frequency of each sample (x1/4 to x256). The spectra of the
// samples are drawn behind with the same scale in dB, their strongest component being at 0 dB.
class GraphFilterFrequencies : public QWidget
{
    Q_OBJECT

public:
    explicit GraphFilterFrequencies(QWidget *parent = nullptr);

    // Spectra: modules from 0 to the Nyquist frequency, and fundamental frequency of the sample
    void clearFourierTransforms();
    void addFourierTransform(const QVector<float> &fData, quint32 sampleRate, double fundamental);

    // Fundamental frequencies of all samples to process, for displaying the frequencies with the note tracking
    void setFundamentals(const QList<double> &fundamentals);

    void setMode(GainCurve::Mode mode, bool noteTracking);

    // Points of the curve, resampled on the grid of the graph
    void setPoints(const QList<QPointF> &points);
    QList<QPointF> getPoints() const;

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    struct Spectrum
    {
        QVector<float> db; // Relative to the strongest module
        double binWidth;   // Hz
        double fundamental;
        QPolygonF shape;   // For the current size and abscissa
    };

    double getMinX() const;
    double indexToX(double index) const;
    float indexToPos(double index) const;
    double posToIndex(double pos) const;
    double getMinDb() const;
    double getMaxDb() const;
    float dbToPos(double db) const;
    double posToDb(double pos) const;
    QString formatFrequency(double frequency) const;
    QString getHoverText() const;
    void computeShapes();
    void write(QPoint pos);

    GainCurve::Mode _mode;
    bool _noteTracking;
    QVector<double> _values; // dB on the grid
    QList<Spectrum> _spectra;
    QList<double> _fundamentals;

    // Edition with the mouse
    bool _isEditing;
    int _previousIndex;
    double _previousDb;
    int _currentIndex; // Point under the mouse, -1 if none

    // Style
    QString _hzUnit, _kHzUnit, _dbUnit;
    QColor _backgroundColor, _labelColor, _spectrumColor, _ceilingAreaColor, _gainAreaColor;
    QPen _gridPen, _minorGridPen, _zeroPen, _ceilingPen, _gainPen, _currentPointPen;
    QFont _fontLabels;

    static const int POINTS_PER_OCTAVE;
    static const int OCTAVES;
};

#endif // GRAPHFILTERFREQUENCIES_H

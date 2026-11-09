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

#include "graphfilterfrequencies.h"
#include "contextmanager.h"
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QLocale>
#include <algorithm>
#include <cmath>

const int GraphFilterFrequencies::POINTS_PER_OCTAVE = 24;
const int GraphFilterFrequencies::OCTAVES = 10;

static const QChar TIMES(0x00D7);

GraphFilterFrequencies::GraphFilterFrequencies(QWidget * parent) : QWidget(parent),
    _mode(GainCurve::MODE_CEILING),
    _noteTracking(false),
    _values(POINTS_PER_OCTAVE * OCTAVES + 1, 0.0),
    _isEditing(false),
    _previousIndex(-1),
    _previousDb(0),
    _currentIndex(-1)
{
    // Colors and pens
    _backgroundColor = ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND);
    QColor red = ContextManager::theme()->getFixedColor(ThemeManager::RED, ThemeManager::LIST_BACKGROUND);
    QColor green = ContextManager::theme()->getFixedColor(ThemeManager::GREEN, ThemeManager::LIST_BACKGROUND);
    _ceilingPen = QPen(red, 2);
    _gainPen = QPen(green, 2);
    _ceilingAreaColor = red;
    _ceilingAreaColor.setAlpha(30);
    _gainAreaColor = green;
    _gainAreaColor.setAlpha(40);
    _spectrumColor = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);
    QColor color = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT);
    _currentPointPen = QPen(color, 2, Qt::SolidLine, Qt::RoundCap);
    _labelColor = color;
    _labelColor.setAlpha(180);
    color.setAlpha(110);
    _zeroPen = QPen(color, 1);
    color.setAlpha(50);
    _gridPen = QPen(color, 1);
    color.setAlpha(25);
    _minorGridPen = QPen(color, 1);

    // Fonts
    _fontLabels = QFont(font().family(), 8);

    // Translation
    _hzUnit = tr("Hz", "unit for Herz");
    _kHzUnit = tr("kHz", "unit for kilo Herz");
    _dbUnit = tr("dB", "unit for decibels");

    // Catch all mouse move events
    this->setMouseTracking(true);
}

void GraphFilterFrequencies::clearFourierTransforms()
{
    _spectra.clear();
    this->update();
}

void GraphFilterFrequencies::addFourierTransform(const QVector<float> &fData, quint32 sampleRate, double fundamental)
{
    if (fData.isEmpty() || sampleRate == 0)
        return;

    // Levels relative to the strongest module
    float max = 0;
    for (float value : fData)
        max = qMax(max, value);
    Spectrum spectrum;
    spectrum.binWidth = 0.5 * sampleRate / fData.size();
    spectrum.fundamental = fundamental;
    spectrum.db.resize(fData.size());
    for (int i = 0; i < fData.size(); i++)
        spectrum.db[i] = (max > 0 && fData[i] > 0) ? 20.f * std::log10(fData[i] / max) : -200.f;
    _spectra << spectrum;

    computeShapes();
    this->update();
}

void GraphFilterFrequencies::setFundamentals(const QList<double> &fundamentals)
{
    _fundamentals = fundamentals;
}

void GraphFilterFrequencies::setMode(GainCurve::Mode mode, bool noteTracking)
{
    _mode = mode;
    _noteTracking = noteTracking;
    computeShapes();
    this->update();
}

void GraphFilterFrequencies::setPoints(const QList<QPointF> &points)
{
    GainCurve curve(_mode, _noteTracking, points);
    for (int i = 0; i < _values.size(); i++)
        _values[i] = qBound(getMinDb(), points.isEmpty() ? 0.0 : curve.getDb(indexToX(i)), getMaxDb());
    this->update();
}

QList<QPointF> GraphFilterFrequencies::getPoints() const
{
    QList<QPointF> points;
    for (int i = 0; i < _values.size(); i++)
        points << QPointF(indexToX(i), _values[i]);
    return points;
}

double GraphFilterFrequencies::getMinX() const
{
    return _noteTracking ? 0.25 : 20.0;
}

double GraphFilterFrequencies::indexToX(double index) const
{
    return getMinX() * std::pow(2.0, index / POINTS_PER_OCTAVE);
}

float GraphFilterFrequencies::indexToPos(double index) const
{
    return static_cast<float>(index / (_values.size() - 1) * (this->width() - 1));
}

double GraphFilterFrequencies::posToIndex(double pos) const
{
    return pos / qMax(1, this->width() - 1) * (_values.size() - 1);
}

double GraphFilterFrequencies::getMinDb() const
{
    return _mode == GainCurve::MODE_GAIN ? GainCurve::MIN_GAIN : GainCurve::CEILING_REMOVED;
}

double GraphFilterFrequencies::getMaxDb() const
{
    return _mode == GainCurve::MODE_GAIN ? GainCurve::MAX_GAIN : 0.0;
}

float GraphFilterFrequencies::dbToPos(double db) const
{
    db = qBound(getMinDb(), db, getMaxDb());
    return static_cast<float>((getMaxDb() - db) / (getMaxDb() - getMinDb()) * (this->height() - 1));
}

double GraphFilterFrequencies::posToDb(double pos) const
{
    return getMaxDb() - pos / qMax(1, this->height() - 1) * (getMaxDb() - getMinDb());
}

QString GraphFilterFrequencies::formatFrequency(double frequency) const
{
    if (frequency < 1000)
        return QLocale::system().toString(frequency, 'f', frequency < 100 ? 1 : 0) + " " + _hzUnit;
    return QLocale::system().toString(frequency / 1000, 'f', frequency < 10000 ? 2 : 1) + " " + _kHzUnit;
}

QString GraphFilterFrequencies::getHoverText() const
{
    double x = indexToX(_currentIndex);
    QString text;
    if (_noteTracking)
    {
        // Ratio, and the frequencies it gives for the samples to process
        text = TIMES + QLocale::system().toString(x, 'f', x < 10 ? 2 : 1);
        if (!_fundamentals.isEmpty())
        {
            double minFrequency = x * *std::min_element(_fundamentals.begin(), _fundamentals.end());
            double maxFrequency = x * *std::max_element(_fundamentals.begin(), _fundamentals.end());
            text += " (" + formatFrequency(minFrequency);
            if (maxFrequency > 1.001 * minFrequency)
                text += QString::fromUtf8(" – ") + formatFrequency(maxFrequency);
            text += ")";
        }
    }
    else
        text = formatFrequency(x);

    double db = _values[_currentIndex];
    text += " | " + QString(db > 0.05 ? "+" : "") + QLocale::system().toString(db, 'f', 1) + " " + _dbUnit;
    return text;
}

void GraphFilterFrequencies::computeShapes()
{
    // Strongest bin within each column of pixels, or the closest bin if a column covers less than a bin
    int width = this->width();
    double bottom = this->height() + 1;
    for (Spectrum &spectrum : _spectra)
    {
        spectrum.shape.clear();
        spectrum.shape << QPointF(-1, bottom);
        double frequencyScale = _noteTracking ? spectrum.fundamental : 1.0;
        int count = spectrum.db.size();
        for (int pos = 0; pos < width; pos++)
        {
            double lowFrequency = indexToX(posToIndex(pos - 0.5)) * frequencyScale;
            double highFrequency = indexToX(posToIndex(pos + 0.5)) * frequencyScale;
            int lowBin = qMax(0, static_cast<int>(std::ceil(lowFrequency / spectrum.binWidth)));
            int highBin = qMin(count - 1, static_cast<int>(std::floor(highFrequency / spectrum.binWidth)));
            double db = -200;
            if (lowBin > highBin)
            {
                int bin = qRound(0.5 * (lowFrequency + highFrequency) / spectrum.binWidth);
                if (bin < count)
                    db = spectrum.db[bin];
            }
            else
            {
                for (int bin = lowBin; bin <= highBin; bin++)
                    db = qMax(db, static_cast<double>(spectrum.db[bin]));
            }
            spectrum.shape << QPointF(pos, dbToPos(db));
        }
        spectrum.shape << QPointF(width, bottom);
    }
}

void GraphFilterFrequencies::write(QPoint pos)
{
    int index = qBound(0, qRound(posToIndex(pos.x())), _values.size() - 1);
    double db = qBound(getMinDb(), posToDb(pos.y()), getMaxDb());
    if (_mode == GainCurve::MODE_GAIN && qAbs(db) < 0.75)
        db = 0; // Neutral gain easy to draw

    // Values between the previous point and this one
    if (_previousIndex >= 0 && _previousIndex != index)
    {
        int step = index > _previousIndex ? 1 : -1;
        for (int i = _previousIndex + step; i != index; i += step)
            _values[i] = _previousDb + (db - _previousDb) * (i - _previousIndex) / (index - _previousIndex);
    }
    _values[index] = db;

    _previousIndex = index;
    _previousDb = db;
}

void GraphFilterFrequencies::mouseMoveEvent(QMouseEvent *event)
{
    if (_isEditing)
        this->write(event->pos());
    _currentIndex = qBound(0, qRound(posToIndex(event->pos().x())), _values.size() - 1);
    this->update();
}

void GraphFilterFrequencies::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;

    _isEditing = true;
    _previousIndex = -1;
    this->write(event->pos());
    this->update();
}

void GraphFilterFrequencies::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton || !_isEditing)
        return;

    _isEditing = false;
    _previousIndex = -1;
    this->update();
}

void GraphFilterFrequencies::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    _currentIndex = -1;
    this->update();
}

void GraphFilterFrequencies::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    computeShapes();
}

void GraphFilterFrequencies::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QRect rect = this->rect();
    painter.fillRect(rect, _backgroundColor);
    painter.setFont(_fontLabels);
    QFontMetrics fm(_fontLabels);

    // Spectra
    if (!_spectra.isEmpty())
    {
        QColor color = _spectrumColor;
        color.setAlpha(qMax(40, 200 / _spectra.count()));
        painter.setPen(Qt::NoPen);
        painter.setBrush(color);
        for (const Spectrum &spectrum : _spectra)
            painter.drawPolygon(spectrum.shape);
    }

    // Vertical lines: frequencies, or octaves and harmonics 3, 5, 6, 7 of the fundamental
    QList<QPair<double, QString> > lines;
    QList<double> minorLines;
    if (_noteTracking)
    {
        for (int octave = 0; octave <= OCTAVES; octave++)
        {
            double ratio = getMinX() * std::pow(2.0, octave);
            lines << qMakePair(ratio, TIMES + (ratio == 0.25 ? QString(QChar(0x00BC)) :
                                               ratio == 0.5 ? QString(QChar(0x00BD)) : QString::number(ratio)));
        }
        minorLines << 3 << 5 << 6 << 7;
    }
    else
    {
        for (double frequency : { 20., 50., 100., 200., 500., 1000., 2000., 5000., 10000., 20000. })
            lines << qMakePair(frequency, frequency < 1000 ? QString::number(frequency) + " " + _hzUnit :
                                                             QString::number(frequency / 1000) + " " + _kHzUnit);
    }
    painter.setPen(_minorGridPen);
    for (double x : minorLines)
    {
        float pos = indexToPos(POINTS_PER_OCTAVE * std::log2(x / getMinX()));
        painter.drawLine(QPointF(pos, 0), QPointF(pos, rect.bottom()));
    }
    for (int i = 0; i < lines.count(); i++)
    {
        float pos = indexToPos(POINTS_PER_OCTAVE * std::log2(lines[i].first / getMinX()));
        painter.setPen(_noteTracking && lines[i].first == 1.0 ? _zeroPen : _gridPen);
        painter.drawLine(QPointF(pos, 0), QPointF(pos, rect.bottom()));
    }

    // Horizontal lines: levels in dB
    QList<double> levels;
    if (_mode == GainCurve::MODE_GAIN)
        levels << 12 << 0 << -12 << -24 << -36 << -48;
    else
        levels << -20 << -40 << -60;
    for (double level : levels)
    {
        float pos = dbToPos(level);
        painter.setPen(level == 0 ? _zeroPen : _gridPen);
        painter.drawLine(QPointF(0, pos), QPointF(rect.right(), pos));
    }

    // Area changed by the curve and the curve itself
    QPainterPath curvePath;
    for (int i = 0; i < _values.size(); i++)
    {
        QPointF point(indexToPos(i), dbToPos(_values[i]));
        if (i == 0)
            curvePath.moveTo(point);
        else
            curvePath.lineTo(point);
    }
    QPainterPath area = curvePath;
    if (_mode == GainCurve::MODE_GAIN)
    {
        // Between the curve and 0 dB
        area.lineTo(indexToPos(_values.size() - 1), dbToPos(0));
        area.lineTo(indexToPos(0), dbToPos(0));
    }
    else
    {
        // Above the curve (removed)
        area.lineTo(rect.right() + 10, dbToPos(_values.last()));
        area.lineTo(rect.right() + 10, -10);
        area.lineTo(-10, -10);
        area.lineTo(-10, dbToPos(_values.first()));
    }
    area.closeSubpath();
    painter.setPen(Qt::NoPen);
    painter.setBrush(_mode == GainCurve::MODE_GAIN ? _gainAreaColor : _ceilingAreaColor);
    painter.drawPath(area);
    painter.setBrush(Qt::NoBrush);
    painter.setPen(_mode == GainCurve::MODE_GAIN ? _gainPen : _ceilingPen);
    painter.drawPath(curvePath);

    // Labels
    painter.setPen(_labelColor);
    for (int i = 0; i < lines.count(); i++)
    {
        float pos = indexToPos(POINTS_PER_OCTAVE * std::log2(lines[i].first / getMinX()));
        int textWidth = fm.horizontalAdvance(lines[i].second);
        int left = qBound(2, static_cast<int>(pos - 0.5f * textWidth), rect.width() - textWidth - 2);
        painter.drawText(QRect(left, rect.bottom() - fm.height() - 1, textWidth, fm.height()),
                         Qt::AlignCenter, lines[i].second);
    }
    for (double level : levels)
    {
        QString text = QString(level > 0 ? "+" : "") + QString::number(level) + " " + _dbUnit;
        painter.drawText(QRectF(3, dbToPos(level) - fm.height() - 1, fm.horizontalAdvance(text) + 2, fm.height()),
                         Qt::AlignLeft | Qt::AlignVCenter, text);
    }

    // Point under the mouse
    if (_currentIndex >= 0)
    {
        float x = indexToPos(_currentIndex);
        float y = dbToPos(_values[_currentIndex]);
        painter.setPen(_currentPointPen);
        painter.drawLine(QPointF(x - 5, y), QPointF(x + 5, y));
        painter.drawLine(QPointF(x, y - 5), QPointF(x, y + 5));

        // Text centered above or below the point, within the graph
        QString label = getHoverText();
        float textHalfWidth = 0.5f * fm.horizontalAdvance(label) + 2.f;
        float textHeight = fm.height();
        float textCenterX = qBound(textHalfWidth, x, rect.width() - textHalfWidth);
        float textY = 2 * y > rect.height() ? y - 1.5f * textHeight : y + 0.5f * textHeight;
        QRectF textRect(textCenterX - textHalfWidth, textY, 2 * textHalfWidth, textHeight);
        QColor background = _backgroundColor;
        background.setAlpha(200);
        painter.fillRect(textRect, background);
        painter.drawText(textRect, Qt::AlignCenter, label);
    }
}

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
#include "graphicswavepainter.h"
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

const int GraphFilterFrequencies::POINT_NUMBER = 201;
const float GraphFilterFrequencies::MIN_Y = 0.0001;

GraphFilterFrequencies::GraphFilterFrequencies(QWidget * parent) : QWidget(parent),
    _flagEdit(false),
    _stiffnessExp(50.0),
    _previousX(-1)
{
    // Prepare data
    _dValues.resize(POINT_NUMBER);
    _dValues.fill(0.5);

    // Colors and pens
    _backgroundColor = ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND);
    QColor color = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT);
    color.setAlpha(40);
    _frequencyPen = QPen(color, 1);
    _textColor = color;
    _textColor.setAlpha(255);
    _removedAreaColor = ContextManager::theme()->getFixedColor(ThemeManager::RED, ThemeManager::LIST_BACKGROUND);
    _penCurve = QPen(_removedAreaColor, 2);
    _removedAreaColor.setAlpha(30);
    _currentPointPen = QPen(_textColor, 2, Qt::SolidLine, Qt::RoundCap);

    // Fonts
    _fontLabels = QFont(font().family(), 9, QFont::Bold);

    // Catch all mouse move events
    this->setMouseTracking(true);
}

GraphFilterFrequencies::~GraphFilterFrequencies()
{
    clearFourierTransforms();
}

void GraphFilterFrequencies::clearFourierTransforms()
{
    while (!_wavePainters.empty())
        delete _wavePainters.takeLast();
}

void GraphFilterFrequencies::addFourierTransform(QVector<float> fData, quint32 sampleRate)
{
    // Get the maximum value for a normalization
    float max = -1;
    for (int i = 0; i < fData.size(); i++)
        max = (max == -1 || max < fData[i]) ? fData[i] : max;
    if (max <= 0)
        max = 1;

    // Maximum length of the vector is 2 * 20000Hz
    int newLength = (long int)(fData.size() * 40000) / sampleRate;
    int initialLength = fData.size();
    fData.resize(newLength);
    for (int i = initialLength + 1; i < newLength; i++)
        fData[i] = 0;

    // Logarithmic scale
    for (int i = 0; i < fData.size(); i++)
    {
        float normalizedValue = fData[i] / max;
        fData[i] = normalizedValue <= MIN_Y ? 0 : (log(MIN_Y) - log(normalizedValue)) / log(MIN_Y);
    }

    GraphicsWavePainter * wavePainter = new GraphicsWavePainter();
    wavePainter->addMeanCurve(false);
    wavePainter->setData(fData);
    wavePainter->setOnlyPositiveValues(true);

    _wavePainters << wavePainter;

    // Set the color
    int nb = _wavePainters.count();
    QColor color = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);
    color.setAlpha(255 / nb);
    for (int i = 0; i < _wavePainters.count(); i++)
        _wavePainters[i]->setWaveColor(color);
}

QVector<float> GraphFilterFrequencies::getValues()
{
    return this->_dValues;
}

void GraphFilterFrequencies::setValues(QVector<float> val)
{
    for (int i = 0; i < qMin(POINT_NUMBER, val.size()); i++)
        _dValues[i] = val.at(i);
}

void GraphFilterFrequencies::write(QPoint pos)
{
    // Convert coordinates
    int x = posToFreq(pos.x());
    if (x < 0)
        x = 0;
    else if (x > POINT_NUMBER - 1)
        x = POINT_NUMBER - 1;
    float y = posToValue(pos.y());
    if (y < 0)
        y = 0;
    else if (y > 1)
        y = 1;

    // Modification valeur
    if (this->_previousX >= 0 && this->_previousX != x)
    {
        if (this->_previousX < x)
        {
            for (int i = this->_previousX + 1; i < x; i++)
                this->_dValues[i] = this->_previousY +
                                    (y - _previousY) * (i - this->_previousX) / (x - this->_previousX);
        }
        else
        {
            for (int i = this->_previousX - 1; i > x; i--)
                this->_dValues[i] = this->_previousY +
                                    (y - _previousY) * (i - this->_previousX) / (x - this->_previousX);
        }
    }
    this->_dValues[x] = y;

    // Mémorisation du point
    this->_previousX = x;
    this->_previousY = y;
}

float GraphFilterFrequencies::freqToPos(int freq)
{
    return static_cast<float>(freq) / (POINT_NUMBER - 1) * (this->width() - 1);
}

int GraphFilterFrequencies::posToFreq(int pos)
{
    int result = static_cast<int>(static_cast<float>(pos) / (this->width() - 1) * (POINT_NUMBER - 1) + 0.5f);
    if (result < 0)
        return 0;
    if (result >= POINT_NUMBER)
        return POINT_NUMBER - 1;
    return result;
}

float GraphFilterFrequencies::posToValue(int pos)
{
    float tmp = log(MIN_Y) * static_cast<float>(pos) / (this->height() - 1);
    float result = exp(tmp);
    return result < MIN_Y ? 0 : result;
}

float GraphFilterFrequencies::valueToPos(float value)
{
    if (value < MIN_Y)
        return this->height() - 1;
    float tmp = (log(MIN_Y) - log(value)) / log(MIN_Y);
    float result = (1.0 - tmp) * (this->height() - 1);
    if (result < 0.f)
        return 0.f;
    if (result >= this->height())
        return this->height() - 1;
    return result;
}

void GraphFilterFrequencies::mouseMoveEvent(QMouseEvent *event)
{
    if (this->_flagEdit)
    {
        _currentFreq = -1;
        this->write(event->pos());
    }
    else
    {
        // Convert coordinates
        QPoint pos = event->pos();
        float posX = static_cast<float>(pos.x());
        float posY = static_cast<float>(pos.y());

        // Find the closest point
        float minDist = -1;
        _currentFreq = -1;
        for (int i = 0; i < _dValues.size(); i++)
        {
            float y = valueToPos(_dValues[i]);
            float x = freqToPos(i);
            float dist = qAbs(posX - x) + qAbs(posY - y);
            if (_currentFreq == -1 || dist < minDist)
            {
                minDist = dist;
                _currentValue = _dValues[i];
                _currentFreq = i;
            }
        }
    }
    this->update();
}

void GraphFilterFrequencies::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;

    _currentFreq = -1;
    _flagEdit = true;

    // Write the first point
    this->write(event->pos());
}

void GraphFilterFrequencies::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;

    if (this->_flagEdit)
    {
        _flagEdit = false;
        _previousX = -1;
        _currentFreq = -1;
        this->update();
    }
}

void GraphFilterFrequencies::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    _currentFreq = -1;
    this->update();
}

void GraphFilterFrequencies::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Background
    QRect rect = this->rect();
    painter.fillRect(rect, _backgroundColor);

    // Fourier transforms
    for (int i = 0; i < _wavePainters.count(); i++)
        _wavePainters[i]->paint(&painter, rect, 0, 0, 1);

    // Frequencies
    painter.setFont(_fontLabels);
    for (int f = 2000; f <= 18000; f += 2000)
    {
        float x = freqToPos(static_cast<float>(f) / 20000 * (POINT_NUMBER - 1));

        painter.setPen(_frequencyPen);
        painter.drawLine(x, rect.top(), x, rect.bottom());

        painter.setPen(_textColor);
        painter.drawText(QRect(x - 200, rect.bottom() - 50, 400, 50), Qt::AlignHCenter | Qt::AlignBottom,
                         QLocale::system().toString(f / 1000) + " " + tr("kHz", "unit for kilo Herz"));
    }

    // Values
    painter.setBrush(_removedAreaColor);
    painter.setPen(_penCurve);
    QPainterPath path;
    for (int i = 0; i < _dValues.size(); i++)
    {
        float x = freqToPos(i);
        float y = valueToPos(_dValues[i]);
        if (i == 0)
            path.moveTo(- 10, y);
        path.lineTo(x, y);
        if (i == _dValues.size() - 1)
        {
            // Close the path
            path.lineTo(x + 10, y);
            path.lineTo(x + 10, -10);
            path.lineTo(- 10, -10);
            path.closeSubpath();
        }
    }
    painter.drawPath(path);

    // Current point
    painter.setPen(_currentPointPen);
    if (_currentFreq >= 0)
    {
        float x = freqToPos(_currentFreq);
        float y = valueToPos(_currentValue);

        painter.drawLine(x - 5, y, x + 5, y);
        painter.drawLine(x, y - 5, x, y + 5);

        // Text to display with its size
        float kHz = static_cast<float>(_currentFreq) / (POINT_NUMBER - 1) * 20;
        QString label = QLocale::system().toString(kHz, 'f', 1) + " " + tr("kHz", "unit for kilo Herz");
        label += " - " + QLocale::system().toString(_currentValue, 'g', 2);

        QFontMetrics fm(_fontLabels);
        float textHalfWidth = 0.5f * fm.horizontalAdvance(label) + 2.f;
        float textHeight = fm.height();

        // Display the text
        float textCenterX = x;
        if (textCenterX + textHalfWidth > rect.width())
            textCenterX = rect.width() - textHalfWidth;
        if (textCenterX - textHalfWidth < 0)
            textCenterX = textHalfWidth;
        painter.drawText(textCenterX - textHalfWidth, 2 * y > rect.height() ? y - 1.5 * textHeight : y + 0.5f * textHeight,
                         2 * textHalfWidth, textHeight, Qt::AlignVCenter | Qt::AlignHCenter, label);
    }
}

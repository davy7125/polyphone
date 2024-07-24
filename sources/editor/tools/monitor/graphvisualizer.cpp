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

#include "graphvisualizer.h"
#include "contextmanager.h"
#include "segmentpainter.h"
#include "segment.h"
#include <QPainter>


GraphVisualizer::GraphVisualizer(QWidget *parent) : QWidget(parent),
    _segmentPainter(new SegmentPainter()),
    _isLog(false),
    _dispWarning(false)
{
    // Colors
    _backgroundColor = ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND);
    _gridColor = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT);
    _gridColor.setAlpha(40);
    _textColor = _gridColor;
    _textColor.setAlpha(255);
    _curveColor = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);
    _warningColor = ContextManager::theme()->getFixedColor(ThemeManager::RED, ThemeManager::LIST_BACKGROUND);
    _segmentPainter->setColor(_curveColor);
    _defaultValueColor = _gridColor;
    _defaultValueColor.setAlpha(180);

    // Fonts
    _fontLabels = QFont(font().family(), 9, QFont::Bold);
    _fontWarning = QFont(font().family(), 10, QFont::Bold);

    // Pen
    _currentPointPen = QPen(_textColor, 2, Qt::SolidLine, Qt::RoundCap);

    // Catch all mouse move events
    this->setMouseTracking(true);
}

GraphVisualizer::~GraphVisualizer()
{
}

void GraphVisualizer::setData(float defaultValue, bool globalValueSet, QList<Segment *> segments)
{
    _defaultValue = defaultValue;
    _globalValueSet = globalValueSet;
    _segments = segments;

    // Limits on X and Y
    _xMin = -1;
    _xMax = -1;
    _fMin = 0;
    _fMax = 0;
    for (int i = 0; i < _segments.length(); i++)
    {
        if (_xMin == -1 || _segments[i]->_value < _fMin)
            _fMin = _segments[i]->_value;
        if (_xMin == -1 || _segments[i]->_value > _fMax)
            _fMax = _segments[i]->_value;
        if (_xMin == -1 || _segments[i]->_keyMax > _xMax)
            _xMax = _segments[i]->_keyMax;
        if (_xMin == -1 || _segments[i]->_keyMin < _xMin)
            _xMin = _segments[i]->_keyMin;
    }
    if (_xMin == -1)
    {
        _xMin = 0;
        _xMax = 127;
        _fMin = 0;
        _fMax = 0;
    }

    _segmentPainter->setData(_segments);
    this->setScale();
}

void GraphVisualizer::setIsLog(bool isLog)
{
    _isLog = isLog;
    this->setScale();
}

void GraphVisualizer::setScale()
{
    if (_isLog)
    {
        // Bornes sur y
        _yMin = qMax(_fMin, SegmentPainter::MIN_LOG);
        _yMax = qMax(_fMax, SegmentPainter::MIN_LOG);
        if (_yMin == _yMax)
        {
            _yMin /= 2;
            _yMax *= 2;
        }
        else
        {
            double delta = _yMax / _yMin;
            _yMin /= qPow(delta, 0.1);
            _yMax *= qPow(delta, 0.05);
        }

        _dispWarning = _fMin < _yMin;
    }
    else
    {
        // Bornes sur y
        _yMin = _fMin;
        _yMax = _fMax;
        if (_yMin == _yMax)
        {
            _yMin -= 1;
            _yMax += 1;
        }
        else
        {
            double delta = _yMax - _yMin;
            _yMin -= delta * 0.1;
            _yMax += delta * 0.05;
        }
        _dispWarning = false;
    }
    _segmentPainter->setLogarithmicScale(_isLog);
    _segmentPainter->setLimits(_xMin, _xMax, _yMin, _yMax);
    this->update();
}

void GraphVisualizer::mouseMoveEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    float posX = static_cast<float>(pos.x());
    float posY = static_cast<float>(pos.y());
    QRect rect = QRect(QPoint(0, 0), this->size());

    // Find the closest point
    float minDist = -1;
    float w;
    _currentKey = -1;
    for (int i = 0; i < _segments.count(); i++)
    {
        float segY = _segmentPainter->valueToCoord(_segments[i]->_value, rect);
        for (int j = _segments[i]->_keyMin; j <= _segments[i]->_keyMax; j++)
        {
            float segX = _segmentPainter->keyToCoord(j, rect, w);
            float dist = qAbs(posX - segX) + qAbs(posY - segY);
            if (_currentKey == -1 || dist < minDist)
            {
                minDist = dist;
                _currentValue = _segments[i]->_value;
                _currentKey = j;
            }
        }
    }

    this->update();
}

void GraphVisualizer::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    _currentKey = -1;
    this->update();
}

void GraphVisualizer::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Background
    QRect rect = this->rect();
    painter.fillRect(rect, _backgroundColor);

    // Octaves
    painter.setPen(_textColor);
    painter.setFont(_fontLabels);
    float w;
    for (int i = _xMin; i <= _xMax; i++)
    {
        if (i % 12 != 0)
            continue;

        float x = _segmentPainter->keyToCoord(i, rect, w);
        painter.fillRect(x - 0.5 * w, rect.top(), w, rect.height(), _gridColor);
        painter.drawText(QRect(x - 50 - 0.5 * w, rect.bottom() - 50, w + 100, 50), Qt::AlignHCenter | Qt::AlignBottom, ContextManager::keyName()->getKeyName(i));
    }

    // Default value
    painter.setPen(QPen(_globalValueSet ? _curveColor : _defaultValueColor, 2));
    float y = _segmentPainter->valueToCoord(_defaultValue, rect);
    painter.drawLine(rect.left(), y, rect.right(), y);

    // Segments
    _segmentPainter->paint(&painter, rect);

    // Current point
    painter.setPen(_currentPointPen);
    if (_currentKey >= 0)
    {
        float x = _segmentPainter->keyToCoord(_currentKey, rect, w);
        float y = _segmentPainter->valueToCoord(_currentValue, rect);

        painter.drawLine(x - 5, y, x + 5, y);
        painter.drawLine(x, y - 5, x, y + 5);

        // Text to display with its size
        QString label = ContextManager::keyName()->getKeyName(_currentKey) + " - ";
        if (_currentValue == 0)
            label += QLocale::system().toString(_currentValue, 'f', 0);
        else if (_currentValue < 1)
            label += QLocale::system().toString(_currentValue, 'f', 3);
        else if (_currentValue < 10)
            label += QLocale::system().toString(_currentValue, 'f', 2);
        else if (_currentValue < 100)
            label += QLocale::system().toString(_currentValue, 'f', 1);
        else
            label += QLocale::system().toString(_currentValue, 'f', 0);

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

    if (_dispWarning)
    {
        painter.setFont(_fontWarning);
        painter.setPen(_warningColor);
        painter.drawText(rect.left() + 2, rect.top() + 2, 2000, 200, Qt::AlignTop | Qt::AlignLeft, tr("Cannot display all the values."));
    }
}

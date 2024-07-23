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

#include "graphparamglobal.h"
#include "contextmanager.h"
#include <QPainter>
#include <QPaintEvent>

GraphParamGlobal::GraphParamGlobal(QWidget * parent) : QWidget(parent),
    _patternType(PATTERN_MANUAL),
    _flagEdit(false),
    _limitEdit(0),
    _patternStiffnessExp(50.0),
    _patternYmin(0.), _patternYmax(1.),
    _patternXmin(0), _patternXmax(140),
    previousX(-1),
    _highlightedRangeMin(-1),
    _highlightedRangeMax(-1)
{
    // Prepare data
    this->_curve.resize(128);
    this->_curve.fill(0.5);

    // Colors / pens
    _backgroundColor = ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND);
    _octaveColor = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT);
    _penCurrentValue = QPen(_octaveColor, 2, Qt::SolidLine, Qt::RoundCap);
    _octaveColor.setAlpha(40);
    _octaveNameColor = _octaveColor;
    _octaveNameColor.setAlpha(180);
    _keyRangeColor = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);
    _penCurve = QPen(_keyRangeColor, 4, Qt::SolidLine, Qt::RoundCap);
    _keyRangeColor.setAlpha(20);
    _fontLabel = QFont(font().family(), 9, QFont::Bold);

    // Enable the mouse tracking for the mouse move event
    this->setMouseTracking(true);
}

GraphParamGlobal::~GraphParamGlobal()
{
}

void GraphParamGlobal::setHighlightedRange(int minKey, int maxKey)
{
    _highlightedRangeMin = minKey;
    _highlightedRangeMax = maxKey;
    this->repaint();
}

void GraphParamGlobal::indexMotifChanged(int motif)
{
    this->_patternType = static_cast<PatternType>(motif);
    this->_flagEdit = false;
    this->_limitEdit = 0;
    this->previousX = -1;
    this->_patternXmin = 0;
    this->_patternXmax = 127;
    this->computePattern();
}

void GraphParamGlobal::raideurChanged(double value)
{
    this->_patternStiffnessExp = value;
    this->computePattern();
}

QVector<float> GraphParamGlobal::getValues()
{
    return this->_curve;
}

void GraphParamGlobal::setValues(QVector<float> val)
{
    for (int i = 0; i <= qMin(127, val.size()); i++)
        _curve[i] = val.at(i);
}

void GraphParamGlobal::computePattern()
{
    // Ecriture début et fin
    if (this->_patternType == PATTERN_LINEAR_ASC || this->_patternType == PATTERN_EXP_ASC)
    {
        // Remplissage de 0 au début
        for (int i = 0; i < this->_patternXmin; i++)
            this->_curve[i] = 0;
        // Remplissage de 1 à la fin
        for (int i = this->_patternXmax; i <= 127; i++)
            this->_curve[i] = 1;
    }
    else if (this->_patternType == PATTERN_LINEAR_DESC || this->_patternType == PATTERN_EXP_DESC)
    {
        // Remplissage de 1 au début
        for (int i = 0; i < this->_patternXmin; i++)
            this->_curve[i] = 1;
        // Remplissage de 0 à la fin
        for (int i = this->_patternXmax; i <= 127; i++)
            this->_curve[i] = 0;
    }

    switch (this->_patternType)
    {
    case PATTERN_MANUAL:
        break;
    case PATTERN_LINEAR_ASC:
        for (int i = this->_patternXmin; i < this->_patternXmax; i++)
            this->_curve[i] = (double)(i - this->_patternXmin) / (this->_patternXmax - this->_patternXmin);
        break;
    case PATTERN_LINEAR_DESC:
        for (int i = this->_patternXmin; i < this->_patternXmax; i++)
            this->_curve[i] = 1.0 - (double)(i - this->_patternXmin) / (this->_patternXmax - this->_patternXmin);
        break;
    case PATTERN_EXP_ASC:{
        double baseExp = 1. + 1.0 / (101.0 - this->_patternStiffnessExp);
        double alpha = 1.0 / (pow(baseExp, this->_patternXmax - this->_patternXmin) - 1.0);
        for (int i = this->_patternXmin; i < this->_patternXmax; i++)
            this->_curve[i] = alpha * (pow(baseExp, i - this->_patternXmin) - 1.0);
    }break;
    case PATTERN_EXP_DESC:{
        double baseExp = 1. + 1.0 / (101.0 - this->_patternStiffnessExp);
        double alpha = 1.0 / (pow(baseExp, this->_patternXmin - this->_patternXmax) - 1.0);
        for (int i = this->_patternXmin; i < this->_patternXmax; i++)
            this->_curve[i] = 1.0 - alpha * (pow(baseExp, this->_patternXmin - i) - 1.0);
    }break;
    case PATTERN_RANDOM:
        for (int i = 0; i <= 127; i++)
        {
            double valTmp = (double)(QRandomGenerator::global()->generate() % 2000) / 1000. - 1.;
            double exp = qExp((50. - this->_patternStiffnessExp) / 15.);
            if (valTmp < 0)
                this->_curve[i] = -qPow(qAbs(valTmp), exp);
            else
                this->_curve[i] = qPow(qAbs(valTmp), exp);
            this->_curve[i] = (this->_curve[i] + 1.) / 2;
        }
        break;
    }

    // Display
    this->repaint();
}

void GraphParamGlobal::write(QPoint pos)
{
    // Convert coordinate
    int x = posToKey(pos.x());
    double y = posToValue(pos.y());
    if (x < 0)
        x = 0;
    else if (x > 127)
        x = 127;
    if (y < 0)
        y = 0;
    else if (y > 1)
        y = 1;

    // Change value
    if (this->previousX >= 0 && this->previousX != x)
    {
        if (this->previousX < x)
        {
            for (int i = this->previousX + 1; i < x; i++)
                this->_curve[i] = this->previousY +
                    (y - previousY) * (i - this->previousX) / (x - this->previousX);
        }
        else
        {
            for (int i = this->previousX - 1; i > x; i--)
                this->_curve[i] = this->previousY +
                    (y - previousY) * (i - this->previousX) / (x - this->previousX);
        }
    }
    this->_curve[x] = y;

    // Store the point
    this->previousX = x;
    this->previousY = y;

    // Display
    this->repaint();
}

void GraphParamGlobal::afficheCoord(int key, float value)
{
    _currentValueKey = key;
    _currentValue = value;
    this->repaint();
}

int GraphParamGlobal::posToKey(int x)
{
    float tmp = static_cast<float>(x) / this->size().width();
    int key = static_cast<int>(tmp * 129 + 0.5) - 1;
    if (key <= 0)
        return 0;
    if (key > 127)
        return 127;
    return key;
}

float GraphParamGlobal::keyToPos(int key, float &w)
{
    w = static_cast<float>(this->size().width()) / 129.;
    return static_cast<float>(key + 1) * w + 0.5f;
}

float GraphParamGlobal::posToValue(int y)
{
    return 1.0f - static_cast<float>(y) / this->size().height();
}

float GraphParamGlobal::valueToPos(float value)
{
    return (1.0f - value) * this->size().height();
}

void GraphParamGlobal::mouseMoveEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    if (this->_flagEdit)
    {
        this->afficheCoord(-1, -1);
        this->write(pos);
    }
    else if (this->_limitEdit == -1 && _patternType != PATTERN_RANDOM)
    {
        // Modification min X
        int x = posToKey(pos.x());
        if (x < 0)
            x = 0;
        else if (x > 127)
            x = 127 ;
        if (x < _patternXmax)
        {
            _patternXmin = x;
            this->computePattern();
        }
    }
    else if (this->_limitEdit == 1 && _patternType != PATTERN_RANDOM)
    {
        // Modification max X
        int x = posToKey(pos.x());
        if (x < 0)
            x = 0;
        else if (x > 127)
            x = 127;
        if (x > _patternXmin)
        {
            _patternXmax = x;
            this->computePattern();
        }
    }
    else
    {
        // Convert coordinates
        float x = posToKey(pos.x());
        float y = posToValue(pos.y());

        // Closest point
        float distanceMin = -1;
        int posX = -1;
        for (int i = 0; i < this->_curve.size(); i++)
        {
            float distanceTmp = 0.05f * qAbs(x - i) + qAbs(y - this->_curve[i]);
            if (distanceMin == -1 || distanceTmp < distanceMin)
            {
                distanceMin = distanceTmp;
                posX = i;
            }
        }

        if (posX != -1)
            this->afficheCoord(posX, this->_curve[posX]);
        else
            this->afficheCoord(-1, -1);
    }
}

void GraphParamGlobal::mousePressEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    if (event->button() == Qt::LeftButton)
    {
        this->afficheCoord(-1, -1);
        if (this->_patternType == PATTERN_MANUAL)
        {
            this->_flagEdit = true;
            // Inscription du premier point
            this->write(pos);
        }
        else
        {
            // Modification min X
            this->_limitEdit = -1;
            int x = posToKey(pos.x());
            if (x < _patternXmax)
            {
                _patternXmin = x;
                this->computePattern();
            }
        }
    }
    else if (event->button() == Qt::RightButton)
    {
        if (this->_patternType != PATTERN_MANUAL)
        {
            this->afficheCoord(-1, -1);
            // Modification max X
            this->_limitEdit = 1;
            int x = posToKey(pos.x());
            if (x > _patternXmin)
            {
                _patternXmax = x;
                this->computePattern();
            }
        }
    }
}

void GraphParamGlobal::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (this->_limitEdit == -1 || this->_flagEdit)
        {
            this->_limitEdit = 0;
            this->_flagEdit = false;
            this->previousX = -1;
            this->repaint();
        }
    }
    else if (event->button() == Qt::RightButton)
    {
        if (this->_limitEdit == 1)
        {
            this->_limitEdit = 0;
            this->repaint();
        }
    }
}

void GraphParamGlobal::leaveEvent(QEvent *event)
{
    this->afficheCoord(-1, -1);
}

void GraphParamGlobal::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QSize size = this->size();

    // Background color
    painter.fillRect(this->rect(), _backgroundColor);

    // Draw octaves
    painter.setPen(_octaveNameColor);
    painter.setFont(_fontLabel);
    float w;
    for (int i = 12; i < 127; i += 12)
    {
        float x = this->keyToPos(i, w);
        painter.fillRect(QRectF(x - 0.5f * w, 0, w, size.height()), _octaveColor);
        painter.drawText(x - 200, size.height() - 200, 400, 200, Qt::AlignBottom | Qt::AlignHCenter,
            ContextManager::keyName()->getKeyName(i));
    }

    // Highlighted range
    if ((_highlightedRangeMin != 0 || _highlightedRangeMax != 127) && _highlightedRangeMin >= 0 &&
        _highlightedRangeMin <= _highlightedRangeMax && _highlightedRangeMax <= 127)
    {
        float xMin = this->keyToPos(_highlightedRangeMin, w);
        float xMax = this->keyToPos(_highlightedRangeMax, w);
        painter.fillRect(QRectF(xMin - 0.5f * w, 0, xMax + 0.5f * w, size.height()), _keyRangeColor);
    }

    // Curve
    painter.setPen(_penCurve);
    for (int i = 0; i <= 127; i++)
        painter.drawPoint(keyToPos(i, w), valueToPos(_curve[i]));

    // Current value
    painter.setPen(_penCurrentValue);
    if (_currentValueKey >= 0)
    {
        float x = keyToPos(_currentValueKey, w);
        float y = valueToPos(_currentValue);
        painter.drawLine(x - 5, y, x + 5, y);
        painter.drawLine(x, y - 5, x, y + 5);

        // Text to display with its size
        QString text = ContextManager::keyName()->getKeyName(_currentValueKey) + ":" +
                       QLocale::system().toString(_patternYmin + (_patternYmax - _patternYmin) * _currentValue, 'f', 2);
        QFontMetrics fm(_fontLabel);
        float textHalfWidth = 0.5f * fm.horizontalAdvance(text) + 2.f;
        float textHeight = fm.height();

        // Display the text
        float textCenterX = x;
        if (textCenterX + textHalfWidth > size.width())
            textCenterX = size.width() - textHalfWidth;
        if (textCenterX - textHalfWidth < 0)
            textCenterX = textHalfWidth;
        painter.drawText(textCenterX - textHalfWidth, _currentValue < 0.5 ? y - 1.5 * textHeight : y + 0.5f * textHeight,
                         2 * textHalfWidth, textHeight, Qt::AlignVCenter | Qt::AlignHCenter, text);
    }
}

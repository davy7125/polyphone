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

#include "graphspace.h"
#include "contextmanager.h"
#include "qmath.h"
#include <QPainter>

GraphSpace::GraphSpace(QWidget * parent) : QWidget(parent),
    _backgroundColor(ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND)),
    _currentLabel("")
{
    // Filter events
    this->setMouseTracking(true);
    this->installEventFilter(this);
}

void GraphSpace::setData(QVector<double> x, QVector<int> y)
{
    _xPan = x;
    _yKey = y;

    // Max pipe length
    _yLength.resize(_yKey.size());
    double freq = qPow(2., (_yKey.first() + 36.3763) / 12);
    double l = 172 / freq; // in meters for an open pipe
    for (int i = 0; i < _yKey.size(); i++)
    {
        // Pipe length
        freq = qPow(2., (_yKey.at(i) + 36.3763) / 12);
        _yLength[i] = 172 / freq / l;
    }

    // Display curve
    this->repaint();
}

void GraphSpace::mouseMoved(QPoint pos)
{
    // Convert coordinates
    double x = static_cast<double>(pos.x()) / this->width();

    // Closest point
    double distanceMin = -1;
    int posX = -1;
    for (int i = 0; i < _xPan.size(); i++)
    {
        double distanceTmp = qAbs(x - _xPan[i]);
        if (distanceMin < -0.5 || distanceTmp < distanceMin)
        {
            distanceMin = distanceTmp;
            posX = i;
        }
    }
    if (posX != -1)
    {
        _currentPan = _xPan[posX];
        _currentLength = _yLength[posX];
        _currentLabel = ContextManager::keyName()->getKeyName(_yKey[posX]) + ":" +
                QLocale::system().toString(_currentPan * 100 - 50, 'f', 1);
    }
    else
        _currentLabel = "";

    repaint();
}

void GraphSpace::mouseLeft()
{
    _currentLabel = "";
    repaint();
}

void GraphSpace::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    const int margin = 5;

    // Background
    QPainter painter(this);
    painter.fillRect(this->rect(), _backgroundColor);

    // Central vertical bar
    QColor colorTmp = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT);
    colorTmp.setAlpha(40);
    painter.setPen(QPen(colorTmp, 1.0, Qt::DashDotLine));
    painter.drawLine(0.5 * this->width(), -1, 0.5 * this->width(), this->height() + 1);

    // Data
    painter.setPen(QPen(ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND), 3.0, Qt::SolidLine));
    for (int i = 0; i < _xPan.size(); i++)
        painter.drawLine(
                    _xPan[i] * (this->width() - 2 * margin) + margin,
                    this->height(),
                    _xPan[i] * (this->width() - 2 * margin) + margin,
                    margin + (this->height() - margin) * (1.0 - _yLength[i]));

    // Add L / R
    QFont font = this->font();
    font.setPointSize(10);
    painter.setFont(font);
    colorTmp.setAlpha(180);
    painter.setPen(colorTmp);

    painter.drawText(2, 0, 50, 50,
                     Qt::AlignLeft | Qt::AlignTop,
                     tr("L", "first letter of Left in your language"));
    painter.drawText(this->width() - 50 - 2, 0, 50, 50,
                     Qt::AlignRight | Qt::AlignTop,
                     tr("R", "first letter of Right in your language"));

    // Current coordinates
    if (!_currentLabel.isEmpty())
    {
        colorTmp.setAlpha(255);
        painter.setPen(QPen(colorTmp, 1.0));

        font.setPointSize(9);
        font.setBold(true);
        painter.setFont(font);

        // Size of the text to display
        QFontMetrics fm(this->font());
        QSize textSize = QSize(fm.horizontalAdvance(_currentLabel), fm.height());

        // Display a cross
        double posTextX = _currentPan * (this->width() - 2 * margin) + margin;
        double posTextY = margin + (1.0 - _currentLength) * (this->height() - margin);
        int crossLength = 3;
        painter.drawLine(posTextX - crossLength, posTextY, posTextX + crossLength, posTextY);
        painter.drawLine(posTextX, posTextY - crossLength, posTextX, posTextY + crossLength);

        // Horizontal offset: half the text width
        if (posTextX < textSize.width() / 2)
            posTextX = textSize.width() / 2;
        else if (posTextX > this->width() - textSize.width() / 2)
            posTextX = this->width() - textSize.width() / 2;
        posTextX -= textSize.width() / 2;

        // Vertical offset: 2 pixels
        if (_currentLength < 0.5)
            posTextY -= 2.0 + textSize.height();
        else
            posTextY += 2.0;

        // Display text
        painter.drawText(posTextX, posTextY, textSize.width(), textSize.height(),
                         Qt::AlignVCenter | Qt::AlignHCenter, _currentLabel);
    }
}

/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2014, Pedro Lopez-Cabanillas <plcl@users.sf.net>
    Copyright (C) 2014,      Davy Triponney         <davy.triponney@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; If not, see <http://www.gnu.org/licenses/>.
*/

#include "pianokey.h"
#include "contextmanager.h"
#include <QPainter>
#include <QPalette>
#include <QtSvg/QSvgRenderer>
#include <qmath.h>

QBrush PianoKey::PIANO_BLACK_BRUSH = QBrush(Qt::black);
QBrush PianoKey::PIANO_WHITE_BRUSH = QBrush(Qt::white);
double PianoKey::s_glowEffect = 0;

PianoKey::PianoKey(const QRectF &rect, const bool black, const int note) : QGraphicsRectItem(rect),
    _pressed(false),
    _brush(black ? PIANO_BLACK_BRUSH : PIANO_WHITE_BRUSH),
    _note(note),
    _black(black),
    _markerType(PianoKeybd::MARKER_TYPE_NONE)
{
    if (_black)
        this->setZValue(1);
    setAcceptedMouseButtons(Qt::NoButton);
}

void PianoKey::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    // Mix between black and gray with the glow color
    QColor glowColor = Qt::white;
    const QPen blackPen(mergeColor(Qt::black, glowColor, s_glowEffect), 1);
    const QPen grayPen(QBrush(mergeColor(Qt::gray, glowColor, s_glowEffect)),
                       1, Qt::SolidLine,  Qt::RoundCap, Qt::RoundJoin);
    if (_pressed)
    {
        if (_selectedBrush.style() != Qt::NoBrush)
            painter->setBrush(_selectedBrush);
        else
            painter->setBrush(ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND));
    }
    else
        painter->setBrush(_brush);

    painter->setPen(blackPen);
    painter->drawRoundedRect(rect(), 15, 15, Qt::RelativeSize);
    if (_black)
        QSvgRenderer(QString(":/vpiano/blkey.svg")).render(painter, rect());
    else
    {
        QPointF points[3] =
        {
             QPointF(rect().left() + 1.5, rect().bottom() - 1),
             QPointF(rect().right() - 1, rect().bottom() - 1),
             QPointF(rect().right() - 1, rect().top()  +1),
        };
        painter->setPen(grayPen);
        painter->drawPolyline(points, 3);
    }

    if (_markerType != PianoKeybd::MARKER_TYPE_NONE)
    {
        QRectF rectMarker = rect();
        qreal w = rectMarker.width();
        qreal margin = w * .2;
        rectMarker.setLeft(rectMarker.left() + margin);
        rectMarker.setRight(rectMarker.right() - margin);
        if (isBlack())
            rectMarker.setBottom(rectMarker.bottom() - 2.3 * margin);
        else
            rectMarker.setBottom(rectMarker.bottom() - margin);
        rectMarker.setTop(rectMarker.bottom() - rectMarker.width());

        drawMarker(painter, rectMarker, _markerType);
    }
}

QColor PianoKey::mergeColor(QColor color1, QColor color2, double fade)
{
    return QColor((1. - fade) * color1.red() + fade * color2.red(),
                  (1. - fade) * color1.green() + fade * color2.green(),
                  (1. - fade) * color1.blue() + fade * color2.blue());
}

void PianoKey::setPressed(bool p)
{
    if (p != _pressed)
    {
        _pressed = p;
        update();
    }
}

void PianoKey::resetBrush()
{
    _brush = _black ? PIANO_BLACK_BRUSH : PIANO_WHITE_BRUSH;
}

void PianoKey::setMarker(PianoKeybd::MarkerType type)
{
    if (_markerType != type)
    {
        _markerType = type;
        update();
    }
}

QColor PianoKey::getBorderColor(PianoKeybd::MarkerType type)
{
    QColor color;
    switch (type)
    {
    case PianoKeybd::MARKER_TYPE_DOT_BLUE:
        color = QColor(0, 0, 127);
        break;
    case PianoKeybd::MARKER_TYPE_DOT_RED:
        color = QColor(127, 0, 0);
        break;
    case PianoKeybd::MARKER_TYPE_DOT_YELLOW:
        color = QColor(127, 127, 0);
        break;
    case PianoKeybd::MARKER_TYPE_DOT_BLACK:
        color = QColor(127, 127, 127);
        break;
    case PianoKeybd::MARKER_TYPE_DOT_WHITE:
        color = QColor(127, 127, 127);
        break;
    default:
        break;
    }
    return color;
}

QColor PianoKey::getFillColor(PianoKeybd::MarkerType type)
{
    QColor color;
    switch (type)
    {
    case PianoKeybd::MARKER_TYPE_DOT_BLUE:
        color = QColor(0, 0, 255);
        break;
    case PianoKeybd::MARKER_TYPE_DOT_RED:
        color = QColor(255, 160, 0);
        break;
    case PianoKeybd::MARKER_TYPE_DOT_YELLOW:
        color = QColor(255, 255, 0);
        break;
    case PianoKeybd::MARKER_TYPE_DOT_BLACK:
        color = QColor(0, 0, 0);
        break;
    case PianoKeybd::MARKER_TYPE_DOT_WHITE:
        color = QColor(255, 255, 255);
        break;
    default:
        break;
    }
    return color;
}

void PianoKey::drawMarker(QPainter *painter, QRectF &rect, PianoKeybd::MarkerType type)
{
    switch (type)
    {
    case PianoKeybd::MARKER_TYPE_DOT_BLUE:
    case PianoKeybd::MARKER_TYPE_DOT_RED:
    case PianoKeybd::MARKER_TYPE_DOT_YELLOW:
    case PianoKeybd::MARKER_TYPE_DOT_BLACK:
    case PianoKeybd::MARKER_TYPE_DOT_WHITE:
        painter->setPen(QPen(getBorderColor(type), 1));
        painter->setBrush(getFillColor(type));
        painter->drawEllipse(rect);
        break;
    default:
        break;
    }
}

void PianoKey::flipPainter(QPainter * painter, QRectF &rect)
{
    painter->rotate(180);
    rect.setCoords(-rect.left()-rect.width(), -rect.bottom(), -rect.right()-rect.width(), -rect.top());
}

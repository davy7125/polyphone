/*
    Virtual Piano Widget for Qt4 
    Copyright (C) 2008-2010, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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
#include <QtGui/QApplication>
#include <QtGui/QPainter>
#include <QtGui/QPalette>
#include <QtSvg/QSvgRenderer>

static const QBrush blackBrush = QBrush(Qt::black);
static const QBrush whiteBrush = QBrush(Qt::white);

PianoKey::PianoKey(const QRectF &rect, const QBrush &brush, const int note)
    : QGraphicsRectItem(rect),
    m_pressed(false),
    m_brush(brush), 
    m_note(note),
    m_black(brush == blackBrush)
{
    setAcceptedMouseButtons(Qt::NoButton);
}

PianoKey::PianoKey(const QRectF &rect, const bool black, const int note)
    : QGraphicsRectItem(rect),
    m_pressed(false),
    m_brush( black ? blackBrush : whiteBrush ),
    m_note(note),
    m_black(black)
{
    setAcceptedMouseButtons(Qt::NoButton);
}

void PianoKey::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    static QSvgRenderer keyRenderer(QString(":/vpiano/blkey.svg"));
    static const QPen blackPen(Qt::black, 1);
    static const QPen grayPen(QBrush(Qt::gray), 1, Qt::SolidLine,  Qt::RoundCap, Qt::RoundJoin);
    if (m_pressed) {
        if (m_selectedBrush.style() != Qt::NoBrush) {
            painter->setBrush(m_selectedBrush);
        } else {
            painter->setBrush(QApplication::palette().highlight());
        }
    } else {
        painter->setBrush(m_brush);
    }
    painter->setPen(blackPen);
    painter->drawRoundRect(rect(), 15, 15);
    if (m_black)
        keyRenderer.render(painter, rect());
    else {
        QPointF points[3] = {
             QPointF(rect().left()+1.5, rect().bottom()-1),
             QPointF(rect().right()-1, rect().bottom()-1),
             QPointF(rect().right()-1, rect().top()+1),
        };
        painter->setPen(grayPen);
        painter->drawPolyline(points, 3);
    }
}

void PianoKey::setPressed(bool p)
{
    if (p != m_pressed) {
        m_pressed = p;
        update();
    }
}

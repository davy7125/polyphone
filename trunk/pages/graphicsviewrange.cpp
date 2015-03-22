/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2014 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "graphicsviewrange.h"
#include "config.h"
#include "graphicssimpletextitem.h"
#include "rectangleitem.h"

GraphicsViewRange::GraphicsViewRange(QWidget *parent) : QGraphicsView(parent),
    _scene(new QGraphicsScene(-0.5, -0.5, 128, 128)),
    _mouseMode(MOUSE_MODE_NONE),
    _zoomX(1),
    _zoomY(1),
    _posX(0.5),
    _posY(0.5),
    _displayedRect(-0.5, -0.5, 128, 128)
{
    // Configuration
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    this->setRenderHint(QPainter::Antialiasing, true);

    // Preparation graphique
    this->setScene(_scene);
    this->initGridAndAxes();
}

GraphicsViewRange::~GraphicsViewRange() {}

void GraphicsViewRange::initGridAndAxes()
{
    QColor lineColor(220, 220, 220);
    QColor textColor(80, 80, 80);

    // Vertical lines
    QPen penVerticalLines(lineColor, 1);
    penVerticalLines.setCosmetic(true);
    for (int note = 12; note <= 120; note += 12)
    {
        QGraphicsLineItem * line = new QGraphicsLineItem(note, -0.5, note, 127.5);
        line->setPen(penVerticalLines);
        _scene->addItem(line);
        GraphicsSimpleTextItem * text = new GraphicsSimpleTextItem(Qt::AlignHCenter + Qt::AlignBottom, line);
        text->setBrush(textColor);
        text->setText(Config::getInstance()->getKeyName(note));
        text->setPos(note, 127.5);
        _bottomLabels << text;
    }

    // Horizontal lines
    QPen penHorizontalLines(lineColor, 1, Qt::DotLine);
    penHorizontalLines.setCosmetic(true);
    for (int vel = 10; vel <= 120; vel += 10)
    {
        QGraphicsLineItem * line = new QGraphicsLineItem(-0.5, 127 - vel, 127.5, 127 - vel);
        line->setPen(penHorizontalLines);
        _scene->addItem(line);
        GraphicsSimpleTextItem * text = new GraphicsSimpleTextItem(Qt::AlignLeft + Qt::AlignVCenter, line);
        text->setBrush(textColor);
        text->setText(QString::number(vel));
        text->setPos(-0.5, 127.5 - vel);
        _leftLabels << text;
    }
}

void GraphicsViewRange::display(EltID id)
{
    QList<QRectF> listRect;
    if (id.typeElement == elementInst)
        id.typeElement = elementInstSmpl;
    else if (id.typeElement == elementPrst)
        id.typeElement = elementPrstInst;

    int count = _sf2->count(id);
    for (int i = 0; i < count; i++)
    {
        id.indexElt2 = i;
        if (!_sf2->get(id, champ_hidden).bValue)
        {
            int minKey = 0, maxKey = 127;
            int minVel = 0, maxVel = 127;
            if (_sf2->isSet(id, champ_keyRange))
            {
                rangesType range = _sf2->get(id, champ_keyRange).rValue;
                minKey = range.byLo;
                maxKey = range.byHi;
            }
            if (_sf2->isSet(id, champ_velRange))
            {
                rangesType range = _sf2->get(id, champ_velRange).rValue;
                minVel = range.byLo;
                maxVel = range.byHi;
            }
            listRect << QRectF(-0.5 + minKey, 126.5 - maxVel, maxKey - minKey + 1, maxVel - minVel + 1);
        }
    }

    display(listRect);
}

void GraphicsViewRange::display(QList<QRectF> rectangles)
{
    // Style
    QPen penRectangle(QColor(250, 0, 0, 120), 1);
    penRectangle.setCosmetic(true);
    QBrush brushRectangle(QColor(250, 0, 0, 60));

    // Clear previous rectangles
    foreach (QGraphicsRectItem * item, _rectangles)
        _scene->removeItem(item);
    _rectangles.clear();

    // Add new rectangles
    foreach (QRectF rectangle, rectangles)
    {
        RectangleItem * item = new RectangleItem(rectangle);
        _scene->addItem(item);
        _rectangles << item;
        item->setPen(penRectangle);
        item->setBrush(brushRectangle);
    }
}

void GraphicsViewRange::resizeEvent(QResizeEvent * event)
{
    fitInView(_displayedRect);
    QGraphicsView::resizeEvent(event);
}

void GraphicsViewRange::mousePressEvent(QMouseEvent *event)
{
    if (_mouseMode != MOUSE_MODE_NONE)
        return;

    // Enregistrement situation
    _xInit = normalizeX(event->x());
    _yInit = normalizeY(event->y());
    _zoomXinit = _zoomX;
    _zoomYinit = _zoomY;
    _posXinit = _posX;
    _posYinit = _posY;
    if (event->button() == Qt::LeftButton)
        _mouseMode = MOUSE_MODE_DRAG;
    else if (event->button() == Qt::RightButton)
        _mouseMode = MOUSE_MODE_ZOOM;
}


void GraphicsViewRange::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (_mouseMode == MOUSE_MODE_DRAG)
        {
            _mouseMode = MOUSE_MODE_NONE;
            this->setCursor(Qt::ArrowCursor);
        }
    }
    else if (event->button() == Qt::RightButton)
    {
        if (_mouseMode == MOUSE_MODE_ZOOM)
        {
            _mouseMode = MOUSE_MODE_NONE;
            this->setZoomLine(-1, 0, 0, 0);
            this->setCursor(Qt::ArrowCursor);
        }
    }
}

void GraphicsViewRange::mouseMoveEvent(QMouseEvent *event)
{
    switch (_mouseMode)
    {
    case MOUSE_MODE_ZOOM:
        this->setCursor(Qt::SizeAllCursor);
        this->setZoomLine(_xInit, _yInit, normalizeX(event->x()), normalizeY(event->y()));
        this->zoom(event->pos());
        break;
    case MOUSE_MODE_DRAG:
        this->setCursor(Qt::ClosedHandCursor);
        this->drag(event->pos());
        break;
    default:
        break;
    }
}

void GraphicsViewRange::drag(QPoint point)
{
    // Décalage
    double decX = normalizeX(point.x()) - _xInit;
    double decY = normalizeY(point.y()) - _yInit;

    // Modification posX et posY
    if (_zoomXinit > 1)
        _posX = _posXinit - decX / (_zoomXinit - 1);
    if (_zoomYinit > 1)
        _posY = _posYinit - decY / (_zoomYinit - 1);

    // Mise à jour
    this->zoomDrag();
}

void GraphicsViewRange::zoom(QPoint point)
{
    // Décalage
    double decX = normalizeX(point.x()) - _xInit;
    double decY = _yInit - normalizeY(point.y());

    // Modification zoom & drag
    _zoomX = _zoomXinit * pow(2, 10.0 * decX);
    _zoomY = _zoomYinit * pow(2, 10.0 * decY);

    // Ajustement posX et posY
    if (_zoomX > 1)
        _posX = (_zoomX * _posXinit * (_zoomXinit - 1) +
                 _xInit * (_zoomX - _zoomXinit)) / (_zoomXinit * (_zoomX - 1));
    if (_zoomY > 1)
        _posY = (_zoomY * _posYinit * (_zoomYinit - 1) +
                 _yInit * (_zoomY - _zoomYinit)) / (_zoomYinit * (_zoomY - 1));

    // Mise à jour
    this->zoomDrag();
}

void GraphicsViewRange::zoomDrag()
{
    // Bornes des paramètres d'affichage
    if (_zoomX < 1)
        _zoomX = 1;
    else if (_zoomX > 8)
        _zoomX = 8;
    if (_zoomY < 1)
        _zoomY = 1;
    else if (_zoomY > 8)
        _zoomY = 8;
    if (_posX < 0)
        _posX = 0;
    else if (_posX > 1)
        _posX = 1;
    if (_posY < 0)
        _posY = 0;
    else if (_posY > 1)
        _posY = 1;

    // Application du drag et zoom
    double etendueX = _scene->width() / _zoomX;
    double offsetX = (_scene->width() - etendueX) * _posX - 0.5;
    double etendueY = _scene->height() / _zoomY;
    double offsetY = (_scene->height() - etendueY) * _posY - 0.5;
    _displayedRect.setRect(offsetX, offsetY, etendueX, etendueY);

    // Déplacement des valeurs des axes
    foreach (GraphicsSimpleTextItem * label, _leftLabels)
        label->setX(offsetX);
    foreach (GraphicsSimpleTextItem * label, _bottomLabels)
        label->setY(offsetY + etendueY);

    // Mise à jour
    this->fitInView(_displayedRect);
}

void GraphicsViewRange::setZoomLine(double x1, double y1, double x2, double y2)
{

}

double GraphicsViewRange::normalizeX(int xPixel)
{
    return (double)xPixel / this->width();
}

double GraphicsViewRange::normalizeY(int yPixel)
{
    return (double)yPixel / this->height();
}

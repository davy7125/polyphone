/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
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
#include "graphicsrectangleitem.h"
#include "graphicslegenditem.h"
#include "graphicslegenditem2.h"
#include "graphicszoomline.h"
#include "graphicskey.h"
#include "mainwindow.h"
#include <QScrollBar>

const double GraphicsViewRange::WIDTH = 128.0;
const double GraphicsViewRange::MARGIN = 0.5;
const double GraphicsViewRange::OFFSET = -0.5;
const QColor GraphicsViewRange::LINE_COLOR = QColor(220, 220, 220);
const QColor GraphicsViewRange::TEXT_COLOR = QColor(100, 100, 100);

// Z values (all GraphicsItem being at the top level)
//   0: grid
//  50: non selected rectangles
//  51: selected rectangles
//  80: play marker
// 100: axis values
// 120: legends
// 150: zoom line

GraphicsViewRange::GraphicsViewRange(QWidget *parent) : QGraphicsView(parent),
    _scene(new QGraphicsScene(OFFSET - MARGIN, OFFSET - MARGIN, WIDTH + 2 * MARGIN, WIDTH + 2 * MARGIN)),
    _legendItem(NULL),
    _legendItem2(NULL),
    _zoomLine(NULL),
    _dontRememberScroll(false),
    _keyTriggered(-1),
    _buttonPressed(Qt::NoButton),
    _moveOccured(false),
    _zoomX(1.),
    _zoomY(1.),
    _posX(0.5),
    _posY(0.5),
    _displayedRect(OFFSET, OFFSET, WIDTH, WIDTH)
{
    // Configuration
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    this->setRenderHint(QPainter::Antialiasing, true);
    this->setMouseTracking(true);

    // Preparation of the graphics
    this->setScene(_scene);
    this->initItems();
}

GraphicsViewRange::~GraphicsViewRange()
{
    _currentRectangles.clear();
    while (!_rectangles.isEmpty())
        delete _rectangles.takeFirst();
    while (!_leftLabels.isEmpty())
        delete _leftLabels.takeFirst();
    while (!_bottomLabels.isEmpty())
        delete _bottomLabels.takeFirst();
    delete _legendItem;
    delete _legendItem2;
    delete _zoomLine;
    while (!_mapGraphicsKeys.isEmpty())
        delete _mapGraphicsKeys.take(_mapGraphicsKeys.keys().first());
}

void GraphicsViewRange::initItems()
{
    // Vertical lines
    QPen penVerticalLines(LINE_COLOR, 1);
    penVerticalLines.setCosmetic(true);
    for (int note = 12; note <= 120; note += 12)
    {
        QGraphicsLineItem * line = new QGraphicsLineItem(note, OFFSET - MARGIN, note, OFFSET + WIDTH + MARGIN);
        _scene->addItem(line);
        line->setPen(penVerticalLines);
        line->setZValue(0);
        GraphicsSimpleTextItem * text = new GraphicsSimpleTextItem(Qt::AlignHCenter + Qt::AlignBottom);
        _scene->addItem(text);
        text->setZValue(100);
        text->setBrush(TEXT_COLOR);
        text->setText(Config::getInstance()->getKeyName(note));
        text->setPos(note, OFFSET + WIDTH);
        _bottomLabels << text;
    }

    // Horizontal lines
    QPen penHorizontalLines(LINE_COLOR, 1, Qt::DotLine);
    penHorizontalLines.setCosmetic(true);
    for (int vel = 10; vel <= 120; vel += 10)
    {
        QGraphicsLineItem * line = new QGraphicsLineItem(OFFSET - MARGIN, 127 - vel, OFFSET + WIDTH + MARGIN, 127 - vel);
        _scene->addItem(line);
        line->setPen(penHorizontalLines);
        line->setZValue(0);
        GraphicsSimpleTextItem * text = new GraphicsSimpleTextItem(Qt::AlignLeft + Qt::AlignVCenter);
        _scene->addItem(text);
        text->setZValue(100);
        text->setBrush(TEXT_COLOR);
        text->setText(QString::number(vel));
        text->setPos(OFFSET, OFFSET + WIDTH - vel);
        _leftLabels << text;
    }

    // Legends
    _legendItem = new GraphicsLegendItem(this->font().family());
    _scene->addItem(_legendItem);
    _legendItem->setZValue(120);
    _legendItem2 = new GraphicsLegendItem2(this->font().family());
    _scene->addItem(_legendItem2);
    _legendItem2->setZValue(120);

    // Zoomline
    _zoomLine = new GraphicsZoomLine();
    _scene->addItem(_zoomLine);
    _zoomLine->setZValue(150);
}

void GraphicsViewRange::updateLabels()
{
    // Current rect
    QRectF rect = getCurrentRect();

    // Update the position of the axis labels (they stay to the left and bottom)
    foreach (GraphicsSimpleTextItem * label, _leftLabels)
        label->setX(qMax(OFFSET, rect.x()));
    foreach (GraphicsSimpleTextItem * label, _bottomLabels)
        label->setY(qMin(WIDTH + OFFSET, rect.y() + rect.height()));

    // Update the position of the legend (it stays in a corner)
    if (_legendItem->isLeft())
    {
        double posX = 35. * rect.width() / this->width() + qMax(OFFSET, rect.x());
        _legendItem->setX(posX);
        _legendItem2->setX(posX);
    }
    else
    {
        double posX = qMin(WIDTH + OFFSET, rect.x() + rect.width()) - 5. * rect.width() / this->width();
        _legendItem->setX(posX);
        _legendItem2->setX(posX);
    }
    _legendItem->setY(5. * rect.height() / this->height() + qMax(OFFSET, rect.y()));
    _legendItem2->setY(13. * rect.height() / this->height() + qMax(OFFSET, rect.y()));

    viewport()->update();
}

void GraphicsViewRange::init(Pile_sf2 * sf2)
{
    _sf2 = sf2;
    GraphicsRectangleItem::init(sf2);
    GraphicsLegendItem::initSf2(sf2);
}

void GraphicsViewRange::display(EltID id)
{
    // Clear previous rectangles
    while (!_rectangles.isEmpty())
    {
        _scene->removeItem(_rectangles.first());
        delete _rectangles.takeFirst();
    }
    _currentRectangles.clear();

    // Add new ones
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
            GraphicsRectangleItem * item = new GraphicsRectangleItem(id);
            item->setZValue(50);
            _scene->addItem(item);
            _rectangles << item;
        }
    }

    updateLabels();
}

void GraphicsViewRange::resizeEvent(QResizeEvent * event)
{
    _dontRememberScroll = true;
    QGraphicsView::resizeEvent(event);
    fitInView(_displayedRect);
    _dontRememberScroll = false;
}

void GraphicsViewRange::mousePressEvent(QMouseEvent *event)
{
    if (_buttonPressed != Qt::NoButton)
        return;

    if (event->button() == Qt::MiddleButton)
    {
        QPointF p = this->mapToScene(event->pos());
        int key = qRound(p.x());
        int velocity = 127 - qRound(p.y());
        if (velocity > 0)
        {
            keyTriggered(key, velocity);
            _keyTriggered = key;
        }
    }
    else if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton)
    {
        // Update current position
        double deltaX = WIDTH - _displayedRect.width();
        if (deltaX == 0)
            _posX = 0.5;
        else
            _posX = (_displayedRect.left() - OFFSET) / deltaX;
        double deltaY = WIDTH - _displayedRect.height();
        if (deltaY == 0)
            _posY = 0.5;
        else
            _posY = (_displayedRect.top() - OFFSET) / deltaY;

        // Remember situation
        _xInit = normalizeX(event->x());
        _yInit = normalizeY(event->y());
        _zoomXinit = _zoomX;
        _zoomYinit = _zoomY;
        _posXinit = _posX;
        _posYinit = _posY;

        if (event->button() == Qt::LeftButton)
        {
            QList<EltID> ids;
            foreach (GraphicsRectangleItem * item, _currentRectangles)
                ids << item->getID();
            divisionsSelected(ids);
        }
    }

    _moveOccured = false;
    _buttonPressed = event->button();
}

void GraphicsViewRange::mouseReleaseEvent(QMouseEvent *event)
{
    if (_buttonPressed != event->button())
        return;

    if (event->button() == Qt::MiddleButton)
    {
        if (_keyTriggered != -1)
        {
            keyTriggered(_keyTriggered, 0);
            _keyTriggered = -1;
        }
    }
    else
    {
        if (!_currentRectangles.isEmpty())
        {
            if (_moveOccured)
            {
                /// Save the changes
                _sf2->prepareNewActions(false); // False because we can't the id again at this stage, if some old actions delete items
                foreach (GraphicsRectangleItem * item, _currentRectangles)
                    item->saveChanges();
                divisionUpdated();
                updateKeyboard();
            }

            // Current rectangles under the mouse
            QList<QList<GraphicsRectangleItem*> > pairs = getRectanglesUnderMouse(event->pos());

            if (pairs.count() == 0)
                setCurrentRectangles(QList<GraphicsRectangleItem*>(), event->pos(), 0, 0);
            else
            {
                // Current index of the pair
                int index = -1;
                for (int i = 0; i < pairs.count(); i++)
                    if (pairs[i].contains(_currentRectangles.first()))
                        index = i;

                // Next index?
                if (!_moveOccured || index == -1)
                    index = (index + 1) % pairs.count();

                // Display selection
                setCurrentRectangles(pairs[index], event->pos(), index, pairs.count());
            }
        }
        else
            this->setCursor(Qt::ArrowCursor);

        _legendItem2->setNewValues(-1, 0, 0, 0);
        this->setZoomLine(-1, 0, 0, 0);
    }

    _buttonPressed = Qt::NoButton;
    viewport()->update();
}

void GraphicsViewRange::mouseMoveEvent(QMouseEvent *event)
{
    _moveOccured = true;

    switch (_buttonPressed)
    {
    case Qt::LeftButton:
        this->setCursor(Qt::ClosedHandCursor);
        if (_currentRectangles.isEmpty())
            this->drag(event->pos());
        else
        {
            foreach (GraphicsRectangleItem * item, _currentRectangles)
            {
                QPointF pointInit = this->mapToScene(_xInit * this->width(), _yInit * this->height());
                QPointF pointFinal = this->mapToScene(event->pos());
                item->computeNewRange(pointInit, pointFinal);
            }
            _legendItem2->setNewValues(_currentRectangles[0]->currentMinKey(), _currentRectangles[0]->currentMaxKey(),
                    _currentRectangles[0]->currentMinVel(), _currentRectangles[0]->currentMaxVel());
            viewport()->update();
        }
        break;
    case Qt::RightButton:
        this->setCursor(Qt::SizeAllCursor);
        this->setZoomLine(_xInit, _yInit, normalizeX(event->x()), normalizeY(event->y()));
        this->zoom(event->pos());
        break;
    case Qt::NoButton: default: {
        // Section of the legend
        bool isLeft = (event->pos().x() > this->width() / 2);
        if (_legendItem->isLeft() != isLeft)
        {
            _legendItem->setLeft(isLeft);
            _legendItem2->setLeft(isLeft);
            updateLabels();
        }

        // Rectangles under the mouse
        QList<QList<GraphicsRectangleItem *> > pairs = getRectanglesUnderMouse(event->pos());

        QList<GraphicsRectangleItem*> rectanglesToSelect;
        int selectionIndex = 0;
        int selectionNumber = pairs.count();
        if (selectionNumber > 0)
        {
            // Current selection already within the rectangles?
            for (int i = 0; i < pairs.count(); i++)
            {
                bool ok = !pairs[i].isEmpty();
                foreach (GraphicsRectangleItem * item, _currentRectangles)
                    ok &= (bool)pairs[i].contains(item);
                if (ok)
                {
                    selectionIndex = i;
                    break;
                }
            }

            rectanglesToSelect = pairs[selectionIndex];
        }

        setCurrentRectangles(rectanglesToSelect, event->pos(), selectionIndex, selectionNumber);

        // Offset and location of the second legend
        _legendItem2->setOffsetY(_legendItem->boundingRect().bottom());
    }
    }
}

void GraphicsViewRange::wheelEvent(QWheelEvent * event)
{
    if (_buttonPressed == Qt::NoButton)
        QGraphicsView::wheelEvent(event);
}

void GraphicsViewRange::scrollContentsBy(int dx, int dy)
{
    QGraphicsView::scrollContentsBy(dx, dy);
    if (_dontRememberScroll)
        return;

    // Update the displayed rect
    _displayedRect = getCurrentRect();

    // Limits
    if (_displayedRect.left() < OFFSET)
        _displayedRect.setLeft(OFFSET);
    if (_displayedRect.right() > WIDTH + OFFSET)
        _displayedRect.setRight(WIDTH + OFFSET);
    if (_displayedRect.top() < OFFSET)
        _displayedRect.setTop(OFFSET);
    if (_displayedRect.bottom() > WIDTH + OFFSET)
        _displayedRect.setBottom(WIDTH + OFFSET);

    updateLabels();
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
    double etendueX = WIDTH / _zoomX;
    double offsetX = (WIDTH - etendueX) * _posX + OFFSET;
    double etendueY = WIDTH / _zoomY;
    double offsetY = (WIDTH - etendueY) * _posY + OFFSET;
    _displayedRect.setRect(offsetX, offsetY, etendueX, etendueY);

    // Mise à jour
    _dontRememberScroll = true;
    this->fitInView(_displayedRect);
    _dontRememberScroll = false;
    updateLabels();
}

void GraphicsViewRange::setZoomLine(double x1Norm, double y1Norm, double x2Norm, double y2Norm)
{
    if (x1Norm < 0)
        _zoomLine->setSize(0, 0);
    else
    {
        QRectF rect = getCurrentRect();

        // Initial position
        _zoomLine->setPos(rect.left() + x1Norm * rect.width(),
                          rect.top() + y1Norm * rect.height());

        // Size
        _zoomLine->setSize((x2Norm - x1Norm) * this->width(),
                           (y2Norm - y1Norm) * this->height());
    }
}

QList<QList<GraphicsRectangleItem*> > GraphicsViewRange::getRectanglesUnderMouse(QPoint mousePos)
{
    // Rectangles under the mouse
    QList<GraphicsRectangleItem *> rectanglesUnderMouse;
    foreach (GraphicsRectangleItem * item, _rectangles)
        if (item->contains(this->mapToScene(mousePos)))
            rectanglesUnderMouse << item;

    // Sort them by pairs
    QList<QList<GraphicsRectangleItem*> > pairs;
    while (!rectanglesUnderMouse.isEmpty())
    {
        QList<GraphicsRectangleItem*> listTmp;
        listTmp << rectanglesUnderMouse.takeFirst();
        EltID idBrother = listTmp[0]->findBrother();
        foreach (GraphicsRectangleItem* item, rectanglesUnderMouse)
        {
            if (*item == idBrother)
            {
                listTmp << item;
                rectanglesUnderMouse.removeOne(item);
                break;
            }
        }
        pairs << listTmp;
    }

    return pairs;
}

void GraphicsViewRange::setCurrentRectangles(QList<GraphicsRectangleItem*> rectanglesToSelect, const QPoint &point,
                                             int selectionIndex, int selectionNumber)
{
    // Update the colors
    foreach (GraphicsRectangleItem * item, _currentRectangles)
        if (!rectanglesToSelect.contains(item))
            item->setHover(false);

    GraphicsRectangleItem::EditingMode editingMode = GraphicsRectangleItem::NONE;
    foreach (GraphicsRectangleItem * item, rectanglesToSelect)
        editingMode = item->setHover(true, point);

    // Cursor
    switch (editingMode)
    {
    case GraphicsRectangleItem::NONE:
        this->setCursor(Qt::ArrowCursor);
        break;
    case GraphicsRectangleItem::MOVE_ALL:
        this->setCursor(Qt::SizeAllCursor);
        break;
    case GraphicsRectangleItem::MOVE_RIGHT: case GraphicsRectangleItem::MOVE_LEFT:
        this->setCursor(Qt::SizeHorCursor);
        break;
    case GraphicsRectangleItem::MOVE_TOP: case GraphicsRectangleItem::MOVE_BOTTOM:
        this->setCursor(Qt::SizeVerCursor);
        break;
    }

    // Remember the current rectangles
    _currentRectangles = rectanglesToSelect;

    // Update legend text
    QList<EltID> ids;
    foreach (GraphicsRectangleItem * item, _currentRectangles)
        ids << item->getID();
    _legendItem->setIds(ids, selectionIndex, selectionNumber);

    viewport()->update();
}

double GraphicsViewRange::normalizeX(int xPixel)
{
    return (double)xPixel / this->width();
}

double GraphicsViewRange::normalizeY(int yPixel)
{
    return (double)yPixel / this->height();
}

QRectF GraphicsViewRange::getCurrentRect()
{
    QPointF tl(horizontalScrollBar()->value(), verticalScrollBar()->value());
    QPointF br = tl + viewport()->rect().bottomRight();
    QMatrix mat = matrix().inverted();
    return mat.mapRect(QRectF(tl,br));
}

void GraphicsViewRange::playKey(int key, int velocity)
{
    if (velocity == 0 && _mapGraphicsKeys[key] != NULL)
    {
        // A key is removed
        delete _mapGraphicsKeys.take(key);
    }
    else if (velocity > 0 && _mapGraphicsKeys[key] == NULL)
    {
        // A key is added
        _mapGraphicsKeys[key] = new GraphicsKey();
        _scene->addItem(_mapGraphicsKeys[key]);
        _mapGraphicsKeys[key]->setPos(QPoint(key, 127 - velocity));
        _mapGraphicsKeys[key]->setZValue(80);
    }
}

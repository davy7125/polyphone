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
#include "mainwindow.h"
#include <QScrollBar>

GraphicsViewRange::GraphicsViewRange(QWidget *parent) : QGraphicsView(parent),
    _scene(new QGraphicsScene(-0.5, -0.5, 128, 128)),
    _legendItem(NULL),
    _dontRememberScroll(false),
    _buttonPressed(Qt::NoButton),
    _moveOccured(false),
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
    this->setMouseTracking(true);

    // Preparation graphique
    this->setScene(_scene);
    this->initGridAndAxes();
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
}

void GraphicsViewRange::initGridAndAxes()
{
    QColor lineColor(220, 220, 220);
    QColor textColor(100, 100, 100);

    // Vertical lines
    QPen penVerticalLines(lineColor, 1);
    penVerticalLines.setCosmetic(true);
    for (int note = 12; note <= 120; note += 12)
    {
        QGraphicsLineItem * line = new QGraphicsLineItem(note, -0.5, note, 127.5);
        line->setPen(penVerticalLines);
        line->setZValue(0);
        _scene->addItem(line);
        GraphicsSimpleTextItem * text = new GraphicsSimpleTextItem(Qt::AlignHCenter + Qt::AlignBottom, line);
        text->setZValue(0);
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
        line->setZValue(0);
        _scene->addItem(line);
        GraphicsSimpleTextItem * text = new GraphicsSimpleTextItem(Qt::AlignLeft + Qt::AlignVCenter, line);
        text->setZValue(0);
        text->setBrush(textColor);
        text->setText(QString::number(vel));
        text->setPos(-0.5, 127.5 - vel);
        _leftLabels << text;
    }

    // Legend
    _legendItem = new GraphicsLegendItem(this->font().family());
    _scene->addItem(_legendItem);
    _legendItem->setZValue(50);
}

void GraphicsViewRange::updateLabels()
{
    // Current rect
    QRectF rect = getCurrentRect();

    // Update the position of the axis labels (they stay to the left and bottom)
    foreach (GraphicsSimpleTextItem * label, _leftLabels)
        label->setX(qMax(-0.5, rect.x()));
    foreach (GraphicsSimpleTextItem * label, _bottomLabels)
        label->setY(qMin(127.5, rect.y() + rect.height()));

    // Update the position of the legend (it stays in a corner)
    if (_legendItem->isLeft())
        _legendItem->setX(35. * rect.width() / this->width() + qMax(-0.5, rect.x()));
    else
        _legendItem->setX(qMin(127.5, rect.x() + rect.width()) - 5. * rect.width() / this->width());
    _legendItem->setY(5. * rect.height() / this->height() + qMax(-0.5, rect.y()));
}

void GraphicsViewRange::init(Pile_sf2 * sf2, MainWindow * mainWindow)
{
    _sf2 = sf2;
    _mainWindow = mainWindow;
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
            item->setZValue(20);
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
    if (_mouseMode != MOUSE_MODE_NONE || _buttonPressed != Qt::NoButton)
        return;

    // Update current position
    double deltaX = 128 - _displayedRect.width();
    if (deltaX == 0)
        _posX = 0.5;
    else
        _posX = (_displayedRect.left() + 0.5) / deltaX;
    double deltaY = 128 - _displayedRect.height();
    if (deltaY == 0)
        _posY = 0.5;
    else
        _posY = (_displayedRect.top() + 0.5) / deltaY;

    // Remember situation
    _xInit = normalizeX(event->x());
    _yInit = normalizeY(event->y());
    _zoomXinit = _zoomX;
    _zoomYinit = _zoomY;
    _posXinit = _posX;
    _posYinit = _posY;

    _moveOccured = false;
    _buttonPressed = event->button();
}

void GraphicsViewRange::mouseReleaseEvent(QMouseEvent *event)
{
    if (_buttonPressed != event->button())
        return;

    if (!_currentRectangles.isEmpty())
    {
        if (_moveOccured)
        {
            /// Save the changes
            _sf2->prepareNewActions(false); // False because we can't the id again at this stage, if some old actions delete items
            foreach (GraphicsRectangleItem * item, _currentRectangles)
                item->saveChanges();
            _mainWindow->updateDo();

            updateKeyboard();
        }
        else
        {
            /// Search other rectangles to select

            // Rectangles under the mouse
            QList<GraphicsRectangleItem *> rectanglesUnderMouse;
            foreach (GraphicsRectangleItem * item, _rectangles)
                if (item->contains(this->mapToScene(event->pos())))
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

            // Current index of the pair
            int index = -1;
            for (int i = 0; i < pairs.count(); i++)
                if (pairs[i].contains(_currentRectangles.first()))
                    index = i;

            setCurrentRectangles(pairs[(index + 1) % pairs.count()]);
        }
    }

    this->setZoomLine(-1, 0, 0, 0);
    this->setCursor(Qt::ArrowCursor);
    _mouseMode = MOUSE_MODE_NONE;
    _buttonPressed = Qt::NoButton;
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
            updateLabels();
        }

        // Rectangles under the mouse
        QList<GraphicsRectangleItem *> rectanglesUnderMouse;
        foreach (GraphicsRectangleItem * item, _rectangles)
            if (item->contains(this->mapToScene(event->pos())))
                rectanglesUnderMouse << item;

        // Current selection already within the rectangles?
        bool alreadySelected = !_currentRectangles.isEmpty();
        foreach (GraphicsRectangleItem * item, _currentRectangles)
            alreadySelected &= rectanglesUnderMouse.contains(item);

        if (!alreadySelected)
        {
            QList<GraphicsRectangleItem*> rectanglesToSelect;

            if (!rectanglesUnderMouse.isEmpty())
            {
                // Select the first element with its brother if any
                rectanglesToSelect << rectanglesUnderMouse.first();
                EltID idBrother = rectanglesToSelect[0]->findBrother();
                if (idBrother.typeElement == elementInstSmpl)
                {
                    foreach (GraphicsRectangleItem * item, rectanglesUnderMouse)
                        if (*item == idBrother)
                            rectanglesToSelect << item;
                }
            }

            setCurrentRectangles(rectanglesToSelect);
        }
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
    if (_displayedRect.left() < -0.5)
        _displayedRect.setLeft(-0.5);
    if (_displayedRect.right() > 127.5)
        _displayedRect.setRight(127.5);
    if (_displayedRect.top() < -0.5)
        _displayedRect.setTop(-0.5);
    if (_displayedRect.bottom() > 127.5)
        _displayedRect.setBottom(127.5);

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
    double etendueX = _scene->width() / _zoomX;
    double offsetX = (_scene->width() - etendueX) * _posX - 0.5;
    double etendueY = _scene->height() / _zoomY;
    double offsetY = (_scene->height() - etendueY) * _posY - 0.5;
    _displayedRect.setRect(offsetX, offsetY, etendueX, etendueY);

    // Mise à jour
    _dontRememberScroll = true;
    this->fitInView(_displayedRect);
    _dontRememberScroll = false;
    updateLabels();
}

void GraphicsViewRange::setZoomLine(double x1, double y1, double x2, double y2)
{
    Q_UNUSED(x1)
    Q_UNUSED(y1)
    Q_UNUSED(x2)
    Q_UNUSED(y2)
}

void GraphicsViewRange::setCurrentRectangles(QList<GraphicsRectangleItem*> rectanglesToSelect)
{
    // Update the colors
    foreach (GraphicsRectangleItem * item, _currentRectangles)
        if (!rectanglesToSelect.contains(item))
            item->setHover(false);
    foreach (GraphicsRectangleItem * item, rectanglesToSelect)
        if (!_currentRectangles.contains(item))
            item->setHover(true);

    // Remember the current rectangles
    _currentRectangles = rectanglesToSelect;

    // Update legend text
    QList<EltID> ids;
    foreach (GraphicsRectangleItem * item, _currentRectangles)
        ids << item->getID();
    _legendItem->setIds(ids);

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

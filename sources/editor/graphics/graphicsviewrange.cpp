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

#include "graphicsviewrange.h"
#include "soundfontmanager.h"
#include "contextmanager.h"
#include "graphicssimpletextitem.h"
#include "graphicsrectangleitem.h"
#include "graphicslegenditem.h"
#include "graphicslegenditem2.h"
#include "graphicszoomline.h"
#include "graphicskey.h"
#include <QScrollBar>
#include <QMouseEvent>
#include <QApplication>

const double GraphicsViewRange::WIDTH = 128.0;
const double GraphicsViewRange::MARGIN = 0.5;
const double GraphicsViewRange::OFFSET = -0.5;

// Z values (all GraphicsItem being at the top level)
//   0: grid
//  50: non selected rectangles
//  51: selected rectangles
//  80: play marker
// 100: axis values
// 120: legends
// 150: zoom line

GraphicsViewRange::GraphicsViewRange(QWidget *parent) : QGraphicsView(parent),
    _sf2(SoundfontManager::getInstance()),
    _scene(new QGraphicsScene(OFFSET, OFFSET, WIDTH, WIDTH)),
    _legendItem(nullptr),
    _legendItem2(nullptr),
    _zoomLine(nullptr),
    _dontRememberScroll(false),
    _keyTriggered(-1),
    _removeIndexOnRelease(false),
    _removeOtherIndexOnRelease(false),
    _editing(false),
    _keepFirstShiftPoint(false),
    _buttonPressed(Qt::NoButton),
    _moveOccured(false),
    _zoomX(1.),
    _zoomY(1.),
    _posX(0.5),
    _posY(0.5),
    _displayedRect(OFFSET, OFFSET, WIDTH, WIDTH),
    _firstShiftRect(-100, 0, 0, 0)
{
    // Colors
    _textColor = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT);
    _textColor.setAlpha(180);
    _lineColor = ContextManager::theme()->getColor(ThemeManager::LIST_ALTERNATIVE_BACKGROUND);

    // Configuration
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    this->setRenderHint(QPainter::Antialiasing, true);
    this->setMouseTracking(true);

    // Preparation of the graphics
    this->setScene(_scene);
    this->initItems();

    this->installEventFilter(this);
}

GraphicsViewRange::~GraphicsViewRange()
{
    while (!_rectangles.isEmpty())
        delete _rectangles.takeFirst();
    while (!_leftLabels.isEmpty())
        delete _leftLabels.takeFirst();
    while (!_bottomLabels.isEmpty())
        delete _bottomLabels.takeFirst();
    delete _legendItem;
    delete _legendItem2;
    delete _zoomLine;
    while (!_lines.isEmpty())
        delete _lines.takeFirst();
    while (!_mapGraphicsKeys.isEmpty())
        delete _mapGraphicsKeys.take(_mapGraphicsKeys.keys().first());
    delete _scene;
}

void GraphicsViewRange::initItems()
{
    // Vertical lines
    QBrush brushLines(_lineColor);
    QPen penLines(_lineColor);
    penLines.setCosmetic(true);
    for (quint32 note = 12; note <= 121; note += 12)
    {
        QGraphicsRectItem * line = new QGraphicsRectItem(note - 0.5, OFFSET - MARGIN, 1, WIDTH + 2 * MARGIN);
        _scene->addItem(line);
        line->setBrush(brushLines);
        line->setPen(penLines);
        line->setZValue(0);
        GraphicsSimpleTextItem * text = new GraphicsSimpleTextItem(Qt::AlignHCenter | Qt::AlignBottom);
        _scene->addItem(text);
        text->setZValue(100);
        text->setBrush(_textColor);
        text->setText(ContextManager::keyName()->getKeyName(note));
        text->setPos(note, OFFSET + WIDTH);
        _bottomLabels << text;
        _lines << line;
    }

    // Horizontal lines
    for (int vel = 10; vel <= 121; vel += 10)
    {
        QGraphicsRectItem * line = new QGraphicsRectItem(OFFSET - MARGIN, 126.5 - vel, WIDTH + 2 * MARGIN, 1);
        _scene->addItem(line);
        line->setBrush(brushLines);
        line->setPen(penLines);
        line->setZValue(0);
        GraphicsSimpleTextItem * text = new GraphicsSimpleTextItem(Qt::AlignLeft | Qt::AlignVCenter);
        _scene->addItem(text);
        text->setZValue(100);
        text->setBrush(_textColor);
        text->setText(QString::number(vel));
        text->setPos(OFFSET, 126.5 - vel);
        _leftLabels << text;
        _lines << line;
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

void GraphicsViewRange::updateLabelPosition()
{
    // Current rect
    QRectF rect = getCurrentRect();

    // Update the position of the axis labels (they stay to the left and bottom)
    foreach (GraphicsSimpleTextItem * label, _leftLabels)
        label->setX(qMax(OFFSET, rect.x()));
    foreach (GraphicsSimpleTextItem * label, _bottomLabels)
        label->setY(qMin(WIDTH + OFFSET, rect.y() + rect.height()));

    // Update the position of the legend (it stays on a corner)
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
}

void GraphicsViewRange::updateHover(QPoint mousePos)
{
    // Rectangles under the mouse
    QList<QList<GraphicsRectangleItem *> > pairs = getRectanglesUnderMouse(mousePos);

    // Highlighted rectangles under the mouse
    QList<GraphicsRectangleItem*> hoveredRectangles;
    int focussedIndex = 0;
    if (pairs.count() > 0)
    {
        int focussedAndSelected = -1;
        int focussed = -1;
        int selected = -1;
        for (int i = 0; i < pairs.count(); i++)
        {
            foreach (GraphicsRectangleItem * item, pairs[i])
            {
                if (item->isHovered())
                {
                    if (item->isSelected())
                        focussedAndSelected = i;
                    else
                        focussed = i;
                }
                else
                {
                    if (item->isSelected())
                        selected = i;
                }
            }
        }

        // Priority for the focussed rectangles, then selected
        focussedIndex = (focussedAndSelected != -1 ? focussedAndSelected : (focussed != -1 ? focussed : (selected != -1 ? selected : 0)));
        hoveredRectangles = pairs[focussedIndex];
    }

    // Update the hover, get the type of editing in the same time
    GraphicsRectangleItem::EditingMode editingMode = GraphicsRectangleItem::NONE;
    foreach (GraphicsRectangleItem * item, _rectangles)
    {
        if (hoveredRectangles.contains(item))
        {
            item->setHover(true, mousePos);
            editingMode = item->getHoverType();
        }
        else
            item->setHover(false);
    }

    // Adapt the cursor depending on the way we can edit the rectangle
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

    // Update the content of the first legend
    QList<EltID> ids;
    QList<int> selectedIds;
    int count = 0;
    for (int i = 0; i < pairs.count(); i++)
    {
        foreach (GraphicsRectangleItem * item, pairs[i])
        {
            ids << item->getID();
            if (i == focussedIndex)
                selectedIds << count;
            count++;
        }
    }
    _legendItem->setIds(ids, selectedIds, focussedIndex, pairs.count());

    // Offset and location of the second legend
    _legendItem2->setOffsetY(_legendItem->boundingRect().bottom());
}

void GraphicsViewRange::display(IdList ids, bool justSelection)
{
    if (!justSelection)
    {
        // Clear previous rectangles
        while (!_rectangles.isEmpty())
        {
            _scene->removeItem(_rectangles.first());
            delete _rectangles.takeFirst();
        }

        // Add new ones
        _defaultID = ids[0];
        EltID idDiv = ids[0];
        switch (_defaultID.typeElement)
        {
        case elementInst: case elementInstSmpl:
            _defaultID.typeElement = elementInst;
            idDiv.typeElement = elementInstSmpl;
            break;
        case elementPrst: case elementPrstInst:
            _defaultID.typeElement = elementPrst;
            idDiv.typeElement = elementPrstInst;
            break;
        default:
            return;
        }

        foreach (int i, _sf2->getSiblings(idDiv))
        {
            idDiv.indexElt2 = i;
            GraphicsRectangleItem * item = new GraphicsRectangleItem(idDiv);
            item->setZValue(50);
            _scene->addItem(item);
            _rectangles << item;
        }

        updateLabelPosition();
    }

    // Selection
    foreach (GraphicsRectangleItem * item, _rectangles)
    {
        if (ids.contains(item->getID()))
        {
            item->setSelected(true);
            if (!_keepFirstShiftPoint)
                _firstShiftRect = item->rect();
        }
        else
            item->setSelected(false);
    }

    viewport()->update();
}

void GraphicsViewRange::showEvent(QShowEvent * event)
{
    QGraphicsView::showEvent(event);
    updateLabelPosition();
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
    {
        // Cancel the current editing
        if (_buttonPressed == Qt::LeftButton && _editing)
        {
            _editing = false;
            foreach (GraphicsRectangleItem * item, _rectangles)
            {
                if (item->isSelected())
                    item->cancelChanges();
            }

            _moveOccured = false;
            _buttonPressed = Qt::NoButton;
            _legendItem2->setNewValues(-1, 0, 0, 0);
            updateHover(event->pos());
            viewport()->update();
        }

        // Stop here
        return;
    }

    if (event->button() == Qt::MiddleButton)
    {
        // Trigger a key
        QPointF p = this->mapToScene(event->pos());
        int key = qRound(p.x());
        int velocity = 127 - qRound(p.y());
        if (velocity > 0)
        {
            ContextManager::midi()->processKeyOn(-1, key, velocity);
            _keyTriggered = key;
        }
    }
    else if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton)
    {
        // Update current position
        double deltaX = WIDTH - _displayedRect.width();
        if (deltaX < 0.5)
            _posX = 0.5;
        else
            _posX = (_displayedRect.left() - OFFSET) / deltaX;
        double deltaY = WIDTH - _displayedRect.height();
        if (deltaY < 0.5)
            _posY = 0.5;
        else
            _posY = (_displayedRect.top() - OFFSET) / deltaY;

        // Remember situation
        _xInit = normalizeX(event->pos().x());
        _yInit = normalizeY(event->pos().y());
        _zoomXinit = _zoomX;
        _zoomYinit = _zoomY;
        _posXinit = _posX;
        _posYinit = _posY;

        if (event->button() == Qt::LeftButton)
        {
            // Rectangles below the mouse
            QList<QList<GraphicsRectangleItem*> > pairs = getRectanglesUnderMouse(event->pos());

            if (pairs.empty())
            {
                if (QApplication::keyboardModifiers() != Qt::ControlModifier && QApplication::keyboardModifiers() != Qt::ShiftModifier)
                {
                    // Remove the selection
                    bool reselect = false;
                    foreach (GraphicsRectangleItem * item, _rectangles)
                    {
                        if (item->isSelected())
                        {
                            item->setSelected(false);
                            reselect = true;
                        }
                    }

                    // Update the selection outside the range editor
                    if (reselect)
                        triggerDivisionSelected();
                }
            }
            else
            {
                _editing = true;
                GraphicsRectangleItem::syncHover(true);

                // Get the highlighted pair, or the first one
                int currentPairIndex = -1;
                for (int i = 0; i < pairs.count(); i++)
                {
                    foreach (GraphicsRectangleItem * item, pairs[i])
                    {
                        if (item->isHovered())
                        {
                            currentPairIndex = i;
                            break;
                        }
                    }
                    if (currentPairIndex != -1)
                        break;
                }
                if (currentPairIndex == -1)
                    currentPairIndex = 0;

                // Is the current pair selected?
                bool currentIsSelected = false;
                bool reselect = false;
                foreach (GraphicsRectangleItem * item, pairs[currentPairIndex])
                {
                    if (item->isSelected())
                    {
                        currentIsSelected = true;

                        // Possibly complete the selection
                        if (ContextManager::configuration()->getValue(ConfManager::SECTION_NONE, "stereo_modification", true).toBool())
                        {
                            // Select the whole pair
                            foreach (GraphicsRectangleItem * item, pairs[currentPairIndex])
                            {
                                if (!item->isSelected())
                                {
                                    item->setSelected(true);
                                    reselect = true;
                                }
                            }
                        }
                        break;
                    }
                }
                if (reselect)
                    triggerDivisionSelected();

                Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();
                if (modifiers == Qt::ShiftModifier)
                {
                    _removeIndexOnRelease = false;
                    _removeOtherIndexOnRelease = false;

                    // Select all rectangles between the base rectangle and the new one
                    if (_firstShiftRect.x() > -1)
                    {
                        QRectF shiftZone = _firstShiftRect.united(pairs[currentPairIndex][0]->rect());
                        foreach (GraphicsRectangleItem * item, _rectangles)
                            item->setSelected(shiftZone.intersects(item->rect()));
                    }
                    else if (!currentIsSelected)
                    {
                        // Select the current pair
                        foreach (GraphicsRectangleItem * item, pairs[currentPairIndex])
                            item->setSelected(true);
                    }

                    // Update the selection outside the range editor
                    triggerDivisionSelected();
                }
                else if (modifiers == Qt::ControlModifier)
                {
                    _removeIndexOnRelease = currentIsSelected;
                    _removeOtherIndexOnRelease = false;

                    if (!currentIsSelected)
                    {
                        // Select the current pair
                        foreach (GraphicsRectangleItem * item, pairs[currentPairIndex])
                        {
                            item->setSelected(true);
                            _firstShiftRect = item->rect();
                        }

                        // Update the selection outside the range editor
                        triggerDivisionSelected();
                    }
                }
                else // No modifiers
                {
                    _removeIndexOnRelease = false;
                    _removeOtherIndexOnRelease = true;

                    if (!currentIsSelected)
                    {
                        // Deselect everything
                        foreach (GraphicsRectangleItem * item, _rectangles)
                            item->setSelected(false);

                        // Select the current pair
                        foreach (GraphicsRectangleItem * item, pairs[currentPairIndex])
                        {
                            item->setSelected(true);
                            _firstShiftRect = item->rect();
                        }

                        // Update the selection outside the range editor
                        triggerDivisionSelected();
                    }
                }
            }
        }
    }

    _moveOccured = false;
    _buttonPressed = event->button();
    updateHover(event->pos());
    viewport()->update();
}

void GraphicsViewRange::mouseReleaseEvent(QMouseEvent *event)
{
    // Only continue if the button released if the one that initiated the move
    if (_buttonPressed != event->button())
        return;

    if (event->button() == Qt::MiddleButton)
    {
        // Stop a key
        if (_keyTriggered != -1)
        {
            ContextManager::midi()->processKeyOff(-1, _keyTriggered);
            _keyTriggered = -1;
        }
    }
    else if (event->button() == Qt::RightButton)
    {
        if (_moveOccured)
        {
            // Stop zooming
            this->setZoomLine(-1, 0, 0, 0);
        }
        else
        {
            // Possibly change the hover
            QList<QList<GraphicsRectangleItem*> > pairs = getRectanglesUnderMouse(event->pos());
            if (!pairs.empty())
            {
                // Last hovered index
                int lastHoverIndex = 0;
                for (int i = 0; i < pairs.count(); i++)
                {
                    for (int j = 0; j < pairs[i].count(); j++)
                    {
                        if (pairs[i][j]->isHovered())
                        {
                            lastHoverIndex = i;
                            break;
                        }
                    }
                }

                // The next pair has the hover
                lastHoverIndex = (lastHoverIndex + 1) % pairs.count();

                // Change the hovered rectangles
                foreach (GraphicsRectangleItem * rectangle, _rectangles)
                {
                    if (pairs[lastHoverIndex].contains(rectangle))
                        rectangle->setHover(true, event->pos());
                    else
                        rectangle->setHover(false);
                }
            }
        }
    }
    else if (event->button() == Qt::LeftButton)
    {
        if (_moveOccured)
        {
            // Save the changes
            bool withChanges = false;
            foreach (GraphicsRectangleItem * item, _rectangles)
                if (item->isSelected())
                    withChanges |= item->saveChanges();

            if (withChanges)
                _sf2->endEditing("rangeEditor");
        }
        else
        {
            // Change the selection
            QList<QList<GraphicsRectangleItem*> > pairs = getRectanglesUnderMouse(event->pos());
            if (!pairs.empty())
            {
                // Current hovered and selected rectangles under the mouse
                int pairIndex = -1;
                for (int i = 0; i < pairs.count(); i++)
                {
                    for (int j = 0; j < pairs[i].count(); j++)
                    {
                        if (pairs[i][j]->isHovered() && pairs[i][j]->isSelected())
                        {
                            pairIndex = i;
                            break;
                        }
                    }
                    if (pairIndex != -1)
                        break;
                }

                bool reselect = false;

                // Possibly deselect them
                if (_removeIndexOnRelease)
                {
                    for (int i = 0; i < pairs[pairIndex].count(); i++)
                    {
                        if (pairs[pairIndex][i]->isSelected())
                        {
                            pairs[pairIndex][i]->setSelected(false);
                            reselect = true;
                        }
                    }

                    // Update the selection outside the range editor
                    triggerDivisionSelected();
                }

                // Possibly deselect all but them
                if (_removeOtherIndexOnRelease)
                {
                    // Deselect all but the current hovered rectangles
                    for (int i = 0; i < _rectangles.count(); i++)
                    {
                        if (!pairs[pairIndex].contains(_rectangles[i]))
                        {
                            if (_rectangles[i]->isSelected())
                            {
                                _rectangles[i]->setSelected(false);
                                reselect = true;
                            }
                        }
                    }

                    // Update the selection outside the range editor
                    if (reselect)
                        triggerDivisionSelected();
                }
            }
        }
    }

    GraphicsRectangleItem::syncHover(false);
    _legendItem2->setNewValues(-1, 0, 0, 0);
    _editing = false;
    _buttonPressed = Qt::NoButton;
    updateHover(event->pos());
    viewport()->update();
}

void GraphicsViewRange::mouseMoveEvent(QMouseEvent *event)
{
    _moveOccured = true;

    switch (_buttonPressed)
    {
    case Qt::LeftButton: {
        this->setCursor(Qt::ClosedHandCursor);
        if (_editing)
        {
            // Try to move rectangles
            GraphicsRectangleItem * highlightedRectangle = nullptr;
            foreach (GraphicsRectangleItem * item, _rectangles)
            {
                if (item->isSelected())
                {
                    QPointF pointInit = this->mapToScene(
                                static_cast<int>(_xInit * this->width()), static_cast<int>(_yInit * this->height()));
                    QPointF pointFinal = this->mapToScene(event->pos());
                    item->computeNewRange(pointInit, pointFinal);

                    if (item->isHovered())
                        highlightedRectangle = item;
                }
            }

            if (highlightedRectangle != nullptr)
            {
                // Update the modification legend
                _legendItem2->setNewValues(highlightedRectangle->currentMinKey(), highlightedRectangle->currentMaxKey(),
                                           highlightedRectangle->currentMinVel(), highlightedRectangle->currentMaxVel());
            }
        }
        else
        {
            // Drag
            this->drag(event->pos());
        }
    } break;
    case Qt::RightButton:
        this->setCursor(Qt::SizeAllCursor);
        this->setZoomLine(_xInit, _yInit, normalizeX(event->pos().x()), normalizeY(event->pos().y()));
        this->zoom(event->pos());
        break;
    case Qt::NoButton: default: {
        // Section of the legend
        bool isLeft = (event->pos().x() > this->width() / 2);
        if (_legendItem->isLeft() != isLeft)
        {
            _legendItem->setLeft(isLeft);
            _legendItem2->setLeft(isLeft);
            updateLabelPosition();
        }

        // Update legend content
        updateHover(event->pos());
    } break;
    }

    viewport()->update();
}

void GraphicsViewRange::wheelEvent(QWheelEvent * event)
{
    if (_buttonPressed == Qt::NoButton)
        QGraphicsView::wheelEvent(event);
}

void GraphicsViewRange::mouseDoubleClickEvent(QMouseEvent * event)
{
    if (event->button() == Qt::LeftButton)
    {
        // Search the focussed divisions
        QList<QList<GraphicsRectangleItem*> > pairs = getRectanglesUnderMouse(event->pos());
        IdList divIds;
        for (int i = 0; i < pairs.count(); i++)
        {
            for (int j = 0; j < pairs[i].count(); j++)
            {
                if (pairs[i][j]->isHovered())
                {
                    for (int k = 0; k < pairs[i].count(); k++)
                        divIds << pairs[i][k]->getID();
                    break;
                }
                if (!divIds.empty())
                    break;
            }
        }

        if (divIds.empty() && !pairs.isEmpty())
        {
            for (int i = 0; i < pairs[0].count(); i++)
                divIds << pairs[0][i]->getID();
        }

        // Get the child ids
        IdList childIds;
        foreach (EltID divId, divIds)
        {
            EltID childId = divId;
            if (divId.typeElement == elementInstSmpl)
            {
                childId.typeElement = elementSmpl;
                childId.indexElt = _sf2->get(divId, champ_sampleID).wValue;
            }
            else if (divId.typeElement == elementPrstInst)
            {
                childId.typeElement = elementInst;
                childId.indexElt = _sf2->get(divId, champ_instrument).wValue;
            }
            else
                continue;
            childIds << childId;
        }

        // Change the selection
        if (!childIds.empty())
            emit selectionChanged(childIds);
    }
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

    updateLabelPosition();
}

void GraphicsViewRange::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        // Cancel the current editing
        if (_buttonPressed == Qt::LeftButton && _editing)
        {
            _editing = false;
            foreach (GraphicsRectangleItem * item, _rectangles)
            {
                if (item->isSelected())
                    item->cancelChanges();
            }

            _moveOccured = false;
            _buttonPressed = Qt::NoButton;
            _legendItem2->setNewValues(-1, 0, 0, 0);
            updateHover(QPoint(0, 0));
            viewport()->update();
        }
    }

    QGraphicsView::keyPressEvent(event);
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
    updateLabelPosition();
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
        EltID idBrother = GraphicsViewRange::findBrother(listTmp[0]->getID());
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

double GraphicsViewRange::normalizeX(int xPixel)
{
    return static_cast<double>(xPixel) / this->width();
}

double GraphicsViewRange::normalizeY(int yPixel)
{
    return static_cast<double>(yPixel) / this->height();
}

QRectF GraphicsViewRange::getCurrentRect()
{
    QPointF tl(horizontalScrollBar()->value(), verticalScrollBar()->value());
    QPointF br = tl + viewport()->rect().bottomRight();
    QTransform mat = transform().inverted();
    return mat.mapRect(QRectF(tl, br));
}

void GraphicsViewRange::playKey(int key, int velocity)
{
    if (velocity == 0 && _mapGraphicsKeys[key] != nullptr)
    {
        // A key is removed
        delete _mapGraphicsKeys.take(key);
    }
    else if (velocity > 0 && _mapGraphicsKeys[key] == nullptr)
    {
        // A key is added
        _mapGraphicsKeys[key] = new GraphicsKey();
        _scene->addItem(_mapGraphicsKeys[key]);
        _mapGraphicsKeys[key]->setPos(QPoint(key, 127 - velocity));
        _mapGraphicsKeys[key]->setZValue(80);
    }
}

void GraphicsViewRange::triggerDivisionSelected()
{
    IdList ids;
    foreach (GraphicsRectangleItem * item, _rectangles)
        if (item->isSelected())
            ids << item->getID();
    if (ids.empty())
        ids << _defaultID;
    _keepFirstShiftPoint = true;
    emit selectionChanged(ids);
    _keepFirstShiftPoint = false;
}

EltID GraphicsViewRange::findBrother(EltID id)
{
    if (ContextManager::configuration()->getValue(ConfManager::SECTION_NONE, "stereo_modification", true).toBool() &&
            id.typeElement == elementInstSmpl)
    {
        // Sample linked to the division
        SoundfontManager * sm = SoundfontManager::getInstance();
        EltID idSmpl = id;
        idSmpl.typeElement = elementSmpl;
        idSmpl.indexElt = sm->get(id, champ_sampleID).wValue;

        // Sample linked to another one?
        SFSampleLink typeLink = sm->get(idSmpl, champ_sfSampleType).sfLinkValue;
        if (typeLink == rightSample || typeLink == leftSample || typeLink == linkedSample ||
                typeLink == RomRightSample || typeLink == RomLeftSample || typeLink == RomLinkedSample)
        {
            // Characteristics to find in the other divisions
            int numSmpl2 = sm->get(idSmpl, champ_wSampleLink).wValue;
            RangesType keyRange = sm->get(id, champ_keyRange).rValue;
            RangesType velRange = sm->get(id, champ_velRange).rValue;

            // Search the brother
            int numBrothers = 0;
            EltID idBrother = id;
            EltID id2 = id;
            foreach (int i, sm->getSiblings(id))
            {
                id2.indexElt2 = i;
                if (i != id.indexElt2)
                {
                    RangesType keyRange2 = sm->get(id2, champ_keyRange).rValue;
                    RangesType velRange2 = sm->get(id2, champ_velRange).rValue;
                    if (keyRange2.byLo == keyRange.byLo && keyRange2.byHi == keyRange.byHi &&
                            velRange2.byLo == velRange.byLo && velRange2.byHi == velRange.byHi)
                    {
                        int iTmp = sm->get(id2, champ_sampleID).wValue;
                        if (iTmp == idSmpl.indexElt)
                            numBrothers = 2; // ambiguity, another sample is like id
                        else if (iTmp == numSmpl2)
                        {
                            numBrothers++;
                            idBrother.indexElt2 = i;
                        }
                    }
                }
            }

            // Return the brother if there is only one (no ambiguity)
            if (numBrothers == 1)
                return idBrother;
            else
                return EltID(elementUnknown, 0, 0, 0, 0);
        }
        else
            return EltID(elementUnknown, 0, 0, 0, 0);
    }
    else
        return EltID(elementUnknown, 0, 0, 0, 0);
}

bool GraphicsViewRange::event(QEvent* event)
{
    if (event->type() == QEvent::NativeGesture)
    {
        QNativeGestureEvent * nge = dynamic_cast<QNativeGestureEvent *>(event);
        if (nge->gestureType() == Qt::ZoomNativeGesture)
        {
            if (QApplication::keyboardModifiers() == Qt::ShiftModifier)
            {
                _zoomYinit = _zoomY;
                _zoomY = _zoomY * (1 + nge->value());

                if (_zoomY > 1)
                {
                    // WARNING: not using nge->position() since it seems to return the global position
#if QT_VERSION < 0x060000
                    _yInit = (nge->globalPos().y() - this->mapToGlobal(QPoint(0, 0)).y()) / this->height();
#else
                    _yInit = (nge->globalPosition().y() - this->mapToGlobal(QPoint(0, 0)).y()) / this->height();
#endif
                    _posYinit = _posY;
                    _posY = (_zoomY * _posYinit * (_zoomYinit - 1) +
                             _yInit * (_zoomY - _zoomYinit)) / (_zoomYinit * (_zoomY - 1));
                }
            }
            else
            {
                _zoomXinit = _zoomX;
                _zoomX = _zoomX * (1 + nge->value());

                if (_zoomX > 1)
                {
                    // WARNING: not using nge->position() since it seems to return the global position
#if QT_VERSION < 0x060000
                    _xInit = (nge->globalPos().x() - this->mapToGlobal(QPoint(0, 0)).x()) / this->width();
#else
                    _xInit = (nge->globalPosition().x() - this->mapToGlobal(QPoint(0, 0)).x()) / this->width();
#endif
                    _posXinit = _posX;
                    _posX = (_zoomX * _posXinit * (_zoomXinit - 1) +
                             _xInit * (_zoomX - _zoomXinit)) / (_zoomXinit * (_zoomX - 1));
                }
            }

            this->zoomDrag();
        }
    }
    return QGraphicsView::event(event);
}

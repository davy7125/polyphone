/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef GRAPHICSVIEWRANGE_H
#define GRAPHICSVIEWRANGE_H

#include <QGraphicsView>
#include "soundfontmanager.h"
#include <QMap>
#include "basetypes.h"
class GraphicsSimpleTextItem;
class GraphicsRectangleItem;
class GraphicsLegendItem;
class GraphicsLegendItem2;
class GraphicsZoomLine;
class GraphicsKey;

class GraphicsViewRange : public QGraphicsView
{
    Q_OBJECT

public:
    explicit GraphicsViewRange(QWidget *parent = nullptr);
    ~GraphicsViewRange();

    void display(IdList ids, bool justSelection);
    void playKey(int key, int velocity);

signals:
    void updateKeyboard();
    void divisionsSelected(IdList divisions);

protected:
    void resizeEvent(QResizeEvent * event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent * event);
    void mouseDoubleClickEvent(QMouseEvent *) {}
    void scrollContentsBy(int dx, int dy);

private:
    void initItems();
    void updateLabelPosition();
    void updateHover(QPoint mousePos);
    QList<QList<GraphicsRectangleItem*> > getRectanglesUnderMouse(QPoint mousePos);
    QRectF getCurrentRect();
    void triggerDivisionSelected();

    SoundfontManager * _sf2;
    EltID _defaultID;

    // Graphics items
    QGraphicsScene * _scene;
    QList<GraphicsRectangleItem *> _rectangles;
    QList<GraphicsSimpleTextItem *> _leftLabels, _bottomLabels;
    GraphicsLegendItem * _legendItem;
    GraphicsLegendItem2 * _legendItem2;
    GraphicsZoomLine * _zoomLine;
    QMap<int, GraphicsKey*> _mapGraphicsKeys;
    QList<QGraphicsLineItem *> _keyLines;

    // Various
    bool _dontRememberScroll;
    int _keyTriggered;
    bool _keepIndexOnRelease;
    bool _editing;

    // Drag & zoom
    Qt::MouseButton _buttonPressed;
    bool _moveOccured;
    double _xInit, _yInit;
    double _zoomX, _zoomY, _posX, _posY;
    double _zoomXinit, _zoomYinit, _posXinit, _posYinit;
    QRectF _displayedRect;
    QVector<GraphicsRectangleItem *> _shiftRectangles;
    void zoom(QPoint point);
    void drag(QPoint point);
    void zoomDrag();
    void setZoomLine(double x1Norm, double y1Norm, double x2Norm, double y2Norm);
    double normalizeX(int xPixel);
    double normalizeY(int yPixel);

    static const double WIDTH;
    static const double MARGIN;
    static const double OFFSET;
    QColor _lineColor;
    QColor _textColor;
};

#endif // GRAPHICSVIEWRANGE_H

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

#ifndef GRAPHICSVIEWRANGE_H
#define GRAPHICSVIEWRANGE_H

#include <QGraphicsView>
#include "pile_sf2.h"
#include "mainwindow.h"
class GraphicsSimpleTextItem;

class GraphicsViewRange : public QGraphicsView
{
public:
    explicit GraphicsViewRange(QWidget *parent = 0);
    ~GraphicsViewRange();

    void init(Pile_sf2 * sf2, MainWindow * mainWindow)
    {
        _sf2 = sf2;
        _mainWindow = mainWindow;
    }

    void display(EltID id);

protected:
    void resizeEvent(QResizeEvent * event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    void initGridAndAxes();
    void display(QList<QRectF> rectangles);

    Pile_sf2 * _sf2;
    MainWindow * _mainWindow;
    QGraphicsScene * _scene;
    QList<QGraphicsRectItem *> _rectangles;
    QList<GraphicsSimpleTextItem *> _leftLabels, _bottomLabels;

    // Drag & zoom
    enum MouseMode
    {
        MOUSE_MODE_NONE,
        MOUSE_MODE_DRAG,
        MOUSE_MODE_ZOOM
    };
    MouseMode _mouseMode;
    double _xInit, _yInit;
    double _zoomX, _zoomY, _posX, _posY;
    double _zoomXinit, _zoomYinit, _posXinit, _posYinit;
    QRectF _displayedRect;
    void zoom(QPoint point);
    void drag(QPoint point);
    void zoomDrag();
    void setZoomLine(double x1, double y1, double x2, double y2);
    double normalizeX(int xPixel);
    double normalizeY(int yPixel);
};

#endif // GRAPHICSVIEWRANGE_H

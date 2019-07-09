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

#ifndef GRAPHICSVIEWENVELOPS_H
#define GRAPHICSVIEWENVELOPS_H

#include "qcustomplot.h"
#include "soundfontmanager.h"
#include "envelop.h"

class GraphicsViewEnvelop : public QCustomPlot
{
    Q_OBJECT

public:
    explicit GraphicsViewEnvelop(QWidget *parent = nullptr);
    ~GraphicsViewEnvelop();

    // Link the scroll bar
    void linkSliderX(QScrollBar * _qScrollX);

    // Clear all envelops
    void clearEnvelops();

    // Prepare an envelop and return the index
    int addEnvelop();

    // Set the value of an envelop
    void setValue(int index, Envelop::ValueType type, double value, bool isDefined = true);
    void setKeyRange(int index, int keyMin, int keyMax);
    void setSample(QVector<double> data, int sampleRate, int loopMode, int startLoop, int endLoop);

    // Set the style of an envelop
    void setEnvelopStyle(int index, bool isGlobal, bool isVolume, bool isMain);

    // Compute all points of all envelops
    void drawEnvelops();
    void zoomDrag();

public slots:
    void setPosX(int _posX);

signals:
    void valueChanged(int index, int type, double value, bool isDefined);

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *) {}
    void wheelEvent(QWheelEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    int _index;
    QMap<int, Envelop *> _envelops;
    bool _dontRememberScroll;
    double _triggeredKeyDuration, _releasedKeyDuration;
    int _posReleaseLine;
    QPen _releaseLinePen;
    QImage _imageNoteOn, _imageNoteOff;

    double _sizeX;
    bool _zoomFlag;
    bool _dragFlag;
    double _xInit, _yInit;
    double _zoomX, _posX;
    double _zoomXinit, _posXinit;
    bool _bFromExt;
    QScrollBar * _qScrollX;
    QCPItemText * _textPositionL;
    QCPItemText * _textPositionR;
    QSharedPointer<QCPAxisTickerFixed> _fixedTicker;

    double getTickStep();
    void updateStyle();
    void zoom(QPoint point);
    void drag(QPoint point);
    void setZoomLine(double x1, double y1, double x2, double y2);
    void displayCurrentRange();
};

#endif // GRAPHICSVIEWENVELOPS_H

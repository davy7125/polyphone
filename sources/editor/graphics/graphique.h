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

#ifndef GRAPHIQUE_H
#define GRAPHIQUE_H

#include "qcustomplot.h"
class QScrollBar;
class QSpinBox;


class Graphique : public QCustomPlot
{
    Q_OBJECT
public:
    explicit Graphique(QWidget *parent = nullptr);
    void clearAll();
    void setData(QByteArray baData, int sampleRate);
    void linkSliderX(QScrollBar * _qScrollX);
    void linkSpinBoxes(QSpinBox * _spinStart, QSpinBox * _spinEnd);
    void zoomDrag();
    void displayMultipleSelection(bool isOn);

public slots:
    void setPosX(int _posX);
    void setStartLoop(int pos, bool replot = true);
    void setEndLoop(int pos, bool replot = true);
    void setCurrentSample(quint32 pos);

signals:
    void startLoopChanged();
    void endLoopChanged();
    void cutOrdered(int start, int end);

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
    QPen _penLecture;
    double _sizeX;
    bool _zoomFlag;
    bool _dragFlag;
    bool _cutFlag;
    bool _modifiedFlag;
    double _xInit, _yInit;
    double _zoomX, _zoomY, _posX, _posY;
    double _zoomXinit, _zoomYinit, _posXinit, _posYinit;
    bool _bFromExt;
    QScrollBar * _qScrollX;
    QSpinBox * _spinStart;
    QSpinBox * _spinEnd;
    int _currentPos;
    bool _filterEventEnabled;
    QCPItemText * _textMultipleSelection;
    QCPItemText * _textPositionL;
    QCPItemText * _textPositionR;
    QCPItemRect * _cutArea;
    QTime _lastUpdate;
    int _sampleRate;

    void updateStyle();
    void zoom(QPoint point);
    void drag(QPoint point);
    void setZoomLine(double x1, double y1, double x2, double y2);
    void plotOverlay();
    void displayCurrentRange();
    int getSamplePosX(double zoomX, double shiftX, double x);
    void cut(int start, int end);
};

#endif // GRAPHIQUE_H

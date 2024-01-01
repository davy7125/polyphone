/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2020 Davy Triponney                                **
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

#include <QWidget>
#include <QPen>
#include "envelop.h"
class GraphicsWavePainter;
class QScrollBar;

class GraphicsViewEnvelop : public QWidget
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
    void setSample(QVector<float> vData, int sampleRate, int loopMode, int startLoop, int endLoop);

    // Set the style of an envelop
    void setEnvelopStyle(int index, bool isGlobal, bool isVolume, bool isMain);

    // Compute all points of all envelops
    void computeEnvelops();

public slots:
    void setPosX(int _posX);

signals:
    void valueChanged(int index, int type, double value, bool isDefined);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *) override {}
    void wheelEvent(QWheelEvent *event) override;
    bool event(QEvent* event) override;

private:
    GraphicsWavePainter * _wavePainter;
    int _sampleRate;

    int _index;
    QMap<int, Envelop *> _envelops;
    bool _dontRememberScroll;
    double _triggeredKeyDuration, _releasedKeyDuration;

    QPen _releaseLinePen, _gridPen, _zoomLinePen;
    QImage _imageNoteOn, _imageNoteOff;
    QColor _backgroundColor, _textColor, _redColor;

    double _sizeX;
    bool _zoomFlag;
    bool _dragFlag;
    double _xInit, _yInit, _x, _y;
    double _zoomX, _posX;
    double _zoomXinit, _posXinit;
    bool _bFromExt;
    QScrollBar * _qScrollX;

    double getTickStep();
    void zoom(QPoint point);
    void drag(QPoint point);
    QString doubleToString(double value, bool accurate);
};

#endif // GRAPHICSVIEWENVELOPS_H

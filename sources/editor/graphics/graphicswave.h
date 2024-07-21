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

#ifndef GRAPHICSWAVE_H
#define GRAPHICSWAVE_H

#include <QWidget>
#include <QDateTime>
class QScrollBar;
class QSpinBox;
class GraphicsWavePainter;

class GraphicsWave : public QWidget
{
    Q_OBJECT
public:
    explicit GraphicsWave(QWidget *parent = nullptr);
    ~GraphicsWave() override;

    void setData(const QVector<float> &vData, quint32 sampleRate);
    void linkSliderX(QScrollBar * qScrollX);
    void linkSpinBoxes(QSpinBox * spinStart, QSpinBox * spinEnd);
    void displayMultipleSelection(bool isOn);

public slots:
    void setPosX(int posX);
    void setStartLoop(int pos, bool repaint = true);
    void setEndLoop(int pos, bool repaint = true);
    void setCurrentSample(quint32 pos);

signals:
    void startLoopChanged();
    void endLoopChanged();
    void cutOrdered(int start, int end);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    bool event(QEvent* event) override;

private:
    int getSamplePosX(double zoomX, double shiftX, double x);
    void zoom(QPoint point);
    void drag(QPoint point);

    GraphicsWavePainter * _wavePainter;

    // Zoom & position
    double _x, _y, _zoomX, _zoomY, _posX;
    double _xInit, _yInit, _zoomXinit, _zoomYinit, _posXinit;
    bool _zoomFlag;
    bool _dragFlag;
    bool _cutFlag;
    bool _modifiedFlag;
    double _sizeX;

    // Input data properties
    quint32 _sampleSize;
    quint32 _sampleRate;

    // Loop, playback position
    quint32 _startLoop, _endLoop, _currentPosition;

    bool _multipleSelection;
    QScrollBar * _qScrollX;
    QSpinBox * _spinStart;
    QSpinBox * _spinEnd;
    bool _bFromExt;

    // Style
    QColor _redColor, _greenColor, _backgroundColor, _textColor, _gridColor;
    QFont _textFont;

    static const int TEXT_MARGIN;
    static const int OVERLAY_SIZE;
};

#endif // GRAPHICSWAVE_H

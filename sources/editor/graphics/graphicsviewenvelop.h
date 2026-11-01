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

#ifndef GRAPHICSVIEWENVELOPS_H
#define GRAPHICSVIEWENVELOPS_H

#include <QWidget>
#include <QPen>
#include <QElapsedTimer>
#include "envelop.h"
class GraphicsWavePainter;
class QScrollBar;
class QTimer;

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

    // Set / get the value of an envelop
    void setValue(int index, Envelop::ValueType type, double value, bool isDefined = true);
    double getValue(int index, Envelop::ValueType type);
    void setKeyRange(int index, int keyMin, int keyMax);
    void setSample(QVector<float> vData, int sampleRate, int loopMode, int startLoop, int endLoop);
    void clearSample();

    // Set the style of an envelop
    // The main envelops can be edited with the mouse
    void setEnvelopStyle(int index, bool isGlobal, bool isVolume, bool isMain);
    void setEnvelopName(int index, QString name);

    // Bounds of a value when dragging the points
    void setValueRange(Envelop::ValueType type, double min, double max);

    // Text describing a value in the legend
    void setValueText(Envelop::ValueType type, QString label, QString suffix, int decimals);

    // Compute all points of all envelops
    void computeEnvelops();

    // Stop a possible edition, without notification
    // Return true if values have been changed
    bool abortEdition();

public slots:
    void setPosX(int _posX);

signals:
    // The values of all main envelops are being changed by dragging a point
    void valuesEdited(QList<int> valueTypes);
    void editionFinished();
    void editionCancelled();

    // Values of all main envelops to restore (double click on a point)
    void resetRequested(QList<int> valueTypes);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    bool event(QEvent* event) override;

private slots:
    void onEdgeTimer();

private:
    void computeSampleData();
    void getDisplayedRange(double &xOffset, double &xRange);
    void updateHandles();
    QString getHoverState();
    void updateCursor();
    void startEdition(QPoint pos);
    void editValues();
    void stopEdition(bool isCancelled);
    void scaleForKey(QMap<Envelop::ValueType, double> &values, Envelop::ValueType type, Envelop::ValueType keyType,
                     double key, double duration);
    void solveForKeys(QMap<Envelop::ValueType, double> &values, Envelop::ValueType type, Envelop::ValueType keyType,
                      double key, double duration, double pivotKey);
    void limitValues(QMap<Envelop::ValueType, double> &values);
    void updateEditionLayout();
    QStringList getLegendLines(QStringList &values);
    void drawLegend(QPainter * painter);
    QString valueToString(Envelop * env, Envelop::ValueType type, double value);
    static QList<Envelop::ValueType> getValueTypes(Envelop::HandleType handle);

    GraphicsWavePainter * _wavePainter;
    int _sampleRate;
    double _sampleDataDuration;
    QVector<float> _sampleData;
    int _loopMode, _startLoop, _endLoop;

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

    // Hovered point
    QPoint _mousePos;
    bool _mouseInside;
    int _hoveredEnvelop;
    Envelop::HandleType _hoveredHandle;

    // Point being dragged
    int _editedEnvelop;
    Envelop::HandleType _editedHandle;
    bool _valuesEdited;
    bool _axisXActive, _axisYActive;
    bool _pivotOnCenter;
    QPoint _editStartPos, _editLastPos;
    QPointF _editStartCoord, _editCoord; // Seconds and level
    QMap<int, QMap<Envelop::ValueType, double> > _editStartValues;
    double _editOffset, _editRange; // Visible part (seconds), kept during the edition
    double _editTriggeredKeyDuration, _editReleasedKeyDuration, _editSizeX, _editZoomX, _editPosX;
    QTimer * _edgeTimer;
    int _edgeDirection;
    QElapsedTimer _sampleDataTimer;

    // Bounds and texts of the values
    QMap<Envelop::ValueType, QPair<double, double> > _valueRanges;
    QMap<Envelop::ValueType, QString> _valueLabels;
    QMap<Envelop::ValueType, QString> _valueSuffixes;
    QMap<Envelop::ValueType, int> _valueDecimals;

    double getTickStep();
    void zoom(QPoint point);
    void drag(QPoint point);
    QString doubleToString(double value, bool accurate);
};

#endif // GRAPHICSVIEWENVELOPS_H

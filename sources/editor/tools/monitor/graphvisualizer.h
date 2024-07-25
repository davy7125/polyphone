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

#ifndef GRAPHVISUALIZER_H
#define GRAPHVISUALIZER_H

#include <QWidget>
#include <QMouseEvent>
#include <QPen>
class SegmentPainter;
class Segment;


class GraphVisualizer : public QWidget
{
    Q_OBJECT

public:
    explicit GraphVisualizer(QWidget *parent = nullptr);
    ~GraphVisualizer();
    void setScale();
    void setData(float defaultValue, bool globalValueSet, QList<Segment*> segments, int minKey, int maxKey);
    void setIsLog(bool isLog);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void mouseMoved(QPoint pos);
    void mouseLeft();

    SegmentPainter * _segmentPainter;
    float _defaultValue;
    bool _globalValueSet;
    QList<Segment *> _segments;

    QColor _backgroundColor, _gridColor, _labelColor, _curveColor, _warningColor, _defaultValueColor;
    QFont _fontLabels, _fontWarning;
    QPen _currentPointPen;

    static const double MIN_LOG;
    int _xMin, _xMax;
    float _fMin, _fMax;
    float _yMin, _yMax;
    bool _isLog;
    bool _dispWarning;
    int _currentKey;
    float _currentValue;
};

#endif // GRAPHVISUALIZER_H

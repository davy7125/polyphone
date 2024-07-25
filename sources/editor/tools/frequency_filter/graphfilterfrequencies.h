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

#ifndef GRAPHFILTERFREQUENCIES_H
#define GRAPHFILTERFREQUENCIES_H

#include <QWidget>
#include <QPen>
class GraphicsWavePainter;

class GraphFilterFrequencies : public QWidget
{
    Q_OBJECT

public:
    explicit GraphFilterFrequencies(QWidget *parent = nullptr);
    ~GraphFilterFrequencies();

    void clearFourierTransforms();
    void addFourierTransform(QVector<float> fData, quint32 sampleRate);
    QVector<float> getValues();
    void setValues(QVector<float> val);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<float> _dValues;
    bool _flagEdit;
    float _stiffnessExp;
    int _previousX;
    float _previousY;
    QList<GraphicsWavePainter *> _wavePainters;
    int _currentFreq;
    float _currentValue;

    QString _kHzUnit;
    QColor _backgroundColor, _labelColor, _removedAreaColor;
    QPen _frequencyPen, _penCurve, _currentPointPen;
    QFont _fontLabels;

    float freqToPos(int freq);
    int posToFreq(int pos);
    float posToValue(int pos);
    float valueToPos(float value);

    void write(QPoint pos);

    static const int POINT_NUMBER;
    static const float MIN_Y;
};

#endif // GRAPHFILTERFREQUENCIES_H

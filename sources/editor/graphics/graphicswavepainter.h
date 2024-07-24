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

#ifndef GRAPHICSWAVEPAINTER_H
#define GRAPHICSWAVEPAINTER_H

#include <QWidget>

class GraphicsWavePainter
{
public:
    GraphicsWavePainter();
    ~GraphicsWavePainter();

    // Set the waveform color
    // Default is not defined
    void setWaveColor(QColor waveColor) { _waveColor = waveColor.rgba(); }

    // If true, values ranging from [0; 1] are displayed
    // If false, values ranging from [-1; 1] are displayed
    // Default is false
    void setOnlyPositiveValues(bool drawPositiveValues) { _drawPositiveValues = drawPositiveValues; }

    // Add a curve representing the mean value for each pixel, making the curve more definite
    // Default is true
    void addMeanCurve(bool addMeanCurve) { _addMeanCurve = addMeanCurve; }

    // Configure the painter with data
    void setData(const QVector<float> &vData);
    void clearData();

    // Draw data
    void paint(QPainter *painter, QRect rect, quint32 start, quint32 end, float zoomY);

    // Get data around a central point
    QPointF * getDataAround(quint32 position, quint32 desiredLength, quint32 &pointNumber);

private:
    void prepareImage(quint32 width, quint32 height);
    float getValueX(float pos1, float value1, float pos2, float value2, float posX);

    // Input data
    QVector<float> _sampleData;

    // Buffered image and associated parameters
    QRgb * _pixels;
    QImage * _image;
    QPointF * _samplePlotMean;
    quint32 _start, _end;
    float _zoomY;

    // Options and colors
    bool _drawPositiveValues, _addMeanCurve;
    QRgb _waveColor;
};

#endif // GRAPHICSWAVEPAINTER_H

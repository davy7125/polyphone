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

#ifndef GRAPHICSWAVEPAINTER_H
#define GRAPHICSWAVEPAINTER_H

#include <QWidget>

class GraphicsWavePainter
{
public:
    GraphicsWavePainter(QWidget * widget);
    ~GraphicsWavePainter();

    // Configure the painter with data
    void setData(QByteArray baData);

    // Draw data
    void paint(quint32 start, quint32 end, float zoomY);

    // Get data around a central point
    QPointF * getDataAround(quint32 position, quint32 desiredLength, quint32 &pointNumber);

private:
    void prepareImage();
    float getValueX(float pos1, float value1, float pos2, float value2, float posX);
    static QRgb mergeRgb(QRgb color1, QRgb color2, float x);

    // Target widget
    QWidget * _widget;

    // Input data
    quint32 _sampleSize;
    qint16 * _sampleData;

    // Buffered image and associated parameters
    QRgb * _pixels;
    QImage * _image;
    QPointF * _samplePlotMean;
    quint32 _start, _end;
    float _zoomY;

    // Colors
    QRgb _backgroundColor, _waveColor, _gridColor, _greenColor, _redColor;
};

#endif // GRAPHICSWAVEPAINTER_H

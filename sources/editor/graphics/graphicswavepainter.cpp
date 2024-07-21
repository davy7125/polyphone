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

#include "graphicswavepainter.h"
#include <QPainter>

static float fastSqrt(float val)
{
    union {
        float f;
        quint32 u;
    } I;
    if (val < 0.0001f)
        return 0.0001f;
    I.f = val;
#define i I.u
    i += 127L << 23; // adjust bias
    i >>= 1; // approximation of square root
#undef i
    return I.f;
}

static quint32 fastCeil(float val)
{
    quint32 i = static_cast<quint32>(val);
    if (i < val)
        i++;
    return i;
}

GraphicsWavePainter::GraphicsWavePainter() :
    _pixels(nullptr),
    _image(nullptr),
    _samplePlotMean(nullptr),
    _drawPositiveValues(false),
    _addMeanCurve(true)
{}

GraphicsWavePainter::~GraphicsWavePainter()
{
    delete _image;
    delete [] _pixels;
    delete [] _samplePlotMean;
}

void GraphicsWavePainter::setData(const QVector<float> &vData)
{
    this->clearData();

    // Store the waveform
    _sampleData = vData;
}

void GraphicsWavePainter::clearData()
{
    delete _image;
    delete [] _pixels;
    delete [] _samplePlotMean;
    _image = nullptr;
    _pixels = nullptr;
    _samplePlotMean = nullptr;
    _sampleData.clear();
}

void GraphicsWavePainter::paint(QPainter * painter, QRect rect, quint32 start, quint32 end, float zoomY)
{
    if (start >= (unsigned int)_sampleData.size())
        start = (unsigned int)_sampleData.size() - 1;
    if (end == 0 || end >= (unsigned int)_sampleData.size())
        end = (unsigned int)_sampleData.size() - 1;
    if (start == end)
        return;

    int width = rect.width();
    int height = rect.height();

    // Possibly update the image
    if (_image == nullptr || _start != start || _end != end || _zoomY != zoomY ||
            _image->width() != width || _image->height() != height)
    {
        delete _image;
        delete [] _pixels;
        _image = nullptr;
        _pixels = nullptr;

        // Store the current parameters
        _start = start;
        _end = end;
        _zoomY = zoomY;

        // Prepare a new image
        prepareImage(width, height);
    }

    // Draw the curve if valid
    if (_image != nullptr)
        painter->drawImage(rect.left(), rect.top(), *_image);
}

void GraphicsWavePainter::prepareImage(quint32 width, quint32 height)
{
    ////////////////////////////////
    /// FIRST STEP: compute data ///
    ////////////////////////////////

    delete [] _samplePlotMean;
    _samplePlotMean = nullptr;

    if (_sampleData.empty())
        return;

    // Wave form larger than the number of pixels
    float * samplePlotMin = new float[width];
    float * samplePlotMax = new float[width];
    _samplePlotMean = new QPointF[width];
    float * samplePlotDeviation = new float[width];

    // Temporary arrays
    float * samplePlotSum = new float[width];
    float * samplePlotSquareSum = new float[width];
    memset(samplePlotSum, 0, width * sizeof(float));
    memset(samplePlotSquareSum, 0, width * sizeof(float));

    // Compute data
    float pointSpace = 1.0f * width / (_end - _start);
    float pointSpaceInv = 1.0f / pointSpace;
    float previousPosition = 0.0f;
    float currentPosition;
    float previousValue = _sampleData[_start];
    float currentValue;
    quint32 previousPixelNumber = 999999;

    for (quint32 i = 1; i <= _end - _start; i++)
    {
        // Current value, current position
        currentValue = _sampleData[_start + i];
        currentPosition = pointSpace * i;

        // Process the segment between {previousPosition, previousValue} and {currentPosition, currentValue}
        float slope = pointSpaceInv * (currentValue - previousValue);
        for (quint32 currentPixelNumber = static_cast<quint32>(previousPosition);
             currentPixelNumber < fastCeil(currentPosition); currentPixelNumber++)
        {
            if (currentPixelNumber >= width)
                continue;

            // Part of the segment crossing pixel {pixelNumber}
            float x1 = currentPixelNumber < previousPosition ? previousPosition : currentPixelNumber;
            float x2 = (currentPixelNumber + 1) > currentPosition ? currentPosition : (currentPixelNumber + 1);
            float y1 = previousValue + (x1 - previousPosition) * slope;
            float y2 = previousValue + (x2 - previousPosition) * slope;

            // Compute the weight and the middle value of the segment
            float weight = x2 - x1;
            float middleValue = 0.5f * (y1 + y2);

            // Compute min / max
            if (currentPixelNumber != previousPixelNumber)
            {
                // First time we are seeing this pixel: min and max or defined
                samplePlotMin[currentPixelNumber] = y1 < y2 ? y1 : y2;
                samplePlotMax[currentPixelNumber] = y1 > y2 ? y1 : y2;
            }
            else
            {
                float minY = y1 < y2 ? y1 : y2;
                if (minY < samplePlotMin[currentPixelNumber])
                    samplePlotMin[currentPixelNumber] = minY;
                float maxY = y1 > y2 ? y1 : y2;
                if (maxY > samplePlotMax[currentPixelNumber])
                    samplePlotMax[currentPixelNumber] = maxY;
            }

            // Sum values with ponderation
            samplePlotSum[currentPixelNumber] += middleValue * weight;
            samplePlotSquareSum[currentPixelNumber] += middleValue * middleValue * weight;

            previousPixelNumber = currentPixelNumber;
        }

        previousPosition = currentPosition;
        previousValue = currentValue;
    }

    // Compute mean, standard deviation, adjust min / max
    float coeff = -_zoomY * static_cast<float>(height) / (_drawPositiveValues ? 1 : 2);
    float offsetY = _drawPositiveValues ? height : 0.5f * height;
    for (quint32 i = 0; i < width; i++)
    {
        _samplePlotMean[i].setX(i);
        _samplePlotMean[i].setY(static_cast<double>(coeff * samplePlotSum[i] + offsetY));
        samplePlotDeviation[i] = coeff * fastSqrt(static_cast<float>(samplePlotSquareSum[i] - samplePlotSum[i] * samplePlotSum[i]));
        samplePlotMin[i] = coeff * samplePlotMin[i] + offsetY;
        samplePlotMax[i] = coeff * samplePlotMax[i] + offsetY;
    }

    // Delete temporary arrays
    delete [] samplePlotSum;
    delete [] samplePlotSquareSum;

    //////////////////////////////////////
    /// SECOND STEP: compute the image ///
    //////////////////////////////////////

    // Prepare a new image
    _pixels = new QRgb[width * height];
    _image = new QImage(reinterpret_cast<uchar*>(_pixels), static_cast<int>(width), static_cast<int>(height), QImage::Format_ARGB32);
    memset(_pixels, 0 /* transparent */, width * height * sizeof(int));

    // Waveform
    float mean, x;
    quint32 currentPixelIndex;
    for (quint32 i = 0; i < width; i++)
    {
        mean = static_cast<float>(_samplePlotMean[i].y());
        for (quint32 j = 0; j < height; j++)
        {
            // Index of the current pixel
            currentPixelIndex = i + j * width;

            x = j < mean ? getValueX(samplePlotMax[i], 0, mean + samplePlotDeviation[i], 1, j) :
                            getValueX(mean - samplePlotDeviation[i], 1, samplePlotMin[i], 0, j);
            if (x > 0)
                _pixels[currentPixelIndex] = static_cast<quint32>(x * 255) << 24 | (_waveColor & RGB_MASK);
        }
    }

    // Possibly add the mean value
    if (_addMeanCurve)
    {
        QPainter painter(_image);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(QPen(QColor::fromRgba(_waveColor), 1.0, Qt::SolidLine));
        painter.drawPolyline(_samplePlotMean, _image->width());
    }

    // Finally, clear data
    delete [] samplePlotMin;
    delete [] samplePlotMax;
    delete [] samplePlotDeviation;
}

float GraphicsWavePainter::getValueX(float pos1, float value1, float pos2, float value2, float posX)
{
    // Condition: pos1 < pos2
    if (posX <= pos1)
        return value1;
    if (posX >= pos2)
        return value2;
    if (pos1 == pos2)
        return 0;//(value1 + value2) / 2;
    return ((posX - pos1) * value2 + (pos2 - posX) * value1) / (pos2 - pos1);
}

QPointF * GraphicsWavePainter::getDataAround(quint32 position, quint32 desiredLength, quint32 &pointNumber)
{
    // Limits
    quint32 leftIndex = position > desiredLength ? position - desiredLength : 0;
    quint32 rightIndex = position + desiredLength;
    if (rightIndex >= (unsigned int)_sampleData.size())
        rightIndex = static_cast<quint32>(_sampleData.size()) - 1;

    if (rightIndex <= leftIndex)
        return nullptr;

    // Create data
    float coeff = -_zoomY * static_cast<float>(_image->height()) / 2;
    float offsetY = 0.5f * static_cast<float>(_image->height());
    pointNumber = rightIndex - leftIndex + 1;
    QPointF * array = new QPointF[pointNumber];
    for (quint32 i = 0; i < pointNumber; i++)
    {
        array[i].setX(leftIndex + i);
        array[i].setY(static_cast<double>(coeff * _sampleData[leftIndex + i] + offsetY));
    }
    return array;
}

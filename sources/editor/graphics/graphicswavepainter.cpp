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

#include "graphicswavepainter.h"
#include <QPainter>

// Blocks of the first level of statistics contain 64 segments, each next level gathers 8 blocks of the previous one
static const quint32 FIRST_LEVEL_SHIFT = 6;
static const quint32 FIRST_LEVEL_SIZE = 1u << FIRST_LEVEL_SHIFT;
static const quint32 LEVEL_SHIFT = 3;
static const quint32 LEVEL_FACTOR = 1u << LEVEL_SHIFT;

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

// Value of the curve joining the samples at a fractional position
static float getValueAt(const float * data, quint32 segmentNumber, double position)
{
    quint32 segment = static_cast<quint32>(position);
    if (segment >= segmentNumber)
        return data[segmentNumber];
    float ratio = static_cast<float>(position - segment);
    return data[segment] + ratio * (data[segment + 1] - data[segment]);
}

GraphicsWavePainter::GraphicsWavePainter() :
    _levelsComputed(false),
    _start(0),
    _end(0),
    _zoomY(1),
    _imageColor(0),
    _drawPositiveValues(false),
    _addMeanCurve(true),
    _waveColor(0)
{}

void GraphicsWavePainter::setData(const QVector<float> &vData)
{
    this->clearData();

    // Store the waveform
    _sampleData = vData;
}

void GraphicsWavePainter::clearData()
{
    _image = QImage();
    _samplePlotMean.clear();
    _sampleData.clear();
    _levels.clear();
    _levelsComputed = false;
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
    if (width <= 0 || height <= 0)
        return;

    // Possibly update the image
    if (_image.isNull() || _start != start || _end != end || _zoomY != zoomY || _imageColor != _waveColor ||
            _image.width() != width || _image.height() != height)
    {
        // Store the current parameters
        _start = start;
        _end = end;
        _zoomY = zoomY;
        _imageColor = _waveColor;

        // Prepare a new image
        prepareImage(static_cast<quint32>(width), static_cast<quint32>(height));
    }

    // Draw the curve if valid
    if (!_image.isNull())
        painter->drawImage(rect.left(), rect.top(), _image);
}

void GraphicsWavePainter::prepareImage(quint32 width, quint32 height)
{
    ////////////////////////////////
    /// FIRST STEP: compute data ///
    ////////////////////////////////

    if (_sampleData.empty())
    {
        _image = QImage();
        _samplePlotMean.clear();
        return;
    }

    // Each pixel covers a part of the curve joining the samples, made of linear segments
    const float * data = _sampleData.constData() + _start;
    const quint32 segmentNumber = _end - _start;
    const double pixelsPerSample = static_cast<double>(width) / segmentNumber;

    // Statistics of the blocks of segments, useful if pixels cover many segments
    if (!_levelsComputed && segmentNumber >= 2 * FIRST_LEVEL_SIZE * width)
        computeLevels();

    QVector<float> samplePlotMin(width);
    QVector<float> samplePlotMax(width);
    QVector<float> samplePlotDeviation(width);
    _samplePlotMean.resize(width);

    float coeff = -_zoomY * static_cast<float>(height) / (_drawPositiveValues ? 1 : 2);
    float offsetY = _drawPositiveValues ? height : 0.5f * height;
    double position2 = 0;
    float value2 = data[0];
    for (quint32 i = 0; i < width; i++)
    {
        // Part of the curve covered by the pixel, between position1 and position2 (in samples)
        double position1 = position2;
        float value1 = value2;
        position2 = static_cast<double>(i + 1) * segmentNumber / width;
        value2 = getValueAt(data, segmentNumber, position2);
        quint32 segment1 = static_cast<quint32>(position1);
        quint32 segment2 = static_cast<quint32>(position2); // Can be segmentNumber for the last pixel

        // Min / max, sum of the middle values of the segment parts weighted by their width in pixel
        float minValue = qMin(value1, value2);
        float maxValue = qMax(value1, value2);
        double sum, squareSum;
        if (segment1 == segment2)
        {
            // The pixel is within a segment
            double middleValue = 0.5 * (value1 + value2);
            sum = middleValue;
            squareSum = middleValue * middleValue;
        }
        else
        {
            // End of the first segment
            double weight = (segment1 + 1 - position1) * pixelsPerSample;
            double middleValue = 0.5 * (value1 + data[segment1 + 1]);
            sum = middleValue * weight;
            squareSum = middleValue * middleValue * weight;

            // Full segments in between
            if (segment2 > segment1 + 1)
            {
                double segmentSum = 0, segmentSquareSum = 0;
                addSegments(_start + segment1 + 1, _start + segment2, minValue, maxValue, segmentSum, segmentSquareSum);
                sum += segmentSum * pixelsPerSample;
                squareSum += segmentSquareSum * pixelsPerSample;
            }
            else
            {
                minValue = qMin(minValue, data[segment1 + 1]);
                maxValue = qMax(maxValue, data[segment1 + 1]);
            }

            // Beginning of the last segment
            if (position2 > segment2)
            {
                weight = (position2 - segment2) * pixelsPerSample;
                middleValue = 0.5 * (data[segment2] + value2);
                sum += middleValue * weight;
                squareSum += middleValue * middleValue * weight;
            }
        }

        // Mean, standard deviation, min / max in pixels
        _samplePlotMean[i].setX(i);
        _samplePlotMean[i].setY(static_cast<double>(coeff * static_cast<float>(sum) + offsetY));
        samplePlotDeviation[i] = coeff * fastSqrt(static_cast<float>(squareSum - sum * sum));
        samplePlotMin[i] = coeff * minValue + offsetY;
        samplePlotMax[i] = coeff * maxValue + offsetY;
    }

    //////////////////////////////////////
    /// SECOND STEP: compute the image ///
    //////////////////////////////////////

    // Prepare the image (premultiplied: faster to draw)
    if (_image.width() != static_cast<int>(width) || _image.height() != static_cast<int>(height))
        _image = QImage(static_cast<int>(width), static_cast<int>(height), QImage::Format_ARGB32_Premultiplied);
    _image.fill(0 /* transparent */);

    // Color for each opacity
    const quint32 maxAlpha = qAlpha(_waveColor);
    QRgb colors[256];
    for (quint32 alpha = 0; alpha <= maxAlpha; alpha++)
        colors[alpha] = qPremultiply(qRgba(qRed(_waveColor), qGreen(_waveColor), qBlue(_waveColor), static_cast<int>(alpha)));

    // Waveform
    QRgb * pixels = reinterpret_cast<QRgb *>(_image.bits());
    const quint32 pixelsPerLine = static_cast<quint32>(_image.bytesPerLine()) / sizeof(QRgb);
    float mean, deviation, x;
    for (quint32 i = 0; i < width; i++)
    {
        mean = static_cast<float>(_samplePlotMean[i].y());
        deviation = samplePlotDeviation[i]; // Negative

        // Pixels outside [top; bottom] are transparent
        float top = qMin(samplePlotMax[i], mean);
        float bottom = qMax(samplePlotMin[i], mean - deviation);
        quint32 firstLine = top > 0 ? (top < height ? static_cast<quint32>(top) : height) : 0;
        quint32 lastLine = bottom >= 0 ? (bottom < height ? static_cast<quint32>(bottom) + 1 : height) : 0;

        for (quint32 j = firstLine; j < lastLine; j++)
        {
            x = j < mean ? getValueX(samplePlotMax[i], 0, mean + deviation, 1, j) :
                           getValueX(mean - deviation, 1, samplePlotMin[i], 0, j);
            if (x > 0)
                pixels[i + j * pixelsPerLine] = colors[static_cast<quint32>(x * maxAlpha)];
        }
    }

    // Possibly add the mean value
    if (_addMeanCurve)
    {
        QPainter painter(&_image);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(QPen(QColor::fromRgba(_waveColor), 1.0, Qt::SolidLine));
        painter.drawPolyline(_samplePlotMean.constData(), static_cast<int>(width));
    }
}

void GraphicsWavePainter::computeLevels()
{
    _levelsComputed = true;
    _levels.clear();

    const quint32 segmentNumber = _sampleData.size() > 1 ? static_cast<quint32>(_sampleData.size() - 1) : 0;
    quint32 blockNumber = segmentNumber >> FIRST_LEVEL_SHIFT;
    if (blockNumber == 0)
        return;

    // First level, computed from the samples
    const float * data = _sampleData.constData();
    QVector<SegmentStats> level(blockNumber);
    for (quint32 block = 0; block < blockNumber; block++)
    {
        const float * blockData = data + (block << FIRST_LEVEL_SHIFT);
        SegmentStats stats = { blockData[0], blockData[0], 0, 0 };
        for (quint32 i = 0; i < FIRST_LEVEL_SIZE; i++)
        {
            float value = blockData[i + 1];
            if (value < stats.min)
                stats.min = value;
            if (value > stats.max)
                stats.max = value;
            float middleValue = 0.5f * (blockData[i] + value);
            stats.sum += middleValue;
            stats.squareSum += middleValue * middleValue;
        }
        level[block] = stats;
    }
    _levels << level;

    // Next levels, gathering the blocks of the previous one
    while ((blockNumber >>= LEVEL_SHIFT) > 0)
    {
        const SegmentStats * previousLevel = _levels.last().constData();
        level = QVector<SegmentStats>(blockNumber);
        for (quint32 block = 0; block < blockNumber; block++)
        {
            const SegmentStats * subBlocks = previousLevel + (block << LEVEL_SHIFT);
            SegmentStats stats = subBlocks[0];
            for (quint32 i = 1; i < LEVEL_FACTOR; i++)
            {
                if (subBlocks[i].min < stats.min)
                    stats.min = subBlocks[i].min;
                if (subBlocks[i].max > stats.max)
                    stats.max = subBlocks[i].max;
                stats.sum += subBlocks[i].sum;
                stats.squareSum += subBlocks[i].squareSum;
            }
            level[block] = stats;
        }
        _levels << level;
    }
}

void GraphicsWavePainter::addSegments(quint32 first, quint32 last, float &minValue, float &maxValue, double &sum, double &squareSum) const
{
    // Add the statistics of the segments [first; last[, using the largest blocks possible
    const float * data = _sampleData.constData();
    while (first < last)
    {
        // Largest block starting at "first" and ending before "last"
        int level = -1;
        quint32 shift = FIRST_LEVEL_SHIFT;
        while (level + 1 < _levels.size() && (first & ((1u << shift) - 1)) == 0 && last - first >= (1u << shift))
        {
            level++;
            shift += LEVEL_SHIFT;
        }

        if (level >= 0)
        {
            shift -= LEVEL_SHIFT;
            const SegmentStats &stats = _levels[level][first >> shift];
            if (stats.min < minValue)
                minValue = stats.min;
            if (stats.max > maxValue)
                maxValue = stats.max;
            sum += stats.sum;
            squareSum += stats.squareSum;
            first += 1u << shift;
        }
        else
        {
            // Samples until the beginning of the next block
            quint32 next = qMin(last, (first | (FIRST_LEVEL_SIZE - 1)) + 1);
            if (data[first] < minValue)
                minValue = data[first];
            if (data[first] > maxValue)
                maxValue = data[first];
            for (; first < next; first++)
            {
                float value = data[first + 1];
                if (value < minValue)
                    minValue = value;
                if (value > maxValue)
                    maxValue = value;
                float middleValue = 0.5f * (data[first] + value);
                sum += middleValue;
                squareSum += middleValue * middleValue;
            }
        }
    }
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
    float coeff = -_zoomY * static_cast<float>(_image.height()) / 2;
    float offsetY = 0.5f * static_cast<float>(_image.height());
    pointNumber = rightIndex - leftIndex + 1;
    QPointF * array = new QPointF[pointNumber];
    for (quint32 i = 0; i < pointNumber; i++)
    {
        array[i].setX(leftIndex + i);
        array[i].setY(static_cast<double>(coeff * _sampleData[leftIndex + i] + offsetY));
    }
    return array;
}

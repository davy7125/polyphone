/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2026 Davy Triponney                                **
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


#include "timbrebarswidget.h"
#include <QPainter>
#include <QHelpEvent>
#include <QMouseEvent>
#include <QToolTip>
#include <cmath>

TimbreBarsWidget::TimbreBarsWidget(QWidget * parent) : QWidget(parent),
    _dynamicRange(60),
    _left(0),
    _top(0),
    _plotWidth(0),
    _plotHeight(0)
{
    this->setMinimumSize(200, 120);
    this->setMouseTracking(true);
}

void TimbreBarsWidget::setData(const QList<TimbreRow> &rows, const QList<Series> &series, double dynamicRange)
{
    _rows = rows;
    _series = series;
    _dynamicRange = dynamicRange;
    update();
}

void TimbreBarsWidget::clear()
{
    _rows.clear();
    _series.clear();
    update();
}

void TimbreBarsWidget::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.fillRect(this->rect(), _palette.background());
    if (_rows.isEmpty() || _series.isEmpty())
        return;

    // Scale: from the strongest value down to the dynamic range
    double top = -1000;
    foreach (Series series, _series)
        foreach (double value, series.values)
            if (std::isfinite(value))
                top = qMax(top, value);
    if (top <= -1000)
        return;
    top = 5 * std::ceil((top + 1) / 5);
    double bottom = top - _dynamicRange;

    QFontMetrics metrics(this->font());
    _left = metrics.horizontalAdvance("-100") + 8;
    _top = metrics.height() + 6; // Legend
    double bottomMargin = metrics.height() + 4;
    _plotWidth = this->width() - _left - 4;
    _plotHeight = this->height() - _top - bottomMargin;
    if (_plotWidth <= 10 || _plotHeight <= 10)
        return;
    auto yOf = [&](double value) -> double {
        return _top + _plotHeight * (top - qBound(bottom, value, top)) / _dynamicRange;
    };

    // Grid every 10 dB
    painter.setFont(this->font());
    for (double value = top; value >= bottom - 0.01; value -= 10)
    {
        double y = yOf(value);
        painter.setPen(_palette.neutral());
        painter.drawLine(QPointF(_left, y), QPointF(_left + _plotWidth, y));
        painter.setPen(_palette.text());
        painter.drawText(QRectF(0, y - metrics.height() / 2.0, _left - 4, metrics.height()), Qt::AlignRight | Qt::AlignVCenter,
                         QString::number(static_cast<int>(value)));
    }

    // Bars
    double groupWidth = _plotWidth / _rows.count();
    int barSeries = 0;
    foreach (Series series, _series)
        if (series.style != STYLE_MARKERS)
            barSeries++;
    double barWidth = qMax(1.0, groupWidth * 0.8 / qMax(1, barSeries));
    int barIndex = 0;
    foreach (Series series, _series)
    {
        for (int row = 0; row < _rows.count() && row < series.values.size(); row++)
        {
            double value = series.values[row];
            if (!std::isfinite(value))
                continue;
            double x = _left + row * groupWidth + groupWidth * 0.1;
            double y = yOf(value);
            if (series.style == STYLE_MARKERS)
            {
                painter.setPen(QPen(series.color, 3, Qt::SolidLine, Qt::FlatCap));
                painter.drawLine(QPointF(x, y), QPointF(x + groupWidth * 0.8, y));
            }
            else
            {
                QRectF rect(x + barIndex * barWidth, y, barWidth, _top + _plotHeight - y);
                if (series.style == STYLE_BARS)
                    painter.fillRect(rect, series.color);
                else
                {
                    painter.setPen(QPen(series.color, 1.5));
                    painter.setBrush(Qt::NoBrush);
                    painter.drawRect(rect.adjusted(0.75, 0.75, -0.75, 0));
                }
            }
        }
        if (series.style != STYLE_MARKERS)
            barIndex++;
    }

    // Row labels (every n if they don't fit)
    painter.setPen(_palette.text());
    int step = 1;
    double maxLabel = 0;
    foreach (TimbreRow row, _rows)
        maxLabel = qMax(maxLabel, static_cast<double>(metrics.horizontalAdvance(row.label)) + 4);
    while (step * groupWidth < maxLabel && step < _rows.count())
        step++;
    for (int row = 0; row < _rows.count(); row += step)
    {
        double width = metrics.horizontalAdvance(_rows[row].label) + 4;
        double x = qBound(0.0, _left + (row + 0.5) * groupWidth - width / 2, this->width() - width);
        painter.drawText(QRectF(x, _top + _plotHeight + 2, width, metrics.height()), Qt::AlignHCenter | Qt::AlignTop,
                         _rows[row].label);
    }

    // Legend
    double x = _left;
    foreach (Series series, _series)
    {
        QRectF sample(x, 3 + metrics.height() / 2.0 - 4, 14, 8);
        if (series.style == STYLE_MARKERS)
        {
            painter.setPen(QPen(series.color, 3, Qt::SolidLine, Qt::FlatCap));
            painter.drawLine(QPointF(sample.left(), sample.center().y()), QPointF(sample.right(), sample.center().y()));
        }
        else if (series.style == STYLE_BARS)
            painter.fillRect(sample, series.color);
        else
        {
            painter.setPen(QPen(series.color, 1.5));
            painter.drawRect(sample);
        }
        painter.setPen(_palette.text());
        painter.drawText(QPointF(x + 18, 3 + metrics.ascent()), series.name);
        x += 26 + metrics.horizontalAdvance(series.name);
    }
}

bool TimbreBarsWidget::event(QEvent * event)
{
    if (event->type() == QEvent::ToolTip && !_rows.isEmpty() && _plotWidth > 0)
    {
        QHelpEvent * helpEvent = static_cast<QHelpEvent *>(event);
        int row = static_cast<int>((helpEvent->pos().x() - _left) / (_plotWidth / _rows.count()));
        if (row < 0 || row >= _rows.count())
        {
            QToolTip::hideText();
            return true;
        }
        QStringList lines;
        lines << "<b>" + _rows[row].label + "</b>";
        QLocale locale;
        foreach (Series series, _series)
            if (row < series.values.size() && std::isfinite(series.values[row]))
                lines << series.name + tr(": ", "separator") + locale.toString(series.values[row], 'f', 1) + " " +
                         tr("dB", "unit for decibels");
        QToolTip::showText(helpEvent->globalPos(), lines.join("<br/>"), this);
        return true;
    }
    return QWidget::event(event);
}

TimbreVelocityStrip::TimbreVelocityStrip(QWidget * parent) : QWidget(parent),
    _rowCount(1),
    _velocity(100)
{
    this->setMinimumSize(80, 18);
    this->setCursor(Qt::PointingHandCursor);
    this->setToolTip(tr("Velocity layers of the instrument: a click selects the middle of a layer"));
}

void TimbreVelocityStrip::setLayers(const QList<QPair<int, int> > &layers)
{
    _layers = layers;

    // Overlapping layers on different rows
    _rows.clear();
    QList<int> rowEnds;
    foreach (auto layer, _layers)
    {
        int row = 0;
        while (row < rowEnds.count() && rowEnds[row] >= layer.first)
            row++;
        if (row == rowEnds.count())
            rowEnds << layer.second;
        else
            rowEnds[row] = layer.second;
        _rows << row;
    }
    _rowCount = qMax(1, static_cast<int>(rowEnds.count()));
    update();
}

void TimbreVelocityStrip::setVelocity(int velocity)
{
    _velocity = velocity;
    update();
}

void TimbreVelocityStrip::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    QRectF area = QRectF(this->rect()).adjusted(1, 1, -1, -1);
    painter.fillRect(area, _palette.uncovered());
    double rowHeight = area.height() / _rowCount;
    for (int i = 0; i < _layers.count(); i++)
    {
        double x1 = area.left() + area.width() * _layers[i].first / 128.0;
        double x2 = area.left() + area.width() * (_layers[i].second + 1) / 128.0;
        QRectF rect(x1, area.top() + _rows[i] * rowHeight, x2 - x1, rowHeight);
        bool active = (_velocity >= _layers[i].first && _velocity <= _layers[i].second);
        painter.fillRect(rect.adjusted(0.5, 0.5, -0.5, -0.5), active ? _palette.highlight() : _palette.grid());
    }
    double x = area.left() + area.width() * (_velocity + 0.5) / 128.0;
    painter.setPen(QPen(_palette.text(), 2));
    painter.drawLine(QPointF(x, area.top()), QPointF(x, area.bottom()));
    painter.setPen(_palette.grid());
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(area);
}

void TimbreVelocityStrip::mousePressEvent(QMouseEvent * event)
{
    QRectF area = QRectF(this->rect()).adjusted(1, 1, -1, -1);
    int velocity = qBound(1, static_cast<int>(128.0 * (event->pos().x() - area.left()) / area.width()), 127);
    double rowHeight = area.height() / _rowCount;
    int row = static_cast<int>((event->pos().y() - area.top()) / rowHeight);
    for (int i = 0; i < _layers.count(); i++)
    {
        if (_rows[i] == row && velocity >= _layers[i].first && velocity <= _layers[i].second)
        {
            velocity = qBound(1, (_layers[i].first + _layers[i].second + 1) / 2, 127);
            break;
        }
    }
    emit velocityClicked(velocity);
}

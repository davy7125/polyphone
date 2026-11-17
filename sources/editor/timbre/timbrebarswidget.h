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


#ifndef TIMBREBARSWIDGET_H
#define TIMBREBARSWIDGET_H

#include <QWidget>
#include "timbremodel.h"
#include "timbrepalette.h"

// Levels per row (rank or band) of several sounds: heard, reference, after correction, rendered...
class TimbreBarsWidget : public QWidget
{
    Q_OBJECT

public:
    enum Style
    {
        STYLE_BARS,
        STYLE_MARKERS,
        STYLE_OUTLINE
    };
    struct Series
    {
        QString name;
        QVector<double> values; // dB per row, NaN if absent
        QColor color;
        Style style;
    };

    explicit TimbreBarsWidget(QWidget * parent = nullptr);
    void setData(const QList<TimbreRow> &rows, const QList<Series> &series, double dynamicRange);
    void clear();

protected:
    void paintEvent(QPaintEvent * event) override;
    bool event(QEvent * event) override;

private:
    QList<TimbreRow> _rows;
    QList<Series> _series;
    double _dynamicRange;
    TimbrePalette _palette;
    double _left, _top, _plotWidth, _plotHeight;
};

#endif // TIMBREBARSWIDGET_H

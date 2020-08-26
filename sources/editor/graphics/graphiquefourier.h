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

#ifndef GRAPHIQUEFOURIER_H
#define GRAPHIQUEFOURIER_H

#include "qcustomplot/qcustomplot.h"
#include "basetypes.h"
class QMenu;

class GraphiqueFourier : public QCustomPlot
{
    Q_OBJECT

public:
    explicit GraphiqueFourier(QWidget *parent = nullptr);
    ~GraphiqueFourier();

    void setBackgroundColor(QColor color);
    void setData(QByteArray baData, quint32 dwSmplRate);
    void setSampleName(QString name) { _name = name; }
    void setPos(quint32 posStart, quint32 posEnd, bool withReplot = true);
    void setPos(quint32 posStart, quint32 posEnd, QList<double> &frequencies, QList<double> &factors,
                QList<int> &pitch, QList<int> &deltas, bool withReplot);
    void getEstimation(int &pitch, int &correction);

protected:
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent * event);

private slots:
    void exportPng();

private:
    class Peak
    {
    public:
        Peak(double intensity, double frequency, int key, int delta) :
            _intensity(intensity),
            _frequency(frequency),
            _key(key),
            _delta(delta)
        {}

        double _intensity;
        double _frequency;
        int _key;
        int _delta;
    };

    QVector<float> _fData;
    quint32 dwSmplRate;
    QString _name;
    QSharedPointer<QCPAxisTickerFixed> _fixedTickerX;
    QSharedPointer<QCPAxisTickerFixed> _fixedTickerY;
    QMenu * _menu;
    int _key, _delta;
    QList<Peak> _peaks;

    void resized();
    void exportPng(QString fileName);
    void dispFourier(QVector<float> vectFourier, float posMaxFourier);
};

#endif // GRAPHIQUEFOURIER_H

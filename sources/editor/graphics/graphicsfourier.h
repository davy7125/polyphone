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

#ifndef GRAPHICSFOURIER_H
#define GRAPHICSFOURIER_H

#include <QWidget>
#include <QMetaType>
#include "basetypes.h"
class QMenu;
class ToolFrequencyPeaks;
class GraphicsWavePainter;

class Peak
{
public:
    Peak() :
        _factor(0),
        _frequency(0),
        _key(-1),
        _correction(0)
    {}

    Peak(double factor, double frequency, int key, int correction) :
        _factor(factor),
        _frequency(frequency),
        _key(key),
        _correction(correction)
    {}

    double _factor;
    double _frequency;
    int _key;
    int _correction;
};
Q_DECLARE_METATYPE(Peak)

class GraphicsFourier : public QWidget
{
    Q_OBJECT

public:
    explicit GraphicsFourier(QWidget *parent = nullptr);
    ~GraphicsFourier() override;

    void setCurrentIds(IdList ids);
    void setSampleName(QString name) { _name = name; }
    void setPos(quint32 posStart, quint32 posEnd, bool withRepaint = true);
    void getEstimation(int &pitch, int &correction, float &score);
    static QList<Peak> computePeaks(QVector<float> fData, quint32 dwSmplRate, quint32 posStart, quint32 posEnd,
                                    QVector<float> &vectFourier, int &posMaxFourier,
                                    int * key = nullptr, int * correction = nullptr, float *reliance = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent * event) override;

private slots:
    void exportPng();
    void exportPeaks();

private:
    void resized();
    void exportPng(QString fileName);
    void dispFourier(QVector<float> vectFourier, float posMaxFourier);
    QRect paintGrid(QPainter &painter, QRect rect);

    GraphicsWavePainter * _wavePainter;

    IdList _currentIds;
    QVector<float> _fData;
    quint32 _dwSmplRate;
    QString _name;
    QMenu * _menu;
    int _key, _delta;
    float _score;
    QList<Peak> _peaks;
    ToolFrequencyPeaks * _toolFrequencyPeak;
    bool _paintForPng;
};

#endif // GRAPHICSFOURIER_H

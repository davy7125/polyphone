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

#ifndef GRAPHICSFOURIER_H
#define GRAPHICSFOURIER_H

#include <QWidget>
#include <QMetaType>
#include "basetypes.h"
#include "soundanalyzer.h"
class QMenu;
class ToolFrequencyPeaks;
class GraphicsWavePainter;

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
    int _key, _correction; // Estimated root key and pitch correction
    float _score;
    QList<Peak> _peaks;
    ToolFrequencyPeaks * _toolFrequencyPeak;
    bool _paintForPng;
};

#endif // GRAPHICSFOURIER_H

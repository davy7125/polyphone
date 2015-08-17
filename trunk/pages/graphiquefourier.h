/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef GRAPHIQUEFOURIER_H
#define GRAPHIQUEFOURIER_H

#include "qcustomplot.h"
#include "sf2_types.h"
class QMenu;

class GraphiqueFourier : public QCustomPlot
{
    Q_OBJECT

public:
    explicit GraphiqueFourier(QWidget *parent = 0);
    ~GraphiqueFourier() {}

    void setBackgroundColor(QColor color);
    void setData(QByteArray baData, quint32 dwSmplRate);
    void setSampleName(QString name) { _name = name; }
    void setPos(qint32 posStart, qint32 posEnd, bool withReplot = true);
    void setPos(qint32 posStart, qint32 posEnd, QList<double> &frequencies, QList<double> &factors,
                QList<int> &pitch, QList<int> &corrections, bool withReplot);
    void getEstimation(int &pitch, int &correction);

protected:
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);

private slots:
    void exportPng();

private:
    QVector<float> _fData;
    quint32 dwSmplRate;
    QString _name;
    QCPItemText * text1; // estimation
    QCPItemText * text2; // intensité
    QCPItemText * text3; // fréquence
    QCPItemText * text4; // note
    QCPItemText * text5; // correction
    QMenu * _menu;
    int _note, _correction;

    void resized();
    void exportPng(QString fileName);
    void dispFourier(QVector<float> vectFourier, float posMaxFourier);
};

#endif // GRAPHIQUEFOURIER_H

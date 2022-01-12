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

#ifndef GRAPHFILTERFREQUENCIES_H
#define GRAPHFILTERFREQUENCIES_H

#include "qcustomplot.h"

class GraphFilterFrequencies : public QCustomPlot
{
    Q_OBJECT

public:
    explicit GraphFilterFrequencies(QWidget *parent = nullptr);
    ~GraphFilterFrequencies();

    bool eventFilter(QObject* o, QEvent* e);

    void setNbFourier(int nbFourier);
    void addFourierTransform(QVector<float> fData, quint32 sampleRate);
    QVector<double> getValues();
    void setValues(QVector<double> val);

private:
    QVector<double> dValues;
    bool flagEdit;
    void replotGraph();
    double raideurExp;
    QCPItemText * labelCoord;
    int previousX;
    double previousY;
    int _nbFourier;

    void mousePressed(QPoint pos);
    void mouseReleased(QPoint pos);
    void mouseMoved(QPoint pos);
    void mouseLeft();
    void write(QPoint pos);
    void afficheCoord(double x, double y);

    static const int POINT_NUMBER;
};

#endif // GRAPHFILTERFREQUENCIES_H

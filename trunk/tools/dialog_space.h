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

#ifndef DIALOG_SPACE_H
#define DIALOG_SPACE_H

#include <QDialog>
#include "qcustomplot.h"

namespace Ui
{
    class DialogSpace;
}

class DialogSpace : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogSpace(bool isPrst, int noteMin, int noteMax, QWidget *parent = 0);
    ~DialogSpace();

signals:
    void accepted(QMap<int, double> mapPan);

private slots:
    void accept();
    void motifChanged(int value);
    void nbDivChanged(int value);
    void etalementChanged(int value);
    void occupationChanged(int value);
    void offsetChanged(int value);
    void sensChanged(int value);
    void sens2Changed(int value);

private:
    double spaceLinear(int note, int nbDiv, int etalement, int occupation, int offset, int sens, bool isAscending);
    double spaceCurve(int note, int nbDiv, int etalement, int occupation, int offset, int sens, int sens2, bool isHollow);

    static double getAxe(int note, int nbDiv, bool sens);

    void updateRenversements();
    void updateGraph();
    void computeData(QVector<double> &x, QVector<int> &y);

    Ui::DialogSpace * ui;
    bool _isPrst;
    int _noteMin, _noteMax;
};


class GraphSpace : public QCustomPlot
{
    Q_OBJECT
public:
    explicit GraphSpace(QWidget *parent = 0);
    void setData(QVector<double> x, QVector<int> y);

    bool eventFilter(QObject* o, QEvent* e)
    {
        if ((e->type() == QEvent::MouseMove ||
             e->type() == QEvent::MouseButtonPress ||
             e->type() == QEvent::MouseButtonRelease ||
             e->type() == QEvent::Leave)
                && o == this)
        {
            QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(e);
            QPoint pos = mouseEvent->pos();
            if (mouseEvent->type() == QEvent::MouseMove)
                this->mouseMoved(pos);
            else if (mouseEvent->type() == QEvent::Leave)
                this->mouseLeft();
            return true;
        }
        return false;
    }
private:
    void mouseMoved(QPoint pos);
    void mouseLeft();

    void afficheCoord(double xPan, double yLength, int key);
    QVector<double> _xPan, _yLength;
    QVector<int> _yKey;
    QCPItemText * labelCoord;
};


#endif // DIALOG_SPACE_H

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

#ifndef DIALOG_VISUALIZER_H
#define DIALOG_VISUALIZER_H

#include <QDialog>
#include "pile_sf2.h"
#include "qcustomplot.h"

namespace Ui {
class DialogVisualizer;
}

class DialogVisualizer : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogVisualizer(Pile_sf2 *sf2, EltID id, QWidget *parent = 0);
    ~DialogVisualizer();
    
private slots:
    void on_comboParameter_currentIndexChanged(int index);

    void on_checkLog_stateChanged(int arg1);

private:
    bool _isPrst;
    Pile_sf2 * _sf2;
    Ui::DialogVisualizer *ui;
    EltID _initialID;

    double getValue(EltID id, Champ champ);
    double getDefaultValue(Champ champ);
};


class GraphVisualizer : public QCustomPlot
{
    Q_OBJECT

public:
    explicit GraphVisualizer(QWidget *parent = 0);
    ~GraphVisualizer();
    void setScale();
    void setData(QVector<QList<double> > listPoints, QVector<QList<double> > listPointsDef);
    void setIsLog(bool isLog);

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
    static const double MIN_LOG;
    double xMinDonnees, xMaxDonnees, yMinDonnees, yMaxDonnees, yMin2Donnees;
    bool isLog;
    QList<QCPItemText *> listTextOctave;
    QCPItemText * textWarning;
    QList<double> listX, listY;
    QCPItemText * labelCoord;
    void mouseMoved(QPoint pos);
    void mouseLeft();
    void afficheCoord(double x, double y);
};

class GraphLegend : public QCustomPlot
{
    Q_OBJECT

public:
    explicit GraphLegend(QWidget *parent = 0);
    void plot(QCPScatterStyle::ScatterShape style, QColor couleur, int size, int epaisseur, bool antiAliased);
};

#endif // DIALOG_VISUALIZER_H

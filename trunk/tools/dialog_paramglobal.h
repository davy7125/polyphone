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

#ifndef DIALOG_PARAMGLOBAL_H
#define DIALOG_PARAMGLOBAL_H

#include <QDialog>
#include "qcustomplot.h"
#include "pile_sf2.h"

namespace Ui
{
    class DialogParamGlobal;
}

class DialogParamGlobal : public QDialog
{
    Q_OBJECT
    
public:
    DialogParamGlobal(Pile_sf2 *sf2, bool isPrst, QWidget *parent = 0);
    ~DialogParamGlobal();

private slots:
    void accept();
    void indexMotifChanged(int index);  // Action sur le combobox motif
    void raideurChanged(double value);  // Action sur spinBox raideur
    void minChanged(double value);      // Action sur spinBox min
    void maxChanged(double value);      // Action sur spinBox max

signals:
    void accepted(QVector<double> dValues, int typeModif, int param, int minVel, int maxVel);

private:
    Pile_sf2 * _sf2;
    Ui::DialogParamGlobal *ui;
    EltID _initialID;
    bool _isPrst;
};

class GraphParamGlobal : public QCustomPlot
{
    Q_OBJECT

public:
    enum TypeForme
    {
        FORME_MANUELLE,
        FORME_LINEAIRE_ASC,
        FORME_LINEAIRE_DESC,
        FORME_EXP_ASC,
        FORME_EXP_DESC,
        FORME_ALEATOIRE
    };

    explicit GraphParamGlobal(QWidget *parent = 0);
    ~GraphParamGlobal();

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
            else if (mouseEvent->button() == Qt::LeftButton)
            {
                if (mouseEvent->type() == QEvent::MouseButtonPress)
                    this->mousePressed(pos);
                else if (mouseEvent->type() == QEvent::MouseButtonRelease)
                    this->mouseReleased(pos);
            }
            else if (mouseEvent->button() == Qt::RightButton)
            {
                if (mouseEvent->type() == QEvent::MouseButtonPress)
                    this->mouseRightPressed(pos);
                else if (mouseEvent->type() == QEvent::MouseButtonRelease)
                    this->mouseRightReleased(pos);
            }
            return true;
        }
        return false;
    }

    void indexMotifChanged(int index);
    void raideurChanged(double value);
    void setEtendueClavier(int keyboardType);
    void setMinMax(double min, double max)  { yMin = qMin(min, max); yMax = qMax(min, max); }
    void setMinMaxX(int min, int max)       { xMin = qMin(min, max); xMax = qMax(min, max); }
    QVector<double> getValues();
    void setValues(QVector<double> val);
    int getXmin()                           { return xMin; }
    int getXmax()                           { return xMax; }

private:
    TypeForme forme;
    QVector<double> dValues;
    bool flagEdit;
    int limitEdit;
    void replot();
    int nbPoints;
    double raideurExp;
    double yMin, yMax;
    int xMin, xMax;
    QCPItemText * labelCoord;
    int previousX;
    double previousY;

    // Méthodes privées
    void mousePressed(QPoint pos);
    void mouseRightPressed(QPoint pos);
    void mouseReleased(QPoint pos);
    void mouseRightReleased(QPoint pos);
    void mouseMoved(QPoint pos);
    void mouseLeft();
    void writeMotif();
    void write(QPoint pos);
    void afficheCoord(double x, double y);
};

#endif // DIALOG_PARAMGLOBAL_H

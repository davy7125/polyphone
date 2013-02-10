/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013 Davy Triponney                                     **
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

namespace Ui
{
    class DialogParamGlobal;
}

class DialogParamGlobal : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogParamGlobal(QWidget *parent = 0);
    ~DialogParamGlobal();
private slots:
    void accept();
    void indexMotifChanged(int index);  // Action sur le combobox motif
    void raideurChanged(double value);  // Action sur spinBox raideur
signals:
    void accepted(QVector<double> dValues, int typeModif, int param);
private:
    Ui::DialogParamGlobal *ui;
};

class GraphParamGlobal : public QCustomPlot
{
    Q_OBJECT
public:
    explicit GraphParamGlobal(QWidget *parent = 0);

    bool eventFilter(QObject* o, QEvent* e)
    {
        if ((e->type() == QEvent::MouseMove ||
             e->type() == QEvent::MouseButtonPress ||
             e->type() == QEvent::MouseButtonRelease)
                && o == this)
        {
            QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(e);
            QPoint pos = mouseEvent->pos();
            if (mouseEvent->type() == QEvent::MouseMove)
                this->mouseMoved(pos);
            else if (mouseEvent->button() == Qt::LeftButton)
            {
                if (mouseEvent->type() == QEvent::MouseButtonPress)
                    this->mousePressed(pos);
                else if (mouseEvent->type() == QEvent::MouseButtonRelease)
                    this->mouseReleased(pos);
            }
            return true;
        }
        return false;
    }

    void indexMotifChanged(int index);
    void raideurChanged(double value);
    QVector<double> getValues();
private:
    QVector<double> dValues;
    bool flagEdit;
    bool editionEnabled;
    void replot();
    int nbPoints;
    double raideurExp;
    // Méthodes privées
    void mousePressed(QPoint pos);
    void mouseReleased(QPoint pos);
    void mouseMoved(QPoint pos);
    void write(QPoint pos);
    void writeLineaireCroissant();
    void writeLineaireDecroissant();
    void writeExpCroissant();
    void writeExpDecroissant();
    void writeAleatoire();
    double calculBaseExp();
};

#endif // DIALOG_PARAMGLOBAL_H

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
    explicit DialogSpace(QWidget *parent = 0);
    ~DialogSpace();
    // Méthode publique
    static double space(int noteMin, int noteMax, int note,
                        int motif, int nbDiv, int etalement, int occupation,
                        int offset, int sens, int sens2);
signals:
    void accepted(int motif, int nbDiv, int etalement, int occupation, int offset, int sens, int sens2);
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
    Ui::DialogSpace * ui;
};


class GraphSpace : public QCustomPlot
{
    Q_OBJECT
public:
    explicit GraphSpace(QWidget *parent = 0);
    void setData(int motif, int nbDiv, int etalement, int occupation, int offset, int sens, int sens2);
};


#endif // DIALOG_SPACE_H

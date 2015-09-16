/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
**                2014      Andrea Celani                                 **
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

#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QTableWidget>
#include <QTimer>
#include "pile_sf2.h"


// Classe QTableWidget avec inclusion d'une ID + effet bleu sur l'entête
class TableWidget : public QTableWidget
{
    Q_OBJECT

public:
    TableWidget(QWidget *parent = 0);
    ~TableWidget() { delete this->itemDelegate(); }
    void clear();
    void addColumn(int column, QString title);
    void setID(EltID id, int colonne);
    EltID getID(int colonne);
    void setEnlighted(int colonne, bool isEnlighted);
    void setColumnCount(int columns);
    void removeColumn(int column);

    // Set the image corresponding to the loop mode value
    // -1 will remove the image
    void setLoopModeImage(int row, int column, int loopModeValue);

    // Association champ - ligne (méthodes virtuelles pures)
    virtual Champ getChamp(int row) = 0;
    virtual int getRow(quint16 champ) = 0;

signals:
    void actionBegin();
    void actionFinished();

protected:
    void keyPressEvent(QKeyEvent *event);

protected slots:
    // Function reimplemented to fill all selected cells in the same time
    virtual void commitData(QWidget *editor);

private slots:
    void updateColors();

private:
    QTimer *_timer;
    QList<QColor> _listColors;
};

#endif // TABLEWIDGET_H

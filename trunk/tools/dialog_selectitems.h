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

#ifndef DIALOG_SELECTITEMS_H
#define DIALOG_SELECTITEMS_H

#include <QDialog>
#include "pile_sf2.h"

namespace Ui {
class DialogSelectItems;
}

class DialogSelectItems : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogSelectItems(EltID id, QList<EltID> listChecked, Pile_sf2 *sf2, QWidget *parent = 0);
    ~DialogSelectItems();

private slots:
    void accept();
    void on_pushSelectAll_clicked();
    void on_pushSelectCurrent_clicked();

signals:
    void accepted(QList<EltID> listID);

private:
    Ui::DialogSelectItems *ui;
    Pile_sf2 *_sf2;
    EltID _initialID;
};

#endif // DIALOG_SELECTITEMS_H

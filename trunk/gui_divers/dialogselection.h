/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2016 Davy Triponney                                **
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
**  Website/Contact: http://polyphone-soundfonts.com                      **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef DIALOGSELECTION_H
#define DIALOGSELECTION_H

#include <QDialog>
#include "sf2_types.h"

class Pile_sf2;

namespace Ui {
class DialogSelection;
}

class DialogSelection : public QDialog
{
    Q_OBJECT

public:
    DialogSelection(Pile_sf2 *sf2, EltID id, QWidget *parent = 0);
    ~DialogSelection();

private slots:
    void on_pushSelectAll_clicked();
    void on_pushSelectNone_clicked();
    void on_pushCancel_clicked();
    void on_pushOk_clicked();

signals:
    void listChosen(QList<int> listIndex);

private:
    Ui::DialogSelection *ui;
};

#endif // DIALOGSELECTION_H

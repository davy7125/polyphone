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

#ifndef DIALOG_LIST_H
#define DIALOG_LIST_H

#include <QDialog>
#include "pile_sf2.h"
#include <QListWidgetItem>

namespace Ui
{
    class DialogList;
    class ListWidgetItem;
}

class DialogList : public QDialog
{
    Q_OBJECT

public:
    enum ModeListDialog
    {
        MODE_ASSOCIATION,
        MODE_REMPLACEMENT
    };

    explicit DialogList(QWidget *parent = 0);
    ~DialogList();

    void init(MainWindow *window, Pile_sf2 *sf2) {this->window = window; this->sf2 = sf2;}
    void showDialog(EltID idSrc, ModeListDialog mode);

private slots:
    void accept();
    void reject();

signals:
    void selectedID(EltID id);

private:
    Ui::DialogList *ui;
    Pile_sf2 *sf2;
    MainWindow *window;
    ModeListDialog _mode;
};

class ListWidgetItem : public QListWidgetItem
{
public:
    explicit ListWidgetItem(const QString &text, QListWidget *view = 0, int type = Type) \
        : QListWidgetItem(text, view, type){}
    EltID id;
};

#endif // DIALOG_LIST_H

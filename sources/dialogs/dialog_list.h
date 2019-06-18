/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
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

#ifndef DIALOG_LIST_H
#define DIALOG_LIST_H

#include <QDialog>
#include "basetypes.h"
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
    explicit DialogList(QWidget *parent = nullptr);
    ~DialogList();

    void showDialog(EltID idSrc, bool isAssociation);

signals:
    void elementSelected(EltID id, bool isAssociation);

private slots:
    void accept();
    void reject();

signals:
    void selectedID(EltID id);

private:
    Ui::DialogList *ui;
    bool _isAssociation;
};

class ListWidgetItem : public QListWidgetItem
{
public:
    explicit ListWidgetItem(const QString &text, QListWidget *view = nullptr, int type = Type)
        : QListWidgetItem(text, view, type){}
    EltID id;
};

#endif // DIALOG_LIST_H

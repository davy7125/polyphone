/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef DIALOGSELECTION_H
#define DIALOGSELECTION_H

#include <QDialog>
#include "basetypes.h"

class SoundfontManager;
class QListWidgetItem;

namespace Ui {
class DialogSelection;
}

class DialogSelection : public QDialog
{
    Q_OBJECT

public:
    DialogSelection(SoundfontManager *sf2, EltID id, QWidget *parent = nullptr);
    ~DialogSelection();

protected:
    bool eventFilter(QObject * obj, QEvent * event) override;

private slots:
    void on_pushSelectAll_clicked();
    void on_pushSelectNone_clicked();
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
    void on_lineSearch_textChanged(const QString &text);
    void on_listWidget_itemDoubleClicked(QListWidgetItem * item);
    void updateCheckedCount();

signals:
    void listChosen(QList<int> listIndex);

private:
    QList<QListWidgetItem *> getVisibleItems();
    void setCheckState(QList<QListWidgetItem *> items, Qt::CheckState state);

    Ui::DialogSelection *ui;
    bool _isPreset;
    QListWidgetItem * _clickedItem;
    Qt::CheckState _clickedItemState;
};

#endif // DIALOGSELECTION_H

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

#ifndef TABLEWIDGETMOD_H
#define TABLEWIDGETMOD_H

#include <QTableWidget>
#include "sf2_types.h"

// Classe QTableWidget pour mod
class TableWidgetMod : public QTableWidget
{
    Q_OBJECT

public:
    explicit TableWidgetMod(QWidget *parent = 0) : QTableWidget(parent) {}
    ~TableWidgetMod() {}

    void clear();
    void addRow(int row, EltID id);
    EltID getID(int row);
    EltID getID();
    QList<EltID> getSelectedIDs();

protected:
    virtual void keyPressEvent(QKeyEvent * event);

signals:
    void deleteModPressed();
    void copyAsked();
    void pasteAsked();
};

#endif // TABLEWIDGETMOD_H

/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
**                2014      Andrea Celani                                 **
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

#ifndef MODULATOREDITOR_H
#define MODULATOREDITOR_H

#include <QWidget>
#include "idlist.h"
#include "attribute.h"

namespace Ui {
class ModulatorEditor;
}

class ModulatorEditor : public QWidget
{
    Q_OBJECT

public:
    explicit ModulatorEditor(QWidget *parent = 0);
    ~ModulatorEditor();

    void setIds(IdList ids, QList<AttributeType> attributes = QList<AttributeType>());

signals:
    void attributesSelected(QList<AttributeType> attributes);

private slots:
    void on_pushExpand_clicked();
    void on_pushCollapse_clicked();
    void on_listWidget_itemSelectionChanged();

private:
    void updateInterface(QList<AttributeType> attributes);
    void updateButtons(bool withSelection);

    Ui::ModulatorEditor *ui;
    static QList<ModulatorEditor *> s_instances;
    EltID _currentId;
};

#endif // MODULATOREDITOR_H

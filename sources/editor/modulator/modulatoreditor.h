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
    explicit ModulatorEditor(QWidget *parent = nullptr);
    ~ModulatorEditor();

    void setIds(IdList ids, QList<AttributeType> attributes = QList<AttributeType>());

signals:
    void attributesSelected(QList<AttributeType> attributes);

private slots:
    void on_pushExpand_clicked();
    void on_pushCollapse_clicked();
    void on_listWidget_itemSelectionChanged();
    void on_pushAdd_clicked();
    void on_pushCopy_clicked();
    void on_pushPaste_clicked();
    void on_pushClone_clicked();
    void on_pushDelete_clicked();
    void duplicateMod(QList<int> listIndex);

private:
    void checkOverrides();
    void updateInterface(QList<AttributeType> attributes);
    void updateButtons(bool withSelection);
    QList<EltID> getSelectedModulators();
    QList<ModulatorData> getModList(EltID id);
    void pasteMod(EltID id, QList<ModulatorData> modulators);

    static QList<ModulatorEditor *> s_instances;
    static QList<ModulatorData> s_modulatorCopy;

    Ui::ModulatorEditor *ui;
    EltID _currentId;
    QColor _mixedColor;
};

#endif // MODULATOREDITOR_H

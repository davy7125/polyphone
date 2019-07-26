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

#include "editordialoginserttable.h"
#include "ui_editordialoginserttable.h"

EditorDialogInsertTable::EditorDialogInsertTable(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditorDialogInsertTable)
{
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint));
    this->setWindowModality(Qt::WindowModal);
    this->setAttribute(Qt::WA_DeleteOnClose);

    ui->setupUi(this);
}

EditorDialogInsertTable::~EditorDialogInsertTable()
{
    delete ui;
}

void EditorDialogInsertTable::on_buttonBox_accepted()
{
    emit(accepted(ui->spinRows->value(), ui->spinColumns->value()));
}

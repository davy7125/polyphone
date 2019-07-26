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

#include "editordialoginsertimage.h"
#include "ui_editordialoginsertimage.h"
#include "contextmanager.h"
#include <QUrl>

EditorDialogInsertImage::EditorDialogInsertImage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditorDialogInsertImage)
{
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint));
    this->setWindowModality(Qt::WindowModal);
    this->setAttribute(Qt::WA_DeleteOnClose);

    ui->setupUi(this);
    ui->labelWarning->setStyleSheet(
                "QLabel{color:" + ContextManager::theme()->getFixedColor(ThemeManager::RED, ThemeManager::WINDOW_BACKGROUND).name() + "}");
    ui->labelWarning->hide();

    ui->lineUrl->setFocus();
}

EditorDialogInsertImage::~EditorDialogInsertImage()
{
    delete ui;
}

void EditorDialogInsertImage::accept()
{
    // Test the validity of the url
    QUrl url(ui->lineUrl->text());
    if ((ui->lineUrl->text().startsWith("http://") || ui->lineUrl->text().startsWith("https://")) && url.isValid())
    {
        emit(accepted(url.toString()));
        QDialog::accept();
    }

    // Notify the url is wrong
    ui->labelWarning->show();
}

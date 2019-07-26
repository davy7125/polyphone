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

#include "editordialoginsertvideo.h"
#include "ui_editordialoginsertvideo.h"
#include "contextmanager.h"

EditorDialogInsertVideo::EditorDialogInsertVideo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditorDialogInsertVideo)
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

EditorDialogInsertVideo::~EditorDialogInsertVideo()
{
    delete ui;
}

void EditorDialogInsertVideo::accept()
{
    // Try to find the video ID
    QString url = ui->lineUrl->text();

    // Example 1: https://www.youtube.com/watch?v=YOpa5Ec3i4s
    QRegExp rx_1("www\\.youtube\\.com/watch\\?v=(\\w+)");
    if (rx_1.indexIn(url) != -1)
    {
        emit(accepted(rx_1.cap(1)));
        QDialog::accept();
    }

    // Example 2: https://youtu.be/YOpa5Ec3i4s
    QRegExp rx_2("youtu\\.be/(\\w+)");
    if (rx_2.indexIn(url) != -1)
    {
        emit(accepted(rx_2.cap(1)));
        QDialog::accept();
    }

    // Example 3: https://www.youtube.com/embed/YOpa5Ec3i4s
    QRegExp rx_3("youtube\\.com/embed/(\\w+)");
    if (rx_3.indexIn(url) != -1)
    {
        emit(accepted(rx_3.cap(1)));
        QDialog::accept();
    }

    // Notify the url is wrong
    ui->labelWarning->show();
}

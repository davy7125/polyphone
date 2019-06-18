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

#include "downloadprogresscell.h"
#include "ui_downloadprogresscell.h"
#include "contextmanager.h"
#include "downloadmanager.h"
#include "windowmanager.h"
#include "inputfactory.h"
#include <QFileInfo>
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>

DownloadProgressCell::DownloadProgressCell(int soundfontId, QString soundfontName, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DownloadProgressCell),
    _soundfontId(soundfontId),
    _soundfontName(soundfontName),
    _percent(0)
{
    ui->setupUi(this);

    // Style
    ui->pushCancel->setIcon(ContextManager::theme()->getColoredSvg(":/icons/close.svg", QSize(16, 16), ThemeManager::LIST_TEXT));
    ui->pushOpen->setIcon(ContextManager::theme()->getColoredSvg(":/icons/document-open.svg", QSize(16, 16), ThemeManager::LIST_TEXT));
    ui->pushOpen->hide();
    ui->iconFile->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/file-description.svg", QSize(16, 16), ThemeManager::LIST_TEXT));

    // Data
    ui->labelTitle->setText(_soundfontName);
}

DownloadProgressCell::~DownloadProgressCell()
{
    delete ui;
}

void DownloadProgressCell::progressChanged(int percent, QString finalFilename)
{
    // Update data
    _percent = percent;
    _filename = finalFilename;

    // Update GUI
    ui->labelPercent->setText(QString::number(_percent) + "%");
    if (finalFilename != "")
    {
        ui->pushOpen->setToolTip(trUtf8("Open \"%1\"").arg(_filename));
        ui->pushCancel->hide();
        ui->pushOpen->show();
    }
}

void DownloadProgressCell::on_pushOpen_clicked()
{
    if (_filename == "")
        return;

    // Is it possible to open the file?
    if (InputFactory::isSuffixSupported(QFileInfo(_filename).suffix()))
        WindowManager::getInstance()->openSoundfont(_filename);
    else
    {
        if (!QDesktopServices::openUrl(QUrl(_filename, QUrl::TolerantMode)))
        {
            // Warning message
            QMessageBox::warning(QApplication::activeWindow(), trUtf8("Warning"),
                                 trUtf8("Couldn't open file \"%1\". If this is an archive, you may have to extract it first.").arg(_filename));
        }
    }
    emit(closeMenu());
}

void DownloadProgressCell::on_pushCancel_clicked()
{
    DownloadManager::getInstance()->cancel(_soundfontId);
}

void DownloadProgressCell::cancel()
{
    // Nothing more to do
    _percent = 100;

    ui->pushCancel->setIcon(ContextManager::theme()->getColoredSvg(":/icons/canceled.svg", QSize(16, 16), ThemeManager::LIST_TEXT));
    ui->pushCancel->setToolTip(trUtf8("Download canceled"));
    ui->pushCancel->setEnabled(false);
}

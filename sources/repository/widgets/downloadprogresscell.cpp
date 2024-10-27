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

#include "downloadprogresscell.h"
#include "ui_downloadprogresscell.h"
#include "contextmanager.h"
#include "downloadmanager.h"
#include "tabmanager.h"
#include "inputfactory.h"
#include <QFileInfo>
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include <QMouseEvent>

DownloadProgressCell::DownloadProgressCell(int soundfontId, QString soundfontName, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DownloadProgressCell),
    _soundfontId(soundfontId),
    _soundfontName(soundfontName),
    _percent(0)
{
    ui->setupUi(this);
    this->setMouseTracking(true);

    // Icons
    _pixClose = ContextManager::theme()->getColoredSvg(":/icons/close.svg", QSize(16, 16), ThemeManager::LIST_TEXT);
    _pixCloseHighlighted = ContextManager::theme()->getColoredSvg(":/icons/close.svg", QSize(16, 16), ThemeManager::HIGHLIGHTED_TEXT);
    _pixFile = ContextManager::theme()->getColoredSvg(":/icons/file-description.svg", QSize(16, 16), ThemeManager::LIST_TEXT);
    _pixFileHighlighted = ContextManager::theme()->getColoredSvg(":/icons/file-description.svg", QSize(16, 16), ThemeManager::HIGHLIGHTED_TEXT);

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
    ui->pushCancel->setVisible(finalFilename.isEmpty());
}

void DownloadProgressCell::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && !_filename.isEmpty())
    {
        // Is it possible to open the file?
        if (InputFactory::isSuffixSupported(QFileInfo(_filename).suffix()))
            TabManager::getInstance()->openSoundfont(_filename, nullptr);
        else
        {
            if (!QDesktopServices::openUrl(QUrl(_filename, QUrl::TolerantMode)))
            {
                // Warning message
                QMessageBox::warning(QApplication::activeWindow(), tr("Warning"),
                                     tr("Couldn't open file \"%1\". If this is an archive, you may have to extract it first.").arg(_filename));
            }
        }

        emit closeMenu();
    }

    QWidget::mousePressEvent(event);
}

void DownloadProgressCell::paintEvent(QPaintEvent * event)
{
    // Adapt the icons before drawing the widget
    ui->pushCancel->setIcon(this->backgroundRole() == QPalette::Highlight ? _pixCloseHighlighted : _pixClose);
    ui->iconFile->setPixmap(this->backgroundRole() == QPalette::Highlight ? _pixFileHighlighted : _pixFile);
    QWidget::paintEvent(event);
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
    ui->pushCancel->setToolTip(tr("Download canceled"));
    ui->pushCancel->setEnabled(false);
}

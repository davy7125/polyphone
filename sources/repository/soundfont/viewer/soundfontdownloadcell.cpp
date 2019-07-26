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

#include "soundfontdownloadcell.h"
#include "ui_soundfontdownloadcell.h"
#include "soundfontdownloaddata.h"
#include "contextmanager.h"
#include "downloadmanager.h"

SoundfontDownloadCell::SoundfontDownloadCell(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SoundfontDownloadCell)
{
    ui->setupUi(this);
    ui->pushDownload->setStyleSheet("QPushButton{padding:4px;}");
    ui->pushDownload->setIcon(ContextManager::theme()->getColoredSvg(":/icons/download.svg", QSize(24, 24), ThemeManager::BUTTON_TEXT));
}

SoundfontDownloadCell::~SoundfontDownloadCell()
{
    delete ui;
}

void SoundfontDownloadCell::initialize(SoundfontDownloadData * data)
{
    // Title
    ui->labelTitle->setText(data->getName());

    // Description, if possible
    if (data->getDescription().isEmpty())
        ui->labelDescription->hide();
    else
        ui->labelDescription->setText(data->getDescription());

    // Format informations
    ui->labelInfo->setText(QString("(%1, %2)").arg(data->getFormattedSize()).arg(data->getLastDate()));

    // Store data
    _id = data->getId();
    _name = data->getName();
}

void SoundfontDownloadCell::on_pushDownload_clicked()
{
    DownloadManager::getInstance()->download(_id, _name);
}

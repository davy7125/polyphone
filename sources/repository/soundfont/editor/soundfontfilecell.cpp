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

#include "soundfontfilecell.h"
#include "ui_soundfontfilecell.h"
#include "soundfontdownloaddata.h"
#include "contextmanager.h"
#include <QFileInfo>

SoundfontFileCell::SoundfontFileCell(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SoundfontFileCell)
{
    ui->setupUi(this);

    // Style
    ui->pushDelete->setStyleSheet("QPushButton{padding:4px;}");
    ui->pushDelete->setIcon(ContextManager::theme()->getColoredSvg(":/icons/close.svg", QSize(24, 24), ThemeManager::BUTTON_TEXT));
    ui->pushReplace->setIcon(ContextManager::theme()->getColoredSvg(":/icons/exchange.svg", QSize(24, 24), ThemeManager::BUTTON_TEXT));
    ui->labelInfo->setStyleSheet("QLabel{color:" +
                                 ThemeManager::mix(ContextManager::theme()->getColor(ThemeManager::LIST_TEXT),
                                                   ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND), 0.5).name() + "}");
    ui->lineTitle->resizeAfterChange(false);
    ui->lineTitle->setColor(ContextManager::theme()->getColor(ThemeManager::ColorType::LIST_TEXT));
}

SoundfontFileCell::~SoundfontFileCell()
{
    delete ui;
}

void SoundfontFileCell::initialize(SoundfontDownloadData * data)
{
    // Title
    ui->lineTitle->setText(data->getName());

    // Description, if possible
    ui->textDescription->setText(data->getDescription());

    // Information
    ui->labelInfo->setText(QString("%1 - %2").arg(data->getFormattedSize())
                           .arg(data->getLastDate().toString(this->locale().dateFormat(QLocale::ShortFormat))));

    // Keep the ID of the distant file
    _id = data->getId();
    _filePath = "";
}

void SoundfontFileCell::initialize(QString filePath, int id)
{
    // Title based on the filePath
    QFileInfo fi(filePath);
    ui->lineTitle->setText(fi.completeBaseName());

    // No description
    ui->textDescription->setText("");

    // Information
    ui->labelInfo->setText(filePath);

    // Id and filepath
    _id = id;
    _filePath = filePath;
}

void SoundfontFileCell::fileReplaced(QString filePath)
{
    // Title based on the filePath if not already specified
    if (ui->lineTitle->text().isEmpty())
    {
        QFileInfo fi(filePath);
        ui->lineTitle->setText(fi.completeBaseName());
    }

    // Information
    ui->labelInfo->setText(filePath);

    // The id is kept, filePath updated
    _filePath = filePath;
}

void SoundfontFileCell::on_pushReplace_clicked()
{
    emit replaced(_id, _filePath);
}

void SoundfontFileCell::on_pushDelete_clicked()
{
    emit removed(_id);
}

QString SoundfontFileCell::getTitle()
{
    return ui->lineTitle->text();
}

QString SoundfontFileCell::getDescription()
{
    return ui->textDescription->toPlainText();
}

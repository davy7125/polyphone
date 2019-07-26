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

#include "soundfontviewercenter.h"
#include "ui_soundfontviewercenter.h"
#include "contextmanager.h"
#include "soundfontdetails.h"
#include "soundfontdescriptiondata.h"

SoundfontViewerCenter::SoundfontViewerCenter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SoundfontViewerCenter)
{
    ui->setupUi(this);

    // Style
    QString styleSheetTitle = "QLabel { background-color:" + ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name() +
            "; color: " + ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name() + ";border-radius: 2px; padding: 0 5px; }";
    ui->labelDescription->setStyleSheet(styleSheetTitle);
    ui->labelDownload->setStyleSheet(styleSheetTitle);
    ui->labelComment->setStyleSheet(styleSheetTitle);
}

SoundfontViewerCenter::~SoundfontViewerCenter()
{
    delete ui;
}

void SoundfontViewerCenter::initialize(SoundfontDetails *details)
{
    ui->widgetDescription->setHtml(details->getDescription()->getText(), true);
    ui->widgetDownload->display(details->getDownloads());
    ui->widgetComment->display(details->getComments());
}

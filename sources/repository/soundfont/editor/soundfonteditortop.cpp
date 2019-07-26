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

#include "soundfonteditortop.h"
#include "ui_soundfonteditortop.h"
#include "contextmanager.h"
#include "soundfontinformation.h"

SoundfontEditorTop::SoundfontEditorTop(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SoundfontEditorTop)
{
    ui->setupUi(this);

    // Style
    ui->lineEdit_name->resizeAfterChange(false);
    ui->lineEdit_name->setColor(ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT));
}

SoundfontEditorTop::~SoundfontEditorTop()
{
    delete ui;
}

void SoundfontEditorTop::initialize(SoundfontInformation * soundfontInfo)
{
    // Title
    ui->lineEdit_name->setText(soundfontInfo->getTitle());
}

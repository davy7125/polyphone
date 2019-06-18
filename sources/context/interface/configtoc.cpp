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

#include "configtoc.h"
#include "ui_configtoc.h"
#include "contextmanager.h"

ConfigToc::ConfigToc(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigToc)
{
    ui->setupUi(this);

    // Style
    ui->frame->setStyleSheet("QFrame{background-color:" + ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND).name() + "}");

    // Buttons
    ui->buttonGeneral->setIconName(":/icons/wrench.svg");
    ui->buttonInterface->setIconName(":/icons/flag.svg");
    ui->buttonSound->setIconName(":/icons/sound.svg");
    ui->buttonKeyboard->setIconName(":/icons/keyboard.svg");
    ui->buttonRepository->setIconName(":/icons/globe.svg");
}

ConfigToc::~ConfigToc()
{
    delete ui;
}

void ConfigToc::on_buttonGeneral_clicked()
{
    emit(sectionClicked(0));
}

void ConfigToc::on_buttonInterface_clicked()
{
    emit(sectionClicked(1));
}

void ConfigToc::on_buttonSound_clicked()
{
    emit(sectionClicked(2));
}

void ConfigToc::on_buttonKeyboard_clicked()
{
    emit(sectionClicked(3));
}

void ConfigToc::on_buttonRepository_clicked()
{
    emit(sectionClicked(4));
}

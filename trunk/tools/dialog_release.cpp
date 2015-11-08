/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "dialog_release.h"
#include "ui_dialog_release.h"
#include "config.h"

DialogRelease::DialogRelease(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogRelease)
{
    ui->setupUi(this);
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint) | Qt::Tool);
    Config * conf = Config::getInstance();
    ui->doubleDuree36->setValue(conf->getTools_i_release_dureeDo());
    ui->doubleDivision->setValue(conf->getTools_i_release_division());
    ui->doubleDeTune->setValue(conf->getTools_i_release_desaccordage());
    ui->label->setText(trUtf8("Durée release (note ") +
                              conf->getKeyName(36) + ")");
}

DialogRelease::~DialogRelease()
{
    delete ui;
}

// ACCEPTATION

void DialogRelease::accept()
{
    // Sauvegarde des valeurs
    Config * conf = Config::getInstance();
    conf->setTools_i_release_dureeDo(this->ui->doubleDuree36->value());
    conf->setTools_i_release_division(this->ui->doubleDivision->value());
    conf->setTools_i_release_desaccordage(this->ui->doubleDeTune->value());
    // Envoi des valeurs
    this->accepted(this->ui->doubleDuree36->value(),
                   this->ui->doubleDivision->value(),
                   this->ui->doubleDeTune->value());
    QDialog::accept();
}

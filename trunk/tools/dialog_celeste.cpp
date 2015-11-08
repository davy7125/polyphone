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

#include "dialog_celeste.h"
#include "ui_dialog_celeste.h"
#include "config.h"

DialogCeleste::DialogCeleste(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogCeleste)
{
    ui->setupUi(this);
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint) | Qt::Tool);
    Config * conf = Config::getInstance();
    ui->doubleSpinHerz->setValue(conf->getTools_i_celeste_herzDo());
    ui->doubleSpinDiv->setValue(conf->getTools_i_celeste_division());

    ui->label->setText(trUtf8("Nombre de battements par secondes (note ") +
                       conf->getKeyName(60) + ")");
}

DialogCeleste::~DialogCeleste()
{
    delete ui;
}

// ACCEPTATION

void DialogCeleste::accept()
{
    // Sauvegarde des valeurs
    Config * conf = Config::getInstance();
    conf->setTools_i_celeste_herzDo(this->ui->doubleSpinHerz->value());
    conf->setTools_i_celeste_division(this->ui->doubleSpinDiv->value());

    // Envoi des valeurs
    this->accepted(this->ui->doubleSpinHerz->value(),
                   this->ui->doubleSpinDiv->value());
    QDialog::accept();
}

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

#include "dialog_sifflements.h"
#include "ui_dialog_sifflements.h"
#include "config.h"

DialogSifflements::DialogSifflements(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSifflements)
{
    ui->setupUi(this);
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint));
    Config * conf = Config::getInstance();
    ui->spinFreq1->setValue(conf->getTools_s_sifflements_debut());
    ui->spinFreq2->setValue(conf->getTools_s_sifflements_fin());
    ui->doubleSpinRaideur->setValue(conf->getTools_s_sifflements_raideur());
}

DialogSifflements::~DialogSifflements()
{
    delete ui;
}

void DialogSifflements::accept()
{
    emit(this->accepted(this->ui->spinFreq1->value(),
                        this->ui->spinFreq2->value(),
                        this->ui->doubleSpinRaideur->value()));
    QDialog::accept();
}

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

#include "dialog_duplication.h"
#include "ui_dialog_duplication.h"
#include "config.h"
#include <stdio.h>

DialogDuplication::DialogDuplication(bool isPrst, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogDuplication),
    _isPrst(isPrst)
{
    ui->setupUi(this);
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint) | Qt::Tool);

    // Chargement des valeurs
    Config * conf = Config::getInstance();
    _listeVelocites = conf->getTools_duplication_velocites(_isPrst);
    this->ui->checkForEachKey->setChecked(conf->getTools_duplication_duplicKey(_isPrst));
    this->ui->checkForEachVelocityRange->setChecked(conf->getTools_duplication_duplicVel(_isPrst));
    this->on_checkForEachVelocityRange_clicked();
    if (_listeVelocites.size() >= 2)
    {
        this->ui->spinMinVel->blockSignals(true);
        this->ui->spinMinVel->setValue(_listeVelocites.at(0));
        this->ui->spinMinVel->blockSignals(false);
        this->ui->spinMaxVel->blockSignals(true);
        this->ui->spinMaxVel->setValue(_listeVelocites.at(1));
        this->ui->spinMaxVel->blockSignals(false);
    }

    // Affichage des vélocités
    this->dispVel();
}

void DialogDuplication::dispVel()
{
    // Effacement liste vélocités
    this->ui->listVelocites->blockSignals(true);
    int selectedIndex = this->ui->listVelocites->currentRow();
    this->ui->listVelocites->clear();
    // Remplissage
    int valMin, valMax;
    for (int i = 0; i < this->_listeVelocites.size() / 2; i++)
    {
        // Ajout élément
        valMin = qMin(_listeVelocites.at(2*i), _listeVelocites.at(2*i+1));
        valMax = qMax(_listeVelocites.at(2*i), _listeVelocites.at(2*i+1));
        char T[30];
        sprintf(T, "%d - %d", valMin, valMax);
        this->ui->listVelocites->addItem(T);
    }
    if (this->ui->listVelocites->count() > selectedIndex)
        this->ui->listVelocites->setCurrentRow(selectedIndex);
    else if (this->ui->listVelocites->count() > selectedIndex - 1)
        this->ui->listVelocites->setCurrentRow(selectedIndex - 1);
    this->ui->listVelocites->blockSignals(false);
}

DialogDuplication::~DialogDuplication()
{
    delete ui;
}

void DialogDuplication::on_pushAdd_clicked()
{
    int minVel = qMin(this->ui->spinMinVel->value(), this->ui->spinMaxVel->value());
    int maxVel = qMax(this->ui->spinMinVel->value(), this->ui->spinMaxVel->value());
    _listeVelocites.append(minVel);
    _listeVelocites.append(maxVel);
    this->dispVel();
    this->ui->listVelocites->setCurrentRow(this->ui->listVelocites->count()-1);
}

void DialogDuplication::on_pushRemove_clicked()
{
    int index = this->ui->listVelocites->currentRow();
    if (index >= 0 && _listeVelocites.size() > 2)
    {
        _listeVelocites.remove(index * 2, 2);
        this->dispVel();
    }
}

void DialogDuplication::on_spinMinVel_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    int index = this->ui->listVelocites->currentRow();
    if (index >= 0)
    {
        int minVel = qMin(this->ui->spinMinVel->value(), this->ui->spinMaxVel->value());
        int maxVel = qMax(this->ui->spinMinVel->value(), this->ui->spinMaxVel->value());
        _listeVelocites[index * 2] = minVel;
        _listeVelocites[index * 2 + 1] = maxVel;
        this->dispVel();
    }
}

void DialogDuplication::on_spinMaxVel_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    this->on_spinMinVel_valueChanged(0);
}

void DialogDuplication::on_checkForEachVelocityRange_clicked()
{
    bool isEnabled = ui->checkForEachVelocityRange->isChecked();
    ui->listVelocites->setEnabled(isEnabled);
    ui->pushAdd->setEnabled(isEnabled);
    ui->pushRemove->setEnabled(isEnabled);
    ui->spinMaxVel->setEnabled(isEnabled);
    ui->spinMinVel->setEnabled(isEnabled);
}

void DialogDuplication::accept()
{
    // Sauvegarde des valeurs
    Config * conf = Config::getInstance();
    conf->setTools_duplication_velocites(_isPrst, _listeVelocites);
    bool duplicKey = this->ui->checkForEachKey->isChecked();
    bool duplicVel = this->ui->checkForEachVelocityRange->isChecked();
    conf->setTools_duplication_duplicKey(_isPrst, duplicKey);
    conf->setTools_duplication_duplicVel(_isPrst, duplicVel);

    // Envoi des valeurs
    if ((duplicKey || duplicVel) && _listeVelocites.size())
        emit(accepted(_listeVelocites, duplicKey, duplicVel));
    QDialog::accept();
}

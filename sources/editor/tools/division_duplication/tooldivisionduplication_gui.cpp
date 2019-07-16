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

#include "tooldivisionduplication_gui.h"
#include "ui_tooldivisionduplication_gui.h"
#include "tooldivisionduplication_parameters.h"
#include "soundfontmanager.h"

ToolDivisionDuplication_gui::ToolDivisionDuplication_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolDivisionDuplication_gui)
{
    ui->setupUi(this);
}

ToolDivisionDuplication_gui::~ToolDivisionDuplication_gui()
{
    delete ui;
}

void ToolDivisionDuplication_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    _isInst = (ids.isEmpty() || ids[0].typeElement == elementInst || ids[0].typeElement == elementInstSmpl);
    ToolDivisionDuplication_parameters * params = dynamic_cast<ToolDivisionDuplication_parameters *>(parameters);

    // Recall values
    _listeVelocites = _isInst ? params->getInstVelocityRanges() : params->getPrstVelocityRanges();
    ui->checkForEachKey->setChecked(_isInst ? params->getInstDuplicKey() : params->getPrstDuplicKey());
    ui->checkForEachVelocityRange->setChecked(_isInst ? params->getInstDuplicVel() : params->getPrstDuplicVel());

    this->on_checkForEachVelocityRange_clicked();
    if (_listeVelocites.size() >= 1)
    {
        ui->spinMinVel->blockSignals(true);
        ui->spinMinVel->setValue(_listeVelocites[0].first);
        ui->spinMinVel->blockSignals(false);
        ui->spinMaxVel->blockSignals(true);
        ui->spinMaxVel->setValue(_listeVelocites[0].second);
        ui->spinMaxVel->blockSignals(false);
    }

    // Display velocities
    this->dispVel();
}

void ToolDivisionDuplication_gui::dispVel()
{
    // Clear velocity list
    ui->listVelocites->blockSignals(true);
    int selectedIndex = ui->listVelocites->currentRow();
    ui->listVelocites->clear();

    // Remplissage
    int valMin, valMax;
    for (int i = 0; i < _listeVelocites.size(); i++)
    {
        // Add an element
        valMin = qMin(_listeVelocites[i].first, _listeVelocites[i].second);
        valMax = qMax(_listeVelocites[i].first, _listeVelocites[i].second);
        ui->listVelocites->addItem(QString::number(valMin) + " - " + QString::number(valMax));
    }
    if (ui->listVelocites->count() > selectedIndex)
        ui->listVelocites->setCurrentRow(selectedIndex);
    else if (ui->listVelocites->count() > selectedIndex - 1)
        ui->listVelocites->setCurrentRow(selectedIndex - 1);
    ui->listVelocites->blockSignals(false);
}

void ToolDivisionDuplication_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolDivisionDuplication_parameters * params = (ToolDivisionDuplication_parameters *) parameters;

    // Save values
    if (_isInst)
    {
        params->setInstVelocityRanges(_listeVelocites);
        params->setInstDuplicKey(ui->checkForEachKey->isChecked());
        params->setInstDuplicVel(ui->checkForEachVelocityRange->isChecked());
    }
    else
    {
        params->setPrstVelocityRanges(_listeVelocites);
        params->setPrstDuplicKey(ui->checkForEachKey->isChecked());
        params->setPrstDuplicVel(ui->checkForEachVelocityRange->isChecked());
    }
}

void ToolDivisionDuplication_gui::on_pushAdd_clicked()
{
    int minVel = qMin(ui->spinMinVel->value(), ui->spinMaxVel->value());
    int maxVel = qMax(ui->spinMinVel->value(), ui->spinMaxVel->value());
    _listeVelocites.append(QPair<int, int>(minVel, maxVel));
    this->dispVel();
    ui->listVelocites->setCurrentRow(ui->listVelocites->count() - 1);
}

void ToolDivisionDuplication_gui::on_pushRemove_clicked()
{
    int index = ui->listVelocites->currentRow();
    if (index >= 0 && _listeVelocites.size() > 1)
    {
        _listeVelocites.removeAt(index);
        this->dispVel();
    }
}

void ToolDivisionDuplication_gui::on_listVelocites_currentRowChanged(int currentRow)
{
    ui->spinMinVel->blockSignals(true);
    ui->spinMaxVel->blockSignals(true);
    ui->spinMinVel->setValue(_listeVelocites[currentRow].first);
    ui->spinMaxVel->setValue(_listeVelocites[currentRow].second);
    ui->spinMinVel->blockSignals(false);
    ui->spinMaxVel->blockSignals(false);
}

void ToolDivisionDuplication_gui::on_spinMinVel_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    int index = ui->listVelocites->currentRow();
    if (index >= 0)
    {
        int minVel = qMin(ui->spinMinVel->value(), ui->spinMaxVel->value());
        int maxVel = qMax(ui->spinMinVel->value(), ui->spinMaxVel->value());
        _listeVelocites[index].first = minVel;
        _listeVelocites[index].second = maxVel;
        this->dispVel();
    }
}

void ToolDivisionDuplication_gui::on_spinMaxVel_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    this->on_spinMinVel_valueChanged(0);
}

void ToolDivisionDuplication_gui::on_checkForEachVelocityRange_clicked()
{
    bool isEnabled = ui->checkForEachVelocityRange->isChecked();
    ui->listVelocites->setEnabled(isEnabled);
    ui->pushAdd->setEnabled(isEnabled);
    ui->pushRemove->setEnabled(isEnabled);
    ui->spinMaxVel->setEnabled(isEnabled);
    ui->spinMinVel->setEnabled(isEnabled);
}

void ToolDivisionDuplication_gui::on_buttonBox_accepted()
{       
    emit(this->validated());
}

void ToolDivisionDuplication_gui::on_buttonBox_rejected()
{
    emit(this->canceled());
}

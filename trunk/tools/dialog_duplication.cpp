/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2016 Davy Triponney                                **
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
**  Website/Contact: http://polyphone-soundfonts.com                      **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "dialog_duplication.h"
#include "ui_dialog_duplication.h"
#include "confmanager.h"
#include <stdio.h>

DialogDuplication::DialogDuplication(bool isPrst, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogDuplication),
    _isPrst(isPrst)
{
    // Preparation of the interface
    ui->setupUi(this);
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint));

    // Recall values
    _listeVelocites = getStoredVelocites();
    ui->checkForEachKey->setChecked(ConfManager::getInstance()->getToolValue(
                                        _isPrst ? ConfManager::TOOL_TYPE_PRESET : ConfManager::TOOL_TYPE_INSTRUMENT,
                                        "duplication", "duplicationKeys", true).toBool());
    ui->checkForEachVelocityRange->setChecked(ConfManager::getInstance()->getToolValue(
                                                  _isPrst ? ConfManager::TOOL_TYPE_PRESET : ConfManager::TOOL_TYPE_INSTRUMENT,
                                                  "duplication", "duplicationVelocity", true).toBool());
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

QVector<QPair<int, int> > DialogDuplication::getStoredVelocites()
{
    QList<QVariant> defaultList;
    defaultList << 0 << 127;
    QList<QVariant> listTmp = ConfManager::getInstance()->getToolValue(
                _isPrst ? ConfManager::TOOL_TYPE_PRESET : ConfManager::TOOL_TYPE_INSTRUMENT,
                "duplication", "velocities", defaultList).toList();
    QVector<QPair<int, int> > vectRet;
    vectRet.resize(listTmp.size() / 2);
    for (int i = 0; i < listTmp.size() / 2; i++)
    {
        vectRet[i].first = listTmp[2 * i].toDouble();
        vectRet[i].second = listTmp[2 * i + 1].toDouble();
    }
    return vectRet;
}

void DialogDuplication::storeVelocities(QVector<QPair<int, int> > val)
{
    QVariantList listTmp;
    for (int i = 0; i < val.size(); i++)
        listTmp << val[i].first << val[i].second;
    ConfManager::getInstance()->setToolValue(
                _isPrst ? ConfManager::TOOL_TYPE_PRESET : ConfManager::TOOL_TYPE_INSTRUMENT,
                "duplication", "velocities", listTmp);
}

void DialogDuplication::dispVel()
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

DialogDuplication::~DialogDuplication()
{
    delete ui;
}

void DialogDuplication::on_pushAdd_clicked()
{
    int minVel = qMin(ui->spinMinVel->value(), ui->spinMaxVel->value());
    int maxVel = qMax(ui->spinMinVel->value(), ui->spinMaxVel->value());
    _listeVelocites.append(QPair<int, int>(minVel, maxVel));
    this->dispVel();
    ui->listVelocites->setCurrentRow(ui->listVelocites->count() - 1);
}

void DialogDuplication::on_pushRemove_clicked()
{
    int index = ui->listVelocites->currentRow();
    if (index >= 0 && _listeVelocites.size() > 1)
    {
        _listeVelocites.remove(index, 1);
        this->dispVel();
    }
}

void DialogDuplication::on_listVelocites_currentRowChanged(int currentRow)
{
    ui->spinMinVel->blockSignals(true);
    ui->spinMaxVel->blockSignals(true);
    ui->spinMinVel->setValue(_listeVelocites[currentRow].first);
    ui->spinMaxVel->setValue(_listeVelocites[currentRow].second);
    ui->spinMinVel->blockSignals(false);
    ui->spinMaxVel->blockSignals(false);
}

void DialogDuplication::on_spinMinVel_valueChanged(int arg1)
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
    // Save values
    storeVelocities(_listeVelocites);
    bool duplicKey = ui->checkForEachKey->isChecked();
    bool duplicVel = ui->checkForEachVelocityRange->isChecked();
    ConfManager::getInstance()->setToolValue(_isPrst ? ConfManager::TOOL_TYPE_PRESET : ConfManager::TOOL_TYPE_INSTRUMENT,
                                             "duplication", "duplicationKeys", duplicKey);
    ConfManager::getInstance()->setToolValue(_isPrst ? ConfManager::TOOL_TYPE_PRESET : ConfManager::TOOL_TYPE_INSTRUMENT,
                                             "duplication", "duplicationVelocity", duplicVel);

    // Send values
    if ((duplicKey || duplicVel) && _listeVelocites.size())
        emit(accepted(_listeVelocites, duplicKey, duplicVel));
    QDialog::accept();
}

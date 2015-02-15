/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2014 Davy Triponney                                **
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

#include "dialog_export.h"
#include "ui_dialog_export.h"
#include "pile_sf2.h"
#include "config.h"
#include <QFileDialog>

DialogExport::DialogExport(Pile_sf2 *sf2, EltID idSf2, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogExport),
    _initialID(idSf2)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // Remplissage de la liste
    idSf2.typeElement = elementPrst;
    char T[40];
    for (int i = 0; i < sf2->count(idSf2); i++)
    {
        idSf2.indexElt = i;
        if (!sf2->get(idSf2, champ_hidden).bValue)
        {
            sprintf(T, "%.3d:%.3d %s", sf2->get(idSf2, champ_wBank).wValue,
                    sf2->get(idSf2, champ_wPreset).wValue,
                    sf2->getQstr(idSf2, champ_name).toStdString().c_str());
            QListWidgetItem * item = new QListWidgetItem(T);
            item->setCheckState(Qt::Checked);
            item->setData(Qt::UserRole, i);
            ui->listPresets->addItem(item);
        }
    }

    // Tri
    ui->listPresets->sortItems();

    ui->comboFormat->setCurrentIndex(Config::getInstance()->getExportType());
    on_comboFormat_currentIndexChanged(ui->comboFormat->currentIndex());
    ui->lineFolder->setText(Config::getInstance()->getLastDirectory(Config::typeFichierExport));

    ui->checkBank->setChecked(Config::getInstance()->getExportBank());
    ui->checkPreset->setChecked(Config::getInstance()->getExportPreset());
    ui->checkGM->setChecked(Config::getInstance()->getExportGM());
}

DialogExport::~DialogExport()
{
    delete ui;
}

void DialogExport::on_pushTick_clicked()
{
    // Tout sélectionner
    for (int i = 0; i < ui->listPresets->count(); i++)
        ui->listPresets->item(i)->setCheckState(Qt::Checked);
}

void DialogExport::on_pushUntick_clicked()
{
    // Tout décocher
    for (int i = 0; i < ui->listPresets->count(); i++)
        ui->listPresets->item(i)->setCheckState(Qt::Unchecked);
}

void DialogExport::on_pushFolder_clicked()
{
    QString qDir = QFileDialog::getExistingDirectory(this, trUtf8("Choisir un répertoire de destination"),
                                                     ui->lineFolder->text());
    if (!qDir.isEmpty())
        ui->lineFolder->setText(qDir);
}

void DialogExport::on_pushAnnuler_clicked()
{
    QDialog::close();
}

void DialogExport::on_pushExport_clicked()
{
    // Sauvegarde des paramètres
    Config::getInstance()->setExportType(ui->comboFormat->currentIndex());
    if (QDir(ui->lineFolder->text()).exists())
        Config::getInstance()->addFile(Config::typeFichierExport, ui->lineFolder->text() + "/soundfont.sfz");
    else
        QDialog::close();

    // Création liste ID
    QList<EltID> listID;
    EltID id = _initialID;
    id.typeElement = elementPrst;
    for (int i = 0; i < ui->listPresets->count(); i++)
    {
        QListWidgetItem *checkBoxItem = ui->listPresets->item(i);
        if (checkBoxItem->checkState() == Qt::Checked)
        {
            id.indexElt = checkBoxItem->data(Qt::UserRole).toInt();
            listID.append(id);
        }
    }

    Config::getInstance()->setExportPreset(ui->checkPreset->isChecked());
    Config::getInstance()->setExportBank(ui->checkBank->isChecked());
    Config::getInstance()->setExportGM(ui->checkGM->isChecked());

    emit(accepted(listID, ui->lineFolder->text(), ui->comboFormat->currentIndex(),
                  ui->checkPreset->isChecked(), ui->checkBank->isChecked(),
                  ui->checkGM->isChecked()));
    QDialog::accept();
}

void DialogExport::on_comboFormat_currentIndexChanged(int index)
{
    ui->checkBank->setVisible(index == 1);
    ui->checkGM->setVisible(index == 1);
    ui->checkPreset->setVisible(index == 1);
}

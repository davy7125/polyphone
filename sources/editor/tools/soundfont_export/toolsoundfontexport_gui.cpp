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

#include "toolsoundfontexport_gui.h"
#include "ui_toolsoundfontexport_gui.h"
#include "toolsoundfontexport_parameters.h"
#include "contextmanager.h"
#include "soundfontmanager.h"
#include <QFileDialog>
#include <QMessageBox>

ToolSoundfontExport_gui::ToolSoundfontExport_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolSoundfontExport_gui)
{
    ui->setupUi(this);
    ui->pushFolder->setIcon(ContextManager::theme()->getColoredSvg(":/icons/document-open.svg", QSize(16, 16), ThemeManager::WINDOW_TEXT));
}

ToolSoundfontExport_gui::~ToolSoundfontExport_gui()
{
    delete ui;
}

void ToolSoundfontExport_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    Q_UNUSED(ids)

    // Populate the tree
    ui->listPresets->clear();
    SoundfontManager * sm = SoundfontManager::getInstance();
    EltID idSf2(elementSf2);
    foreach (int i, sm->getSiblings(idSf2))
    {
        EltID id(elementSf2, i);
        QTreeWidgetItem * rootItem = new QTreeWidgetItem(QStringList(sm->getQstr(id, champ_name)));
        ui->listPresets->addTopLevelItem(rootItem);

        id.typeElement = elementPrst;
        foreach (int i, sm->getSiblings(id))
        {
            id.indexElt = i;
            QTreeWidgetItem * item = new QTreeWidgetItem(
                        QStringList() << QString("%1:%2   %3").arg(QString::number(sm->get(id, champ_wBank).wValue), 3, QChar('0'))
                        .arg(QString::number(sm->get(id, champ_wPreset).wValue), 3, QChar('0'))
                        .arg(sm->getQstr(id, champ_name)));
            item->setCheckState(0, Qt::Checked);
            item->setData(0, Qt::UserRole, QPoint(id.indexSf2, id.indexElt));
            rootItem->addChild(item);
            rootItem->setExpanded(true);
        }

        // Tri
        rootItem->sortChildren(0, Qt::AscendingOrder);
    }
    ui->listPresets->resizeColumnToContents(0);

    // Load parameters
    ToolSoundfontExport_parameters * params = (ToolSoundfontExport_parameters *) parameters;
    int exportType = params->getFormat();
    if (exportType < 0 || exportType >= ui->comboFormat->count())
        exportType = 0;
    ui->comboFormat->setCurrentIndex(exportType);
    on_comboFormat_currentIndexChanged(exportType);

    ui->lineFolder->setText(params->getDirectory());
    ui->checkBank->setChecked(params->getBankDirectory());
    ui->checkPreset->setChecked(params->getPresetPrefix());
    ui->checkGM->setChecked(params->getGmSort());

    int exportQuality = params->getQuality();
    if (exportQuality < 0 || exportQuality >= ui->comboQuality->count())
        exportQuality = 1;
    ui->comboQuality->setCurrentIndex(2 - exportQuality);
}

void ToolSoundfontExport_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolSoundfontExport_parameters * params = (ToolSoundfontExport_parameters *) parameters;

    // Sauvegarde des paramètres
    params->setDirectory(ui->lineFolder->text());
    params->setFormat(ui->comboFormat->currentIndex());
    params->setQuality(2 - ui->comboQuality->currentIndex());
    params->setPresetPrefix(ui->checkPreset->isChecked());
    params->setBankDirectory(ui->checkBank->isChecked());
    params->setGmSort(ui->checkGM->isChecked());

    // Preset list
    params->setSelectedPresets(_presetList);
}

void ToolSoundfontExport_gui::on_pushTick_clicked()
{
    // Tout sélectionner
    int nbTopLevelItems = ui->listPresets->topLevelItemCount();
    for (int i = 0; i < nbTopLevelItems; i++)
    {
        QTreeWidgetItem * rootItem = ui->listPresets->topLevelItem(i);
        int nbItems = rootItem->childCount();
        for (int j = 0; j < nbItems; j++)
            rootItem->child(j)->setCheckState(0, Qt::Checked);
    }
}

void ToolSoundfontExport_gui::on_pushUntick_clicked()
{
    // Tout décocher
    int nbTopLevelItems = ui->listPresets->topLevelItemCount();
    for (int i = 0; i < nbTopLevelItems; i++)
    {
        QTreeWidgetItem * rootItem = ui->listPresets->topLevelItem(i);
        int nbItems = rootItem->childCount();
        for (int j = 0; j < nbItems; j++)
            rootItem->child(j)->setCheckState(0, Qt::Unchecked);
    }
}

void ToolSoundfontExport_gui::on_pushFolder_clicked()
{
    QString qDir = QFileDialog::getExistingDirectory(this, trUtf8("Select the destination directory"),
                                                     ui->lineFolder->text());
    if (!qDir.isEmpty())
        ui->lineFolder->setText(qDir);
}

void ToolSoundfontExport_gui::on_pushAnnuler_clicked()
{
    emit(this->canceled());
}

void ToolSoundfontExport_gui::on_pushExport_clicked()
{
    // Check the directory
    if (ui->lineFolder->text().isEmpty() || !QDir(ui->lineFolder->text()).exists())
    {
        QMessageBox::warning(this, trUtf8("Warning"), trUtf8("Invalid directory."));
        return;
    }

    // Update the preset list by soundfont
    _presetList.clear();
    for (int i = 0; i < ui->listPresets->topLevelItemCount(); i++)
    {
        QTreeWidgetItem * rootItem = ui->listPresets->topLevelItem(i);
        for (int j = 0; j < rootItem->childCount(); j++)
        {
            QTreeWidgetItem * item = rootItem->child(j);
            if (item->checkState(0) == Qt::Checked)
            {
                QPoint dataPoint = item->data(0, Qt::UserRole).toPoint();
                _presetList[dataPoint.x()] << dataPoint.y();
            }
        }
    }

    // Remove empty lists
    QList<int> soundfonts = _presetList.keys();
    foreach (int soundfont, soundfonts)
        if (_presetList[soundfont].empty())
            _presetList.remove(soundfont);

    // Check on the number of selected elements
    if (_presetList.empty())
    {
        QMessageBox::warning(this, trUtf8("Warning"), trUtf8("At least one preset must be selected."));
        return;
    }

    if (_presetList.count() > 127)
    {
        QMessageBox::warning(this, trUtf8("Warning"), trUtf8("The maximal number of soundfont to export is 127."));
        return;
    }

    int maxNumberOfPresets = 0;
    foreach (int i, _presetList.keys())
        maxNumberOfPresets += qMax(maxNumberOfPresets, _presetList[i].count());
    if (_presetList.count() > 1 && maxNumberOfPresets > 127)
    {
        QMessageBox::warning(this, trUtf8("Warning"), trUtf8("In the case where several soundfonts are exported, the maximal number of presets per soundfonts is 127."));
        return;
    }

    emit(this->validated());
}

void ToolSoundfontExport_gui::on_comboFormat_currentIndexChanged(int index)
{
    // Options for sf3
    ui->labelQuality->setVisible(index == 1);
    ui->comboQuality->setVisible(index == 1);

    // Options for sfz
    ui->checkBank->setVisible(index == 2);
    ui->checkGM->setVisible(index == 2);
    ui->checkPreset->setVisible(index == 2);
}

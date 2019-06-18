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

#include "toolpresetlist_gui.h"
#include "ui_toolpresetlist_gui.h"
#include "toolpresetlist_parameters.h"
#include "contextmanager.h"
#include <QClipboard>
#include "soundfontmanager.h"

ToolPresetList_gui::ToolPresetList_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolPresetList_gui)
{
    ui->setupUi(this);
    ui->label->setStyleSheet("QLabel { color : " + ContextManager::theme()->getFixedColor(
                                 ThemeManager::GREEN, ThemeManager::WINDOW_BACKGROUND).name() + "; }");
}

ToolPresetList_gui::~ToolPresetList_gui()
{
    delete ui;
}

void ToolPresetList_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    Q_UNUSED(ids)
    ToolPresetList_parameters * params = (ToolPresetList_parameters *) parameters;

    // Preset list by bank and preset number
    _mapName.clear();
    SoundfontManager * sm = SoundfontManager::getInstance();
    foreach (EltID id, ids.getSelectedIds(elementSf2)) // Should be only one sf2
    {
        id.typeElement = elementPrst;
        foreach (int i, sm->getSiblings(id))
        {
            id.indexElt = i;
            _mapName[sm->get(id, champ_wBank).wValue][sm->get(id, champ_wPreset).wValue] = sm->getQstr(id, champ_name);
        }
    }

    // Initialize the interface
    ui->label->hide();

    // Recall previous format
    switch (params->getFormat())
    {
    case 1:
        ui->radioHtml->setChecked(true);
        on_radioHtml_clicked();
        break;
    default:
        ui->radioCsv->setChecked(true);
        on_radioCsv_clicked();
        break;
    }
}

void ToolPresetList_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolPresetList_parameters * params = (ToolPresetList_parameters *) parameters;

    // Save the current format
    params->setFormat(ui->radioCsv->isChecked() ? 0 : 1);
}

void ToolPresetList_gui::on_radioCsv_clicked()
{
    // Format a css
    QString str = "";
    QList<int> listBank = _mapName.keys();
    qSort(listBank.begin(), listBank.end());
    for (int i = 0; i < listBank.count(); i++)
    {
        int bankNumber = listBank[i];
        QList<int> listPreset = _mapName[bankNumber].keys();
        qSort(listPreset.begin(), listPreset.end());
        for (int j = 0; j < listPreset.count(); j++)
        {
            int presetNumber = listPreset[j];
            if (!str.isEmpty())
                str += "\n";
            str += QString::number(bankNumber) + "\t" +
                    QString::number(presetNumber) + "\t" +
                    _mapName[bankNumber][presetNumber];
        }
    }

    ui->plainTextEdit->setPlainText(str);
}

void ToolPresetList_gui::on_radioHtml_clicked()
{
    // Format a html table
    QString str = "<table><thead><tr><th>Bank</th><th>Preset</th><th>Name</th></tr></thead><tbody>";
    QList<int> listBank = _mapName.keys();
    qSort(listBank.begin(), listBank.end());
    for (int i = 0; i < listBank.count(); i++)
    {
        int bankNumber = listBank[i];
        QList<int> listPreset = _mapName[bankNumber].keys();
        qSort(listPreset.begin(), listPreset.end());
        for (int j = 0; j < listPreset.count(); j++)
        {
            int presetNumber = listPreset[j];
            str += "<tr><td>" + QString::number(bankNumber) + "</td><td>" +
                    QString::number(presetNumber) + "</td><td>" +
                    _mapName[bankNumber][presetNumber] + "</td></tr>";
        }
    }
    str += "</tbody></table>";

    ui->plainTextEdit->setPlainText(str);
}

void ToolPresetList_gui::on_pushCopy_clicked()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(ui->plainTextEdit->toPlainText());
    ui->label->show();
}

void ToolPresetList_gui::on_pushClose_clicked()
{
    emit(this->validated());
}

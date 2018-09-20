/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2017 Davy Triponney                                **
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

#include "dialog_exportlist.h"
#include "ui_dialog_exportlist.h"
#include "contextmanager.h"
#include <QClipboard>

DialogExportList::DialogExportList(SoundfontManager *sf2, EltID id, QWidget *parent) :
    QDialog(parent),
    _init(true),
    ui(new Ui::DialogExportList)
{
    // Prepare the interface
    ui->setupUi(this);
    ui->label->hide();
    ui->label->setStyleSheet("QLabel { color : " + this->palette().color(QPalette::NoRole).name() + "; }");

    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint));

    // List of preset names by bank and preset number
    id.typeElement = elementPrst;
    foreach (int i, sf2->getSiblings(id))
    {
        id.indexElt = i;
        _mapName[sf2->get(id, champ_wBank).wValue][sf2->get(id, champ_wPreset).wValue] = sf2->getQstr(id, champ_name);
    }

    // Recall previous format
    _init = false;
    switch (ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_SF2, "presetlist", "format", 0).toInt())
    {
    case 0:
        ui->radioCsv->setChecked(true);
        on_radioCsv_clicked();
        break;
    case 1:
        ui->radioHtml->setChecked(true);
        on_radioHtml_clicked();
        break;
    }
}

DialogExportList::~DialogExportList()
{
    delete ui;
}

void DialogExportList::on_radioCsv_clicked()
{
    if (_init)
        return;

    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_SF2, "presetlist", "format", 0);

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

void DialogExportList::on_radioHtml_clicked()
{
    if (_init)
        return;

    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_SF2, "presetlist", "format", 1);

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

void DialogExportList::on_pushCopy_clicked()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(ui->plainTextEdit->toPlainText());
    ui->label->show();
}

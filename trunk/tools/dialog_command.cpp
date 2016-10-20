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

#include "dialog_command.h"
#include "ui_dialog_command.h"
#include "confmanager.h"
#include "recentfilemanager.h"
#include <QMessageBox>
#include <QFileDialog>

const int DialogCommand::HISTORY_SIZE = 10;

DialogCommand::DialogCommand(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogCommand)
{
    // Prepare the interface
    ui->setupUi(this);
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint));

    // Load previous commands
    for (int i = 0; i < HISTORY_SIZE; i++)
    {
        QString previous = ConfManager::getInstance()->getToolValue(
                    ConfManager::TOOL_TYPE_SAMPLE, "command",
                    "previous_" + QString("%1").arg(i+1, 2, 10, QChar('0')), "").toString();
        if (!previous.isEmpty())
            ui->comboPrevious->addItem(previous);
    }
    if (ui->comboPrevious->count() == 0)
        ui->comboPrevious->addItem("-");
    ui->checkStereo->setChecked(ConfManager::getInstance()->getToolValue(
                                    ConfManager::TOOL_TYPE_SAMPLE, "command",
                                    "stereo", true).toBool());
    ui->checkRemplaceInfo->setChecked(ConfManager::getInstance()->getToolValue(
                                          ConfManager::TOOL_TYPE_SAMPLE, "command",
                                          "replace_info", false).toBool());

    // Select the text
    ui->lineCommand->setFocus();
}

DialogCommand::~DialogCommand()
{
    delete ui;
}

void DialogCommand::on_comboPrevious_currentIndexChanged(const QString &arg1)
{
    // Recall a previous command
    ui->lineCommand->setText(arg1);
}

void DialogCommand::on_pushOpen_clicked()
{
    // Open an executable
    QString command = QFileDialog::getOpenFileName(
                this, trUtf8("Fichier exécutable"),
                RecentFileManager::getInstance()->getLastDirectory(RecentFileManager::FILE_TYPE_SOUNDFONT));

    if (command != "")
        ui->lineCommand->setText("\"" + command + "\" {wav}");
}

void DialogCommand::accept()
{
    QString command = ui->lineCommand->text();

    // Split the command
    QStringList split = command.split(QRegExp(" +(?=(?:[^\"]*\"[^\"]*\")*[^\"]*$)"));

    // Command with at least "wav" as argument
    if (split.count() < 2 || split.first() == "{wav}")
    {
        QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Vous devez entrer une commande avec au moins {wav} comme argument."));
        return;
    }

    // Check that {wav} is present
    if (!split.contains("{wav}"))
    {
        QMessageBox::warning(this, trUtf8("Attention"), trUtf8("La commande doit contenir l'argument {wav}."));
        return;
    }

    // Store the command used
    QList<QString> previousCommands;
    previousCommands << command;
    for (int i = 0; i < ui->comboPrevious->count(); i++)
    {
        QString strTmp = ui->comboPrevious->itemText(i);
        if (strTmp != "-" && !previousCommands.contains(strTmp))
            previousCommands << strTmp;
    }
    for (int i = 0; i < qMin(HISTORY_SIZE, previousCommands.count()); i++)
        ConfManager::getInstance()->setToolValue(
                    ConfManager::TOOL_TYPE_SAMPLE, "command",
                    "previous_" + QString("%1").arg(i+1, 2, 10, QChar('0')), previousCommands[i]);
    ConfManager::getInstance()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "command",
                                             "stereo", ui->checkStereo->isChecked());
    ConfManager::getInstance()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "command",
                                             "replace_info", ui->checkRemplaceInfo->isChecked());

    emit(accepted(command, ui->checkStereo->isChecked(), ui->checkRemplaceInfo->isChecked()));
    QDialog::accept();
}

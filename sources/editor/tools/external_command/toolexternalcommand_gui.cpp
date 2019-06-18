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

#include "toolexternalcommand_gui.h"
#include "ui_toolexternalcommand_gui.h"
#include "contextmanager.h"
#include "toolexternalcommand_parameters.h"
#include <QMessageBox>
#include <QFileDialog>

const int ToolExternalCommand_gui::HISTORY_SIZE = 10;

ToolExternalCommand_gui::ToolExternalCommand_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolExternalCommand_gui)
{
    // Prepare the interface
    ui->setupUi(this);
    ui->pushOpen->setIcon(ContextManager::theme()->getColoredSvg(":/icons/document-open.svg", QSize(24, 24), ThemeManager::WINDOW_TEXT));

    // Select the text
    ui->lineCommand->setFocus();
}

ToolExternalCommand_gui::~ToolExternalCommand_gui()
{
    delete ui;
}

void ToolExternalCommand_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    Q_UNUSED(ids)
    ToolExternalCommand_parameters * params = (ToolExternalCommand_parameters *)parameters;

    // Command history
    ui->comboPrevious->clear();
    ui->comboPrevious->addItems(params->getCommandHistory());
    if (ui->comboPrevious->count() == 0)
        ui->comboPrevious->addItem("-");
    if (!params->getCommandHistory().empty())
        ui->lineCommand->setText(params->getCommandHistory()[0]);

    // Stereo and replace info
    ui->checkStereo->setChecked(params->getStereo());
    ui->checkReplaceInfo->setChecked(params->getReplaceInfo());
}

void ToolExternalCommand_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolExternalCommand_parameters * params = (ToolExternalCommand_parameters *)parameters;

    // Update the command history
    QList<QString> history;
    history << ui->lineCommand->text();
    for (int i = 0; i < ui->comboPrevious->count(); i++)
    {
        QString strTmp = ui->comboPrevious->itemText(i);
        if (strTmp != "-" && !history.contains(strTmp))
            history << strTmp;
    }
    params->setCommandHistory(history);

    // Update stereo and replace info
    params->setStereo(ui->checkStereo->isChecked());
    params->setReplaceInfo(ui->checkReplaceInfo->isChecked());
}

void ToolExternalCommand_gui::on_comboPrevious_currentIndexChanged(const QString &arg1)
{
    // Recall a previous command
    ui->lineCommand->setText(arg1);
}

void ToolExternalCommand_gui::on_pushOpen_clicked()
{
    // Open an executable
    QString command = QFileDialog::getOpenFileName(
                this, trUtf8("Executable file"),
                ContextManager::recentFile()->getLastDirectory(RecentFileManager::FILE_TYPE_SOUNDFONT));

    if (command != "")
        ui->lineCommand->setText("\"" + command + "\" {wav}");
}

void ToolExternalCommand_gui::on_buttonBox_accepted()
{
    QString command = ui->lineCommand->text();

    // Split the command
    QStringList split = command.split(QRegExp(" +(?=(?:[^\"]*\"[^\"]*\")*[^\"]*$)"));

    // Command with at least "wav" as argument
    if (split.count() < 2 || split.first() == "{wav}")
    {
        QMessageBox::warning(this, trUtf8("Warning"), trUtf8("You must enter a command with at least {wav} as argument."));
        return;
    }

    // Check that {wav} is present
    if (!split.contains("{wav}"))
    {
        QMessageBox::warning(this, trUtf8("Warning"), trUtf8("The command must contain the argument {wav}."));
        return;
    }

    // Notify about the validation
    emit(this->validated());
}

void ToolExternalCommand_gui::on_buttonBox_rejected()
{
    emit(this->canceled());
}

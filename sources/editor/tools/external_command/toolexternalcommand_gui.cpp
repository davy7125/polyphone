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

    ui->comboPrevious->addItems(params->getCommandHistory());
    if (ui->comboPrevious->count() == 0)
        ui->comboPrevious->addItem("-");

    ui->checkStereo->setChecked(params->getStereo());
    ui->checkRemplaceInfo->setChecked(params->getReplaceInfo());
}

void ToolExternalCommand_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolExternalCommand_parameters * params = (ToolExternalCommand_parameters *)parameters;

    params->setStereo(ui->checkStereo->isChecked());
    params->setReplaceInfo(ui->checkRemplaceInfo->isChecked());
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
                this, trUtf8("Fichier exécutable"),
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
        ContextManager::configuration()->setToolValue(
                    ConfManager::TOOL_TYPE_SAMPLE, "command",
                    "previous_" + QString("%1").arg(i+1, 2, 10, QChar('0')), previousCommands[i]);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "command",
                                             "stereo", ui->checkStereo->isChecked());
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "command",
                                             "replace_info", ui->checkRemplaceInfo->isChecked());

    // Notify about the validation
    emit(this->validated());
}

void ToolExternalCommand_gui::on_buttonBox_rejected()
{
    emit(this->canceled());
}

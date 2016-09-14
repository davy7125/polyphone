#include "dialog_transposition.h"
#include "ui_dialog_transposition.h"
#include "confmanager.h"

DialogTransposition::DialogTransposition(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogTransposition)
{
    // Prepare the interface
    ui->setupUi(this);
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint));
    this->setAttribute(Qt::WA_DeleteOnClose, true);

    // Recall last configuration
    ui->spinTon->setValue(ConfManager::getInstance()->getToolValue(
                              ConfManager::TOOL_TYPE_INSTRUMENT, "transposition", "ton", 12.).toDouble());
    ui->checkKeyRange->setChecked(ConfManager::getInstance()->getToolValue(
                                      ConfManager::TOOL_TYPE_INSTRUMENT, "transposition", "keyrange", 12).toBool());
}

DialogTransposition::~DialogTransposition()
{
    delete ui;
}

void DialogTransposition::on_buttonBox_accepted()
{
    // Save configuration
    ConfManager::getInstance()->setToolValue(
                ConfManager::TOOL_TYPE_INSTRUMENT, "transposition", "ton", ui->spinTon->value());
    ConfManager::getInstance()->setToolValue(
                ConfManager::TOOL_TYPE_INSTRUMENT, "transposition", "keyrange", ui->checkKeyRange->isChecked());

    // Emit signal and quit
    accepted(ui->spinTon->value(), ui->checkKeyRange->isChecked());
    this->close();
}

void DialogTransposition::on_buttonBox_rejected()
{
    this->close();
}

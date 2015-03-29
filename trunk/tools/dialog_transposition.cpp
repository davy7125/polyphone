#include "dialog_transposition.h"
#include "ui_dialog_transposition.h"
#include "config.h"

DialogTransposition::DialogTransposition(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogTransposition)
{
    ui->setupUi(this);
    this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->setAttribute(Qt::WA_DeleteOnClose, true);

    // Load last configuration
    Config * conf = Config::getInstance();
    ui->spinTon->setValue(conf->getTools_i_transpo_ton());
    ui->checkKeyRange->setChecked(conf->getTools_i_transpo_keyrange());
}

DialogTransposition::~DialogTransposition()
{
    delete ui;
}

void DialogTransposition::on_pushCancel_clicked()
{
    this->close();
}

void DialogTransposition::on_pushOk_clicked()
{
    // Save configuration
    Config * conf = Config::getInstance();
    conf->setTools_i_transpo_ton(ui->spinTon->value());
    conf->setTools_i_transpo_keyrange(ui->checkKeyRange->isChecked());

    // Emit signal and quit
    accepted(ui->spinTon->value(), ui->checkKeyRange->isChecked());
    this->close();
}

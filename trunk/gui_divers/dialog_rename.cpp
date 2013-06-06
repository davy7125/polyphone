#include "dialog_rename.h"
#include "ui_dialog_rename.h"

DialogRename::DialogRename(QString defaultValue, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogRename)
{
    ui->setupUi(this);
    this->ui->lineEdit->setText(defaultValue);
    this->ui->lineEdit->selectAll();
    this->ui->lineEdit->setFocus();
}

DialogRename::~DialogRename()
{
    delete ui;
}

void DialogRename::accept()
{
    int type = -1;
    if (ui->radioRootkey->isChecked())
        type = 0;
    else if (ui->radioIncrement->isChecked())
        type = 1;
    else if (ui->radioPrefix->isChecked())
        type = 2;
    else if (ui->radioSuffix->isChecked())
        type = 3;
    if (type >= 0 && !ui->lineEdit->text().isEmpty())
        this->updateNames(ui->lineEdit->text(), type);

    QDialog::accept();
}

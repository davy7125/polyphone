#include "waitingtooldialog.h"
#include "ui_waitingtooldialog.h"
#include <QCloseEvent>

WaitingToolDialog::WaitingToolDialog(QString title, int stepNumber, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WaitingToolDialog),
    _stepNumber(stepNumber),
    _isCanceled(false)
{
    ui->setupUi(this);
    this->setWindowTitle(title);
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint));
    this->setWindowModality(Qt::WindowModal);
    this->setFixedWidth(350);

    // Progress bar
    ui->progressBar->setMinimum(0);
    this->setValue(0);
}

WaitingToolDialog::~WaitingToolDialog()
{
    delete ui;
}

void WaitingToolDialog::on_pushCancel_clicked()
{
    this->cancel();
}

void WaitingToolDialog::setValue(int value)
{
    if (_isCanceled)
        return;
    ui->progressBar->setMaximum(value == 0 ? 0 : _stepNumber);
    ui->progressBar->setValue(value);
}

void WaitingToolDialog::closeEvent(QCloseEvent *event)
{
    this->cancel();
    event->ignore();
}

void WaitingToolDialog::cancel()
{
    if (_isCanceled)
        return;

    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(0);
    ui->labelProcess->setText(trUtf8("Canceling..."));
    _isCanceled = true;
    emit(canceled());
}

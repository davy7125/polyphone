#include "modulatorcell.h"
#include "ui_modulatorcell.h"

ModulatorCell::ModulatorCell(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModulatorCell)
{
    ui->setupUi(this);
}

ModulatorCell::~ModulatorCell()
{
    delete ui;
}

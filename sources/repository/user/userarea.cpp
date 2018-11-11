#include "userarea.h"
#include "ui_userarea.h"

UserArea::UserArea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserArea)
{
    ui->setupUi(this);
}

UserArea::~UserArea()
{
    delete ui;
}

void UserArea::initializeInterface()
{
    // TODO
}

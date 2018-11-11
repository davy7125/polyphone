#include "soundfontviewer.h"
#include "ui_soundfontviewer.h"

SoundfontViewer::SoundfontViewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SoundfontViewer)
{
    ui->setupUi(this);
}

SoundfontViewer::~SoundfontViewer()
{
    delete ui;
}

void SoundfontViewer::initialize(int soundfontId)
{
    _soundfontId = soundfontId;
}

#include "soundfontdownload.h"
#include "ui_soundfontdownload.h"
#include "soundfontdownloaddata.h"
#include "soundfontdownloadcell.h"

SoundfontDownload::SoundfontDownload(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SoundfontDownload)
{
    ui->setupUi(this);
}

SoundfontDownload::~SoundfontDownload()
{
    delete ui;
}

bool sortFunction(const SoundfontDownloadData *s1, const SoundfontDownloadData *s2)
{
    return s1->getOrdering() < s2->getOrdering();
}

void SoundfontDownload::display(QList<SoundfontDownloadData *> data)
{
    // First order the list
    qSort(data.begin(), data.end(), sortFunction);

    // Add each download
    for (int i = 0; i < data.count(); i++)
    {
        SoundfontDownloadCell * cell = new SoundfontDownloadCell(this);
        cell->initialize(data[i]);
        ui->verticalLayout->addWidget(cell);
    }
}

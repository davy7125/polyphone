#include "soundfontdownloadcell.h"
#include "ui_soundfontdownloadcell.h"
#include "soundfontdownloaddata.h"
#include "contextmanager.h"
#include "downloadmanager.h"

SoundfontDownloadCell::SoundfontDownloadCell(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SoundfontDownloadCell)
{
    ui->setupUi(this);
    ui->pushDownload->setStyleSheet("QPushButton{padding:4px;}");
    ui->pushDownload->setIcon(ContextManager::theme()->getColoredSvg(":/icons/download.svg", QSize(24, 24), ThemeManager::BUTTON_TEXT));
}

SoundfontDownloadCell::~SoundfontDownloadCell()
{
    delete ui;
}

void SoundfontDownloadCell::initialize(SoundfontDownloadData * data)
{
    // Title
    ui->labelTitle->setText(data->getName());

    // Description, if possible
    if (data->getDescription().isEmpty())
        ui->labelDescription->hide();
    else
        ui->labelDescription->setText(data->getDescription());

    // Format informations
    ui->labelInfo->setText(QString("(%1, %2)").arg(getSize(data)).arg(getDate(data)));

    // Store data
    _id = data->getId();
    _name = data->getName();
}

QString SoundfontDownloadCell::getSize(SoundfontDownloadData * data)
{
    qint64 bytes = data->getSize();
    if (bytes < 1024)
        return QString::number(bytes) + " " + trUtf8("bytes");
    if (bytes < 1024 * 1024)
        return QString::number((double)bytes / 1024, 'f', 2) + " " + trUtf8("kB", "kilobytes");
    if (bytes < 1024 * 1024 * 1024)
        return QString::number((double)bytes / 1024 / 1024, 'f', 2) + " " + trUtf8("MB", "megabytes");
    if (bytes / 1024 < 1024 * 1024 * 1024)
        return QString::number((double)bytes / 1024 / 1024 / 1024, 'f', 2) + " " + trUtf8("GB", "gigabytes");
    return QString::number((double)bytes / 1024 / 1024 / 1024 / 1024, 'f', 2) + " " + trUtf8("TB", "terabytes");
}

QString SoundfontDownloadCell::getDate(SoundfontDownloadData * data)
{
    QDateTime lastDate = (data->getUpdatedTime().isValid() ? data->getUpdatedTime() : data->getCreatedTime());
    return lastDate.date().toString(Qt::SystemLocaleShortDate);
}

void SoundfontDownloadCell::on_pushDownload_clicked()
{
    DownloadManager::getInstance()->download(_id, _name);
}

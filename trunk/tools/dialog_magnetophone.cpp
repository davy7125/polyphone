#include "dialog_magnetophone.h"
#include "ui_dialog_magnetophone.h"
#include <QTime>
#include <QFileInfo>
#include <QDir>
#include <QDesktopServices>
#include <QFileDialog>
#include "config.h"

DialogMagnetophone::DialogMagnetophone(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DialogMagnetophone)
{
    ui->setupUi(this);

    // Initialisation
    this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
    this->initGui();
}

DialogMagnetophone::~DialogMagnetophone()
{
    delete ui;
}

void DialogMagnetophone::initGui()
{
    ui->lcdNumber->display("00:00:00");
    ui->pushRecord->blockSignals(true);
    ui->pushRecord->setChecked(false);
    ui->pushRecord->blockSignals(false);
    ui->pushPlayPause->setIcon(QIcon(":/icones/play"));

    // Initialisation des variables
    _isPause = false;
    _currentSample = 0;
    _sampleRate = 0;
}

void DialogMagnetophone::setSampleRate(qint32 sampleRate)
{
    _sampleRate = sampleRate;
}

void DialogMagnetophone::avanceSamples(int number)
{
    _currentSample += number;
    // Mise à jour de l'affichage
    if (_sampleRate != 0)
    {
        QTime time(0, 0);
        time = time.addMSecs((int)((double)_currentSample / ((double)_sampleRate / 1000.)));
        ui->lcdNumber->display(time.toString("HH:mm:ss"));
    }
}

void DialogMagnetophone::closeEvent(QCloseEvent *)
{
    // Arrêt de l'enregistrement si besoin
    if (ui->pushRecord->isChecked())
        this->endRecord();
    this->initGui();
}

void DialogMagnetophone::on_pushRecord_toggled(bool checked)
{
    if (checked)
    {
        // Nom de fichier
        QString defaultPath = this->getDefaultPath();
        defaultPath = QFileDialog::getSaveFileName(this, tr("Sauvegarder un enregistrement"),
                                                   defaultPath, tr("Fichier .wav (*.wav)"));
        if (defaultPath.size())
        {
            Config::getInstance()->setRecordFile(defaultPath);
            // Début de l'enregistrement
            ui->pushPlayPause->setIcon(QIcon(":/icones/pause"));
            _isPause = false;
            this->startRecord(defaultPath);
        }
        else
        {
            ui->pushRecord->blockSignals(true);
            ui->pushRecord->setChecked(false);
            ui->pushRecord->blockSignals(false);
        }
    }
    else
    {
        // Arrêt de l'enregistrement
        ui->pushPlayPause->setIcon(QIcon(":/icones/play"));
        _isPause = true;
        this->endRecord();
    }
}

void DialogMagnetophone::on_pushPlayPause_clicked()
{
    if (ui->pushRecord->isChecked())
    {
        _isPause = !_isPause;
        if (_isPause)
            ui->pushPlayPause->setIcon(QIcon(":/icones/play"));
        else
            ui->pushPlayPause->setIcon(QIcon(":/icones/pause"));
        this->pause(_isPause);
    }
}

QString DialogMagnetophone::getDefaultPath()
{
    QString defaultPath = Config::getInstance()->getRecordFile();
    QFileInfo info(defaultPath);
    if (info.dir().exists() && defaultPath.size())
    {
        QString name = info.fileName();
        QRegExp exp("^(.*)-[0-9][0-9]*.[wW][aA][vV]$");
        if (exp.exactMatch(name))
            name = exp.cap(1);
        else
            name = name.left(name.size() - 4);
        // Recherche du premier nom disponible pour un nouveau fichier
        QString folderName = info.dir().path();
        if (QFile(folderName + "/" + name + ".wav").exists())
        {
            int suffix = 2;
            while (QFile(folderName + "/" + name + "-" + QString::number(suffix) + ".wav").exists())
                suffix++;
            defaultPath = folderName + "/" + name + "-" + QString::number(suffix);
        }
        else
            defaultPath = folderName + "/" + name;
    }
    else
        defaultPath = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation) + "/" + trUtf8("enregistrement");
    return defaultPath + ".wav";
}

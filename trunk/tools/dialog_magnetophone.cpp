/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "dialog_magnetophone.h"
#include "ui_dialog_magnetophone.h"
#include <QTime>
#include <QFileInfo>
#include <QDir>
#include <QDesktopServices>
#include <QFileDialog>
#include "config.h"
#include "synth.h"

DialogMagnetophone::DialogMagnetophone(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogMagnetophone),
    _sampleRate(0)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
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
        _synth->endRecord();
    this->initGui();
}

void DialogMagnetophone::on_pushRecord_toggled(bool checked)
{
    ui->pushPlayPause->setEnabled(checked);
    if (checked)
    {
        // Nom de fichier
        QString defaultPath = this->getDefaultPath();
        defaultPath = QFileDialog::getSaveFileName(this, trUtf8("Sauvegarder un enregistrement"),
                                                   defaultPath, trUtf8("Fichier .wav (*.wav)"));
        if (defaultPath.size())
        {
            if (defaultPath.right(4).toLower() != ".wav")
                defaultPath.append(".wav");
            Config::getInstance()->addFile(Config::typeFichierEnregistrement, defaultPath);
            // Début de l'enregistrement
            ui->pushPlayPause->setIcon(QIcon(":/icones/pause"));
            _isPause = false;
            _synth->startNewRecord(defaultPath);
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
        this->initGui();
        _synth->endRecord();
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
        _synth->pause(_isPause);
    }
}

QString DialogMagnetophone::getDefaultPath()
{
    QString defaultPath = Config::getInstance()->getLastFile(Config::typeFichierEnregistrement);
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
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        defaultPath = QDir::home().path() + "/" + trUtf8("enregistrement");
#else
        defaultPath = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation) + "/" + trUtf8("enregistrement");
#endif

    return defaultPath + ".wav";
}

/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013 Davy Triponney                                     **
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
****************************************************************************/

#include "config.h"
#include "ui_config.h"
#include <QAudioOutput>
#include <QSettings>
#include <QFileInfo>
#include <QDir>

Config::Config(QWidget *parent) : QDialog(parent), ui(new Ui::Config)
{
    this->loaded = 0;
    ui->setupUi(this);
    // chemin du fichier de configuration (dépendant du système d'exploitation)
    QSettings ini(QSettings::IniFormat, QSettings::UserScope,
                  QCoreApplication::organizationName(),
                  QCoreApplication::applicationName());
    QString dirPath = QFileInfo(ini.fileName()).absolutePath();
    QDir dir(dirPath);
    if (!dir.exists())
        dir.mkdir(dirPath);
    this->confFile =  dirPath + QDir::separator() + "conf";
    // CHARGEMENT DE LA CONFIGURATION
    this->load();
    // initialisation des élements
    if (this->ram)
        ui->comboRam->setCurrentIndex(1);
    else
        ui->comboRam->setCurrentIndex(0);
    // liste des sorties audio
    foreach (const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices(QAudio::AudioOutput))
        this->ui->comboAudioOuput->addItem(deviceInfo.deviceName(), QVariant::fromValue(deviceInfo));
    if (this->ui->comboAudioOuput->count() > this->audioIndex)
        this->ui->comboAudioOuput->setCurrentIndex(this->audioIndex);
    this->ui->checkBoucle->setChecked(this->wavAutoLoop);
    this->ui->checkBlanc->setChecked(this->wavRemoveBlank);
    this->loaded = 1;
}

Config::~Config()
{
    delete ui;
}

void Config::setRam(int val)
{
    if (this->loaded)
    {
        if (val == 0)
            this->ram = 0;
        else
            this->ram = 1;
        QMessageBox::information(QApplication::activeWindow(), tr("Information"), \
            QString::fromUtf8(tr("La modification sera prise en compte lors du prochain démarrage du logiciel.").toStdString().c_str()));
        this->store();
    }
}
void Config::setAfficheMod(int val)
{
    if (this->loaded)
    {
        this->afficheMod = val;
        this->store();
    }
}
void Config::setAfficheToolBar(int val)
{
    if (this->loaded)
    {
        this->afficheToolBar = val;
        this->store();
    }
}
void Config::setAudioOutput(int index)
{
    if (this->loaded)
    {
        this->audioIndex = index;
        QMessageBox::information(QApplication::activeWindow(), "Information", \
            QString::fromUtf8(tr("La modification sera prise en compte lors du prochain démarrage du logiciel.").toStdString().c_str()));
        this->store();
    }
}
void Config::setWavAutoLoop(bool checked)
{
    if (this->loaded)
    {
        this->wavAutoLoop = checked;
        this->store();
    }
}
void Config::setWavRemoveBlank(bool checked)
{
    if (this->loaded)
    {
        this->wavRemoveBlank = checked;
        this->store();
    }
}

void Config::addFavorite(QString filePath)
{
    // Modification des fichiers récemment ouverts
    int n = 4;
    if (strcmp(filePath.toStdString().c_str(), this->files[0]) == 0)
        n = 0;
    else if (strcmp(filePath.toStdString().c_str(), this->files[1]) == 0)
        n = 1;
    else if (strcmp(filePath.toStdString().c_str(), this->files[2]) == 0)
        n = 2;
    else if (strcmp(filePath.toStdString().c_str(), this->files[3]) == 0)
        n = 3;
    else if (strcmp(filePath.toStdString().c_str(), this->files[4]) == 0)
        n = 4;
    for (int i = n-1; i >= 0; i--)
        strcpy(this->files[i+1], this->files[i]);
    strcpy(this->files[0], filePath.toStdString().c_str());
    this->store();
}

// Gestion du fichier de configuration
void Config::init()
{
    char cTmp;
    FILE *fi = fopen(confFile.toStdString().c_str(), "w");
    fwrite("\0", 5*256, sizeof(char), fi);      // Fichiers récents
    fwrite("\0", 1, sizeof(char), fi);          // Chargement dans la ram
    cTmp = 1;
    fwrite(&cTmp, 1, sizeof(char), fi);         // Affichage toolbar
    fwrite(&cTmp, 1, sizeof(char), fi);         // Affichage mod
    int iTmp = 0;
    fwrite(&iTmp, 1, sizeof(int), fi);          // Index audio output
    cTmp = 0;
    fwrite(&cTmp, 1, sizeof(char), fi);         // Import wav : autoloop
    fwrite(&cTmp, 1, sizeof(char), fi);         // Import wav : autoblank
    fclose(fi);
    for (int i = 0; i < 5; i++)
        this->files[i][0] = '\0';
    this->ram = 0;
    this->afficheMod = 1;
    this->afficheToolBar = 1;
    this->audioIndex = 0;
    this->wavAutoLoop = 0;
    this->wavRemoveBlank = 0;
}
void Config::load()
{
    FILE *fi = fopen(confFile.toStdString().c_str(), "r+");
    FILE *fiTmp;
    if (!fi)
    {
        // Le fichier n'existe pas, création
        this->init();
    }
    else
    {
        // Le fichier existe, lecture
        // fichiers récents
        char str[256];
        int j = 0;
        for (int i = 0; i < 5; i++)
        {
            // placement du curseur pour lecture
            fseek(fi,i*256*sizeof(char), SEEK_SET);
            if (fgets(str, 255, fi))
            {
                // vérification du lien
                fiTmp = fopen(QString(str).toUtf8().data(), "r");
                if (fiTmp)
                {
                    // Lien valide
                    fclose(fiTmp);
                    strcpy(this->files[j], str);
                    if (j > 0 && j != i)
                    {
                        fseek(fi,j*256*sizeof(char), SEEK_SET);
                        fwrite("\0", 256, sizeof(char), fi);
                        fseek(fi,j*256*sizeof(char), SEEK_SET);
                        fwrite(this->files[j], 256, sizeof(char), fi);
                    }
                    j++;
                }
            }
        }
        // Correction du fichier de configuration
        for (int i = j; i < 5; i++)
        {
            fseek(fi,i*256*sizeof(char), SEEK_SET);
            fwrite("\0", 256, sizeof(char), fi);
            for (int k = 0; k < 256; k++)
                this->files[i][k] = '\0';
        }
        // Chargement ram
        fseek(fi,5*256*sizeof(char), SEEK_SET);
        fread(&this->ram, sizeof(char), 1, fi);
        fread(&this->afficheToolBar, sizeof(char), 1, fi);
        fread(&this->afficheMod, sizeof(char), 1, fi);
        fread(&this->audioIndex, sizeof(int), 1, fi);
        fread(&this->wavAutoLoop, sizeof(char), 1, fi);
        fread(&this->wavRemoveBlank, sizeof(char), 1, fi);
        fclose(fi);
    }
}
void Config::store()
{
    FILE *fi = fopen(confFile.toStdString().c_str(), "r+");
    // Fichiers récents
    for (int i = 0; i < 5; i++)
        fwrite(this->files[i], 256, sizeof(char), fi);
    fwrite(&this->ram, 1, sizeof(char), fi);            // Chargement dans la ram
    fwrite(&this->afficheToolBar, 1, sizeof(char), fi); // Affichage barre d'outils
    fwrite(&this->afficheMod, 1, sizeof(char), fi);     // Affichage section mod
    fwrite(&this->audioIndex, 1, sizeof(int), fi);      // Index audio output
    fwrite(&this->wavAutoLoop, 1, sizeof(char), fi);    // Import wav : autoloop
    fwrite(&this->wavRemoveBlank, 1, sizeof(char), fi); // Import wav : autoblank
    fclose(fi);
}

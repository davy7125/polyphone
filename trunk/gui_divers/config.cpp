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
***************************************************************************/

//#include <QtMultimedia/QAudioOutput>
#include <QAudioOutput>
#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include "config.h"
#include "ui_config.h"
#include "mainwindow.h"

Config::Config(QWidget *parent) : QDialog(parent), ui(new Ui::Config)
{
    this->mainWindow = (MainWindow *)parent;
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
    this->confFile = dirPath + QDir::separator() + "conf";
    // CHARGEMENT DE LA CONFIGURATION
    this->load();
    // initialisation des élements
    if (this->ram)
        ui->comboRam->setCurrentIndex(1);
    else
        ui->comboRam->setCurrentIndex(0);
    // liste des sorties audio
    this->ui->comboAudioOuput->addItem("-");
    foreach (const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices(QAudio::AudioOutput))
        this->ui->comboAudioOuput->addItem(deviceInfo.deviceName());
#ifdef WIN32
    bool isWindows = true;
#else
    bool isWindows = false;
#endif
    this->ui->comboAudioOuput->addItem("Jack");
    if (isWindows)
        this->ui->comboAudioOuput->addItem("Asio");
    int nbItem = this->ui->comboAudioOuput->count();
    if (this->audioType == 0)
    {
        if (this->audioIndex < nbItem - 2 - isWindows && this->audioIndex >= 0)
            this->ui->comboAudioOuput->setCurrentIndex(this->audioIndex+1);
        else
        {
            if (nbItem > 2 + isWindows)
                this->ui->comboAudioOuput->setCurrentIndex(1);
            else
                this->ui->comboAudioOuput->setCurrentIndex(0);
        }
    }
    else if (this->audioType == -1)
            this->ui->comboAudioOuput->setCurrentIndex(0);
    else if (this->audioType == -2)
        this->ui->comboAudioOuput->setCurrentIndex(nbItem - 1 - isWindows); // Jack
    else if (this->audioType == -3)
        this->ui->comboAudioOuput->setCurrentIndex(nbItem - 1); // Asio
    this->ui->checkBoucle->setChecked(this->wavAutoLoop);
    this->ui->checkBlanc->setChecked(this->wavRemoveBlank);
    // Paramètres synthétiseur
    this->ui->sliderGain->setValue(this->synthGain);
    this->ui->labelGain->setNum(this->synthGain);
    this->loaded = 1;
    // Correction
    if (this->keyboardType < 0 || this->keyboardType > 3)
        this->keyboardType = 1;
    if (this->keyboardVelocity < 0)
        this->keyboardVelocity = 0;
    else if (this->keyboardVelocity > 127)
        this->keyboardVelocity = 127;

}
Config::~Config()
{
    delete ui;
}

void Config::show()
{
    // Liste des entrées midi à mettre à jour continuellement
    this->ui->comboMidiInput->clear();
    QStringList listMidi = this->mainWindow->getListMidi();
    this->ui->comboMidiInput->addItems(listMidi);
    // Sélection
    if (this->ui->comboMidiInput->count() > this->numPortMidi)
        this->ui->comboMidiInput->setCurrentIndex(this->numPortMidi);
    // Affichage du dialogue
    QDialog::show();
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
        if (!this->ui->comboAudioOuput->itemText(index).compare("-"))
        {
            this->audioIndex = 0;
            this->audioType = -1;
            this->mainWindow->setAudioEngine(-1);
        }
        else if (!this->ui->comboAudioOuput->itemText(index).compare("Jack"))
        {
            this->audioIndex = 0;
            this->audioType = -2;
            this->mainWindow->setAudioEngine(-2);
        }
        else if (!this->ui->comboAudioOuput->itemText(index).compare("Asio"))
        {
            this->audioIndex = 0;
            this->audioType = -3;
            this->mainWindow->setAudioEngine(-3);
        }
        else
        {
            this->audioIndex = index-1;
            this->audioType = 0;
            this->mainWindow->setAudioEngine(index-1);
        }
        this->store();
    }
}
void Config::setSynthGain(int val)
{
    if (this->loaded)
    {
        this->synthGain = val;
        this->store();
        this->mainWindow->setSynthGain(val);
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
void Config::setKeyboardType(int val)
{
    if (this->loaded)
    {
        this->keyboardType = val;
        this->store();
    }
}
void Config::setKeyboardVelocity(int val)
{
    if (this->loaded)
    {
        this->keyboardVelocity = val;
        this->store();
    }
}
void Config::setNumPortMidi(int val)
{
    if (this->loaded)
    {
        this->numPortMidi = val;
        this->store();
        this->mainWindow->openMidiPort(val);
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
    for (int i = 0; i < 5; i++)
        this->files[i][0] = '\0';
    this->ram = 0;
    this->afficheMod = 1;
    this->afficheToolBar = 1;
    this->audioIndex = 0;
    this->wavAutoLoop = 0;
    this->wavRemoveBlank = 0;
    this->keyboardType = 1;
    this->keyboardVelocity = 64;
    this->numPortMidi = 0;
    this->audioType = 0;
    this->synthGain = 0;
    // Stockage
    this->store();
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
        fread(&this->ram,               sizeof(char), 1, fi);
        fread(&this->afficheToolBar,    sizeof(char), 1, fi);
        fread(&this->afficheMod,        sizeof(char), 1, fi);
        fread(&this->audioIndex,        sizeof(int),  1, fi);
        fread(&this->wavAutoLoop,       sizeof(char), 1, fi);
        fread(&this->wavRemoveBlank,    sizeof(char), 1, fi);
        fread(&this->keyboardType,      sizeof(int),  1, fi);
        fread(&this->keyboardVelocity,  sizeof(int),  1, fi);
        fread(&this->numPortMidi,       sizeof(int),  1, fi);
        fread(&this->audioType,         sizeof(char), 1, fi);
        fread(&this->synthGain,         sizeof(int),  1, fi);
        fclose(fi);
    }
}
void Config::store()
{
    FILE *fi = fopen(confFile.toStdString().c_str(), "r+");
    // Fichiers récents
    for (int i = 0; i < 5; i++)
        fwrite(this->files[i], 256, sizeof(char), fi);
    fwrite(&this->ram,              1, sizeof(char), fi);   // Chargement dans la ram
    fwrite(&this->afficheToolBar,   1, sizeof(char), fi);   // Affichage barre d'outils
    fwrite(&this->afficheMod,       1, sizeof(char), fi);   // Affichage section mod
    fwrite(&this->audioIndex,       1, sizeof(int),  fi);   // Index audio output
    fwrite(&this->wavAutoLoop,      1, sizeof(char), fi);   // Import wav : autoloop
    fwrite(&this->wavRemoveBlank,   1, sizeof(char), fi);   // Import wav : autoblank
    fwrite(&this->keyboardType,     1, sizeof(int),  fi);   // type de clavier
    fwrite(&this->keyboardVelocity, 1, sizeof(int),  fi);   // vélocité du clavier
    fwrite(&this->numPortMidi,      1, sizeof(int),  fi);   // port midi
    fwrite(&this->audioType,        1, sizeof(char), fi);   // utilisation jack / asio / rien
    fwrite(&this->synthGain,        1, sizeof(int),  fi);   // synthé : gain
    fclose(fi);
}

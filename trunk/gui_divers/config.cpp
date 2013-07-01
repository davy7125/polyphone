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

#include <QFileInfo>
#include <QDir>
#include "config.h"
#include "ui_config.h"
#include "mainwindow.h"
#include "portaudio.h"

Config * Config::_instance = NULL;


Config * Config::getInstance(QWidget * parent)
{
    if (_instance == NULL)
        _instance = new Config(parent);
    return _instance;
}

void Config::kill()
{
    delete _instance;
}

Config::Config(QWidget *parent) : QDialog(parent),
    settings(this),
    ui(new Ui::Config)
{
    for (int i = 0; i < 5; i++)
        listFiles.append("");

    this->mainWindow = (MainWindow *)parent;
    this->loaded = 0;
    ui->setupUi(this);
    // CHARGEMENT DE LA CONFIGURATION
    this->load();
    // initialisation des élements
    if (this->ram)
        ui->comboRam->setCurrentIndex(1);
    else
        ui->comboRam->setCurrentIndex(0);
    // liste des sorties audio
    this->ui->comboAudioOuput->addItem("-");
    this->ui->comboAudioOuput->addItem(trUtf8("Défaut"));

#ifdef PA_USE_ASIO
    bool isAsioEnabled = true;
#else
    bool isAsioEnabled = false;
#endif
    this->ui->comboAudioOuput->addItem("Jack");
    if (isAsioEnabled)
        this->ui->comboAudioOuput->addItem("Asio");
    int nbItem = this->ui->comboAudioOuput->count();
    if (this->audioType == 0)
    {
        if (this->audioIndex < nbItem - 2 - isAsioEnabled && this->audioIndex >= 0)
            this->ui->comboAudioOuput->setCurrentIndex(this->audioIndex+1);
        else
        {
            if (nbItem > 2 + isAsioEnabled)
                this->ui->comboAudioOuput->setCurrentIndex(1);
            else
                this->ui->comboAudioOuput->setCurrentIndex(0);
        }
    }
    else if (this->audioType == -1)
        this->ui->comboAudioOuput->setCurrentIndex(0);
    else if (this->audioType == -2)
        this->ui->comboAudioOuput->setCurrentIndex(nbItem - 1 - isAsioEnabled); // Jack
    else if (this->audioType == -3)
        this->ui->comboAudioOuput->setCurrentIndex(nbItem - 1); // Asio
    this->ui->checkBoucle->setChecked(this->wavAutoLoop);
    this->ui->checkBlanc->setChecked(this->wavRemoveBlank);
    // Paramètres synthétiseur
    if (this->synthGain < -50)
        this->synthGain = -50;
    else if (this->synthGain > 50)
        this->synthGain = 50;
    this->ui->sliderGain->setValue(this->synthGain);
    this->ui->labelGain->setNum(this->synthGain);
    this->ui->dialRevNiveau->setValue(this->revLevel);
    this->ui->dialRevProfondeur->setValue(this->revLevel);
    this->ui->dialRevDensite->setValue(this->revWidth);
    this->ui->dialRevAttenuation->setValue(this->revDamping);
    this->ui->dialChoNiveau->setValue(this->choLevel);
    this->ui->dialChoAmplitude->setValue(this->choDepth);
    this->ui->dialChoFrequence->setValue(this->choFrequency);
    this->loaded = 1;
    // Correction
    if (this->keyboardType < 0 || this->keyboardType > 3)
        this->keyboardType = 1;
    if (this->keyboardVelocity < 0)
        this->keyboardVelocity = 0;
    else if (this->keyboardVelocity > 127)
        this->keyboardVelocity = 127;
    this->ui->comboRam->setVisible(false); // Temporaire : tout charger dans la ram n'apporte rien pour l'instant (sur linux)
    this->ui->label_2->setVisible(false);
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
    if (filePath.compare(this->listFiles.at(0)) == 0)
        n = 0;
    else if (filePath.compare(this->listFiles.at(1)) == 0)
        n = 1;
    else if (filePath.compare(this->listFiles.at(2)) == 0)
        n = 2;
    else if (filePath.compare(this->listFiles.at(3)) == 0)
        n = 3;
    else if (filePath.compare(this->listFiles.at(4)) == 0)
        n = 4;
    for (int i = n-1; i >= 0; i--)
        listFiles[i+1] = this->listFiles.at(i);
    listFiles[0] = filePath;
    this->store();
}
void Config::setRecordFile(QString filePath)
{
    recordFile = filePath;
    this->store();
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
void Config::on_dialRevNiveau_valueChanged(int value)
{
    if (this->loaded)
    {
        this->revLevel = value;
        this->store();
        this->mainWindow->setSynthReverb(revLevel, revSize, revWidth, revDamping);
    }
}
void Config::on_dialRevProfondeur_valueChanged(int value)
{
    if (this->loaded)
    {
        this->revSize = value;
        this->store();
        this->mainWindow->setSynthReverb(revLevel, revSize, revWidth, revDamping);
    }
}
void Config::on_dialRevDensite_valueChanged(int value)
{
    if (this->loaded)
    {
        this->revWidth = value;
        this->store();
        this->mainWindow->setSynthReverb(revLevel, revSize, revWidth, revDamping);
    }
}
void Config::on_dialRevAttenuation_valueChanged(int value)
{
    if (this->loaded)
    {
        this->revDamping = value;
        this->store();
        this->mainWindow->setSynthReverb(revLevel, revSize, revWidth, revDamping);
    }
}
void Config::on_dialChoNiveau_valueChanged(int value)
{
    if (this->loaded)
    {
        this->choLevel = value;
        this->store();
        this->mainWindow->setSynthChorus(choLevel, choDepth, choFrequency);
    }
}
void Config::on_dialChoAmplitude_valueChanged(int value)
{
    if (this->loaded)
    {
        this->choDepth = value;
        this->store();
        this->mainWindow->setSynthChorus(choLevel, choDepth, choFrequency);
    }
}
void Config::on_dialChoFrequence_valueChanged(int value)
{
    if (this->loaded)
    {
        this->choFrequency = value;
        this->store();
        this->mainWindow->setSynthChorus(choLevel, choDepth, choFrequency);
    }
}

// Gestion des configurations
void Config::load()
{
    // Chargement des fichiers récents
    this->recordFile        = settings.value("recent_file/record", "").toString();
    int j = 0;
    QString strTmp;
    for (int i = 0; i < 5; i++)
    {
        strTmp = settings.value("recent_file/file_" + QString::number(i), "\0").toString();
        // test
        QFile file(strTmp);
        if (file.exists())
        {
            settings.setValue("recent_file/file_" + QString::number(i), strTmp);
            listFiles[j++] = strTmp;
        }
    }
    // Correction du fichier de configuration
    for (int i = j; i < 5; i++)
    {
        listFiles[i] = "";
        settings.setValue("recent_file/file_" + QString::number(i), "");
    }
    // Chargement des autres paramètres
    this->afficheToolBar    = settings.value("affichage/tool_bar", true).toBool();
    this->afficheMod        = settings.value("affichage/section_modulateur", true).toBool();
    this->audioIndex        = settings.value("audio/index", 0).toInt();
    this->audioType         = settings.value("audio/type", 0).toInt();
    this->wavAutoLoop       = settings.value("wav_auto_loop", false).toBool();
    this->wavRemoveBlank    = settings.value("wav_remove_bank", false).toBool();
    this->keyboardType      = settings.value("keyboard/type", 1).toInt();
    this->keyboardVelocity  = settings.value("keyboard/velocity", 64).toInt();
    this->numPortMidi       = settings.value("midi/index_port", 0).toInt();
    this->synthGain         = settings.value("synth/gain", 0).toInt();
    this->revLevel          = settings.value("synth/rev_level", 0).toInt();
    this->revSize           = settings.value("synth/rev_size", 0).toInt();
    this->revWidth          = settings.value("synth/rev_width", 0).toInt();
    this->revDamping        = settings.value("synth/rev_damping", 0).toInt();
    this->choLevel          = settings.value("synth/cho_level", 0).toInt();
    this->choDepth          = settings.value("synth/cho_depth", 0).toInt();
    this->choFrequency      = settings.value("synth/cho_frequency", 0).toInt();
}
void Config::store()
{
    settings.setValue("recent_file/record",             this->recordFile);
    settings.setValue("recent_file/file_0",             listFiles.at(0));
    settings.setValue("recent_file/file_1",             listFiles.at(1));
    settings.setValue("recent_file/file_2",             listFiles.at(2));
    settings.setValue("recent_file/file_3",             listFiles.at(3));
    settings.setValue("recent_file/file_4",             listFiles.at(4));
    settings.setValue("affichage/tool_bar",             this->afficheToolBar);
    settings.setValue("affichage/section_modulateur",   this->afficheMod);
    settings.setValue("audio/index",                    this->audioIndex);
    settings.setValue("audio/type",                     this->audioType);
    settings.setValue("wav_auto_loop",                  this->wavAutoLoop);
    settings.setValue("wav_remove_bank",                this->wavRemoveBlank);
    settings.setValue("keyboard/type",                  this->keyboardType);
    settings.setValue("keyboard/velocity",              this->keyboardVelocity);
    settings.setValue("midi/index_port",                this->numPortMidi);
    settings.setValue("synth/gain",                     this->synthGain);
    settings.setValue("synth/rev_level",                this->revLevel);
    settings.setValue("synth/rev_size",                 this->revSize);
    settings.setValue("synth/rev_width",                this->revWidth);
    settings.setValue("synth/rev_damping",              this->revDamping);
    settings.setValue("synth/cho_level",                this->choLevel);
    settings.setValue("synth/cho_depth",                this->choDepth);
    settings.setValue("synth/cho_frequency",            this->choFrequency);
}

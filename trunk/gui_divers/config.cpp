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

#include <QFileInfo>
#include <QDir>
#include <QColorDialog>
#include <QDesktopServices>
#include "config.h"
#include "ui_config.h"
#include "mainwindow.h"
#include "portaudio.h"
#include "translationsystem.h"
#include <qmath.h>

Config * Config::_instance = NULL;

Config * Config::getInstance(PianoKeybdCustom * keyboard, AudioDevice * audioDevice, QWidget * parent)
{
    if (_instance == NULL)
        _instance = new Config(parent, keyboard, audioDevice);
    return _instance;
}

void Config::kill()
{
    delete _instance;
}

Config::Config(QWidget *parent, PianoKeybdCustom *keyboard, AudioDevice * audioDevice) : QDialog(parent),
    settings(this),
    ui(new Ui::Config),
    _keyboard(keyboard)
{
    ui->setupUi(this);
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint) | Qt::Tool);

#ifdef Q_OS_MAC
    ui->groupBox->layout()->setMargin(10);
#endif
    for (int i = 0; i < 5; i++)
        listFiles.append("");

    this->mainWindow = (MainWindow *)parent;

    // CHARGEMENT DE LA CONFIGURATION
    this->loaded = false;
    this->load();

//    if (this->ram)
//        ui->comboRam->setCurrentIndex(1);
//    else
//        ui->comboRam->setCurrentIndex(0);

    // Liste des sorties audio
    QList<HostInfo> hostInfos = (audioDevice != NULL ? audioDevice->getHostInfo() : QList<HostInfo>());
    bool configFound = false;
    int comboboxIndex = 0;
    int comboboxDefaultIndex = 0;
    for (int i = 0; i < hostInfos.size(); i++)
    {
        if (hostInfos[i]._index < 0)
        {
            ui->comboAudioOuput->addItem(hostInfos[i]._name, QString::number(hostInfos[i]._index) + "#-1");
            if (hostInfos[i]._index == _audioType)
            {
                configFound = true;
                comboboxIndex = ui->comboAudioOuput->count() - 1;
            }
        }
        else
        {
            for (int j = 0; j < hostInfos[i]._devices.size(); j++)
            {
                QString suffix = "";
                ui->comboAudioOuput->addItem(hostInfos[i]._name + ": " +
                                             hostInfos[i]._devices[j]._name + suffix,
                                             QString::number(hostInfos[i]._index) + "#" +
                                             QString::number(hostInfos[i]._devices[j]._index));
                if (hostInfos[i]._devices[j]._isDefault)
                {
//                    ui->comboAudioOuput->setItemData(ui->comboAudioOuput->count() - 1,
//                                                     QColor(0, 0, 200),
//                                                     Qt::ForegroundRole);
                    if (hostInfos[i]._isDefault)
                        comboboxDefaultIndex = ui->comboAudioOuput->count() - 1;
                }
                if (!configFound && hostInfos[i]._index == _audioType)
                {
                    if (hostInfos[i]._devices[j]._index == _audioIndex)
                    {
                        configFound = true;
                        comboboxIndex = ui->comboAudioOuput->count() - 1;
                    }
                    else if (hostInfos[i]._devices[j]._isDefault)
                        comboboxIndex = ui->comboAudioOuput->count() - 1;
                }
            }
        }
    }
    if (!configFound)
        comboboxIndex = comboboxDefaultIndex;
    this->ui->comboAudioOuput->setCurrentIndex(comboboxIndex);

    int pos = qRound(qLn(_bufferSize) / 0.69314718056 - 4);
    if (pos < 0)
        pos = 0;
    if (pos > ui->comboBufferSize->count() - 1)
        pos = ui->comboBufferSize->count() - 1;
    this->ui->comboBufferSize->setCurrentIndex(pos);
    this->ui->checkBoucle->setChecked(this->wavAutoLoop);
    this->ui->checkBlanc->setChecked(this->wavRemoveBlank);
    this->ui->checkRowSameWidth->setChecked(this->sameWidthTable);

    // Paramètres synthétiseur
    if (this->synthGain < -50)
        this->synthGain = -50;
    else if (this->synthGain > 50)
        this->synthGain = 50;
    ui->sliderGain->setValue(this->synthGain);
    ui->labelGain->setNum(this->synthGain);
    ui->dialRevNiveau->setValue(this->revLevel);
    ui->dialRevProfondeur->setValue(this->revSize);
    ui->dialRevDensite->setValue(this->revWidth);
    ui->dialRevAttenuation->setValue(this->revDamping);
    ui->dialChoNiveau->setValue(this->choLevel);
    ui->dialChoAmplitude->setValue(this->choDepth);
    ui->dialChoFrequence->setValue(this->choFrequency);
    ui->checkRepercussionStereo->setChecked(this->modifStereo);
    ui->comboKeyName->setCurrentIndex((int)this->nameMiddleC);
    ui->spinDefaultVelocity->setValue(_velocity);
    if (this->keyboardType < 0 || this->keyboardType > 4)
        this->keyboardType = 1;
    //ui->comboRam->setVisible(false); // Temporaire : tout charger dans la ram n'apporte rien pour l'instant (sur linux)
    //ui->label_2->setVisible(false);
    QTimer::singleShot(1, this, SLOT(setColors())); // trick that fixes a bug which appeared with Qt5

    // Initialisation mappage
    if (_keyboard != NULL)
    {
        octaveMapping = this->getOctaveMap();
        if (octaveMapping >= ui->comboDo->count())
            octaveMapping = 3;
        ui->comboDo->setCurrentIndex(octaveMapping);
        _keyboard->set(PianoKeybd::PROPERTY_MAPPING_FIRST_NOTE, 12 * octaveMapping);
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 13; j++)
                _keyboard->setMapping((PianoKeybd::Key)j, i, QKeySequence::fromString(getKeyMapped(i, (PianoKeybd::Key)j)));
        }
        this->ui->tableKeyboardMap->setKeyboard(_keyboard);
        connect(ui->tableKeyboardMap, SIGNAL(combinaisonChanged(int,int,QString)), this, SLOT(combinaisonChanged(int,int,QString)));
        renameComboDo();

        // Vélocité par défaut du clavier
        _keyboard->set(PianoKeybd::PROPERTY_VELOCITY, _velocity);
    }

    initComboLanguage();
    this->loaded = true;
}
Config::~Config()
{
    delete ui;
}

void Config::show()
{
    // Liste des entrées midi à mettre à jour continuellement
    this->ui->comboMidiInput->blockSignals(true);
    this->ui->comboMidiInput->clear();
    QStringList listMidi = this->mainWindow->getListMidi();
    this->ui->comboMidiInput->addItem("-");
    this->ui->comboMidiInput->addItems(listMidi);

    // Sélection
    if (this->ui->comboMidiInput->count() > this->numPortMidi + 1)
        this->ui->comboMidiInput->setCurrentIndex(this->numPortMidi + 1);
    else
        this->ui->comboMidiInput->setCurrentIndex(0);
    this->ui->comboMidiInput->blockSignals(false);

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
        QMessageBox::information(QApplication::activeWindow(), trUtf8("Information"),
                                 trUtf8("La modification sera prise en compte lors du prochain démarrage du logiciel."));
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
    Q_UNUSED(index)
    if (this->loaded)
    {
        this->mainWindow->setAudioDevice(getAudioType(), getAudioIndex(), _bufferSize);
        if (getAudioIndex() == 0)
            storeAudioConfig();
    }
}
void Config::on_comboBufferSize_activated(int index)
{
    // Modification de la taille du buffer
    if (this->loaded)
    {
        _bufferSize = pow(2.0f, index + 4);
        setAudioOutput(ui->comboAudioOuput->currentIndex());
        if (getAudioIndex() == 0)
            storeAudioConfig();
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
void Config::setKeyboardDocked(bool val)
{
    if (this->loaded)
    {
        this->keyboardDocked = val;
        this->store();
    }
}
void Config::setNumPortMidi(int val)
{
    if (this->loaded)
    {
        this->numPortMidi = val-1;
        this->store();
        this->mainWindow->openMidiPort(val-1);
    }
}
void Config::addFile(TypeFichier typeFichier, QString filePath)
{
    filePath = filePath.replace("\\", "/");

    // Modification des fichiers récemment ouverts
    switch (typeFichier)
    {
    case typeFichierEnregistrement:
        recordFile = filePath;
        break;
    case typeFichierSample:
        sampleFile = filePath;
        break;
    case typeFichierExport:
        exportFile = filePath;
        break;
    case typeFichierSoundfont:
        importFile = filePath;
        break;
    case typeFichierFrequences:
        pngFile = filePath;
        break;
    case typeFichierSf2:{
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
        }break;
    }
    this->store();
}

QString Config::getLastFile(TypeFichier typeFichier, int num)
{
    QString lastFile;
    switch (typeFichier)
    {
    case typeFichierEnregistrement:
        lastFile = recordFile;
        break;
    case typeFichierSample:
        lastFile = sampleFile;
        break;
    case typeFichierSf2:
        if (num >= 0 && num < 5)
            lastFile = listFiles.at(num);
        break;
    case typeFichierExport:
        lastFile = exportFile;
        break;
    case typeFichierSoundfont:
        lastFile = importFile;
        break;
    case typeFichierFrequences:
        lastFile = pngFile;
        break;
    }

    return lastFile;
}

QString Config::getLastDirectory(TypeFichier typeFichier)
{
    QString lastDir = getLastFile(typeFichier);
    if (!lastDir.isEmpty())
        lastDir = QFileInfo(lastDir).dir().path();

    if (lastDir.isEmpty() || !QDir(lastDir).exists())
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        lastDir =  QDir::home().path();
#else
        lastDir = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
#endif

    return lastDir;
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

void Config::setVolume(int val)
{
    setSynthGain(val);
    ui->sliderGain->blockSignals(true);
    ui->sliderGain->setValue(val);
    ui->labelGain->setText(QString::number(val));
    ui->sliderGain->blockSignals(false);
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

void Config::on_checkRepercussionStereo_clicked()
{
    if (this->loaded)
    {
        this->modifStereo = ui->checkRepercussionStereo->isChecked();
        this->store();
    }
}


// Gestion des configurations
void Config::load()
{
    // Chargement des fichiers récents
    this->recordFile        = settings.value("recent_file/record", "").toString();
    this->sampleFile        = settings.value("recent_file/sample", "").toString();
    this->exportFile        = settings.value("recent_file/export", "").toString();
    this->importFile        = settings.value("recent_file/import", "").toString();
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
    this->_audioIndex        = settings.value("audio/index", 0).toInt();
    this->_bufferSize        = settings.value("audio/buffer_size", 512).toInt();
    this->_audioType         = settings.value("audio/type", 0).toInt();
    this->wavAutoLoop       = settings.value("wav_auto_loop", false).toBool();
    if (settings.contains("wav_remove_bank"))
    {
        settings.setValue("wav_remove_blank", settings.value("wav_remove_bank").toBool());
        settings.remove("wav_remove_bank");
    }
    this->wavRemoveBlank    = settings.value("wav_remove_blank", false).toBool();
    this->keyboardType      = settings.value("keyboard/type", 1).toInt();
    this->keyboardDocked    = settings.value("keyboard/keyboard_in_toolbar", true).toBool();
    this->numPortMidi       = settings.value("midi/index_port", -1).toInt();
    this->synthGain         = settings.value("synth/gain", 0).toInt();
    this->revLevel          = settings.value("synth/rev_level", 0).toInt();
    this->revSize           = settings.value("synth/rev_size", 0).toInt();
    this->revWidth          = settings.value("synth/rev_width", 0).toInt();
    this->revDamping        = settings.value("synth/rev_damping", 0).toInt();
    this->choLevel          = settings.value("synth/cho_level", 0).toInt();
    this->choDepth          = settings.value("synth/cho_depth", 0).toInt();
    this->choFrequency      = settings.value("synth/cho_frequency", 0).toInt();
    this->colorList.clear();
    this->colorList << settings.value("colors/graph_background", QColor(0, 0, 0)).toString()
                    << settings.value("colors/graph_waveform", QColor(0, 0, 255)).toString()
                    << settings.value("colors/graph_grid", QColor(60, 60, 100)).toString()
                    << settings.value("colors/graph_startloop", QColor(100, 255, 100)).toString()
                    << settings.value("colors/graph_endloop", QColor(255, 0, 0)).toString()
                    << settings.value("colors/graph_timecursor", QColor(255, 255, 255)).toString();
    this->modifStereo       = settings.value("stereo_modification", false).toBool();
    this->nameMiddleC       = (NameMiddleC)settings.value("name_middle_c", 0).toInt();
    this->_velocity         = settings.value("keyboard/velocity", 127).toInt();
    this->sameWidthTable    = settings.value("affichage/same_width_table", false).toBool();
}

void Config::store()
{
    settings.setValue("recent_file/record",             this->recordFile);
    settings.setValue("recent_file/sample",             this->sampleFile);
    settings.setValue("recent_file/export",             this->exportFile);
    settings.setValue("recent_file/import",             this->importFile);
    settings.setValue("recent_file/file_0",             listFiles.at(0));
    settings.setValue("recent_file/file_1",             listFiles.at(1));
    settings.setValue("recent_file/file_2",             listFiles.at(2));
    settings.setValue("recent_file/file_3",             listFiles.at(3));
    settings.setValue("recent_file/file_4",             listFiles.at(4));
    settings.setValue("affichage/tool_bar",             this->afficheToolBar);
    settings.setValue("affichage/section_modulateur",   this->afficheMod);
    settings.setValue("wav_auto_loop",                  this->wavAutoLoop);
    settings.setValue("wav_remove_blank",               this->wavRemoveBlank);
    settings.setValue("keyboard/type",                  this->keyboardType);
    settings.setValue("keyboard/keyboard_in_toolbar",   this->keyboardDocked);
    settings.setValue("midi/index_port",                this->numPortMidi);
    settings.setValue("synth/gain",                     this->synthGain);
    settings.setValue("synth/rev_level",                this->revLevel);
    settings.setValue("synth/rev_size",                 this->revSize);
    settings.setValue("synth/rev_width",                this->revWidth);
    settings.setValue("synth/rev_damping",              this->revDamping);
    settings.setValue("synth/cho_level",                this->choLevel);
    settings.setValue("synth/cho_depth",                this->choDepth);
    settings.setValue("synth/cho_frequency",            this->choFrequency);
    settings.setValue("colors/graph_background",        this->colorList.at(0).name());
    settings.setValue("colors/graph_waveform",          this->colorList.at(1).name());
    settings.setValue("colors/graph_grid",              this->colorList.at(2).name());
    settings.setValue("colors/graph_startloop",         this->colorList.at(3).name());
    settings.setValue("colors/graph_endloop",           this->colorList.at(4).name());
    settings.setValue("colors/graph_timecursor",        this->colorList.at(5).name());
    settings.setValue("stereo_modification",            this->modifStereo);
    settings.setValue("name_middle_c",                  (int)this->nameMiddleC);
    settings.setValue("keyboard/velocity",              this->_velocity);
    settings.setValue("affichage/same_width_table",     this->sameWidthTable);
}

void Config::setColors()
{
    QString styleStart = "QPushButton{border: 1px solid #888; background-color: ";
    this->ui->pushColorBackground->setStyleSheet(styleStart + this->colorList.at(0).name() + ";}");
    this->ui->pushColorForeground->setStyleSheet(styleStart + this->colorList.at(1).name() + ";}");
    this->ui->pushColorGrid->setStyleSheet(styleStart + this->colorList.at(2).name() + ";}");
    this->ui->pushColorStartloop->setStyleSheet(styleStart + this->colorList.at(3).name() + ";}");
    this->ui->pushColorEndloop->setStyleSheet(styleStart + this->colorList.at(4).name() + ";}");
    this->ui->pushColorPlay->setStyleSheet(styleStart + this->colorList.at(5).name() + ";}");
    this->colorsChanged();
}

void Config::on_pushColorBackground_clicked()
{
    QColor color = QColorDialog::getColor(this->colorList.at(0), this, trUtf8("Couleur du fond"));
    if (color.isValid())
    {
        this->colorList[0] = color;
        this->store();
        this->setColors();
    }
}

void Config::on_pushColorForeground_clicked()
{
    QColor color = QColorDialog::getColor(this->colorList.at(1), this, trUtf8("Couleur de l'onde"));
    if (color.isValid())
    {
        this->colorList[1] = color;
        this->store();
        this->setColors();
    }
}

void Config::on_pushColorGrid_clicked()
{
    QColor color = QColorDialog::getColor(this->colorList.at(2), this, trUtf8("Couleur de la grille"));
    if (color.isValid())
    {
        this->colorList[2] = color;
        this->store();
        this->setColors();
    }
}

void Config::on_pushColorStartloop_clicked()
{
    QColor color = QColorDialog::getColor(this->colorList.at(3), this, trUtf8("Couleur du début de la boucle"));
    if (color.isValid())
    {
        this->colorList[3] = color;
        this->store();
        this->setColors();
    }
}

void Config::on_pushColorEndloop_clicked()
{
    QColor color = QColorDialog::getColor(this->colorList.at(4), this, trUtf8("Couleur de la fin de la boucle"));
    if (color.isValid())
    {
        this->colorList[4] = color;
        this->store();
        this->setColors();
    }
}

void Config::on_pushColorPlay_clicked()
{
    QColor color = QColorDialog::getColor(this->colorList.at(5), this, trUtf8("Couleur du curseur de lecture"));
    if (color.isValid())
    {
        this->colorList[5] = color;
        this->store();
        this->setColors();
    }
}

void Config::on_pushColorRestore_clicked()
{
    this->colorList.clear();
    this->colorList << QColor(0, 0, 0) << QColor(0, 0, 255) << QColor(60, 60, 100)
                    << QColor(100, 255, 100) << QColor(255, 0, 0) << QColor(255, 255, 255);
    this->store();
    this->setColors();
}

void Config::setListeActions(QList<QAction *> actions)
{
    actionList = actions;
    actionListToolbar = settings.value("affichage/actions", this->getDefaultListActions()).toByteArray();
    this->fillActions();
}

void Config::fillActions()
{
    // Liste des actions disponibles
    ui->listActions->clear();
    for (int i = 0; i < actionList.size(); i++)
    {
        if (!actionListToolbar.contains((char)i))
        {
            ui->listActions->addItem(actionList.at(i)->text().replace("&", ""));
            QListWidgetItem * item = ui->listActions->item(ui->listActions->count()-1);
            item->setIcon(actionList.at(i)->icon());
            item->setData(Qt::UserRole, i);
        }
    }
    ui->listActions->sortItems();
    ui->listActions->insertItem(0, trUtf8("---- séparateur ----"));
    ui->listActions->item(0)->setData(Qt::UserRole, -1);
    QFont font = ui->listActions->item(0)->font();
    font.setItalic(true);
    ui->listActions->item(0)->setFont(font);

    // Liste des éléments dans la toolbar
    ui->listToolbar->clear();
    char num;
    for (int i = 0; i < actionListToolbar.size(); i++)
    {
        num = actionListToolbar.at(i);
        if (num < actionList.size())
        {
            if (num == -1)
            {
                ui->listToolbar->addItem(trUtf8("---- séparateur ----"));
                QListWidgetItem * item = ui->listToolbar->item(ui->listToolbar->count()-1);
                item->setData(Qt::UserRole, -1);
                item->setFont(font);
            }
            else
            {
                ui->listToolbar->addItem(actionList.at(num)->text().replace("&", ""));
                QListWidgetItem * item = ui->listToolbar->item(ui->listToolbar->count()-1);
                item->setIcon(actionList.at(num)->icon());
                item->setData(Qt::UserRole, num);
            }
        }
    }

    // Sauvegarde et mise à jour MainWindow
    if (loaded)
    {
        settings.setValue("affichage/actions", actionListToolbar);
        QList<QAction *> liste;
        for (int i = 0; i < actionListToolbar.size(); i++)
        {
            char index = actionListToolbar.at(i);
            if (index < actionList.size())
            {
                if (index == -1)
                    liste << NULL;
                else
                    liste << actionList.at(index);
            }
        }
        mainWindow->setListeActions(liste);
    }
}

void Config::on_pushUp_clicked()
{
    if (this->ui->listToolbar->currentRow() > 0)
    {
        int index = this->ui->listToolbar->currentRow();
        if (index < this->actionListToolbar.size())
        {
            char numTmp = this->actionListToolbar.at(index);
            this->actionListToolbar[index] = this->actionListToolbar.at(index - 1);
            this->actionListToolbar[index - 1] = numTmp;
            this->fillActions();
            ui->listToolbar->setCurrentRow(index - 1);
        }
    }
}

void Config::on_pushDown_clicked()
{
    if (this->ui->listToolbar->currentRow() != -1 &&
            this->ui->listToolbar->currentRow() != this->ui->listToolbar->count() - 1)
    {
        int index = this->ui->listToolbar->currentRow();
        if (index < this->actionListToolbar.size() - 1)
        {
            char numTmp = this->actionListToolbar.at(index);
            this->actionListToolbar[index] = this->actionListToolbar.at(index + 1);
            this->actionListToolbar[index + 1] = numTmp;
            this->fillActions();
            ui->listToolbar->setCurrentRow(index + 1);
        }
    }
}

void Config::on_pushRight_clicked()
{
    int index = this->ui->listActions->currentRow();
    if (index != -1)
    {
        actionListToolbar.append((char)this->ui->listActions->currentItem()->data(Qt::UserRole).toInt());
        this->fillActions();
        if (this->ui->listActions->count())
        {
            if (index >= this->ui->listActions->count())
                index--;
            this->ui->listActions->setCurrentRow(index);
        }
    }
}

void Config::on_pushLeft_clicked()
{
    int index = this->ui->listToolbar->currentRow();
    if (index != -1 && index < actionListToolbar.size())
    {
        actionListToolbar.remove(index, 1);
        this->fillActions();
        if (this->ui->listToolbar->count())
        {
            if (index >= this->ui->listToolbar->count())
                index--;
            this->ui->listToolbar->setCurrentRow(index);
        }
    }
}

void Config::on_pushResetToolbar_clicked()
{
    this->actionListToolbar = this->getDefaultListActions();
    this->fillActions();
}

void Config::on_listToolbar_itemSelectionChanged()
{
    QList<QListWidgetItem *> listItems = this->ui->listToolbar->selectedItems();
    this->ui->pushLeft->setEnabled(listItems.count());
    if (listItems.count() == 1)
    {
        this->ui->pushLeft->setEnabled(true);
        if (ui->listToolbar->count() > 1)
        {
            this->ui->pushDown->setEnabled(listItems.at(0) != ui->listToolbar->item(ui->listToolbar->count()-1));
            this->ui->pushUp->setEnabled(listItems.at(0) != ui->listToolbar->item(0));
        }
        else
        {
            this->ui->pushDown->setEnabled(false);
            this->ui->pushUp->setEnabled(false);
        }
    }
    else
    {
        this->ui->pushLeft->setEnabled(false);
        this->ui->pushUp->setEnabled(false);
        this->ui->pushDown->setEnabled(false);
    }
}

void Config::on_listActions_itemSelectionChanged()
{
    this->ui->pushRight->setEnabled(this->ui->listActions->selectedItems().count());
}

QByteArray Config::getDefaultListActions()
{
    QByteArray baData;
    baData.append((char)0);
    baData.append((char)1);
    baData.append((char)2);
    baData.append((char)-1);
    baData.append((char)9);
    baData.append((char)10);
    baData.append((char)-1);
    baData.append((char)4);
    baData.append((char)7);
    baData.append((char)8);
    baData.append((char)-1);
    baData.append((char)6);
    return baData;
}


QList<QList<int> > Config::getTools_i_mixture_ranks()
{
    QList<QList<int> > listRet;
    if (settings.contains("tools/instrument/mixture_ranks"))
    {
        QList<QVariant> listTmp;
        QByteArray sousListTmp;
        listTmp = settings.value("tools/instrument/mixture_ranks").toList();
        for (int i = 0; i < listTmp.size(); i++)
        {
            listRet << QList<int>();
            sousListTmp = listTmp.at(i).toByteArray();
            for (int j = 0; j < sousListTmp.size(); j++)
                listRet[i] << sousListTmp.at(j);
        }
    }
    return listRet;
}

void Config::setTools_i_mixture_ranks(QList<QList<int> > val)
{
    QList<QVariant> listTmp;
    QByteArray sousListTmp;
    for (int i = 0; i < val.size(); i++)
    {
        sousListTmp.clear();
        for (int j = 0; j < val.at(i).size(); j++)
            sousListTmp.append(val.at(i).at(j));
        listTmp << sousListTmp;
    }
    settings.setValue("tools/instrument/mixture_ranks", listTmp);
}

void Config::combinaisonChanged(int key, int numOctave, QString combinaison)
{
    settings.setValue("map/key_" + QString::number(numOctave) + "_" + QString::number(key), combinaison);
}

void Config::on_comboKeyName_currentIndexChanged(int index)
{
    if (this->loaded)
    {
        this->nameMiddleC = (NameMiddleC)index;
        this->store();
        this->mainWindow->noteNameChanged();
        renameComboDo();
    }
}

QString Config::getKeyName(int keyNum, bool forceTexte, bool with0, bool forceC4)
{
    if (this->nameMiddleC == middleC_60 && !forceTexte)
    {
        if (with0)
        {
            if (keyNum < 10)
                return "00" + QString::number(keyNum);
            else if (keyNum < 100)
                return "0" + QString::number(keyNum);
            else
                return QString::number(keyNum);
        }
        else
            return QString::number(keyNum);
    }

    QString keyName;
    if (nameMiddleC == middleC_C3_with_flats || nameMiddleC == middleC_C4_with_flats || nameMiddleC == middleC_C5_with_flats)
    {
        switch (keyNum % 12)
        {
        case 0:  keyName = "C";  break;
        case 1:  keyName = "Db"; break;
        case 2:  keyName = "D";  break;
        case 3:  keyName = "Eb"; break;
        case 4:  keyName = "E";  break;
        case 5:  keyName = "F";  break;
        case 6:  keyName = "Gb"; break;
        case 7:  keyName = "G";  break;
        case 8:  keyName = "Ab"; break;
        case 9:  keyName = "A";  break;
        case 10: keyName = "Bb"; break;
        case 11: keyName = "B";  break;
        }
    }
    else
    {
        switch (keyNum % 12)
        {
        case 0:  keyName = "C";  break;
        case 1:  keyName = "C#"; break;
        case 2:  keyName = "D";  break;
        case 3:  keyName = "D#"; break;
        case 4:  keyName = "E";  break;
        case 5:  keyName = "F";  break;
        case 6:  keyName = "F#"; break;
        case 7:  keyName = "G";  break;
        case 8:  keyName = "G#"; break;
        case 9:  keyName = "A";  break;
        case 10: keyName = "A#"; break;
        case 11: keyName = "B";  break;
        }
    }

    int numOctave = (keyNum) / 12 - 1;
    if ((nameMiddleC == middleC_C3_with_flats || nameMiddleC == middleC_C3_with_sharps) && !forceC4)
        numOctave -= 1;
    else if ((nameMiddleC == middleC_C5_with_flats || nameMiddleC == middleC_C5_with_sharps) && !forceC4)
        numOctave += 1;
    keyName += QString::number(numOctave);

    return keyName;
}

int Config::getKeyNum(QString keyName, bool forceC4)
{
    keyName = keyName.replace(",", ".").toLower();
    int note = qRound(keyName.toDouble());
    if (note == 0 && keyName != "0" && keyName.size() >= 2)
    {
        switch (keyName.at(0).unicode())
        {
        case 'c': note = 60; break;
        case 'd': note = 62; break;
        case 'e': note = 64; break;
        case 'f': note = 65; break;
        case 'g': note = 67; break;
        case 'a': note = 69; break;
        case 'b': note = 71; break;
        default : return -1; break;
        }
        keyName = keyName.right(keyName.size() - 1);
        if (keyName.at(0).unicode() == '#' || keyName.at(0) == QString::fromUtf8("♯").at(0))
        {
            note++;
            keyName = keyName.right(keyName.size() - 1);
        }
        else if (keyName.at(0).unicode() == 'b' || keyName.at(0) == QString::fromUtf8("♭").at(0))
        {
            note --;
            keyName = keyName.right(keyName.size() - 1);
        }

        if (!keyName.isEmpty())
        {
            int octave = keyName.toInt();
            if ((octave == 0 && keyName != "0"))
                return -1;
            else
                note += (octave - 4) * 12;
        }

        if ((nameMiddleC == middleC_C3_with_flats || nameMiddleC == middleC_C3_with_sharps) && !forceC4)
            note += 12;
        else if ((nameMiddleC == middleC_C5_with_flats || nameMiddleC == middleC_C5_with_sharps) && !forceC4)
            note -= 12;
    }
    return note;
}

void Config::on_comboDo_currentIndexChanged(int index)
{
    this->setOctaveMap(index);
    _keyboard->set(PianoKeybd::PROPERTY_MAPPING_FIRST_NOTE, index * 12);
}

void Config::setOctaveMap(int octave)
{
    settings.setValue("map/octave_offset", octave);
    ui->comboDo->blockSignals(true);
    ui->comboDo->setCurrentIndex(octave);
    ui->comboDo->blockSignals(false);
}

void Config::renameComboDo()
{
    int nbElement = ui->comboDo->count();
    for (int i = 0; i < nbElement; i++)
        ui->comboDo->setItemText(i, getKeyName(12 * i));
}

void Config::on_spinDefaultVelocity_editingFinished()
{
    if (this->loaded)
    {
        this->_velocity = ui->spinDefaultVelocity->value();
        this->store();
        _keyboard->set(PianoKeybd::PROPERTY_VELOCITY, _velocity);
    }
}

bool caseInsensitiveLessThan(const QString &s1, const QString &s2)
{
    return s1.toLower() < s2.toLower();
}

void Config::initComboLanguage()
{
    // Load the different languages
    ui->comboLangue->blockSignals(true);
    QMap<QString, QString> languages = TranslationSystem::getLanguages();
    QStringList languageNames = languages.values();
    qSort(languageNames.begin(), languageNames.end(), caseInsensitiveLessThan);

    foreach (QString languageName, languageNames)
        ui->comboLangue->addItem(languageName, languages.key(languageName));

    // Selection of the current language
    QString locale = QLocale::system().name().section('_', 0, 0);
    locale = settings.value("language", locale).toString();
    bool found = false;
    for (int i = 0; i < ui->comboLangue->count(); i++)
    {
        if (ui->comboLangue->itemData(i).toString() == locale)
        {
            found = true;
            ui->comboLangue->setCurrentIndex(i);
            break;
        }
    }

    // If not found, english is the default
    if (!found)
    {
        for (int i = 0; i < ui->comboLangue->count(); i++)
        {
            if (ui->comboLangue->itemData(i).toString() == "en")
            {
                ui->comboLangue->setCurrentIndex(i);
                break;
            }
        }
    }

    ui->comboLangue->blockSignals(false);
}

void Config::on_comboLangue_currentIndexChanged(int index)
{
    settings.setValue("language", ui->comboLangue->itemData(index).toString());
    QMessageBox::information(QApplication::activeWindow(), trUtf8("Information"),
                             trUtf8("La modification sera prise en compte lors du prochain démarrage du logiciel."));
}

void Config::on_checkRowSameWidth_clicked()
{
    if (this->loaded)
    {
        this->sameWidthTable = ui->checkRowSameWidth->isChecked();
        this->store();
    }
}

int Config::getAudioType()
{
    QString str = ui->comboAudioOuput->itemData(ui->comboAudioOuput->currentIndex()).toString();
    QStringList listStr = str.split("#");
    if (listStr.size() >= 1)
        return listStr[0].toInt();
    else
        return 0;
}

int Config::getAudioIndex()
{
    QString str = ui->comboAudioOuput->itemData(ui->comboAudioOuput->currentIndex()).toString();
    QStringList listStr = str.split("#");
    if (listStr.size() >= 2)
        return listStr[1].toInt();
    else
        return 0;
}

void Config::storeAudioConfig()
{
    _audioType = getAudioType();
    _audioIndex = getAudioIndex();

    settings.setValue("audio/index",                    this->_audioIndex);
    settings.setValue("audio/buffer_size",              this->_bufferSize);
    settings.setValue("audio/type",                     this->_audioType);
}

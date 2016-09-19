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
**  Website/Contact: http://polyphone-soundfonts.com                      **
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
#include "translationmanager.h"
#include <qmath.h>
#include "thememanager.h"
#include "keynamemanager.h"

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
    ui(new Ui::Config),
    _keyboard(keyboard)
{
    ui->setupUi(this);
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint));

#ifdef Q_OS_MAC
    ui->groupBox->layout()->setMargin(10);
#endif

    _mainWindow = (MainWindow *)parent;

    // LOAD CONFIGURATION
    _loaded = false;
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
    ui->comboAudioOuput->setCurrentIndex(comboboxIndex);

    int pos = qRound(qLn(_bufferSize) / 0.69314718056 - 4);
    if (pos < 0)
        pos = 0;
    if (pos > ui->comboBufferSize->count() - 1)
        pos = ui->comboBufferSize->count() - 1;
    ui->comboBufferSize->setCurrentIndex(pos);
    ui->checkBoucle->setChecked(ConfManager::getInstance()->getValue(ConfManager::SECTION_NONE, "wav_auto_loop", false).toBool());
    ui->checkBlanc->setChecked(ConfManager::getInstance()->getValue(ConfManager::SECTION_NONE, "wav_remove_blank", false).toBool());

    // Paramètres synthétiseur
    int gain = ConfManager::getInstance()->getValue(ConfManager::SECTION_SOUND_ENGINE, "gain", 0).toInt();
    ui->sliderGain->setValue(gain);
    ui->labelGain->setNum(gain);
    ui->dialRevNiveau->setValue(ConfManager::getInstance()->getValue(ConfManager::SECTION_SOUND_ENGINE, "rev_level", 0).toInt());
    ui->dialRevProfondeur->setValue(ConfManager::getInstance()->getValue(ConfManager::SECTION_SOUND_ENGINE, "rev_size", 0).toInt());
    ui->dialRevDensite->setValue(ConfManager::getInstance()->getValue(ConfManager::SECTION_SOUND_ENGINE, "rev_width", 0).toInt());
    ui->dialRevAttenuation->setValue(ConfManager::getInstance()->getValue(ConfManager::SECTION_SOUND_ENGINE, "rev_damping", 0).toInt());
    ui->dialChoNiveau->setValue(ConfManager::getInstance()->getValue(ConfManager::SECTION_SOUND_ENGINE, "cho_level", 0).toInt());
    ui->dialChoAmplitude->setValue(ConfManager::getInstance()->getValue(ConfManager::SECTION_SOUND_ENGINE, "cho_depth", 0).toInt());
    ui->dialChoFrequence->setValue(ConfManager::getInstance()->getValue(ConfManager::SECTION_SOUND_ENGINE, "cho_frequency", 0).toInt());
    ui->checkRepercussionStereo->setChecked(
                ConfManager::getInstance()->getValue(ConfManager::SECTION_NONE, "stereo_modification", false).toBool());
    ui->comboKeyName->setCurrentIndex(
                (int)KeyNameManager::getInstance()->getNameMiddleC());
    ui->spinDefaultVelocity->setValue(
                ConfManager::getInstance()->getValue(ConfManager::SECTION_KEYBOARD, "velocity", 127).toInt());

    //ui->comboRam->setVisible(false); // Temporaire : tout charger dans la ram n'apporte rien pour l'instant (sur linux)
    //ui->label_2->setVisible(false);
    QTimer::singleShot(1, this, SLOT(fillColors())); // trick that fixes a bug which appeared with Qt5

    // Initialisation mappage
    if (_keyboard != NULL)
    {
        _octaveMapping = this->getOctaveMap();
        if (_octaveMapping >= ui->comboDo->count())
            _octaveMapping = 3;
        ui->comboDo->setCurrentIndex(_octaveMapping);
        _keyboard->set(PianoKeybd::PROPERTY_MAPPING_FIRST_NOTE, 12 * _octaveMapping);
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 13; j++)
                _keyboard->setMapping((PianoKeybd::Key)j, i, QKeySequence::fromString(getKeyMapped(i, (PianoKeybd::Key)j)));
        }
        ui->tableKeyboardMap->setKeyboard(_keyboard);
        connect(ui->tableKeyboardMap, SIGNAL(combinaisonChanged(int,int,QString)), this, SLOT(combinaisonChanged(int,int,QString)));
        renameComboDo();

        // Vélocité par défaut du clavier
        _keyboard->set(PianoKeybd::PROPERTY_VELOCITY,
                       ConfManager::getInstance()->getValue(ConfManager::SECTION_KEYBOARD, "velocity", 127).toInt());
    }

    initComboLanguage();

    // Populate color themes and select the current one
    ThemeManager::getInstance()->populateCombobox(ui->comboColorTheme);

    _loaded = true;
}
Config::~Config()
{
    delete ui;
}

void Config::show()
{
    // Midi input to contineously update
    ui->comboMidiInput->blockSignals(true);
    ui->comboMidiInput->clear();
    QStringList listMidi = _mainWindow->getListMidi();
    ui->comboMidiInput->addItem("-");
    ui->comboMidiInput->addItems(listMidi);

    // Sélection
    int numMidiPort = ConfManager::getInstance()->getValue(ConfManager::SECTION_MIDI, "index_port", -1).toInt();
    if (ui->comboMidiInput->count() > numMidiPort + 1)
        ui->comboMidiInput->setCurrentIndex(numMidiPort + 1);
    else
        ui->comboMidiInput->setCurrentIndex(0);
    ui->comboMidiInput->blockSignals(false);

    // Show dialog
    QDialog::show();
}

void Config::setRam(int val)
{
    if (_loaded)
    {
        ConfManager::getInstance()->setValue(ConfManager::SECTION_NONE, "ram", (val == 1));
        QMessageBox::information(QApplication::activeWindow(), trUtf8("Information"),
                                 trUtf8("La modification sera prise en compte lors du prochain démarrage du logiciel."));
    }
}
void Config::setAudioOutput(int index)
{
    Q_UNUSED(index)
    if (_loaded)
    {
        _mainWindow->setAudioDevice(getAudioType(), getAudioIndex(), _bufferSize);
        if (getAudioIndex() == 0)
            storeAudioConfig();
    }
}
void Config::on_comboBufferSize_activated(int index)
{
    // Modification de la taille du buffer
    if (_loaded)
    {
        _bufferSize = pow(2.0f, index + 4);
        setAudioOutput(ui->comboAudioOuput->currentIndex());
        if (getAudioIndex() == 0)
            storeAudioConfig();
    }
}
void Config::setWavAutoLoop(bool checked)
{
    if (_loaded)
    {
        ConfManager::getInstance()->setValue(ConfManager::SECTION_NONE, "wav_auto_loop", checked);
    }
}
void Config::setWavRemoveBlank(bool checked)
{
    if (_loaded)
    {
        ConfManager::getInstance()->setValue(ConfManager::SECTION_NONE, "wav_remove_blank", checked);
    }
}
void Config::setNumPortMidi(int val)
{
    if (_loaded)
    {
        int numPortMidi = val-1;
        ConfManager::getInstance()->setValue(ConfManager::SECTION_MIDI, "index_port", numPortMidi);
        _mainWindow->openMidiPort(numPortMidi);
    }
}

void Config::setSynthGain(int val)
{
    if (_loaded)
    {
        ConfManager::getInstance()->setValue(ConfManager::SECTION_SOUND_ENGINE, "gain", val);
        _mainWindow->setSynthGain(val);
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
    if (_loaded)
    {
        ConfManager::getInstance()->setValue(ConfManager::SECTION_SOUND_ENGINE, "rev_level", value);
        _mainWindow->updateSynthChoRev();
    }
}

void Config::on_dialRevProfondeur_valueChanged(int value)
{
    if (_loaded)
    {
        ConfManager::getInstance()->setValue(ConfManager::SECTION_SOUND_ENGINE, "rev_size", value);
        _mainWindow->updateSynthChoRev();
    }
}

void Config::on_dialRevDensite_valueChanged(int value)
{
    if (_loaded)
    {
        ConfManager::getInstance()->setValue(ConfManager::SECTION_SOUND_ENGINE, "rev_width", value);
        _mainWindow->updateSynthChoRev();
    }
}

void Config::on_dialRevAttenuation_valueChanged(int value)
{
    if (_loaded)
    {
        ConfManager::getInstance()->setValue(ConfManager::SECTION_SOUND_ENGINE, "rev_damping", value);
        _mainWindow->updateSynthChoRev();
    }
}

void Config::on_dialChoNiveau_valueChanged(int value)
{
    if (_loaded)
    {
        ConfManager::getInstance()->setValue(ConfManager::SECTION_SOUND_ENGINE, "cho_level", value);
        _mainWindow->updateSynthChoRev();
    }
}

void Config::on_dialChoAmplitude_valueChanged(int value)
{
    if (_loaded)
    {
        ConfManager::getInstance()->setValue(ConfManager::SECTION_SOUND_ENGINE, "cho_depth", value);
        _mainWindow->updateSynthChoRev();
    }
}

void Config::on_dialChoFrequence_valueChanged(int value)
{
    if (_loaded)
    {
        ConfManager::getInstance()->setValue(ConfManager::SECTION_SOUND_ENGINE, "cho_frequency", value);
        _mainWindow->updateSynthChoRev();
    }
}

void Config::on_checkRepercussionStereo_clicked()
{
    if (_loaded)
    {
        ConfManager::getInstance()->setValue(ConfManager::SECTION_NONE, "stereo_modification",
                                             ui->checkRepercussionStereo->isChecked());
    }
}

void Config::load()
{
    _audioIndex = ConfManager::getInstance()->getValue(ConfManager::SECTION_AUDIO, "index", 0).toInt();
    _bufferSize = ConfManager::getInstance()->getValue(ConfManager::SECTION_AUDIO, "buffer_size", 512).toInt();
    _audioType = ConfManager::getInstance()->getValue(ConfManager::SECTION_AUDIO, "type", 0).toInt();
}

void Config::setListeActions(QList<QAction *> actions)
{
    _actionList = actions;
    _actionListToolbar = ConfManager::getInstance()->getValue(
                ConfManager::SECTION_DISPLAY, "actions", this->getDefaultListActions()).toByteArray();
    this->fillActions();
}

void Config::fillActions()
{
    // Liste des actions disponibles
    ui->listActions->clear();
    for (int i = 0; i < _actionList.size(); i++)
    {
        if (!_actionListToolbar.contains((char)i))
        {
            ui->listActions->addItem(_actionList.at(i)->text().replace("&", ""));
            QListWidgetItem * item = ui->listActions->item(ui->listActions->count()-1);
            item->setIcon(_actionList.at(i)->icon());
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
    for (int i = 0; i < _actionListToolbar.size(); i++)
    {
        num = _actionListToolbar.at(i);
        if (num < _actionList.size())
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
                ui->listToolbar->addItem(_actionList.at(num)->text().replace("&", ""));
                QListWidgetItem * item = ui->listToolbar->item(ui->listToolbar->count()-1);
                item->setIcon(_actionList.at(num)->icon());
                item->setData(Qt::UserRole, num);
            }
        }
    }

    // Sauvegarde et mise à jour MainWindow
    if (_loaded)
    {
        ConfManager::getInstance()->setValue(ConfManager::SECTION_DISPLAY, "actions", _actionListToolbar);
        QList<QAction *> liste;
        for (int i = 0; i < _actionListToolbar.size(); i++)
        {
            char index = _actionListToolbar.at(i);
            if (index < _actionList.size())
            {
                if (index == -1)
                    liste << NULL;
                else
                    liste << _actionList.at(index);
            }
        }
        _mainWindow->setListeActions(liste);
    }
}

void Config::on_pushUp_clicked()
{
    if (ui->listToolbar->currentRow() > 0)
    {
        int index = ui->listToolbar->currentRow();
        if (index < _actionListToolbar.size())
        {
            char numTmp = _actionListToolbar.at(index);
            _actionListToolbar[index] = _actionListToolbar.at(index - 1);
            _actionListToolbar[index - 1] = numTmp;
            this->fillActions();
            ui->listToolbar->setCurrentRow(index - 1);
        }
    }
}

void Config::on_pushDown_clicked()
{
    if (ui->listToolbar->currentRow() != -1 &&
            ui->listToolbar->currentRow() != ui->listToolbar->count() - 1)
    {
        int index = ui->listToolbar->currentRow();
        if (index < _actionListToolbar.size() - 1)
        {
            char numTmp = _actionListToolbar.at(index);
            _actionListToolbar[index] = _actionListToolbar.at(index + 1);
            _actionListToolbar[index + 1] = numTmp;
            this->fillActions();
            ui->listToolbar->setCurrentRow(index + 1);
        }
    }
}

void Config::on_pushRight_clicked()
{
    int index = ui->listActions->currentRow();
    if (index != -1)
    {
        _actionListToolbar.append((char)ui->listActions->currentItem()->data(Qt::UserRole).toInt());
        this->fillActions();
        if (ui->listActions->count())
        {
            if (index >= ui->listActions->count())
                index--;
            ui->listActions->setCurrentRow(index);
        }
    }
}

void Config::on_pushLeft_clicked()
{
    int index = ui->listToolbar->currentRow();
    if (index != -1 && index < _actionListToolbar.size())
    {
        _actionListToolbar.remove(index, 1);
        this->fillActions();
        if (ui->listToolbar->count())
        {
            if (index >= ui->listToolbar->count())
                index--;
            ui->listToolbar->setCurrentRow(index);
        }
    }
}

void Config::on_pushResetToolbar_clicked()
{
    _actionListToolbar = this->getDefaultListActions();
    this->fillActions();
}

void Config::on_listToolbar_itemSelectionChanged()
{
    QList<QListWidgetItem *> listItems = ui->listToolbar->selectedItems();
    ui->pushLeft->setEnabled(listItems.count());
    if (listItems.count() == 1)
    {
        ui->pushLeft->setEnabled(true);
        if (ui->listToolbar->count() > 1)
        {
            ui->pushDown->setEnabled(listItems.at(0) != ui->listToolbar->item(ui->listToolbar->count()-1));
            ui->pushUp->setEnabled(listItems.at(0) != ui->listToolbar->item(0));
        }
        else
        {
            ui->pushDown->setEnabled(false);
            ui->pushUp->setEnabled(false);
        }
    }
    else
    {
        ui->pushLeft->setEnabled(false);
        ui->pushUp->setEnabled(false);
        ui->pushDown->setEnabled(false);
    }
}

void Config::on_listActions_itemSelectionChanged()
{
    ui->pushRight->setEnabled(ui->listActions->selectedItems().count());
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

void Config::combinaisonChanged(int key, int numOctave, QString combinaison)
{
    ConfManager::getInstance()->setValue(ConfManager::SECTION_MAP,
                                         "key_" + QString::number(numOctave) + "_" + QString::number(key),
                                         combinaison);
}

void Config::on_comboKeyName_currentIndexChanged(int index)
{
    if (_loaded)
    {
        KeyNameManager::getInstance()->setMiddleKey((KeyNameManager::NameMiddleC)index);
        _mainWindow->noteNameChanged();
        renameComboDo();
    }
}

void Config::on_comboDo_currentIndexChanged(int index)
{
    this->setOctaveMap(index);
    _keyboard->set(PianoKeybd::PROPERTY_MAPPING_FIRST_NOTE, index * 12);
}

void Config::setOctaveMap(int octave)
{
    ConfManager::getInstance()->setValue(ConfManager::SECTION_MAP, "octave_offset", octave);
    ui->comboDo->blockSignals(true);
    ui->comboDo->setCurrentIndex(octave);
    ui->comboDo->blockSignals(false);
}

void Config::renameComboDo()
{
    int nbElement = ui->comboDo->count();
    for (int i = 0; i < nbElement; i++)
        ui->comboDo->setItemText(i, KeyNameManager::getInstance()->getKeyName(12 * i));
}

void Config::on_spinDefaultVelocity_editingFinished()
{
    if (_loaded)
    {
        int velocity = ui->spinDefaultVelocity->value();
        ConfManager::getInstance()->setValue(ConfManager::SECTION_KEYBOARD, "velocity", velocity);
        _keyboard->set(PianoKeybd::PROPERTY_VELOCITY, velocity);
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
    QMap<QString, QString> languages = TranslationManager::getLanguages();
    QStringList languageNames = languages.values();
    qSort(languageNames.begin(), languageNames.end(), caseInsensitiveLessThan);

    foreach (QString languageName, languageNames)
        ui->comboLangue->addItem(languageName, languages.key(languageName));

    // Selection of the current language
    QString locale = QLocale::system().name().section('_', 0, 0);
    locale = ConfManager::getInstance()->getValue(ConfManager::SECTION_NONE, "language", locale).toString();
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
    ConfManager::getInstance()->setValue(ConfManager::SECTION_NONE, "language", ui->comboLangue->itemData(index));
    QMessageBox::information(QApplication::activeWindow(), trUtf8("Information"),
                             trUtf8("La modification sera prise en compte lors du prochain démarrage du logiciel."));
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

    ConfManager::getInstance()->setValue(ConfManager::SECTION_AUDIO, "index", _audioIndex);
    ConfManager::getInstance()->setValue(ConfManager::SECTION_AUDIO, "buffer_size", _bufferSize);
    ConfManager::getInstance()->setValue(ConfManager::SECTION_AUDIO, "type", _audioType);
}


//////////////
/// Colors ///
//////////////


void Config::on_comboColorTheme_currentIndexChanged(int index)
{
    if (_loaded) {
        if (index == 0) {
            ThemeManager::getInstance()->resetTheme();
            fillColors();
            QMessageBox::information(QApplication::activeWindow(), trUtf8("Information"),
                                     trUtf8("La modification sera prise en compte lors du prochain démarrage du logiciel."));
        } else {
            int themeIndex = ui->comboColorTheme->itemData(index).toInt();
            if (themeIndex != -1) {
                ThemeManager::getInstance()->applyTheme(themeIndex);
                fillColors();
                QMessageBox::information(QApplication::activeWindow(), trUtf8("Information"),
                                         trUtf8("La modification sera prise en compte lors du prochain démarrage du logiciel."));
            }
        }
    }
}

void Config::on_pushColorWindowBackground_clicked()
{
    QColor color = QColorDialog::getColor(ThemeManager::getInstance()->getColor(ThemeManager::WINDOW_BACKGROUND),
                                          this, trUtf8("Choisissez une couleur"));
    if (color.isValid())
    {
        ThemeManager::getInstance()->setColor(ThemeManager::WINDOW_BACKGROUND, color);
        this->fillColors();
    }
}

void Config::on_pushColorButtonBackground_clicked()
{
    QColor color = QColorDialog::getColor(ThemeManager::getInstance()->getColor(ThemeManager::BUTTON_BACKGROUND),
                                          this, trUtf8("Choisissez une couleur"));
    if (color.isValid())
    {
        ThemeManager::getInstance()->setColor(ThemeManager::BUTTON_BACKGROUND, color);
        this->fillColors();
    }
}

void Config::on_pushColorSelectionBackground_clicked()
{
    QColor color = QColorDialog::getColor(ThemeManager::getInstance()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND),
                                          this, trUtf8("Choisissez une couleur"));
    if (color.isValid())
    {
        ThemeManager::getInstance()->setColor(ThemeManager::HIGHLIGHTED_BACKGROUND, color);
        this->fillColors();
    }
}

void Config::on_pushColorListBackground_clicked()
{
    QColor color = QColorDialog::getColor(ThemeManager::getInstance()->getColor(ThemeManager::LIST_BACKGROUND),
                                          this, trUtf8("Choisissez une couleur"));
    if (color.isValid())
    {
        ThemeManager::getInstance()->setColor(ThemeManager::LIST_BACKGROUND, color);
        this->fillColors();
    }
}

void Config::on_pushColorListAlternativeBackground_clicked()
{
    QColor color = QColorDialog::getColor(ThemeManager::getInstance()->getColor(ThemeManager::LIST_ALTERNATIVE_BACKGROUND),
                                          this, trUtf8("Choisissez une couleur"));
    if (color.isValid())
    {
        ThemeManager::getInstance()->setColor(ThemeManager::LIST_ALTERNATIVE_BACKGROUND, color);
        this->fillColors();
    }
}

void Config::on_pushColorWindowText_clicked()
{
    QColor color = QColorDialog::getColor(ThemeManager::getInstance()->getColor(ThemeManager::WINDOW_TEXT),
                                          this, trUtf8("Choisissez une couleur"));
    if (color.isValid())
    {
        ThemeManager::getInstance()->setColor(ThemeManager::WINDOW_TEXT, color);
        this->fillColors();
    }
}

void Config::on_pushColorButtonText_clicked()
{
    QColor color = QColorDialog::getColor(ThemeManager::getInstance()->getColor(ThemeManager::BUTTON_TEXT),
                                          this, trUtf8("Choisissez une couleur"));
    if (color.isValid())
    {
        ThemeManager::getInstance()->setColor(ThemeManager::BUTTON_TEXT, color);
        this->fillColors();
    }
}

void Config::on_pushColorSelectionText_clicked()
{
    QColor color = QColorDialog::getColor(ThemeManager::getInstance()->getColor(ThemeManager::HIGHLIGHTED_TEXT),
                                          this, trUtf8("Choisissez une couleur"));
    if (color.isValid())
    {
        ThemeManager::getInstance()->setColor(ThemeManager::HIGHLIGHTED_TEXT, color);
        this->fillColors();
    }
}

void Config::on_pushColorListText_clicked()
{
    QColor color = QColorDialog::getColor(ThemeManager::getInstance()->getColor(ThemeManager::LIST_TEXT),
                                          this, trUtf8("Choisissez une couleur"));
    if (color.isValid())
    {
        ThemeManager::getInstance()->setColor(ThemeManager::LIST_TEXT, color);
        this->fillColors();
    }
}

void Config::fillColors()
{
    QString styleStart = "QPushButton{border: 1px solid #888; background-color: ";
    ui->pushColorWindowBackground->setStyleSheet(
                styleStart + ThemeManager::getInstance()->getColor(ThemeManager::WINDOW_BACKGROUND).name() + ";}");
    ui->pushColorWindowText->setStyleSheet(
                styleStart + ThemeManager::getInstance()->getColor(ThemeManager::WINDOW_TEXT).name() + ";}");
    ui->pushColorButtonBackground->setStyleSheet(
                styleStart + ThemeManager::getInstance()->getColor(ThemeManager::BUTTON_BACKGROUND).name() + ";}");
    ui->pushColorButtonText->setStyleSheet(
                styleStart + ThemeManager::getInstance()->getColor(ThemeManager::BUTTON_TEXT).name() + ";}");
    ui->pushColorSelectionBackground->setStyleSheet(
                styleStart + ThemeManager::getInstance()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name() + ";}");
    ui->pushColorSelectionText->setStyleSheet(
                styleStart + ThemeManager::getInstance()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name() + ";}");
    ui->pushColorListBackground->setStyleSheet(
                styleStart + ThemeManager::getInstance()->getColor(ThemeManager::LIST_BACKGROUND).name() + ";}");
    ui->pushColorListAlternativeBackground->setStyleSheet(
                styleStart + ThemeManager::getInstance()->getColor(ThemeManager::LIST_ALTERNATIVE_BACKGROUND).name() + ";}");
    ui->pushColorListText->setStyleSheet(
                styleStart + ThemeManager::getInstance()->getColor(ThemeManager::LIST_TEXT).name() + ";}");
    ThemeManager::getInstance()->selectIndex(ui->comboColorTheme);
}

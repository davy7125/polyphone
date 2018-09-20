/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2017 Davy Triponney                                **
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
#include "editor_old.h"
#include "portaudio.h"
#include <qmath.h>
#include "contextmanager.h"


Config::Config(QWidget *parent) : QWidget(parent),
    ui(new Ui::Config)
{
    ui->setupUi(this);

#ifdef Q_OS_MAC
    ui->groupBox->layout()->setMargin(10);
#endif

    // LOAD CONFIGURATION
    _loaded = false;

    // Liste des sorties audio
    int audioType = ContextManager::configuration()->getValue(ConfManager::SECTION_AUDIO, "type", 0).toInt();
    int audioIndex = ContextManager::configuration()->getValue(ConfManager::SECTION_AUDIO, "index", 0).toInt();

    QList<HostInfo> hostInfos = ContextManager::audio()->getHostInfo();
    bool configFound = false;
    int comboboxIndex = 0;
    int comboboxDefaultIndex = 0;
    for (int i = 0; i < hostInfos.size(); i++)
    {
        if (hostInfos[i]._index < 0)
        {
            ui->comboAudioOuput->addItem(hostInfos[i]._name, QString::number(hostInfos[i]._index) + "#-1");
            if (hostInfos[i]._index == audioType)
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
                if (!configFound && hostInfos[i]._index == audioType)
                {
                    if (hostInfos[i]._devices[j]._index == audioIndex)
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

    int bufferSize = ContextManager::configuration()->getValue(ConfManager::SECTION_AUDIO, "buffer_size", 512).toInt();
    int pos = qRound(qLn(bufferSize) / 0.69314718056 - 4);
    if (pos < 0)
        pos = 0;
    if (pos > ui->comboBufferSize->count() - 1)
        pos = ui->comboBufferSize->count() - 1;
    ui->comboBufferSize->setCurrentIndex(pos);
    ui->checkBoucle->setChecked(ContextManager::configuration()->getValue(ConfManager::SECTION_NONE, "wav_auto_loop", false).toBool());
    ui->checkBlanc->setChecked(ContextManager::configuration()->getValue(ConfManager::SECTION_NONE, "wav_remove_blank", false).toBool());

    // Paramètres synthétiseur
    int gain = ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "gain", 0).toInt();
    ui->sliderGain->setValue(gain);
    ui->labelGain->setNum(gain);
    ui->dialRevNiveau->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "rev_level", 0).toInt());
    ui->dialRevProfondeur->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "rev_size", 0).toInt());
    ui->dialRevDensite->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "rev_width", 0).toInt());
    ui->dialRevAttenuation->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "rev_damping", 0).toInt());
    ui->dialChoNiveau->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "cho_level", 0).toInt());
    ui->dialChoAmplitude->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "cho_depth", 0).toInt());
    ui->dialChoFrequence->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "cho_frequency", 0).toInt());
    ui->checkRepercussionStereo->setChecked(
                ContextManager::configuration()->getValue(ConfManager::SECTION_NONE, "stereo_modification", false).toBool());
    ui->comboKeyName->setCurrentIndex(
                (int)ContextManager::keyName()->getNameMiddleC());
    ui->spinDefaultVelocity->setValue(
                ContextManager::configuration()->getValue(ConfManager::SECTION_KEYBOARD, "velocity", 127).toInt());

    //ui->comboRam->setVisible(false); // Temporaire : tout charger dans la ram n'apporte rien pour l'instant (sur linux)
    //ui->label_2->setVisible(false);
    QTimer::singleShot(1, this, SLOT(fillColors())); // trick that fixes a bug which appeared with Qt5

    // Initialisation mappage

    int octaveMapping = ContextManager::configuration()->getValue(ConfManager::SECTION_MAP, "octave_offset", 3).toInt();
    if (octaveMapping >= ui->comboDo->count())
        octaveMapping = 3;
    ui->comboDo->setCurrentIndex(octaveMapping);
//    KeyboardManager::set(PianoKeybd::PROPERTY_MAPPING_FIRST_NOTE, 12 * octaveMapping);
//    for (int i = 0; i < 4; i++)
//    {
//        for (int j = 0; j < 13; j++)
//            KeyboardManager::setMapping((PianoKeybd::Key)j, i, QKeySequence::fromString(getKeyMapped(i, (PianoKeybd::Key)j)));
//    }
//    connect(ui->tableKeyboardMap, SIGNAL(combinaisonChanged(int,int,QString)), this, SLOT(combinaisonChanged(int,int,QString)));
//    renameComboDo();

//    // Vélocité par défaut du clavier
//    KeyboardManager::set(PianoKeybd::PROPERTY_VELOCITY,
//                         ContextManager::configuration()->getValue(ConfManager::SECTION_KEYBOARD, "velocity", 127).toInt());

    initComboLanguage();

    // Populate color themes and select the current one
    ContextManager::theme()->populateCombobox(ui->comboColorTheme);

    _loaded = true;
}

Config::~Config()
{
    delete ui;
}

void Config::initializeInterface()
{
    // Style
    QString styleSheet = QString("QLabel{border-radius:5px;padding: 0px 5px;") +
            "background-color:" + ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name() + ";" +
            "color: " + ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name() + ";}";
    ui->generalLabel->setStyleSheet(styleSheet);
    ui->interfaceLabel->setStyleSheet(styleSheet);
    ui->soundLabel->setStyleSheet(styleSheet);
    ui->keyboardLabel->setStyleSheet(styleSheet);

    // Midi input to contineously update
    ui->comboMidiInput->blockSignals(true);
    ui->comboMidiInput->clear();
    QStringList listMidi = ContextManager::midi()->getMidiList();
    ui->comboMidiInput->addItem("-");
    ui->comboMidiInput->addItems(listMidi);

    // Sélection
    int numMidiPort = ContextManager::configuration()->getValue(ConfManager::SECTION_MIDI, "index_port", -1).toInt();
    if (ui->comboMidiInput->count() > numMidiPort + 1)
        ui->comboMidiInput->setCurrentIndex(numMidiPort + 1);
    else
        ui->comboMidiInput->setCurrentIndex(0);
    ui->comboMidiInput->blockSignals(false);
}

void Config::setAudioOutput(int index)
{
    Q_UNUSED(index)
    if (_loaded)
    {
        QString str = ui->comboAudioOuput->itemData(ui->comboAudioOuput->currentIndex()).toString();
        QStringList listStr = str.split("#");
        int audioType = listStr.size() >= 1 ? listStr[0].toInt() : 0;
        int audioIndex = listStr.size() >= 2 ? listStr[1].toInt() : 0;

        ContextManager::configuration()->setValue(ConfManager::SECTION_AUDIO, "index", audioIndex);
        ContextManager::configuration()->setValue(ConfManager::SECTION_AUDIO, "type", audioType);
    }
}
void Config::on_comboBufferSize_activated(int index)
{
    // Modification de la taille du buffer
    if (_loaded)
    {
        int bufferSize = pow(2.0f, index + 4);
        ContextManager::configuration()->setValue(ConfManager::SECTION_AUDIO, "buffer_size", bufferSize);
    }
}
void Config::setWavAutoLoop(bool checked)
{
    if (_loaded)
    {
        ContextManager::configuration()->setValue(ConfManager::SECTION_NONE, "wav_auto_loop", checked);
    }
}
void Config::setWavRemoveBlank(bool checked)
{
    if (_loaded)
    {
        ContextManager::configuration()->setValue(ConfManager::SECTION_NONE, "wav_remove_blank", checked);
    }
}
void Config::setNumPortMidi(int val)
{
    if (_loaded)
    {
        int numPortMidi = val-1;
        ContextManager::configuration()->setValue(ConfManager::SECTION_MIDI, "index_port", numPortMidi);
        ContextManager::midi()->openMidiPort(numPortMidi);
    }
}

void Config::setSynthGain(int val)
{
    if (_loaded)
    {
        ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "gain", val);
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
        ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "rev_level", value);
    }
}

void Config::on_dialRevProfondeur_valueChanged(int value)
{
    if (_loaded)
    {
        ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "rev_size", value);
    }
}

void Config::on_dialRevDensite_valueChanged(int value)
{
    if (_loaded)
    {
        ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "rev_width", value);
    }
}

void Config::on_dialRevAttenuation_valueChanged(int value)
{
    if (_loaded)
    {
        ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "rev_damping", value);
    }
}

void Config::on_dialChoNiveau_valueChanged(int value)
{
    if (_loaded)
    {
        ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "cho_level", value);
    }
}

void Config::on_dialChoAmplitude_valueChanged(int value)
{
    if (_loaded)
    {
        ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "cho_depth", value);
    }
}

void Config::on_dialChoFrequence_valueChanged(int value)
{
    if (_loaded)
    {
        ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "cho_frequency", value);
    }
}

void Config::on_checkRepercussionStereo_clicked()
{
    if (_loaded)
    {
        ContextManager::configuration()->setValue(ConfManager::SECTION_NONE, "stereo_modification",
                                             ui->checkRepercussionStereo->isChecked());
    }
}

void Config::combinaisonChanged(int key, int numOctave, QString combinaison)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_MAP,
                                         "key_" + QString::number(numOctave) + "_" + QString::number(key),
                                         combinaison);
}

void Config::on_comboKeyName_currentIndexChanged(int index)
{
    if (_loaded)
    {
        ContextManager::keyName()->setMiddleKey((KeyNameManager::NameMiddleC)index);
        renameComboDo();
    }
}

void Config::on_comboDo_currentIndexChanged(int index)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_MAP, "octave_offset", index);
    ui->comboDo->blockSignals(true);
    ui->comboDo->setCurrentIndex(index);
    ui->comboDo->blockSignals(false);
    //KeyboardManager::set(PianoKeybd::PROPERTY_MAPPING_FIRST_NOTE, index * 12);
}

void Config::renameComboDo()
{
    int nbElement = ui->comboDo->count();
    for (int i = 0; i < nbElement; i++)
        ui->comboDo->setItemText(i, ContextManager::keyName()->getKeyName(12 * i));
}

void Config::on_spinDefaultVelocity_editingFinished()
{
    if (_loaded)
    {
        int velocity = ui->spinDefaultVelocity->value();
        ContextManager::configuration()->setValue(ConfManager::SECTION_KEYBOARD, "velocity", velocity);
        //KeyboardManager::set(PianoKeybd::PROPERTY_VELOCITY, velocity);
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
    QMap<QString, QString> languages = ContextManager::translation()->getLanguages();
    QStringList languageNames = languages.values();
    qSort(languageNames.begin(), languageNames.end(), caseInsensitiveLessThan);

    foreach (QString languageName, languageNames)
        ui->comboLangue->addItem(languageName, languages.key(languageName));

    // Selection of the current language
    QString locale = QLocale::system().name().section('_', 0, 0);
    locale = ContextManager::configuration()->getValue(ConfManager::SECTION_NONE, "language", locale).toString();
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
    ContextManager::configuration()->setValue(ConfManager::SECTION_NONE, "language", ui->comboLangue->itemData(index));
    QMessageBox::information(QApplication::activeWindow(), trUtf8("Information"),
                             trUtf8("La modification sera prise en compte lors du prochain démarrage du logiciel."));
}


//////////////
/// Colors ///
//////////////

void Config::on_comboColorTheme_currentIndexChanged(int index)
{
    if (_loaded) {
        if (index == 0) {
            ContextManager::theme()->resetTheme();
            fillColors();
            QMessageBox::information(QApplication::activeWindow(), trUtf8("Information"),
                                     trUtf8("La modification sera prise en compte lors du prochain démarrage du logiciel."));
        } else {
            int themeIndex = ui->comboColorTheme->itemData(index).toInt();
            if (themeIndex != -1) {
                ContextManager::theme()->applyTheme(themeIndex);
                fillColors();
                QMessageBox::information(QApplication::activeWindow(), trUtf8("Information"),
                                         trUtf8("La modification sera prise en compte lors du prochain démarrage du logiciel."));
            }
        }
    }
}

void Config::on_pushColorWindowBackground_clicked()
{
    QColor color = QColorDialog::getColor(ContextManager::theme()->getColor(ThemeManager::WINDOW_BACKGROUND),
                                          this, trUtf8("Choisissez une couleur"));
    if (color.isValid())
    {
        ContextManager::theme()->setColor(ThemeManager::WINDOW_BACKGROUND, color);
        this->fillColors();
    }
}

void Config::on_pushColorButtonBackground_clicked()
{
    QColor color = QColorDialog::getColor(ContextManager::theme()->getColor(ThemeManager::BUTTON_BACKGROUND),
                                          this, trUtf8("Choisissez une couleur"));
    if (color.isValid())
    {
        ContextManager::theme()->setColor(ThemeManager::BUTTON_BACKGROUND, color);
        this->fillColors();
    }
}

void Config::on_pushColorSelectionBackground_clicked()
{
    QColor color = QColorDialog::getColor(ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND),
                                          this, trUtf8("Choisissez une couleur"));
    if (color.isValid())
    {
        ContextManager::theme()->setColor(ThemeManager::HIGHLIGHTED_BACKGROUND, color);
        this->fillColors();
    }
}

void Config::on_pushColorListBackground_clicked()
{
    QColor color = QColorDialog::getColor(ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND),
                                          this, trUtf8("Choisissez une couleur"));
    if (color.isValid())
    {
        ContextManager::theme()->setColor(ThemeManager::LIST_BACKGROUND, color);
        this->fillColors();
    }
}

void Config::on_pushColorListAlternativeBackground_clicked()
{
    QColor color = QColorDialog::getColor(ContextManager::theme()->getColor(ThemeManager::LIST_ALTERNATIVE_BACKGROUND),
                                          this, trUtf8("Choisissez une couleur"));
    if (color.isValid())
    {
        ContextManager::theme()->setColor(ThemeManager::LIST_ALTERNATIVE_BACKGROUND, color);
        this->fillColors();
    }
}

void Config::on_pushColorWindowText_clicked()
{
    QColor color = QColorDialog::getColor(ContextManager::theme()->getColor(ThemeManager::WINDOW_TEXT),
                                          this, trUtf8("Choisissez une couleur"));
    if (color.isValid())
    {
        ContextManager::theme()->setColor(ThemeManager::WINDOW_TEXT, color);
        this->fillColors();
    }
}

void Config::on_pushColorButtonText_clicked()
{
    QColor color = QColorDialog::getColor(ContextManager::theme()->getColor(ThemeManager::BUTTON_TEXT),
                                          this, trUtf8("Choisissez une couleur"));
    if (color.isValid())
    {
        ContextManager::theme()->setColor(ThemeManager::BUTTON_TEXT, color);
        this->fillColors();
    }
}

void Config::on_pushColorSelectionText_clicked()
{
    QColor color = QColorDialog::getColor(ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT),
                                          this, trUtf8("Choisissez une couleur"));
    if (color.isValid())
    {
        ContextManager::theme()->setColor(ThemeManager::HIGHLIGHTED_TEXT, color);
        this->fillColors();
    }
}

void Config::on_pushColorListText_clicked()
{
    QColor color = QColorDialog::getColor(ContextManager::theme()->getColor(ThemeManager::LIST_TEXT),
                                          this, trUtf8("Choisissez une couleur"));
    if (color.isValid())
    {
        ContextManager::theme()->setColor(ThemeManager::LIST_TEXT, color);
        this->fillColors();
    }
}

void Config::fillColors()
{
    QString styleStart = "QPushButton{border: 1px solid #888; background-color: ";
    ui->pushColorWindowBackground->setStyleSheet(
                styleStart + ContextManager::theme()->getColor(ThemeManager::WINDOW_BACKGROUND).name() + ";}");
    ui->pushColorWindowText->setStyleSheet(
                styleStart + ContextManager::theme()->getColor(ThemeManager::WINDOW_TEXT).name() + ";}");
    ui->pushColorButtonBackground->setStyleSheet(
                styleStart + ContextManager::theme()->getColor(ThemeManager::BUTTON_BACKGROUND).name() + ";}");
    ui->pushColorButtonText->setStyleSheet(
                styleStart + ContextManager::theme()->getColor(ThemeManager::BUTTON_TEXT).name() + ";}");
    ui->pushColorSelectionBackground->setStyleSheet(
                styleStart + ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name() + ";}");
    ui->pushColorSelectionText->setStyleSheet(
                styleStart + ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name() + ";}");
    ui->pushColorListBackground->setStyleSheet(
                styleStart + ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND).name() + ";}");
    ui->pushColorListAlternativeBackground->setStyleSheet(
                styleStart + ContextManager::theme()->getColor(ThemeManager::LIST_ALTERNATIVE_BACKGROUND).name() + ";}");
    ui->pushColorListText->setStyleSheet(
                styleStart + ContextManager::theme()->getColor(ThemeManager::LIST_TEXT).name() + ";}");
    ContextManager::theme()->selectIndex(ui->comboColorTheme);
}

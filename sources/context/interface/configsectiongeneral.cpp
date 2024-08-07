/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program. If not, see http://www.gnu.org/licenses/.    **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "configsectiongeneral.h"
#include "ui_configsectiongeneral.h"
#include "contextmanager.h"

ConfigSectionGeneral::ConfigSectionGeneral(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigSectionGeneral)
{
    ui->setupUi(this);

    // Style
    ui->labelSubTitle1->setStyleSheet("QLabel{margin: 20px 0;}");
    ui->labelSubTitle2->setStyleSheet("QLabel{margin: 20px 0;}");

    // Tool tip
    ui->checkRepercussionStereo->setToolTip(ContextManager::translation()->getTranslation(TranslationManager::TRANSLATION_STEREO_EDITING_HELP));
}

ConfigSectionGeneral::~ConfigSectionGeneral()
{
    delete ui;
}

void ConfigSectionGeneral::initialize()
{
    // Audo settings
    this->initializeAudio();

    // Midi settings
    this->initializeMidi();

    // Various options
    ui->checkBoucle->blockSignals(true);
    ui->checkBoucle->setChecked(ContextManager::configuration()->getValue(ConfManager::SECTION_NONE, "wav_auto_loop", false).toBool());
    ui->checkBoucle->blockSignals(false);

    ui->checkBlanc->blockSignals(true);
    ui->checkBlanc->setChecked(ContextManager::configuration()->getValue(ConfManager::SECTION_NONE, "wav_remove_blank", false).toBool());
    ui->checkBlanc->blockSignals(false);

    // Other configuration that needs to be updated more often
    showEvent(nullptr);
}

void ConfigSectionGeneral::showEvent(QShowEvent * event)
{
    Q_UNUSED(event)

    // The stereo modification state may change
    ui->checkRepercussionStereo->blockSignals(true);
    ui->checkRepercussionStereo->setChecked(ContextManager::configuration()->getValue(ConfManager::SECTION_NONE, "stereo_modification", true).toBool());
    ui->checkRepercussionStereo->blockSignals(false);

    if (event != nullptr)
        QWidget::showEvent(event);
}

void ConfigSectionGeneral::initializeAudio()
{
    ui->comboAudioOuput->blockSignals(true);
    ui->comboBufferSize->blockSignals(true);

    // Current hosts with devices
    QList<AudioDevice::HostInfo> hosts = ContextManager::audio()->getAllHosts();
    ui->comboAudioOuput->clear();
    ui->comboAudioOuput->addItem("-", "none");
    for (int i = 0; i < hosts.count(); i++)
    {
        QString hostIdentifier = hosts[i].identifier;
        QString hostName = hosts[i].name;
        for (int j = 0; j < hosts[i].devices.count(); j++)
        {
            QString deviceName = hosts[i].devices[j].name;
            ui->comboAudioOuput->addItem(hostName + " - " + deviceName, hostIdentifier + "|" + deviceName);
        }
    }

    // Select the right device
    QString audioType = ContextManager::configuration()->getValue(ConfManager::SECTION_AUDIO, "type", "").toString();
    ui->comboAudioOuput->setCurrentIndex(AudioDevice::getCurrentDeviceIndex(hosts, audioType) + 1);

    int bufferSize = ContextManager::configuration()->getValue(ConfManager::SECTION_AUDIO, "buffer_size", 0).toInt();
    switch (bufferSize)
    {
    case 16: ui->comboBufferSize->setCurrentIndex(1); break;
    case 32: ui->comboBufferSize->setCurrentIndex(2); break;
    case 64: ui->comboBufferSize->setCurrentIndex(3); break;
    case 128: ui->comboBufferSize->setCurrentIndex(4); break;
    case 256: ui->comboBufferSize->setCurrentIndex(5); break;
    case 512: ui->comboBufferSize->setCurrentIndex(6); break;
    case 1024: ui->comboBufferSize->setCurrentIndex(7); break;
    case 2048: ui->comboBufferSize->setCurrentIndex(8); break;
    case 4096: ui->comboBufferSize->setCurrentIndex(9); break;
    case 0: default: ui->comboBufferSize->setCurrentIndex(0); break;
    }

    ui->comboAudioOuput->blockSignals(false);
    ui->comboBufferSize->blockSignals(false);
}

void ConfigSectionGeneral::on_comboAudioOuput_currentIndexChanged(int index)
{
    QString audioType = ui->comboAudioOuput->itemData(index).toString();
    ContextManager::configuration()->setValue(ConfManager::SECTION_AUDIO, "type", audioType);
}

void ConfigSectionGeneral::on_comboBufferSize_currentIndexChanged(int index)
{
    int bufferSize = 0;
    switch (index)
    {
    case 1: bufferSize = 16; break;
    case 2: bufferSize = 32; break;
    case 3: bufferSize = 64; break;
    case 4: bufferSize = 128; break;
    case 5: bufferSize = 256; break;
    case 6: bufferSize = 512; break;
    case 7: bufferSize = 1024; break;
    case 8: bufferSize = 2048; break;
    case 9: bufferSize = 4096; break;
    case 0: default: break;
    }

    ContextManager::configuration()->setValue(ConfManager::SECTION_AUDIO, "buffer_size", bufferSize);
}

void ConfigSectionGeneral::initializeMidi()
{
    // Update the possible midi inputs
    ui->comboMidiInput->blockSignals(true);
    ui->comboMidiInput->clear();
    QMap<QString, QString> listMidi = ContextManager::midi()->getMidiList();
    ui->comboMidiInput->addItem("-", "-1#-1");
    foreach (QString key, listMidi.keys())
        ui->comboMidiInput->addItem(listMidi[key], key);

    // Select the current one
    QString indexPort = ContextManager::configuration()->getValue(ConfManager::SECTION_MIDI, "index_port", "-1#-1").toString();
    for (int i = 0; i < ui->comboMidiInput->count(); i++)
    {
        if (ui->comboMidiInput->itemData(i).toString() == indexPort)
        {
            ui->comboMidiInput->setCurrentIndex(i);
            break;
        }
    }
    ui->comboMidiInput->blockSignals(false);
}

void ConfigSectionGeneral::on_comboMidiInput_currentIndexChanged(int index)
{
    QString indexInput = ui->comboMidiInput->itemData(index).toString();
    ContextManager::configuration()->setValue(ConfManager::SECTION_MIDI, "index_port", indexInput);
    ContextManager::midi()->openMidiPort(indexInput);
}

void ConfigSectionGeneral::on_checkBoucle_toggled(bool checked)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_NONE, "wav_auto_loop", checked);
}

void ConfigSectionGeneral::on_checkBlanc_toggled(bool checked)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_NONE, "wav_remove_blank", checked);
}

void ConfigSectionGeneral::on_checkRepercussionStereo_toggled(bool checked)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_NONE, "stereo_modification", checked);
}

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
    ui(new Ui::ConfigSectionGeneral),
    _audioDevicesChanged(false)
{
    ui->setupUi(this);

    // Style
    ui->labelSubTitle1->setStyleSheet("QLabel{margin: 20px 0;}");
    ui->labelSubTitle2->setStyleSheet("QLabel{margin: 20px 0;}");

    // Tool tip
    ui->checkRepercussionStereo->setToolTip(ContextManager::translation()->getTranslation(TranslationManager::TRANSLATION_STEREO_EDITING_HELP));
    ui->comboPolyphony->setToolTip(tr("Maximum number of voices, the quietest notes fading out above it"));
    ui->comboCpuBudget->setToolTip(tr("Maximum share of the processor time used by the voices, the quietest notes fading out above it"));

    // Audio outputs plugged or unplugged
    connect(ContextManager::audio(), SIGNAL(devicesChanged()), this, SLOT(onAudioDevicesChanged()));
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

    // The audio outputs may have changed while the section was hidden
    if (_audioDevicesChanged)
        initializeAudio();

    if (event != nullptr)
        QWidget::showEvent(event);
}

void ConfigSectionGeneral::onAudioDevicesChanged()
{
    // Probing the devices may interrupt the sound: only done if the list can be seen
    if (isVisible())
        initializeAudio();
    else
        _audioDevicesChanged = true;
}

void ConfigSectionGeneral::initializeAudio()
{
    _audioDevicesChanged = false;
    ui->comboAudioOuput->blockSignals(true);
    ui->comboBufferSize->blockSignals(true);
    ui->comboPolyphony->blockSignals(true);
    ui->comboCpuBudget->blockSignals(true);

    // Configured device
    QString audioType = ContextManager::configuration()->getValue(ConfManager::SECTION_AUDIO, "type", "").toString();
    QString selectedHostIdentifier, selectedHostName, selectedDeviceName;
    if (audioType != "none")
        AudioDevice::getHostAndDeviceFromConfig(audioType, selectedHostIdentifier, selectedHostName, selectedDeviceName);

    // Current hosts with devices
    QList<AudioDevice::HostInfo> hosts = ContextManager::audio()->getAllHosts();
    ui->comboAudioOuput->clear();
    ui->comboAudioOuput->addItem("-", "none");
    int selectedIndex = -1;
    int selectedHostEnd = -1;
    for (int i = 0; i < hosts.count(); i++)
    {
        QString hostIdentifier = hosts[i].identifier;
        QString hostName = hosts[i].name;
        bool isSelectedHost = (hostIdentifier == selectedHostIdentifier);
        if (hosts[i].canFollowDefault)
        {
            if (isSelectedHost && selectedDeviceName.isEmpty())
                selectedIndex = ui->comboAudioOuput->count();
            ui->comboAudioOuput->addItem(hostName + " - " + tr("System default"), hostIdentifier + "|");
        }
        for (int j = 0; j < hosts[i].devices.count(); j++)
        {
            QString deviceName = hosts[i].devices[j].name;
            if (isSelectedHost && (deviceName == selectedDeviceName ||
                                   (selectedDeviceName.isEmpty() && !hosts[i].canFollowDefault && hosts[i].devices[j].isDefault)))
                selectedIndex = ui->comboAudioOuput->count();
            ui->comboAudioOuput->addItem(hostName + " - " + deviceName, hostIdentifier + "|" + deviceName);
        }
        if (isSelectedHost)
            selectedHostEnd = ui->comboAudioOuput->count();
    }

    // The configured device is still displayed if disconnected (the default device being used meanwhile)
    if (selectedIndex == -1 && !selectedDeviceName.isEmpty())
    {
        selectedIndex = (selectedHostEnd == -1 ? ui->comboAudioOuput->count() : selectedHostEnd);
        ui->comboAudioOuput->insertItem(selectedIndex, selectedHostName + " - " + selectedDeviceName + " (" + tr("disconnected") + ")",
                                        selectedHostIdentifier + "|" + selectedDeviceName);
    }
    ui->comboAudioOuput->setCurrentIndex(qMax(0, selectedIndex));

    int bufferSize = ContextManager::configuration()->getValue(ConfManager::SECTION_AUDIO, "buffer_size", 0).toInt();
    switch (bufferSize)
    {
    case 16: case 32: // No longer proposed, 64 is used (see AudioDevice)
    case 64: ui->comboBufferSize->setCurrentIndex(1); break;
    case 128: ui->comboBufferSize->setCurrentIndex(2); break;
    case 256: ui->comboBufferSize->setCurrentIndex(3); break;
    case 512: ui->comboBufferSize->setCurrentIndex(4); break;
    case 1024: ui->comboBufferSize->setCurrentIndex(5); break;
    case 2048: ui->comboBufferSize->setCurrentIndex(6); break;
    case 4096: ui->comboBufferSize->setCurrentIndex(7); break;
    case 0: default: ui->comboBufferSize->setCurrentIndex(0); break;
    }

    // Polyphony from 64 to 4096 (powers of 2), processor load from 20 to 100% (by 10%)
    int polyphony = ContextManager::configuration()->getValue(ConfManager::SECTION_AUDIO, "polyphony", DEFAULT_POLYPHONY).toInt();
    int polyphonyIndex = 0;
    while (polyphonyIndex < ui->comboPolyphony->count() - 1 && (64 << polyphonyIndex) < polyphony)
        polyphonyIndex++;
    ui->comboPolyphony->setCurrentIndex(polyphonyIndex);

    int cpuBudget = ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "cpu_budget", DEFAULT_CPU_BUDGET).toInt();
    ui->comboCpuBudget->setCurrentIndex(qBound(0, (cpuBudget + 5) / 10 - 2, ui->comboCpuBudget->count() - 1));

    ui->comboAudioOuput->blockSignals(false);
    ui->comboBufferSize->blockSignals(false);
    ui->comboPolyphony->blockSignals(false);
    ui->comboCpuBudget->blockSignals(false);
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
    case 1: bufferSize = 64; break;
    case 2: bufferSize = 128; break;
    case 3: bufferSize = 256; break;
    case 4: bufferSize = 512; break;
    case 5: bufferSize = 1024; break;
    case 6: bufferSize = 2048; break;
    case 7: bufferSize = 4096; break;
    case 0: default: break;
    }

    ContextManager::configuration()->setValue(ConfManager::SECTION_AUDIO, "buffer_size", bufferSize);
}

void ConfigSectionGeneral::on_comboPolyphony_currentIndexChanged(int index)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_AUDIO, "polyphony", 64 << index);
}

void ConfigSectionGeneral::on_comboCpuBudget_currentIndexChanged(int index)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "cpu_budget", 10 * (index + 2));
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

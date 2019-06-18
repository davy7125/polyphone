/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
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

    ui->checkRepercussionStereo->blockSignals(true);
    ui->checkRepercussionStereo->setChecked(ContextManager::configuration()->getValue(ConfManager::SECTION_NONE, "stereo_modification", false).toBool());
    ui->checkRepercussionStereo->blockSignals(false);
}

void ConfigSectionGeneral::initializeAudio()
{
    ui->comboAudioOuput->blockSignals(true);
    ui->comboBufferSize->blockSignals(true);

    QString audioTypeStr = ContextManager::configuration()->getValue(ConfManager::SECTION_AUDIO, "type", "0#0").toString();
    QStringList listStr = audioTypeStr.split("#");
    int audioType = listStr.size() >= 1 ? listStr[0].toInt() : 0;
    int audioIndex = listStr.size() >= 2 ? listStr[1].toInt() : 0;

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
    switch (bufferSize)
    {
    case 16: ui->comboBufferSize->setCurrentIndex(0); break;
    case 32: ui->comboBufferSize->setCurrentIndex(1); break;
    case 64: ui->comboBufferSize->setCurrentIndex(2); break;
    case 128: ui->comboBufferSize->setCurrentIndex(3); break;
    case 256: ui->comboBufferSize->setCurrentIndex(4); break;
    case 1024: ui->comboBufferSize->setCurrentIndex(6); break;
    case 512: default: ui->comboBufferSize->setCurrentIndex(5); break;
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
    int bufferSize = 16;
    for (int i = 0; i < index; i++)
        bufferSize *= 2;
    ContextManager::configuration()->setValue(ConfManager::SECTION_AUDIO, "buffer_size", bufferSize);
}

void ConfigSectionGeneral::initializeMidi()
{
    // Update the possible midi inputs
    ui->comboMidiInput->blockSignals(true);
    ui->comboMidiInput->clear();
    QStringList listMidi = ContextManager::midi()->getMidiList();
    ui->comboMidiInput->addItem("-");
    ui->comboMidiInput->addItems(listMidi);

    // Select the current one
    int numMidiPort = ContextManager::configuration()->getValue(ConfManager::SECTION_MIDI, "index_port", -1).toInt();
    if (numMidiPort + 1 < ui->comboMidiInput->count())
        ui->comboMidiInput->setCurrentIndex(numMidiPort + 1);
    else
        ui->comboMidiInput->setCurrentIndex(0); // None
    ui->comboMidiInput->blockSignals(false);
}

void ConfigSectionGeneral::on_comboMidiInput_currentIndexChanged(int index)
{
    int numPortMidi = index - 1;
    ContextManager::configuration()->setValue(ConfManager::SECTION_MIDI, "index_port", numPortMidi);
    ContextManager::midi()->openMidiPort(numPortMidi);
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

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

#include "midiplayerchannel.h"
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QApplication>
#include <QRegularExpression>
#include "midifileplayer.h"
#include "resettableslider.h"
#include "contextmanager.h"
#include "soundfontmanager.h"

MidiPlayerChannel::MidiPlayerChannel(MidiFilePlayer * player, int channel, QString name, QList<int> usedPrograms,
                                     QGridLayout * layout, int row) : QObject(),
    _player(player),
    _channel(channel),
    _usedPrograms(usedPrograms),
    _sf2Index(-1),
    _displayedProgram(-2),
    _noteCount(player->getNoteCount(channel)),
    _isLedOn(false)
{
    // Led with the colors of the controller area, and red warning
    QMap<QString, QString> replacement;
    replacement["border1"] = ContextManager::theme()->getColor(ThemeManager::BORDER).darker(130).name();
    replacement["border2"] = ContextManager::theme()->getColor(ThemeManager::BORDER).name();
    replacement["color1"] = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).darker(130).name();
    replacement["color2"] = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name();
    _ledOn = ContextManager::theme()->getColoredSvg(":/icons/led.svg", QSize(16, 16), replacement);
    replacement["color1"] = replacement["border1"];
    replacement["color2"] = replacement["border2"];
    _ledOff = ContextManager::theme()->getColoredSvg(":/icons/led.svg", QSize(16, 16), replacement);
    QMap<QString, QString> warningReplacement;
    warningReplacement["currentColor"] = ContextManager::theme()->getFixedColor(
                                                              ThemeManager::FixedColorType::RED, ThemeManager::WINDOW_BACKGROUND).name();
    _warning = ContextManager::theme()->getColoredSvg(":/icons/warning.svg", QSize(16, 16), warningReplacement);

    _labelLed = new QLabel();
    _labelLed->setPixmap(_ledOff);
    _labelLed->setToolTip(tr("Activity"));

    _labelChannel = new QLabel(QString::number(channel + 1));
    _labelChannel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    _labelName = new QLabel();
    _labelName->setFixedWidth(110);
    _labelName->setText(_labelName->fontMetrics().elidedText(name, Qt::ElideRight, _labelName->width()));
    _labelName->setToolTip(name);

    _comboSoundfont = new QComboBox();
    _comboSoundfont->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    _comboSoundfont->setMinimumContentsLength(16);

    _checkForced = new QCheckBox();
    _checkForced->setToolTip(tr("Force a bank and a preset, otherwise they are chosen by the MIDI file"));

    _comboPreset = new QComboBox();
    _comboPreset->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    _comboPreset->setMinimumContentsLength(22);
    _comboPreset->setEnabled(false);

    _labelWarning = new QLabel();
    _labelWarning->setFixedSize(16, 16);

    _sliderVolume = new ResettableSlider();
    _sliderVolume->setOrientation(Qt::Horizontal);
    _sliderVolume->setRange(-40, 10);
    _sliderVolume->setValue(0);
    _sliderVolume->setFixedWidth(100);
    _sliderVolume->setToolTip(tr("Volume"));

    _labelVolume = new QLabel();
    _labelVolume->setMinimumWidth(50);
    _labelVolume->setAlignment(Qt::AlignCenter);
    onVolumeChanged(0);

    _pushMute = new QPushButton(tr("M", "short for Mute"));
    _pushMute->setCheckable(true);
    _pushMute->setFixedWidth(28);
    _pushMute->setFocusPolicy(Qt::NoFocus);
    _pushMute->setToolTip(tr("Mute"));

    _pushSolo = new QPushButton(tr("S", "short for Solo"));
    _pushSolo->setCheckable(true);
    _pushSolo->setFixedWidth(28);
    _pushSolo->setFocusPolicy(Qt::NoFocus);
    _pushSolo->setToolTip(tr("Solo"));

    layout->addWidget(_labelLed, row, 0);
    layout->addWidget(_labelChannel, row, 1);
    layout->addWidget(_labelName, row, 2);
    layout->addWidget(_comboSoundfont, row, 3);
    layout->addWidget(_checkForced, row, 4);
    layout->addWidget(_comboPreset, row, 5);
    layout->addWidget(_labelWarning, row, 6);
    layout->addWidget(_sliderVolume, row, 7);
    layout->addWidget(_labelVolume, row, 8);
    layout->addWidget(_pushMute, row, 9);
    layout->addWidget(_pushSolo, row, 10);

    updateSoundfontList();
    connect(_comboSoundfont, SIGNAL(currentIndexChanged(int)), this, SLOT(onSoundfontChanged(int)));
    connect(_checkForced, SIGNAL(toggled(bool)), this, SLOT(onForcedToggled(bool)));
    connect(_comboPreset, SIGNAL(currentIndexChanged(int)), this, SLOT(onPresetChanged(int)));
    connect(_sliderVolume, SIGNAL(valueChanged(int)), this, SLOT(onVolumeChanged(int)));
    connect(_pushMute, SIGNAL(toggled(bool)), this, SIGNAL(mixChanged()));
    connect(_pushSolo, SIGNAL(toggled(bool)), this, SIGNAL(mixChanged()));
}

MidiPlayerChannel::~MidiPlayerChannel()
{
    delete _labelLed;
    delete _labelChannel;
    delete _labelName;
    delete _comboSoundfont;
    delete _checkForced;
    delete _comboPreset;
    delete _labelWarning;
    delete _sliderVolume;
    delete _labelVolume;
    delete _pushMute;
    delete _pushSolo;
}

void MidiPlayerChannel::updateSoundfontList()
{
    // "None" and the soundfonts that are open, named as their tab
    SoundfontManager * sm = SoundfontManager::getInstance();
    static QRegularExpression reg("(/|\\\\)");
    _comboSoundfont->blockSignals(true);
    _comboSoundfont->clear();
    _comboSoundfont->addItem(tr("None"), -1);
    EltID idSf2(elementSf2);
    foreach (int sf2Index, sm->getSiblings(idSf2))
    {
        idSf2.indexSf2 = sf2Index;
        QString name = sm->getQstr(idSf2, champ_name);
        if (name.isEmpty())
            name = sm->getQstr(idSf2, champ_filenameInitial).split(reg).last();
        if (name.isEmpty())
            name = tr("Untitled");
        _comboSoundfont->addItem(name, sf2Index);
    }
    int index = _comboSoundfont->findData(_sf2Index);
    _comboSoundfont->setCurrentIndex(index < 0 ? 0 : index);
    _comboSoundfont->blockSignals(false);

    // The soundfont has been closed
    if (index < 0)
        applySoundfont(-1);
}

void MidiPlayerChannel::setSoundfont(int sf2Index)
{
    int index = _comboSoundfont->findData(sf2Index);
    _comboSoundfont->blockSignals(true);
    _comboSoundfont->setCurrentIndex(index < 0 ? 0 : index);
    _comboSoundfont->blockSignals(false);
    applySoundfont(index < 0 ? -1 : sf2Index);
}

void MidiPlayerChannel::onSoundfontChanged(int index)
{
    applySoundfont(_comboSoundfont->itemData(index).toInt());
}

void MidiPlayerChannel::applySoundfont(int sf2Index)
{
    _sf2Index = sf2Index;
    _player->setSoundfont(_channel, sf2Index);
    _checkForced->setEnabled(sf2Index >= 0);
    _comboPreset->setEnabled(sf2Index >= 0 && _checkForced->isChecked());
    updatePresetList();
    loadSamples();
}

void MidiPlayerChannel::updatePresetList()
{
    // Presets of the soundfont, by bank and preset numbers
    QList<QPair<int, QString> > presets;
    if (_sf2Index >= 0)
    {
        SoundfontManager * sm = SoundfontManager::getInstance();
        EltID idPrst(elementPrst, _sf2Index);
        foreach (int presetIndex, sm->getSiblings(idPrst))
        {
            idPrst.indexElt = presetIndex;
            presets << QPair<int, QString>(sm->get(idPrst, champ_wBank).wValue * 128 + sm->get(idPrst, champ_wPreset).wValue,
                                           sm->getQstr(idPrst, champ_name));
        }
        std::sort(presets.begin(), presets.end(), [](const QPair<int, QString> &a, const QPair<int, QString> &b) {
            return a.first < b.first;
        });
    }

    // The forced program is kept if possible, otherwise it is the first one
    bool isForced = _checkForced->isChecked();
    int previousProgram = getForcedProgram();
    _comboPreset->blockSignals(true);
    _comboPreset->clear();
    for (int i = 0; i < presets.count(); i++)
        _comboPreset->addItem(getProgramText(presets[i].first) + " " + presets[i].second, presets[i].first);
    if (isForced)
    {
        int index = _comboPreset->findData(previousProgram);
        _comboPreset->setCurrentIndex(index < 0 ? 0 : index);
    }
    _comboPreset->blockSignals(false);

    if (isForced)
    {
        int program = getForcedProgram();
        _player->setForcedProgram(_channel, true, program);
        if (program != previousProgram)
            loadSamples();
    }
    else
        displayFileProgram(_player->getFileProgram(_channel));
}

void MidiPlayerChannel::onForcedToggled(bool isChecked)
{
    _comboPreset->setEnabled(isChecked && _sf2Index >= 0);
    if (isChecked)
    {
        // Program of the file if the soundfont has it, otherwise the first one
        _comboPreset->blockSignals(true);
        if (_comboPreset->count() > 0 && _comboPreset->itemData(0, Qt::UserRole + 1).toBool())
            _comboPreset->removeItem(0);
        int index = _comboPreset->findData(_player->getFileProgram(_channel));
        _comboPreset->setCurrentIndex(index < 0 ? 0 : index);
        _comboPreset->blockSignals(false);
        _labelWarning->setPixmap(QPixmap());
        _labelWarning->setToolTip("");
        _displayedProgram = -2;

        _player->setForcedProgram(_channel, true, getForcedProgram());
        loadSamples();
    }
    else
    {
        _player->setForcedProgram(_channel, false, -1);
        displayFileProgram(_player->getFileProgram(_channel));
    }
}

void MidiPlayerChannel::onPresetChanged(int index)
{
    Q_UNUSED(index)
    if (!_checkForced->isChecked())
        return;
    _player->setForcedProgram(_channel, true, getForcedProgram());
    loadSamples();
}

int MidiPlayerChannel::getForcedProgram()
{
    if (!_checkForced->isChecked() || _comboPreset->currentIndex() < 0)
        return -1;
    return _comboPreset->currentData().toInt();
}

void MidiPlayerChannel::displayFileProgram(int program)
{
    // A program that the soundfont does not have is displayed by a first item, with a warning
    _displayedProgram = program;
    _comboPreset->blockSignals(true);
    if (_comboPreset->count() > 0 && _comboPreset->itemData(0, Qt::UserRole + 1).toBool())
        _comboPreset->removeItem(0);
    int index = _comboPreset->findData(program);
    bool isMissing = (index < 0);
    if (isMissing)
    {
        _comboPreset->insertItem(0, getProgramText(program), program);
        _comboPreset->setItemData(0, true, Qt::UserRole + 1);
        index = 0;
    }
    _comboPreset->setCurrentIndex(index);
    _comboPreset->blockSignals(false);

    isMissing &= (_sf2Index >= 0);
    _labelWarning->setPixmap(isMissing ? _warning : QPixmap());
    _labelWarning->setToolTip(isMissing ? tr("The soundfont has no preset with this bank and this number: the notes are not played.") : "");
}

void MidiPlayerChannel::updateState()
{
    // Led lit for a moment when notes start
    quint32 noteCount = _player->getNoteCount(_channel);
    if (noteCount != _noteCount)
    {
        _noteCount = noteCount;
        _ledTimer.start();
    }
    bool isLedOn = _ledTimer.isValid() && _ledTimer.elapsed() < 100;
    if (isLedOn != _isLedOn)
    {
        _isLedOn = isLedOn;
        _labelLed->setPixmap(isLedOn ? _ledOn : _ledOff);
    }

    // Program chosen by the file
    if (!_checkForced->isChecked())
    {
        int program = _player->getFileProgram(_channel);
        if (program != _displayedProgram)
            displayFileProgram(program);
    }
}

void MidiPlayerChannel::onVolumeChanged(int value)
{
    _labelVolume->setText((value >= 0 ? "+" : "") + QString::number(value) + " " + tr("dB", "unit for decibels"));
    emit mixChanged();
}

bool MidiPlayerChannel::isSolo()
{
    return _pushSolo->isChecked();
}

void MidiPlayerChannel::updateGain(bool isSoloEnabled)
{
    bool isSilent = _pushMute->isChecked() || (isSoloEnabled && !_pushSolo->isChecked());
    _player->setGain(_channel, isSilent ? -1000.0f : static_cast<float>(_sliderVolume->value()));
}

void MidiPlayerChannel::loadSamples()
{
    // Samples of the presets that can be played, loaded now rather than by the thread of the player
    if (_sf2Index < 0)
        return;
    QList<int> programs;
    if (_checkForced->isChecked())
        programs << getForcedProgram();
    else
        programs = _usedPrograms;

    QApplication::setOverrideCursor(Qt::WaitCursor);
    SoundfontManager * sm = SoundfontManager::getInstance();
    QMutexLocker locker(sm->getMutex()); // The synth might load them at the same time
    EltID idPrst(elementPrst, _sf2Index);
    EltID idPrstInst(elementPrstInst, _sf2Index);
    EltID idInstSmpl(elementInstSmpl, _sf2Index);
    EltID idSmpl(elementSmpl, _sf2Index);
    foreach (int presetIndex, sm->getSiblings(idPrst))
    {
        idPrst.indexElt = presetIndex;
        if (!programs.contains(sm->get(idPrst, champ_wBank).wValue * 128 + sm->get(idPrst, champ_wPreset).wValue))
            continue;

        idPrstInst.indexElt = presetIndex;
        foreach (int i, sm->getSiblings(idPrstInst))
        {
            idPrstInst.indexElt2 = i;
            idInstSmpl.indexElt = sm->get(idPrstInst, champ_instrument).wValue;
            foreach (int j, sm->getSiblings(idInstSmpl))
            {
                idInstSmpl.indexElt2 = j;
                idSmpl.indexElt = sm->get(idInstSmpl, champ_sampleID).wValue;
                Sound * sound = sm->getSound(idSmpl);
                if (sound != nullptr)
                    sound->loadSample();
            }
        }
    }
    QApplication::restoreOverrideCursor();
}

QString MidiPlayerChannel::getProgramText(int program)
{
    return QString("%1:%2").arg(program / 128, 3, 10, QChar('0')).arg(program % 128, 3, 10, QChar('0'));
}

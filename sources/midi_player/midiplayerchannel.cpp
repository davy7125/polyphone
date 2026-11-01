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
#include <QTableWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QToolButton>
#include <QMenu>
#include <QWidgetAction>
#include <QApplication>
#include <QRegularExpression>
#include "midifileplayer.h"
#include "midiplayerdelegate.h"
#include "resettableslider.h"
#include "modulatorcombosrc.h"
#include "contextmanager.h"
#include "soundfontmanager.h"

MidiPlayerChannel::MidiPlayerChannel(MidiFilePlayer * player, int channel, QString name, QList<int> usedPrograms,
                                     QTableWidget * table, int row) : QObject(),
    _player(player),
    _channel(channel),
    _usedPrograms(usedPrograms),
    _sf2Index(-1),
    _forcedProgram(-1),
    _displayedProgram(-2),
    _noteCount(player->getNoteCount(channel)),
    _isLedOn(false)
{
    // Led with the colors of the controller area, and red warning
    ThemeManager * theme = ContextManager::theme();
    QMap<QString, QString> replacement;
    replacement["border1"] = theme->getColor(ThemeManager::BORDER).darker(130).name();
    replacement["border2"] = theme->getColor(ThemeManager::BORDER).name();
    replacement["color1"] = theme->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).darker(130).name();
    replacement["color2"] = theme->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name();
    _ledOn = theme->getColoredSvg(":/icons/led.svg", QSize(16, 16), replacement);
    replacement["color1"] = replacement["border1"];
    replacement["color2"] = replacement["border2"];
    _ledOff = theme->getColoredSvg(":/icons/led.svg", QSize(16, 16), replacement);
    QMap<QString, QString> warningReplacement;
    warningReplacement["currentColor"] = theme->getFixedColor(ThemeManager::FixedColorType::RED, ThemeManager::LIST_BACKGROUND).name();
    _warning = theme->getColoredSvg(":/icons/warning.svg", QSize(16, 16), warningReplacement);

    // Channel: number (with the name of its track as tooltip), then mute, solo and the MIDI values, flat and separated by
    // vertical lines as the buttons of the status bar of the virtual keyboard
    QColor text = theme->getColor(ThemeManager::LIST_TEXT);
    QColor highlighted = theme->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);
    QColor dimmed = ThemeManager::mix(theme->getColor(ThemeManager::LIST_BACKGROUND), text, 0.45);
    QColor hover = ThemeManager::mix(theme->getColor(ThemeManager::LIST_BACKGROUND), highlighted, 0.2);
    QString buttonStyle = "QPushButton,QToolButton{border:0;border-left:1px solid " + theme->getColor(ThemeManager::BORDER).name() +
                          ";background:transparent;padding:0;color:" + dimmed.name() + "}" +
                          "QPushButton:checked{color:" + highlighted.name() + ";font-weight:bold}" +
                          "QPushButton:hover,QToolButton:hover,QToolButton:pressed{background-color:" + hover.name() + "}" +
                          "QToolButton::menu-indicator{width:0px;image:url(:/misc/transparent.png)}";
    QWidget * widgetChannel = new QWidget();
    QHBoxLayout * layoutChannel = new QHBoxLayout(widgetChannel);
    layoutChannel->setContentsMargins(6, 0, 0, 0);
    layoutChannel->setSpacing(0);
    QLabel * labelNumber = new QLabel(QString::number(channel + 1));
    labelNumber->setMinimumWidth(30);
    labelNumber->setToolTip(name);
    layoutChannel->addWidget(labelNumber);

    _pushMute = new QPushButton(tr("M", "short for Mute"));
    _pushMute->setToolTip(tr("Mute"));
    _pushMute->setCursor(Qt::PointingHandCursor);
    _pushSolo = new QPushButton(tr("S", "short for Solo"));
    _pushSolo->setToolTip(tr("Solo"));
    _pushSolo->setCursor(Qt::PointingHandCursor);
    foreach (QPushButton * button, QList<QPushButton *>() << _pushMute << _pushSolo)
    {
        button->setCheckable(true);
        button->setFocusPolicy(Qt::NoFocus);
        button->setFixedWidth(30);
        button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        button->setStyleSheet(buttonStyle);
        layoutChannel->addWidget(button);
    }

    // Current MIDI values, in a menu as the information of the soundfont page
    QToolButton * toolButtonMidi = new QToolButton();
    toolButtonMidi->setIcon(theme->getColoredSvg(":/icons/MIDI.svg", QSize(16, 16), ThemeManager::HIGHLIGHTED_BACKGROUND));
    toolButtonMidi->setToolTip(tr("MIDI values"));
    toolButtonMidi->setFocusPolicy(Qt::NoFocus);
    toolButtonMidi->setFixedWidth(30);
    toolButtonMidi->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    toolButtonMidi->setStyleSheet(buttonStyle);
    toolButtonMidi->setPopupMode(QToolButton::InstantPopup);
    toolButtonMidi->setCursor(Qt::PointingHandCursor);
    _menuMidi = new QMenu(toolButtonMidi);
    _menuMidi->setStyleSheet(theme->getMenuTheme());
    _labelMidi = new QLabel();
    _labelMidi->setStyleSheet("QLabel{padding:10px 15px;color:" + text.name() + "}");
    QWidgetAction * actionMidi = new QWidgetAction(_menuMidi);
    actionMidi->setDefaultWidget(_labelMidi);
    _menuMidi->addAction(actionMidi);
    toolButtonMidi->setMenu(_menuMidi);
    layoutChannel->addWidget(toolButtonMidi);
    connect(_menuMidi, SIGNAL(aboutToShow()), this, SLOT(updateMidiValues()));

    // Soundfont and preset, edited by MidiPlayerDelegate
    _itemSoundfont = new QTableWidgetItem();
    _itemPreset = new QTableWidgetItem();

    // Volume and activity
    QWidget * widgetVolume = new QWidget();
    QHBoxLayout * layoutVolume = new QHBoxLayout(widgetVolume);
    layoutVolume->setContentsMargins(6, 0, 6, 0);
    layoutVolume->setSpacing(6);
    _labelLed = new QLabel();
    _labelLed->setPixmap(_ledOff);
    _labelLed->setToolTip(tr("Activity"));
    _sliderVolume = new ResettableSlider();
    _sliderVolume->setOrientation(Qt::Horizontal);
    _sliderVolume->setRange(-40, 10);
    _sliderVolume->setValue(0);
    _sliderVolume->setFixedWidth(100);
    _sliderVolume->setFocusPolicy(Qt::NoFocus);
    _sliderVolume->setToolTip(tr("Volume"));
    _labelVolume = new QLabel();
    _labelVolume->setMinimumWidth(45);
    _labelVolume->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    onVolumeChanged(0);
    layoutVolume->addWidget(_labelLed);
    layoutVolume->addWidget(_sliderVolume);
    layoutVolume->addWidget(_labelVolume);

    // The cells with widgets are not edited
    foreach (int column, QList<int>() << MidiPlayerDelegate::COLUMN_CHANNEL << MidiPlayerDelegate::COLUMN_VOLUME)
    {
        QTableWidgetItem * item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsEnabled);
        table->setItem(row, column, item);
    }
    table->setCellWidget(row, MidiPlayerDelegate::COLUMN_CHANNEL, widgetChannel);
    table->setItem(row, MidiPlayerDelegate::COLUMN_SOUNDFONT, _itemSoundfont);
    table->setItem(row, MidiPlayerDelegate::COLUMN_PRESET, _itemPreset);
    table->setCellWidget(row, MidiPlayerDelegate::COLUMN_VOLUME, widgetVolume);

    connect(_sliderVolume, SIGNAL(valueChanged(int)), this, SLOT(onVolumeChanged(int)));
    connect(_pushMute, SIGNAL(toggled(bool)), this, SIGNAL(mixChanged()));
    connect(_pushSolo, SIGNAL(toggled(bool)), this, SIGNAL(mixChanged()));
}

QList<QPair<QString, int> > MidiPlayerChannel::getSoundfontChoices()
{
    // "None" and the soundfonts that are open, named as their tab
    QList<QPair<QString, int> > choices;
    choices << QPair<QString, int>(tr("None"), -1);
    SoundfontManager * sm = SoundfontManager::getInstance();
    static QRegularExpression reg("(/|\\\\)");
    EltID idSf2(elementSf2);
    foreach (int sf2Index, sm->getSiblings(idSf2))
    {
        // Not the soundfonts loaded without tab by the soundfont explorer
        if (sm->isTemporary(sf2Index))
            continue;

        idSf2.indexSf2 = sf2Index;
        QString name = sm->getQstr(idSf2, champ_name);
        if (name.isEmpty())
            name = sm->getQstr(idSf2, champ_filenameInitial).split(reg).last();
        if (name.isEmpty())
            name = tr("Untitled");
        choices << QPair<QString, int>(name, sf2Index);
    }
    return choices;
}

void MidiPlayerChannel::setSoundfont(int sf2Index)
{
    // A soundfont that is not open is "None"
    QList<QPair<QString, int> > choices = getSoundfontChoices();
    QString name = choices.first().first;
    bool isFound = false;
    for (int i = 0; i < choices.count() && !isFound; i++)
    {
        if (choices[i].second == sf2Index)
        {
            name = choices[i].first;
            isFound = true;
        }
    }
    _sf2Index = isFound ? sf2Index : -1;
    _itemSoundfont->setText(name);

    _player->setSoundfont(_channel, _sf2Index);
    updatePresetList();
    loadSamples();
}

void MidiPlayerChannel::updateSoundfont()
{
    // Possibly a new name, or the soundfont has been closed
    QList<QPair<QString, int> > choices = getSoundfontChoices();
    for (int i = 0; i < choices.count(); i++)
    {
        if (choices[i].second == _sf2Index)
        {
            _itemSoundfont->setText(choices[i].first);
            return;
        }
    }
    setSoundfont(-1);
}

void MidiPlayerChannel::updatePresetList()
{
    // Presets of the soundfont, by bank and preset numbers
    _presets.clear();
    if (_sf2Index >= 0)
    {
        SoundfontManager * sm = SoundfontManager::getInstance();
        EltID idPrst(elementPrst, _sf2Index);
        foreach (int presetIndex, sm->getSiblings(idPrst))
        {
            idPrst.indexElt = presetIndex;
            _presets << QPair<int, QString>(sm->get(idPrst, champ_wBank).wValue * 128 + sm->get(idPrst, champ_wPreset).wValue,
                                            sm->getQstr(idPrst, champ_name));
        }
        std::sort(_presets.begin(), _presets.end(), [](const QPair<int, QString> &a, const QPair<int, QString> &b) {
            return a.first < b.first;
        });
    }

    // A forced program that the soundfont does not have is replaced by its first preset (by "auto" without preset)
    if (_forcedProgram >= 0 && !hasPreset(_forcedProgram))
    {
        setForcedProgram(_presets.isEmpty() ? -1 : _presets.first().first);
        return;
    }
    updatePresetText();
}

QList<QPair<QString, int> > MidiPlayerChannel::getPresetChoices()
{
    // "Auto" with the preset chosen by the file, then the presets of the soundfont
    QList<QPair<QString, int> > choices;
    choices << QPair<QString, int>(tr("Auto (%1)").arg(getPresetText(_player->getFileProgram(_channel))), -1);
    for (int i = 0; i < _presets.count(); i++)
        choices << QPair<QString, int>(getProgramText(_presets[i].first) + " " + _presets[i].second, _presets[i].first);
    return choices;
}

void MidiPlayerChannel::setForcedProgram(int program)
{
    _forcedProgram = program;
    _player->setForcedProgram(_channel, program >= 0, program);
    updatePresetText();
    loadSamples();
}

void MidiPlayerChannel::updatePresetText()
{
    // Forced preset, or "auto" with the preset chosen by the file and a warning if the soundfont does not have it
    bool isMissing = false;
    if (_forcedProgram >= 0)
        _itemPreset->setText(getPresetText(_forcedProgram));
    else
    {
        _displayedProgram = _player->getFileProgram(_channel);
        _itemPreset->setText(tr("Auto (%1)").arg(getPresetText(_displayedProgram)));
        isMissing = _sf2Index >= 0 && !hasPreset(_displayedProgram);
    }
    _itemPreset->setIcon(isMissing ? QIcon(_warning) : QIcon());
    _itemPreset->setToolTip(isMissing ? tr("The soundfont has no preset with this bank and this number: the notes are not played.") : "");
}

QString MidiPlayerChannel::getPresetText(int program)
{
    for (int i = 0; i < _presets.count(); i++)
        if (_presets[i].first == program)
            return getProgramText(program) + " " + _presets[i].second;
    return getProgramText(program);
}

bool MidiPlayerChannel::hasPreset(int program)
{
    for (int i = 0; i < _presets.count(); i++)
        if (_presets[i].first == program)
            return true;
    return false;
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
    if (_forcedProgram < 0 && _player->getFileProgram(_channel) != _displayedProgram)
        updatePresetText();

    // MIDI values, if they are displayed
    if (_menuMidi->isVisible())
        updateMidiValues();
}

void MidiPlayerChannel::updateMidiValues()
{
    // Controllers that are not at their default value, except the bank (displayed with the preset), the parameter
    // numbers with their data (RPN, NRPN) and the channel mode messages
    MidiProcessor * states = _player->getChannelStates();
    QString labelStyle = "padding-right:15px;color:" + ThemeManager::mix(
                             ContextManager::theme()->getColor(ThemeManager::LIST_TEXT),
                             ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND), 0.4).name();
    QString rowPattern = "<tr><td style=\"" + labelStyle + "\">%1</td><td align=\"right\">%2</td></tr>";
    QString rows;
    for (int number = 1; number < 120; number++)
    {
        if (number == 6 || number == 32 || number == 38 || (number >= 96 && number <= 101))
            continue;
        int value = states->getControllerValue(_channel, number);
        if (value != states->getDefaultControllerValue(_channel, number))
            rows += rowPattern.arg(ModulatorComboSrc::getIndexName(number, true), QString::number(value));
    }

    // Pitch wheel in semitones, its sensitivity and the channel pressure
    float sensitivity = states->getBendSensitivityValue(_channel);
    float bend = states->getBendValue(_channel);
    if (bend != 0.0f)
        rows += rowPattern.arg(ModulatorComboSrc::getIndexName(14, false),
                               (bend > 0 ? "+" : "") + QLocale().toString(bend * sensitivity, 'f', 2) + " " + tr("semitones"));
    if (sensitivity != 2.0f)
        rows += rowPattern.arg(ModulatorComboSrc::getIndexName(16, false),
                               QLocale().toString(sensitivity, 'g', 4) + " " + tr("semitones"));
    int pressure = states->getMonoPressure(_channel);
    if (pressure != 0)
        rows += rowPattern.arg(ModulatorComboSrc::getIndexName(13, false), QString::number(pressure));

    QString text = rows.isEmpty() ? tr("All controllers are at their default value.") : "<table>" + rows + "</table>";
    if (text != _labelMidi->text())
    {
        _labelMidi->setText(text);
        _menuMidi->adjustSize();
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

float MidiPlayerChannel::getGain(bool isSoloEnabled)
{
    bool isSilent = _pushMute->isChecked() || (isSoloEnabled && !_pushSolo->isChecked());
    return isSilent ? -1000.0f : static_cast<float>(_sliderVolume->value());
}

void MidiPlayerChannel::updateGain(bool isSoloEnabled)
{
    _player->setGain(_channel, getGain(isSoloEnabled));
}

void MidiPlayerChannel::loadSamples()
{
    // Samples of the presets that can be played, loaded now rather than by the thread of the player
    if (_sf2Index < 0)
        return;
    QList<int> programs;
    if (_forcedProgram >= 0)
        programs << _forcedProgram;
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

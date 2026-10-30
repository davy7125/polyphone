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

#include "dialogmidiplayer.h"
#include "ui_dialogmidiplayer.h"
#include <QTimer>
#include <QFileInfo>
#include <QFileDialog>
#include <QKeyEvent>
#include "contextmanager.h"
#include "soundfontmanager.h"
#include "soundfonts.h"
#include "tabmanager.h"
#include "synth.h"
#include "midifile.h"
#include "midifileplayer.h"
#include "midiplayerchannel.h"
#include "editortoolbar.h"
#include "player.h"

DialogMidiPlayer::DialogMidiPlayer(QWidget *parent) :
    QDialog(parent, Qt::Tool | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint),
    ui(new Ui::DialogMidiPlayer),
    _stateUpdate(true),
    _isPlaying(false),
    _updatingSlider(false)
{
    ui->setupUi(this);

    // Player using the synth of the editor, its channels having their own controller values
    SoundfontManager * sm = SoundfontManager::getInstance();
    Synth * synth = ContextManager::audio()->getSynth();
    _player = new MidiFilePlayer(synth, sm->getSoundfonts(), sm->getMutex());
    synth->setMidiFileValues(_player->getMidiValues());

    // Icons, the loop being lit when it is enabled
    ui->pushOpen->setIcon(ContextManager::theme()->getColoredSvg(":/icons/folder.svg", QSize(28, 28), ThemeManager::WINDOW_TEXT));
    ui->pushStop->setIcon(ContextManager::theme()->getColoredSvg(":/icons/recorder_stop.svg", QSize(32, 32), ThemeManager::WINDOW_TEXT));
    QIcon loopIcon;
    loopIcon.addPixmap(ContextManager::theme()->getColoredSvg(":/icons/reload.svg", QSize(24, 24), ThemeManager::HIGHLIGHTED_BACKGROUND),
                       QIcon::Normal, QIcon::On);
    QMap<QString, QString> replacement;
    replacement["currentColor"] = ThemeManager::mix(ContextManager::theme()->getColor(ThemeManager::WINDOW_BACKGROUND),
                                                    ContextManager::theme()->getColor(ThemeManager::WINDOW_TEXT), 0.4).name();
    loopIcon.addPixmap(ContextManager::theme()->getColoredSvg(":/icons/reload.svg", QSize(24, 24), replacement), QIcon::Normal, QIcon::Off);
    ui->pushLoop->setIcon(loopIcon);
    updatePlayPauseIcon(false);

    // Titles of the channel columns
    QStringList titles;
    titles << tr("Channel") << tr("Soundfont") << tr("Preset") << tr("Volume");
    QList<int> columns, spans;
    columns << 1 << 3 << 4 << 7;
    spans << 2 << 1 << 3 << 2;
    for (int i = 0; i < titles.count(); i++)
    {
        QLabel * label = new QLabel(titles[i]);
        QFont font = label->font();
        font.setBold(true);
        label->setFont(font);
        ui->gridChannels->addWidget(label, 0, columns[i], 1, spans[i]);
    }
    ui->gridChannels->setColumnStretch(5, 1);

    // No file at the beginning
    ui->labelFile->setText("-");
    ui->labelMessage->setText(tr("Open a MIDI file to play it with the soundfonts that are open."));
    ui->scrollArea->hide();
    ui->pushPlayPause->setEnabled(false);
    ui->pushStop->setEnabled(false);
    ui->sliderPosition->setEnabled(false);

    // Periodic update of the position, the activity and the programs
    _timer = new QTimer(this);
    connect(_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));

    connect(sm, SIGNAL(soundfontLoaded(int)), this, SLOT(onSoundfontLoaded(int)));
    connect(sm, SIGNAL(soundfontClosed(int)), this, SLOT(onSoundfontClosed(int)));
    connect(sm, SIGNAL(editingDone(QString,QList<int>)), this, SLOT(onEditingDone(QString,QList<int>)));
    connect(sm, SIGNAL(inputModulatorChanged(int,bool,bool)), this, SLOT(onInputModulatorChanged(int,bool,bool)));
}

DialogMidiPlayer::~DialogMidiPlayer()
{
    clearChannels();
    ContextManager::audio()->getSynth()->setMidiFileValues(nullptr);
    delete _player;
    delete ui;
}

void DialogMidiPlayer::on_pushOpen_clicked()
{
    // In the folder of the last file
    QString lastFile = ContextManager::recentFile()->getLastFile(RecentFileManager::FILE_TYPE_MIDI);
    QString directory = ContextManager::recentFile()->getLastDirectory(RecentFileManager::FILE_TYPE_MIDI);
    QString filePath = QFileDialog::getOpenFileName(
        this, tr("Open a MIDI file"), QFileInfo::exists(lastFile) ? lastFile : directory,
        tr("MIDI files") + " (*.mid *.midi *.kar *.rmi *.MID *.MIDI *.KAR *.RMI)");
    if (!filePath.isEmpty())
        loadFile(filePath);
}

void DialogMidiPlayer::loadFile(QString filePath)
{
    clearChannels();
    _player->setFile(nullptr);
    _file.reset();
    updatePlayPauseIcon(false);
    ui->labelFile->setText(QFileInfo(filePath).fileName());
    ui->labelFile->setToolTip(filePath);

    std::shared_ptr<MidiFile> file(new MidiFile());
    bool isValid = file->read(filePath);
    ui->pushPlayPause->setEnabled(isValid);
    ui->pushStop->setEnabled(isValid);
    ui->sliderPosition->setEnabled(isValid);
    _updatingSlider = true;
    ui->sliderPosition->setRange(0, isValid ? static_cast<int>(file->getDuration() / 1000) : 0);
    ui->sliderPosition->setValue(0);
    _updatingSlider = false;
    if (!isValid)
    {
        ui->labelMessage->setText(file->getError());
        ui->labelMessage->show();
        ui->scrollArea->hide();
        ui->labelTime->setText(getTimeText(0) + " / " + getTimeText(0));
        return;
    }
    ui->labelMessage->hide();
    ui->scrollArea->show();
    ContextManager::recentFile()->addRecentFile(RecentFileManager::FILE_TYPE_MIDI, filePath);
    _file = file;
    _player->setFile(file);
    updatePosition(0);

    // The channels play the soundfont of the current tab, otherwise the first one open
    TabManager * tabManager = TabManager::getInstance();
    int sf2Index = tabManager == nullptr ? -1 : tabManager->getCurrentSf2();
    if (sf2Index < 0)
    {
        EltID idSf2(elementSf2);
        QList<int> sf2Indexes = SoundfontManager::getInstance()->getSiblings(idSf2);
        if (!sf2Indexes.isEmpty())
            sf2Index = sf2Indexes.first();
    }

    // A row for each channel having notes
    int row = 1;
    for (int channel = 0; channel < 16; channel++)
    {
        if (!file->isChannelUsed(channel))
            continue;
        MidiPlayerChannel * playerChannel = new MidiPlayerChannel(
            _player, channel, file->getChannelName(channel), MidiFilePlayer::getUsedPrograms(file.get(), channel),
            ui->gridChannels, row++);
        connect(playerChannel, SIGNAL(mixChanged()), this, SLOT(onMixChanged()));
        playerChannel->setSoundfont(sf2Index);
        _channels << playerChannel;
    }
}

void DialogMidiPlayer::clearChannels()
{
    while (!_channels.isEmpty())
        delete _channels.takeFirst();

    // Default configuration of all channels
    for (int channel = 0; channel < 16; channel++)
    {
        _player->setSoundfont(channel, -1);
        _player->setForcedProgram(channel, false, -1);
        _player->setGain(channel, 0);
    }
}

void DialogMidiPlayer::on_pushPlayPause_clicked()
{
    if (_file == nullptr)
        return;
    if (_player->isPlaying())
    {
        _player->pause();
        updatePlayPauseIcon(false);
    }
    else
    {
        _player->play();
        updatePlayPauseIcon(true);
    }
}

void DialogMidiPlayer::on_pushStop_clicked()
{
    _player->stop();
    updatePlayPauseIcon(false);
    updatePosition(0);
}

void DialogMidiPlayer::on_pushLoop_toggled(bool checked)
{
    _player->setLoop(checked);
}

void DialogMidiPlayer::on_sliderPosition_valueChanged(int value)
{
    if (_updatingSlider)
        return;

    // The position changes when the slider is released, or directly with a click on the groove
    ui->labelTime->setText(getTimeText(1000LL * value) + " / " + getTimeText(_file == nullptr ? 0 : _file->getDuration()));
    if (!ui->sliderPosition->isSliderDown())
        _player->setPosition(1000LL * value);
}

void DialogMidiPlayer::on_sliderPosition_sliderReleased()
{
    _player->setPosition(1000LL * ui->sliderPosition->value());
}

void DialogMidiPlayer::onTimeout()
{
    if (_file == nullptr)
        return;

    bool isPlaying = _player->isPlaying();
    if (isPlaying != _isPlaying)
        updatePlayPauseIcon(isPlaying);
    if (!ui->sliderPosition->isSliderDown())
        updatePosition(_player->getPosition());
    foreach (MidiPlayerChannel * channel, _channels)
        channel->updateState();
}

void DialogMidiPlayer::updatePosition(qint64 position)
{
    _updatingSlider = true;
    ui->sliderPosition->setValue(static_cast<int>(position / 1000));
    _updatingSlider = false;
    ui->labelTime->setText(getTimeText(position) + " / " + getTimeText(_file == nullptr ? 0 : _file->getDuration()));
}

void DialogMidiPlayer::updatePlayPauseIcon(bool isPlaying)
{
    _isPlaying = isPlaying;
    ui->pushPlayPause->setIcon(ContextManager::theme()->getColoredSvg(
        isPlaying ? ":/icons/recorder_pause.svg" : ":/icons/recorder_play.svg", QSize(32, 32), ThemeManager::WINDOW_TEXT));
    ui->pushPlayPause->setToolTip(isPlaying ? tr("Pause") : tr("Play"));
}

QString DialogMidiPlayer::getTimeText(qint64 microseconds)
{
    qint64 seconds = microseconds / 1000000;
    if (seconds >= 3600)
        return QString("%1:%2:%3").arg(seconds / 3600).arg((seconds / 60) % 60, 2, 10, QChar('0')).arg(seconds % 60, 2, 10, QChar('0'));
    return QString("%1:%2").arg(seconds / 60).arg(seconds % 60, 2, 10, QChar('0'));
}

void DialogMidiPlayer::onMixChanged()
{
    bool isSoloEnabled = false;
    foreach (MidiPlayerChannel * channel, _channels)
        isSoloEnabled |= channel->isSolo();
    foreach (MidiPlayerChannel * channel, _channels)
        channel->updateGain(isSoloEnabled);
}

void DialogMidiPlayer::onSoundfontLoaded(int sf2Index)
{
    // The first soundfont opened is played by all channels, and the channels possibly playing it while it was loaded
    // are updated
    bool withoutSoundfont = true;
    foreach (MidiPlayerChannel * channel, _channels)
    {
        withoutSoundfont &= (channel->getSoundfont() == -1);
        channel->updateSoundfontList();
    }
    foreach (MidiPlayerChannel * channel, _channels)
        if (withoutSoundfont || channel->getSoundfont() == sf2Index)
            channel->setSoundfont(sf2Index);
}

void DialogMidiPlayer::onSoundfontClosed(int sf2Index)
{
    // The channels playing it play nothing
    Q_UNUSED(sf2Index)
    foreach (MidiPlayerChannel * channel, _channels)
        channel->updateSoundfontList();
}

void DialogMidiPlayer::onEditingDone(QString editingSource, QList<int> sf2Indexes)
{
    // Possibly new soundfont names, presets or programs
    Q_UNUSED(editingSource)
    foreach (MidiPlayerChannel * channel, _channels)
    {
        channel->updateSoundfontList();
        if (sf2Indexes.contains(channel->getSoundfont()))
            channel->updatePresetList();
    }
}

void DialogMidiPlayer::onInputModulatorChanged(int controllerNumber, bool isBipolar, bool isDescending)
{
    _player->setDefaultControllerValue(controllerNumber, isBipolar, isDescending);
}

void DialogMidiPlayer::showEvent(QShowEvent * event)
{
    QDialog::showEvent(event);
    if (_stateUpdate)
    {
        EditorToolBar::updateMidiPlayerButtonsState(true);
        Player::updateMidiPlayerButtonsState(true);
    }

    // Restore the geometry (not working with Wayland)
    QByteArray geometry = ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "midiPlayerGeometry", QByteArray()).toByteArray();
    if (!geometry.isEmpty())
        this->restoreGeometry(geometry);

    // The last file is opened again if there is none
    if (_file == nullptr)
    {
        QString lastFile = ContextManager::recentFile()->getLastFile(RecentFileManager::FILE_TYPE_MIDI);
        if (!lastFile.isEmpty() && QFileInfo::exists(lastFile))
            loadFile(lastFile);
    }

    _timer->start(40);
}

void DialogMidiPlayer::hideEvent(QHideEvent * event)
{
    QDialog::hideEvent(event);
    if (_stateUpdate)
    {
        EditorToolBar::updateMidiPlayerButtonsState(false);
        Player::updateMidiPlayerButtonsState(false);
    }

    // Save the geometry
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "midiPlayerGeometry", this->saveGeometry());

    // Stop the playback
    _timer->stop();
    _player->stop();
    updatePlayPauseIcon(false);
    updatePosition(0);
}

void DialogMidiPlayer::keyPressEvent(QKeyEvent * event)
{
    // Space plays or pauses, and the escape key does not close the window
    if (event->key() == Qt::Key_Space && event->modifiers() == Qt::NoModifier)
        on_pushPlayPause_clicked();
    else if (event->key() != Qt::Key_Escape)
        QDialog::keyPressEvent(event);
}

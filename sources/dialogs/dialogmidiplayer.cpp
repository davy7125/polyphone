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
#include <QHeaderView>
#include <QGridLayout>
#include <QCheckBox>
#include <QLabel>
#include <QMessageBox>
#include "contextmanager.h"
#include "soundfontmanager.h"
#include "soundfonts.h"
#include "synth.h"
#include "midifile.h"
#include "midifileplayer.h"
#include "midifilerenderer.h"
#include "midiplayerchannel.h"
#include "midiplayerdelegate.h"
#include "midiplayerselection.h"
#include "temporarysoundfonts.h"
#include "editortoolbar.h"
#include "bannermenu.h"
#include "dialogdecoration.h"
#include "player.h"
#include "resettableslider.h"
#include "waitingtooldialog.h"

DialogMidiPlayer::DialogMidiPlayer(QWidget *parent) :
    QDialog(parent, Qt::Tool | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint),
    ui(new Ui::DialogMidiPlayer),
    _selection(new MidiPlayerSelection()),
    _delegate(nullptr),
    _isShown(false),
    _stateUpdate(true),
    _isPlaying(false),
    _updatingSlider(false),
    _speed(1.0),
    _sliderTempo(nullptr),
    _sliderPitch(nullptr),
    _labelTempoTitle(nullptr),
    _labelTempo(nullptr),
    _labelPitch(nullptr),
    _checkLoop(nullptr),
    _actionExport(nullptr),
    _renderer(nullptr),
    _isExportFinished(false),
    _exportDialog(nullptr)
{
    ui->setupUi(this);

    // Player using the synth of the editor, its channels having their own controller values
    SoundfontManager * sm = SoundfontManager::getInstance();
    Synth * synth = ContextManager::audio()->getSynth();
    _player = new MidiFilePlayer(synth, sm->getSoundfonts(), sm->getMutex());
    synth->setMidiFileValues(_player->getChannelStates());

    // Banner, its buttons being as the ones of the editor toolbar (lighter icon when hovered, arrow for the menu)
    ThemeManager * theme = ContextManager::theme();
    QColor bannerColor = theme->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);
    QColor bannerTextColor = theme->getColor(ThemeManager::HIGHLIGHTED_TEXT);
    ui->frameTop->setStyleSheet(theme->getBannerTheme() +
                                "QSlider::groove:horizontal{height:4px;border-radius:2px;background:" +
                                ThemeManager::mix(bannerColor, bannerTextColor, 0.35).name() + "}" +
                                "QSlider::sub-page:horizontal{border-radius:2px;background:" + bannerTextColor.name() + "}" +
                                "QSlider::handle:horizontal{width:12px;margin:-4px 0;border-radius:6px;background:" +
                                bannerTextColor.name() + "}");
    ui->pushOpen->setIcon(theme->getBannerIcon(":/icons/folder.svg", 24));
    ui->pushOpen->setCursor(Qt::PointingHandCursor);
    ui->pushStop->setIcon(theme->getBannerIcon(":/icons/recorder_stop.svg", 28));
    ui->pushStop->setCursor(Qt::PointingHandCursor);
    ui->toolButtonSettings->setIcon(theme->getBannerIcon(":/icons/menu.svg", 24));
    ui->toolButtonSettings->setCursor(Qt::PointingHandCursor);
    ui->layoutTop->setAlignment(ui->layoutPosition, Qt::AlignVCenter); // Height of the other banners, the content centered
    updatePlayPauseIcon(false);
    initializeSettingsMenu();
    ui->pushPlayPause->setCursor(Qt::PointingHandCursor);
    DialogDecoration::apply(this, ui->frameTop);

    // Table of the channels, in the style of the tables of the editor but its edges being the ones of the window: no
    // border, and no line on the right side of the last column and at the bottom of the last row (grid drawn by
    // MidiPlayerDelegate). A list opens with a click on the selected cell or with a double click.
    QTableWidget * table = ui->tableChannels;
    _delegate = new MidiPlayerDelegate(_channels, table);
    table->setItemDelegate(_delegate);
    table->setColumnCount(4);
    table->setHorizontalHeaderLabels(QStringList() << tr("Channel") << tr("Soundfont") << tr("Preset") << tr("Volume"));
    table->setStyleSheet(theme->getTableTheme() + "QTableWidget{border:0}QHeaderView::section:last{border-right:0}");
    QPalette palette = table->palette();
    palette.setColor(QPalette::Base, theme->getColor(ThemeManager::LIST_BACKGROUND));
    palette.setColor(QPalette::AlternateBase, theme->getColor(ThemeManager::LIST_ALTERNATIVE_BACKGROUND));
    palette.setColor(QPalette::Text, theme->getColor(ThemeManager::LIST_TEXT));
    table->setPalette(palette);
    table->setWordWrap(false);
    table->horizontalHeader()->setSectionsClickable(false);
    table->horizontalHeader()->setHighlightSections(false);
    table->horizontalHeader()->setSectionResizeMode(MidiPlayerDelegate::COLUMN_CHANNEL, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(MidiPlayerDelegate::COLUMN_SOUNDFONT, QHeaderView::Fixed);
    table->horizontalHeader()->setSectionResizeMode(MidiPlayerDelegate::COLUMN_PRESET, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(MidiPlayerDelegate::COLUMN_VOLUME, QHeaderView::ResizeToContents);
    table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    table->verticalHeader()->setDefaultSectionSize(28);
    connect(table, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(onItemChanged(QTableWidgetItem*)));

    // No file at the beginning
    ui->labelFile->setTextToElide("-");
    ui->labelMessage->setText(tr("Open a MIDI file to play it with the soundfonts that are open."));
    ui->tableChannels->hide();
    ui->pushPlayPause->setEnabled(false);
    ui->pushStop->setEnabled(false);
    ui->sliderPosition->setEnabled(false);
    _actionExport->setEnabled(false);
    updateSize();

    // Periodic update of the position, the activity, the programs and the MIDI values
    _timer = new QTimer(this);
    connect(_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));

    // Progress of an export
    _exportTimer = new QTimer(this);
    connect(_exportTimer, SIGNAL(timeout()), this, SLOT(onExportTimeout()));

    connect(sm, SIGNAL(soundfontLoaded(int)), this, SLOT(onSoundfontLoaded(int)));
    connect(sm, SIGNAL(soundfontClosed(int)), this, SLOT(onSoundfontClosed(int)));
    connect(sm, SIGNAL(editingDone(QString,QList<int>)), this, SLOT(onEditingDone(QString,QList<int>)));
    connect(sm, SIGNAL(inputModulatorChanged(int,bool,bool)), this, SLOT(onInputModulatorChanged(int,bool,bool)));

    // Soundfont selected in the main window
    connect(_selection, SIGNAL(soundfontChanged()), this, SLOT(onSelectionSoundfontChanged()));
    connect(_selection, SIGNAL(stateChanged()), this, SLOT(onSelectionStateChanged()));
}

DialogMidiPlayer::~DialogMidiPlayer()
{
    // The selection is not followed anymore
    _selection->setActive(false);

    // Possibly an export not finished
    if (_exportThread.joinable())
    {
        _renderer->cancel();
        _exportThread.join();
    }
    delete _renderer;
    foreach (int sf2Index, _exportSoundfonts)
        TemporarySoundfonts::release(sf2Index);

    clearChannels();
    delete _selection;
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

void DialogMidiPlayer::openFile(QString filePath)
{
    loadFile(filePath);
}

void DialogMidiPlayer::loadFile(QString filePath)
{
    clearChannels();
    _player->setFile(nullptr);
    _file.reset();
    updatePlayPauseIcon(false);
    ui->labelFile->setTextToElide(QFileInfo(filePath).fileName()); // All the room up to the time, elided in the middle
    ui->labelFile->setToolTip(filePath);

    std::shared_ptr<MidiFile> file(new MidiFile());
    bool isValid = file->read(filePath);
    ui->pushPlayPause->setEnabled(isValid);
    ui->pushStop->setEnabled(isValid);
    ui->sliderPosition->setEnabled(isValid);
    _actionExport->setEnabled(isValid);
    _updatingSlider = true;
    ui->sliderPosition->setRange(0, isValid ? static_cast<int>(file->getDuration() / 1000) : 0);
    ui->sliderPosition->setValue(0);
    _updatingSlider = false;
    if (!isValid)
    {
        ui->labelMessage->setText(file->getError());
        ui->labelMessage->show();
        ui->tableChannels->hide();
        updateTempoToolTip();
        updateTime(0);
        updateSize();
        updateSelectionUse(); // Without channel
        return;
    }
    ui->labelMessage->hide();
    ui->tableChannels->show();
    ContextManager::recentFile()->addRecentFile(RecentFileManager::FILE_TYPE_MIDI, filePath);
    _file = file;
    _filePath = filePath;
    _player->setFile(file);
    updateTempoToolTip();
    updatePosition(0);

    // A row for each channel having notes, playing the soundfont selected in the main window (its samples are loaded by
    // the selection)
    int rowCount = 0;
    for (int channel = 0; channel < 16; channel++)
        if (file->isChannelUsed(channel))
            rowCount++;
    ui->tableChannels->setRowCount(rowCount);
    for (int channel = 0; channel < 16; channel++)
    {
        if (!file->isChannelUsed(channel))
            continue;
        MidiPlayerChannel * playerChannel = new MidiPlayerChannel(
            _player, _selection, channel, file->getChannelName(channel), MidiFilePlayer::getUsedPrograms(file.get(), channel),
            ui->tableChannels, _channels.count());
        connect(playerChannel, SIGNAL(mixChanged()), this, SLOT(onMixChanged()));
        _channels << playerChannel;
        playerChannel->setSoundfont(MidiPlayerChannel::SELECTION, false);
        connect(playerChannel, SIGNAL(selectionFollowingChanged()), this, SLOT(updateSelectionUse()));
    }
    updateSelectionUse();

    // Size once the rows are shown (the size hints of their widgets are then known)
    updateSize();
    QTimer::singleShot(0, this, SLOT(updateSize()));
}

void DialogMidiPlayer::updateSize()
{
    // The table shows all channels
    QTableWidget * table = ui->tableChannels;
    int height = table->horizontalHeader()->sizeHint().height() + 2 * table->frameWidth();
    for (int i = 0; i < table->rowCount(); i++)
        height += table->rowHeight(i);
    table->setFixedHeight(height);

    // Soundfont names as wide as needed (up to a limit), with the room of the spinner in front of the selection, the presets
    // having the remaining space
    int charWidth = table->fontMetrics().averageCharWidth();
    int soundfontWidth = table->fontMetrics().horizontalAdvance(
                             table->horizontalHeaderItem(MidiPlayerDelegate::COLUMN_SOUNDFONT)->text()) + 4 * charWidth;
    int spinnerWidth = 0;
    for (int i = 0; i < table->rowCount(); i++)
    {
        QTableWidgetItem * item = table->item(i, MidiPlayerDelegate::COLUMN_SOUNDFONT); // Null while the rows are created
        if (item != nullptr)
            soundfontWidth = qMax(soundfontWidth, table->fontMetrics().horizontalAdvance(item->text()) + 2 * charWidth);
        if (i < _channels.count() && _channels[i]->isFollowingSelection())
            spinnerWidth = MidiPlayerDelegate::getSpinnerWidth();
    }
    soundfontWidth = qMin(soundfontWidth, 25 * charWidth) + spinnerWidth;
    table->horizontalHeader()->resizeSection(MidiPlayerDelegate::COLUMN_SOUNDFONT, soundfontWidth);
    int width = soundfontWidth + 25 * charWidth + 2 * table->frameWidth();
    foreach (int column, QList<int>() << MidiPlayerDelegate::COLUMN_CHANNEL << MidiPlayerDelegate::COLUMN_VOLUME)
        if (table->rowCount() > 0 && table->cellWidget(0, column) != nullptr)
            width += table->cellWidget(0, column)->sizeHint().width();
    table->setMinimumWidth(width);

    // Fixed height of the window, depending on the number of channels
    this->setFixedHeight(this->sizeHint().height());
}

void DialogMidiPlayer::onItemChanged(QTableWidgetItem * item)
{
    // Another soundfont name
    if (item->column() == MidiPlayerDelegate::COLUMN_SOUNDFONT)
        updateSize();
}

void DialogMidiPlayer::clearChannels()
{
    while (!_channels.isEmpty())
        delete _channels.takeFirst();
    ui->tableChannels->setRowCount(0);

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

void DialogMidiPlayer::on_sliderPosition_valueChanged(int value)
{
    if (_updatingSlider)
        return;

    // The position changes when the slider is released, or directly with a click on the groove
    updateTime(1000LL * value);
    if (!ui->sliderPosition->isSliderDown())
        _player->setPosition(1000LL * value);
}

void DialogMidiPlayer::on_sliderPosition_sliderReleased()
{
    _player->setPosition(1000LL * ui->sliderPosition->value());
}

void DialogMidiPlayer::initializeSettingsMenu()
{
    // Menu under the button on the right side of the banner
    BannerMenu * menu = new BannerMenu(ui->toolButtonSettings, this);
    QGridLayout * layout = menu->getGrid();

    // Loop
    _checkLoop = menu->addCheckBox(tr("Loop playback"), 0, 3);

    // Tempo as a percentage of the one of the file (all its tempo changes being kept), and transposition except for the
    // percussions, both kept for all files and restored with a double click
    _labelTempoTitle = new QLabel(tr("Tempo"));
    _sliderTempo = new ResettableSlider();
    _sliderTempo->setRange(25, 200);
    _sliderTempo->setValue(100);
    _sliderTempo->setDefaultValue(100);
    _labelTempo = new QLabel();
    _labelTempo->setMinimumWidth(_labelTempo->fontMetrics().horizontalAdvance(tr("%1%", "percentage").arg(200)));
    QLabel * labelPitchTitle = new QLabel(tr("Pitch"));
    _sliderPitch = new ResettableSlider();
    _sliderPitch->setRange(-12, 12);
    _sliderPitch->setPageStep(1);
    _sliderPitch->setValue(0);
    _labelPitch = new QLabel();
    _labelPitch->setMinimumWidth(_labelPitch->fontMetrics().horizontalAdvance("+12"));
    foreach (QWidget * pitchWidget, QList<QWidget *>() << labelPitchTitle << _sliderPitch << _labelPitch)
        pitchWidget->setToolTip(tr("The percussions (bank 128) are not transposed"));
    foreach (ResettableSlider * slider, QList<ResettableSlider *>() << _sliderTempo << _sliderPitch)
    {
        slider->setOrientation(Qt::Horizontal);
        slider->setFocusPolicy(Qt::NoFocus);
        slider->setMinimumWidth(160);
    }
    foreach (QLabel * label, QList<QLabel *>() << _labelTempo << _labelPitch)
        label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    layout->addWidget(_labelTempoTitle, 1, 0);
    layout->addWidget(_sliderTempo, 1, 1);
    layout->addWidget(_labelTempo, 1, 2);
    layout->addWidget(labelPitchTitle, 2, 0);
    layout->addWidget(_sliderPitch, 2, 1);
    layout->addWidget(_labelPitch, 2, 2);

    // Export of the file as it is heard
    menu->addSeparator();
    _actionExport = menu->addAction(tr("Export audio..."));
    connect(_actionExport, SIGNAL(triggered()), this, SLOT(onExportTriggered()));

    connect(_checkLoop, SIGNAL(toggled(bool)), this, SLOT(onLoopToggled(bool)));
    connect(_sliderTempo, SIGNAL(valueChanged(int)), this, SLOT(onTempoChanged(int)));
    connect(_sliderPitch, SIGNAL(valueChanged(int)), this, SLOT(onPitchChanged(int)));
    onTempoChanged(100);
    onPitchChanged(0);
    updateTempoToolTip();
}

void DialogMidiPlayer::updateTempoToolTip()
{
    // Tempo of the file, if there is one
    int tempo = _file == nullptr ? 0 : qRound(_file->getTempo());
    QString toolTip;
    if (tempo > 0)
        toolTip = (_file->isTempoVariable() ? tr("Tempo of the file: %1 bpm, variable") : tr("Tempo of the file: %1 bpm")).arg(tempo);
    else if (_file != nullptr)
        toolTip = tr("File without tempo (SMPTE time code)");
    _labelTempoTitle->setToolTip(toolTip);
    _sliderTempo->setToolTip(toolTip);
    _labelTempo->setToolTip(toolTip);
}

void DialogMidiPlayer::onLoopToggled(bool checked)
{
    _player->setLoop(checked);
}

void DialogMidiPlayer::onTempoChanged(int value)
{
    _speed = value / 100.0;
    _player->setSpeed(_speed);
    _labelTempo->setText(tr("%1%", "percentage").arg(value));
    updateTime(1000LL * ui->sliderPosition->value());
}

void DialogMidiPlayer::onPitchChanged(int value)
{
    _player->setTransposition(value);
    _labelPitch->setText((value > 0 ? "+" : "") + QString::number(value));
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
    updateTime(position);
}

void DialogMidiPlayer::updateTime(qint64 position)
{
    // Times at the current speed
    qint64 duration = _file == nullptr ? 0 : _file->getDuration();
    ui->labelTime->setText(getTimeText(static_cast<qint64>(position / _speed)) + " / " +
                           getTimeText(static_cast<qint64>(duration / _speed)));
}

void DialogMidiPlayer::updatePlayPauseIcon(bool isPlaying)
{
    _isPlaying = isPlaying;
    QString svgPath = isPlaying ? ":/icons/recorder_pause.svg" : ":/icons/recorder_play.svg";
    ui->pushPlayPause->setIcon(ContextManager::theme()->getBannerIcon(svgPath, 28));
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
    // The first soundfont opened is played by all channels if none plays a soundfont (the selection possibly plays it if
    // it had no soundfont), and the channels possibly playing it while it was loaded are updated
    bool withoutSoundfont = true;
    foreach (MidiPlayerChannel * channel, _channels)
        withoutSoundfont &= (!channel->isFollowingSelection() && channel->getSoundfont() == -1);
    foreach (MidiPlayerChannel * channel, _channels)
    {
        if (channel->isFollowingSelection())
        {
            if (channel->getSoundfont() == sf2Index)
                channel->updatePresetList();
        }
        else if (withoutSoundfont || channel->getSoundfont() == sf2Index)
            channel->setSoundfont(sf2Index);
    }
    _selection->update();
}

void DialogMidiPlayer::onSoundfontClosed(int sf2Index)
{
    // The channels playing it play nothing, or another soundfont selected
    _selection->onSoundfontClosed(sf2Index);
    foreach (MidiPlayerChannel * channel, _channels)
        channel->updateSoundfont();
}

void DialogMidiPlayer::onSoundfontSelectionChanged()
{
    _selection->update();
}

void DialogMidiPlayer::updateSelectionUse()
{
    // Programs of the channels following the selection, which is followed while the window is visible
    QList<int> programs;
    bool isFollowed = false;
    foreach (MidiPlayerChannel * channel, _channels)
    {
        if (!channel->isFollowingSelection())
            continue;
        isFollowed = true;
        foreach (int program, channel->getUsedPrograms())
            if (!programs.contains(program))
                programs << program;
    }
    _selection->setPrograms(programs);
    _selection->setActive(isFollowed && _isShown);
    onSelectionStateChanged();
}

void DialogMidiPlayer::onSelectionSoundfontChanged()
{
    foreach (MidiPlayerChannel * channel, _channels)
        channel->onSelectionSoundfontChanged();
}

void DialogMidiPlayer::onSelectionStateChanged()
{
    // Name of the selection, and spinner while it is loading
    bool isFollowed = false;
    foreach (MidiPlayerChannel * channel, _channels)
    {
        if (channel->isFollowingSelection())
        {
            isFollowed = true;
            channel->updateSoundfont();
        }
    }
    _delegate->setLoading(isFollowed && _selection->isLoading());
}

void DialogMidiPlayer::onEditingDone(QString editingSource, QList<int> sf2Indexes)
{
    // Possibly new soundfont names, presets or programs
    Q_UNUSED(editingSource)
    foreach (MidiPlayerChannel * channel, _channels)
    {
        channel->updateSoundfont();
        if (sf2Indexes.contains(channel->getSoundfont()))
            channel->updatePresetList();
    }
}

void DialogMidiPlayer::onInputModulatorChanged(int controllerNumber, bool isBipolar, bool isDescending)
{
    _player->setDefaultControllerValue(controllerNumber, isBipolar, isDescending);
}

void DialogMidiPlayer::onExportTriggered()
{
    if (_file == nullptr || _renderer != nullptr)
        return;

    // Wav file named after the MIDI file, in the directory of the last record
    QString filePath = QFileDialog::getSaveFileName(
        this, tr("Export audio"),
        ContextManager::recentFile()->getLastDirectory(RecentFileManager::FILE_TYPE_RECORD) + "/" +
            QFileInfo(_filePath).completeBaseName() + ".wav",
        tr("Wav file") + " (*.wav)");
    if (filePath.isEmpty())
        return;
    if (!filePath.endsWith(".wav", Qt::CaseInsensitive))
        filePath.append(".wav");
    ContextManager::recentFile()->addRecentFile(RecentFileManager::FILE_TYPE_RECORD, filePath);

    // The playback pauses during the export
    if (_player->isPlaying())
    {
        _player->pause();
        updatePlayPauseIcon(false);
    }

    // Same settings as the synth of the editor (its sample rate being shared by the effects of all synths), and same
    // channels, tempo and transposition as the player
    ConfManager * configuration = ContextManager::configuration();
    _renderer = new MidiFileRenderer();
    _renderer->copyDefaultControllerValues(*_player->getChannelStates());
    _renderer->configure(configuration->getSynthConfig(),
                         configuration->getValue(ConfManager::SECTION_AUDIO, "polyphony", DEFAULT_POLYPHONY).toInt(),
                         ContextManager::audio()->getSynth()->getSampleRate());
    bool isSoloEnabled = false;
    foreach (MidiPlayerChannel * channel, _channels)
        isSoloEnabled |= channel->isSolo();
    foreach (MidiPlayerChannel * channel, _channels)
    {
        _renderer->setChannel(channel->getChannel(), channel->getSoundfont(), channel->getForcedProgram(),
                              channel->getGain(isSoloEnabled));

        // The soundfonts loaded by the soundfont explorer are kept until the end of the export
        int sf2Index = channel->getSoundfont();
        if (sf2Index >= 0 && !_exportSoundfonts.contains(sf2Index))
        {
            _exportSoundfonts << sf2Index;
            TemporarySoundfonts::hold(sf2Index);
        }
    }
    _renderer->setSpeed(_speed);
    _renderer->setTransposition(_sliderPitch->value());

    // Rendering in another thread, the window waiting for it (the soundfonts cannot be edited meanwhile)
    _exportDialog = new WaitingToolDialog(tr("Export audio"), 1000, this);
    connect(_exportDialog, SIGNAL(canceled()), this, SLOT(onExportCanceled()));
    _exportDialog->show();
    _isExportFinished = false;
    std::shared_ptr<const MidiFile> file = _file;
    MidiFileRenderer * renderer = _renderer;
    _exportThread = std::thread([this, file, renderer, filePath]() {
        renderer->render(file.get(), filePath);
        _isExportFinished = true;
    });
    _exportTimer->start(50);
}

void DialogMidiPlayer::onExportCanceled()
{
    // The incomplete file is removed
    if (_renderer != nullptr)
        _renderer->cancel();
}

void DialogMidiPlayer::onExportTimeout()
{
    if (_renderer == nullptr)
        return;
    if (!_isExportFinished)
    {
        _exportDialog->setValue(_renderer->getProgress());
        return;
    }

    // End of the export, possibly with an error
    _exportTimer->stop();
    _exportThread.join();
    foreach (int sf2Index, _exportSoundfonts)
        TemporarySoundfonts::release(sf2Index);
    _exportSoundfonts.clear();
    QString error = _renderer->getError();
    delete _renderer;
    _renderer = nullptr;
    delete _exportDialog;
    _exportDialog = nullptr;
    if (!error.isEmpty())
        QMessageBox::warning(this, tr("Warning"), error);
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

    // The last file is opened again if there is none, the channels following the selection again
    _isShown = true;
    if (_file == nullptr)
    {
        QString lastFile = ContextManager::recentFile()->getLastFile(RecentFileManager::FILE_TYPE_MIDI);
        if (!lastFile.isEmpty() && QFileInfo::exists(lastFile))
            loadFile(lastFile);
    }
    updateSelectionUse();

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

    // Stop the playback, the selection being not followed anymore
    _timer->stop();
    _player->stop();
    updatePlayPauseIcon(false);
    updatePosition(0);
    _isShown = false;
    updateSelectionUse();
}

void DialogMidiPlayer::keyPressEvent(QKeyEvent * event)
{
    // Space plays or pauses, and the escape key does not close the window
    if (event->key() == Qt::Key_Space && event->modifiers() == Qt::NoModifier)
        on_pushPlayPause_clicked();
    else if (event->key() != Qt::Key_Escape)
        QDialog::keyPressEvent(event);
}

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
#include <QMenu>
#include <QWidgetAction>
#include <QGridLayout>
#include <QCheckBox>
#include <QScreen>
#include <QLabel>
#include "contextmanager.h"
#include "soundfontmanager.h"
#include "soundfonts.h"
#include "tabmanager.h"
#include "synth.h"
#include "midifile.h"
#include "midifileplayer.h"
#include "midiplayerchannel.h"
#include "midiplayerdelegate.h"
#include "editortoolbar.h"
#include "player.h"
#include "resettableslider.h"

DialogMidiPlayer::DialogMidiPlayer(QWidget *parent) :
    QDialog(parent, Qt::Tool | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint),
    ui(new Ui::DialogMidiPlayer),
    _stateUpdate(true),
    _isPlaying(false),
    _updatingSlider(false),
    _speed(1.0),
    _sliderTempo(nullptr),
    _sliderPitch(nullptr),
    _labelTempoTitle(nullptr),
    _labelTempo(nullptr),
    _labelPitch(nullptr),
    _labelLoop(nullptr),
    _checkLoop(nullptr)
{
    ui->setupUi(this);

    // Player using the synth of the editor, its channels having their own controller values
    SoundfontManager * sm = SoundfontManager::getInstance();
    Synth * synth = ContextManager::audio()->getSynth();
    _player = new MidiFilePlayer(synth, sm->getSoundfonts(), sm->getMutex());
    synth->setMidiFileValues(_player->getChannelStates());

    // Banner with the colors of the status bar of the virtual keyboard, its buttons being as the ones of the editor
    // toolbar (lighter icon when hovered, arrow for the menu)
    ThemeManager * theme = ContextManager::theme();
    QColor bannerColor = theme->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);
    QColor bannerTextColor = theme->getColor(ThemeManager::HIGHLIGHTED_TEXT);
    ui->frameTop->setStyleSheet("QFrame#frameTop{background-color:" + bannerColor.name() + "}" +
                                "QLabel{color:" + bannerTextColor.name() + "}" +
                                "QToolButton{border:0;background:transparent}" +
                                "QToolButton#toolButtonSettings{padding-right:6px}" +
                                "QToolButton::menu-indicator{image:url(:/icons/arrow_down.svg);height:12px;width:12px}" +
                                "QSlider::groove:horizontal{height:4px;border-radius:2px;background:" +
                                ThemeManager::mix(bannerColor, bannerTextColor, 0.35).name() + "}" +
                                "QSlider::sub-page:horizontal{border-radius:2px;background:" + bannerTextColor.name() + "}" +
                                "QSlider::handle:horizontal{width:12px;margin:-4px 0;border-radius:6px;background:" +
                                bannerTextColor.name() + "}");
    ui->pushOpen->setIcon(getBannerIcon(":/icons/folder.svg", 24));
    ui->pushStop->setIcon(getBannerIcon(":/icons/recorder_stop.svg", 28));
    ui->toolButtonSettings->setIcon(getBannerIcon(":/icons/gear.svg", 24));
    updatePlayPauseIcon(false);
    initializeSettingsMenu();

    // Table of the channels, in the style of the tables of the editor but its edges being the ones of the window: no
    // border, and no line on the right side of the last column and at the bottom of the last row (grid drawn by
    // MidiPlayerDelegate). A list opens with a click on the selected cell or with a double click.
    QTableWidget * table = ui->tableChannels;
    table->setItemDelegate(new MidiPlayerDelegate(_channels, table));
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
    ui->labelFile->setText("-");
    ui->labelMessage->setText(tr("Open a MIDI file to play it with the soundfonts that are open."));
    ui->tableChannels->hide();
    ui->pushPlayPause->setEnabled(false);
    ui->pushStop->setEnabled(false);
    ui->sliderPosition->setEnabled(false);
    updateSize();

    // Periodic update of the position, the activity, the programs and the MIDI values
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
        ui->tableChannels->hide();
        updateTempoToolTip();
        updateTime(0);
        updateSize();
        return;
    }
    ui->labelMessage->hide();
    ui->tableChannels->show();
    ContextManager::recentFile()->addRecentFile(RecentFileManager::FILE_TYPE_MIDI, filePath);
    _file = file;
    _player->setFile(file);
    updateTempoToolTip();
    updatePosition(0);

    // The channels play the soundfont of the current tab, otherwise the first one open
    TabManager * tabManager = TabManager::getInstance();
    int sf2Index = tabManager == nullptr ? -1 : tabManager->getCurrentSf2();
    if (sf2Index < 0)
    {
        // Not the soundfonts loaded without tab by the soundfont explorer
        SoundfontManager * sm = SoundfontManager::getInstance();
        EltID idSf2(elementSf2);
        foreach (int index, sm->getSiblings(idSf2))
        {
            if (!sm->isTemporary(index))
            {
                sf2Index = index;
                break;
            }
        }
    }

    // A row for each channel having notes
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
            _player, channel, file->getChannelName(channel), MidiFilePlayer::getUsedPrograms(file.get(), channel),
            ui->tableChannels, _channels.count());
        connect(playerChannel, SIGNAL(mixChanged()), this, SLOT(onMixChanged()));
        _channels << playerChannel;
        playerChannel->setSoundfont(sf2Index);
    }

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

    // Soundfont names as wide as needed (up to a limit), the presets having the remaining space
    int charWidth = table->fontMetrics().averageCharWidth();
    int soundfontWidth = table->fontMetrics().horizontalAdvance(
                             table->horizontalHeaderItem(MidiPlayerDelegate::COLUMN_SOUNDFONT)->text()) + 4 * charWidth;
    for (int i = 0; i < table->rowCount(); i++)
    {
        QTableWidgetItem * item = table->item(i, MidiPlayerDelegate::COLUMN_SOUNDFONT); // Null while the rows are created
        if (item != nullptr)
            soundfontWidth = qMax(soundfontWidth, table->fontMetrics().horizontalAdvance(item->text()) + 2 * charWidth);
    }
    soundfontWidth = qMin(soundfontWidth, 25 * charWidth);
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
    // Menu under the button on the right side of the banner (not a child of the banner, whose style would apply)
    ThemeManager * theme = ContextManager::theme();
    QMenu * menu = new QMenu(this);
    menu->setStyleSheet(theme->getMenuTheme());
    menu->installEventFilter(this); // Aligned on the right side of the button
    QWidget * widget = new QWidget();
    widget->setAttribute(Qt::WA_NoMousePropagation); // A click on a label does not close the menu
    widget->setStyleSheet("QLabel,QCheckBox{color:" + theme->getColor(ThemeManager::LIST_TEXT).name() + "}");
    QGridLayout * layout = new QGridLayout(widget);
    layout->setContentsMargins(15, 5, 15, 5);
    layout->setHorizontalSpacing(10);
    layout->setVerticalSpacing(10);

    // Loop, the checkbox being on the right side and toggled by a click on its label
    _labelLoop = new QLabel(tr("Loop playback"));
    _labelLoop->installEventFilter(this);
    _checkLoop = new QCheckBox();
    _checkLoop->setFocusPolicy(Qt::NoFocus);
    layout->addWidget(_labelLoop, 0, 0, 1, 2);
    layout->addWidget(_checkLoop, 0, 2, Qt::AlignRight);

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

    QWidgetAction * action = new QWidgetAction(menu);
    action->setDefaultWidget(widget);
    menu->addAction(action);
    ui->toolButtonSettings->setMenu(menu);

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
    ui->pushPlayPause->setIcon(getBannerIcon(isPlaying ? ":/icons/recorder_pause.svg" : ":/icons/recorder_play.svg", 28));
    ui->pushPlayPause->setToolTip(isPlaying ? tr("Pause") : tr("Play"));
}

QIcon DialogMidiPlayer::getBannerIcon(QString svgPath, int size)
{
    // Text color of the banner, changed when the button is hovered (active mode of an auto-raise button) and dimmed
    // when it is disabled
    QIcon icon;
    icon.addPixmap(ContextManager::theme()->getColoredSvg(svgPath, QSize(size, size), ThemeManager::HIGHLIGHTED_TEXT), QIcon::Normal);
    QMap<QString, QString> replacement;
    replacement["currentColor"] = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT, ThemeManager::HOVERED).name();
    icon.addPixmap(ContextManager::theme()->getColoredSvg(svgPath, QSize(size, size), replacement), QIcon::Active);
    replacement["currentColor"] = ThemeManager::mix(ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND),
                                                    ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT), 0.4).name();
    icon.addPixmap(ContextManager::theme()->getColoredSvg(svgPath, QSize(size, size), replacement), QIcon::Disabled);
    return icon;
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
        withoutSoundfont &= (channel->getSoundfont() == -1);
    foreach (MidiPlayerChannel * channel, _channels)
        if (withoutSoundfont || channel->getSoundfont() == sf2Index)
            channel->setSoundfont(sf2Index);
}

void DialogMidiPlayer::onSoundfontClosed(int sf2Index)
{
    // The channels playing it play nothing
    Q_UNUSED(sf2Index)
    foreach (MidiPlayerChannel * channel, _channels)
        channel->updateSoundfont();
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

bool DialogMidiPlayer::eventFilter(QObject * watched, QEvent * event)
{
    // A click on the label of the loop toggles it
    if (watched == _labelLoop && event->type() == QEvent::MouseButtonRelease &&
        static_cast<QMouseEvent *>(event)->button() == Qt::LeftButton)
    {
        _checkLoop->toggle();
        return true;
    }

    // The settings menu is aligned on the right side of its button (a right-to-left button would have its arrow on the
    // left side), within the screen
    if (watched == ui->toolButtonSettings->menu() && event->type() == QEvent::Show)
    {
        QWidget * menu = ui->toolButtonSettings->menu();
        QPoint position = ui->toolButtonSettings->mapToGlobal(
            QPoint(ui->toolButtonSettings->width() - menu->width(), ui->toolButtonSettings->height()));
        if (menu->screen() != nullptr)
            position.setX(qMax(position.x(), menu->screen()->availableGeometry().left()));
        menu->move(position.x(), menu->y());
    }
    return QDialog::eventFilter(watched, event);
}

void DialogMidiPlayer::keyPressEvent(QKeyEvent * event)
{
    // Space plays or pauses, and the escape key does not close the window
    if (event->key() == Qt::Key_Space && event->modifiers() == Qt::NoModifier)
        on_pushPlayPause_clicked();
    else if (event->key() != Qt::Key_Escape)
        QDialog::keyPressEvent(event);
}

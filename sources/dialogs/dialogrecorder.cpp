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

#include "dialogrecorder.h"
#include "ui_dialogrecorder.h"
#include <QCheckBox>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QKeyEvent>
#include <QMessageBox>
#include <QPushButton>
#include <QShortcut>
#include <QTime>
#include <QUrl>
#include <cmath>
#include "bannermenu.h"
#include "contextmanager.h"
#include "dialogdecoration.h"
#include "editortoolbar.h"
#include "midifile.h"
#include "player.h"
#include "recorder.h"
#include "resettableslider.h"
#include "interactivesampleloader.h"
#include "soundfontmanager.h"
#include "synth.h"
#include "tabmanager.h"
#include "utils.h"

// Refresh period of the level meter and the elapsed time (ms)
static const int REFRESH_PERIOD = 33;

// Sound detection: fixed level for the output of Polyphone, which has a real silence, and default level for an audio
// input, which always has a background noise (dB)
static const float OUTPUT_THRESHOLD = -90;
static const int DEFAULT_INPUT_THRESHOLD = -50;

// Messages on 2 lines on the right side of the record button: missing source (red) or record waiting (color of the
// banner)
enum StatusMessage { STATUS_NO_OUTPUT, STATUS_NO_INPUT, STATUS_WAITING_SOUND, STATUS_WAITING_MIDI, STATUS_COUNT };
static const char * const STATUS_MESSAGES[STATUS_COUNT] = {
    QT_TRANSLATE_NOOP("DialogRecorder", "No audio\noutput"),
    QT_TRANSLATE_NOOP("DialogRecorder", "No audio\ninput"),
    QT_TRANSLATE_NOOP("DialogRecorder", "Waiting\nfor a sound"),
    QT_TRANSLATE_NOOP("DialogRecorder", "Waiting\nfor a signal")
};

// Shortcuts working in all windows
static QKeySequence getRecordShortcut() { return QKeySequence(Qt::CTRL | Qt::Key_R); }
static QKeySequence getPauseShortcut() { return QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_R); }

DialogRecorder::DialogRecorder(QWidget *parent) :
    QDialog(parent, Qt::Tool | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint),
    ui(new Ui::DialogRecorder),
    _mode(MODE_OUTPUT),
    _isRecording(false),
    _isPaused(false),
    _isInHistory(false),
    _stateUpdate(true)
{
    ui->setupUi(this);
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);
    AudioDevice * audio = ContextManager::audio();
    _synth = audio->getSynth();
    _recorder = _synth->getRecorder();
    _midiRecorder = ContextManager::midi()->getRecorder();
    _filePlayer = _synth->getFilePlayer();
    connect(_filePlayer, SIGNAL(finished()), this, SLOT(onPlaybackFinished()), Qt::QueuedConnection);
    connect(_synth->getRecorder(), SIGNAL(interrupted(int)), this, SLOT(onInterrupted(int)), Qt::QueuedConnection);
    connect(audio->getInputRecorder(), SIGNAL(interrupted(int)), this, SLOT(onInterrupted(int)), Qt::QueuedConnection);
    connect(ContextManager::configuration(), SIGNAL(audioInputConfigurationChanged()), this, SLOT(onInputChanged()));
    connect(&_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));

    // Banner, with a flat list for the recording mode
    ThemeManager * theme = ContextManager::theme();
    ui->frameTop->setStyleSheet(theme->getBannerTheme() +
                                "QComboBox{combobox-popup:0;border:0;padding:0;background:transparent;color:" +
                                theme->getColor(ThemeManager::HIGHLIGHTED_TEXT).name() + "}" +
                                "QComboBox:hover{color:" + theme->getColor(ThemeManager::HIGHLIGHTED_TEXT, ThemeManager::HOVERED).name() + "}" +
                                "QComboBox::drop-down{border:0;width:14px}" +
                                "QComboBox::down-arrow{image:url(:/icons/arrow_down.svg);height:12px;width:12px}");
    ui->toolButtonSettings->setIcon(theme->getBannerIcon(":/icons/menu.svg", 24));
    ui->layoutTop->setAlignment(ui->layoutInfo, Qt::AlignVCenter); // Height of the other banners, the content centered
    initializeSettingsMenu();

    // Buttons, elapsed time and level on the background of the lists, which goes with the color of the banner used by
    // the messages
    QPalette palette = ui->frameContent->palette();
    palette.setColor(QPalette::Window, theme->getColor(ThemeManager::LIST_BACKGROUND));
    palette.setColor(QPalette::WindowText, theme->getColor(ThemeManager::LIST_TEXT));
    ui->frameContent->setPalette(palette);
    ui->frameContent->setAutoFillBackground(true);

    // The window is wide enough for the longest message on the right side of the record button, the elapsed time
    // keeping the size of its digits (as high as possible when the width of a QLCDNumber is at least
    // (6 * digitCount + 1) / 12 of its height). Without message, the elapsed time is centered.
    QFont font = ui->labelStatus->font();
    font.setBold(true);
    if (font.pointSizeF() > 0)
        font.setPointSizeF(0.9 * font.pointSizeF());
    ui->labelStatus->setFont(font);
    int messageWidth = 0;
    for (int i = 0; i < STATUS_COUNT; i++)
        messageWidth = qMax(messageWidth, ui->labelStatus->fontMetrics().size(0, tr(STATUS_MESSAGES[i])).width());
    int timeWidth = (6 * ui->lcdNumber->digitCount() + 1) * ui->pushRecord->minimumHeight() / 12 + 1;
    int rowWidth = ui->pushRecord->minimumWidth() + messageWidth + timeWidth + ui->pushPlayPause->minimumWidth() +
                   3 * ui->horizontalLayout->spacing();
    ui->layoutContent->addSpacerItem(new QSpacerItem(rowWidth, 0, QSizePolicy::Fixed, QSizePolicy::Fixed));
    ui->labelStatus->hide();
    DialogDecoration::apply(this, ui->frameTop);

    // History
    connect(ui->history, SIGNAL(listenRequested(QString)), this, SLOT(onListenRequested(QString)));
    connect(ui->history, SIGNAL(folderRequested(QString)), this, SLOT(onFolderRequested(QString)));
    connect(ui->history, SIGNAL(deleteRequested(QString)), this, SLOT(onDeleteRequested(QString)));
    connect(ui->history, SIGNAL(importRequested(QString)), this, SLOT(onImportRequested(QString)));

    // Shortcuts of the main window, the recorder being possibly hidden
    QShortcut * shortcut = new QShortcut(getRecordShortcut(), parent);
    shortcut->setContext(Qt::ApplicationShortcut);
    connect(shortcut, SIGNAL(activated()), this, SLOT(onRecordShortcut()));
    shortcut = new QShortcut(getPauseShortcut(), parent);
    shortcut->setContext(Qt::ApplicationShortcut);
    connect(shortcut, SIGNAL(activated()), this, SLOT(on_pushPlayPause_clicked()));

    // Last mode
    int mode = ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "recorder_mode", MODE_OUTPUT).toInt();
    ui->comboMode->setCurrentIndex(qBound(0, mode, 2));
    on_comboMode_activated(ui->comboMode->currentIndex());
}

DialogRecorder::~DialogRecorder()
{
    delete ui;
}

void DialogRecorder::initializeSettingsMenu()
{
    // Menu under the button on the right side of the banner
    ConfManager * configuration = ContextManager::configuration();
    BannerMenu * menu = new BannerMenu(ui->toolButtonSettings, this);
    QGridLayout * layout = menu->getGrid();

    // Automatic start and stop
    _checkAutoStart = menu->addCheckBox(tr("Start automatically"), 0, 3);
    _checkAutoStart->setChecked(configuration->getValue(ConfManager::SECTION_DISPLAY, "recorder_auto_start", false).toBool());
    _checkAutoStart->setToolTip(tr("The record button waits for a sound or a MIDI signal, and waits again after an automatic stop"));
    connect(_checkAutoStart, SIGNAL(toggled(bool)), this, SLOT(onAutoStartToggled(bool)));

    QLabel * labelAutoStopTitle = new QLabel(tr("Stop after a silence"));
    _sliderAutoStop = new ResettableSlider();
    _sliderAutoStop->setRange(0, 10);
    _sliderAutoStop->setPageStep(1);
    _sliderAutoStop->setDefaultValue(0);
    _sliderAutoStop->setValue(configuration->getValue(ConfManager::SECTION_DISPLAY, "recorder_auto_stop", 0).toInt());
    _labelAutoStop = new QLabel();

    // Level from which there is a sound, for an audio input
    _labelThresholdTitle = new QLabel(tr("Sound level"));
    _sliderThreshold = new ResettableSlider();
    _sliderThreshold->setRange(-70, -20);
    _sliderThreshold->setDefaultValue(DEFAULT_INPUT_THRESHOLD);
    _sliderThreshold->setValue(configuration->getValue(ConfManager::SECTION_DISPLAY, "recorder_threshold", DEFAULT_INPUT_THRESHOLD).toInt());
    _labelThreshold = new QLabel();
    foreach (QWidget * widget, QList<QWidget *>() << _labelThresholdTitle << _sliderThreshold << _labelThreshold)
        widget->setToolTip(tr("Level from which the audio input is considered as a sound, above its background noise"));

    QFontMetrics metrics(_labelAutoStop->font());
    int labelWidth = qMax(metrics.horizontalAdvance(tr("%1 s", "seconds").arg(10)), metrics.horizontalAdvance(tr("disabled")));
    labelWidth = qMax(labelWidth, metrics.horizontalAdvance("-70 " + tr("dB", "unit for decibels")));
    foreach (ResettableSlider * slider, QList<ResettableSlider *>() << _sliderAutoStop << _sliderThreshold)
    {
        slider->setOrientation(Qt::Horizontal);
        slider->setFocusPolicy(Qt::NoFocus);
        slider->setMinimumWidth(120);
    }
    foreach (QLabel * label, QList<QLabel *>() << _labelAutoStop << _labelThreshold)
    {
        label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        label->setMinimumWidth(labelWidth);
    }
    layout->addWidget(labelAutoStopTitle, 1, 0);
    layout->addWidget(_sliderAutoStop, 1, 1);
    layout->addWidget(_labelAutoStop, 1, 2);
    layout->addWidget(_labelThresholdTitle, 2, 0);
    layout->addWidget(_sliderThreshold, 2, 1);
    layout->addWidget(_labelThreshold, 2, 2);
    connect(_sliderAutoStop, SIGNAL(valueChanged(int)), this, SLOT(onAutoStopChanged(int)));
    connect(_sliderThreshold, SIGNAL(valueChanged(int)), this, SLOT(onThresholdChanged(int)));
    onAutoStopChanged(_sliderAutoStop->value());
    onThresholdChanged(_sliderThreshold->value());

    // File name and folder, common to all modes
    QCheckBox * checkAskFileName = menu->addCheckBox(tr("Ask for a file name"), 3, 3);
    checkAskFileName->setChecked(configuration->getValue(ConfManager::SECTION_DISPLAY, "recorder_ask_file_name", false).toBool());
    connect(checkAskFileName, SIGNAL(toggled(bool)), this, SLOT(onAskFileNameToggled(bool)));
    menu->addSeparator();
    connect(menu->addAction(tr("Change the folder...")), SIGNAL(triggered()), this, SLOT(onChangeFolderTriggered()));
}

void DialogRecorder::on_comboMode_activated(int index)
{
    this->stopPlayback();
    _recorder->setPeaksEnabled(false);
    _mode = static_cast<Mode>(index);
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "recorder_mode", index);
    _recorder = (_mode == MODE_INPUT ? ContextManager::audio()->getInputRecorder() : _synth->getRecorder());

    // The sound level only concerns an audio input
    foreach (QWidget * widget, QList<QWidget *>() << _labelThresholdTitle << _sliderThreshold << _labelThreshold)
        widget->setVisible(_mode == MODE_INPUT);

    ui->lcdNumber->display("00:00:00");
    ui->history->setImportDisplayed(_mode != MODE_MIDI);
    ui->history->setEmptyMessage(_mode == MODE_MIDI ? tr("No MIDI recording yet") : tr("No audio recording yet"));
    loadHistory();
    updateSource();
    updateInterface();
}

void DialogRecorder::updateSource()
{
    // The audio input is only open when it is displayed
    AudioDevice * audio = ContextManager::audio();
    if (_mode == MODE_INPUT && this->isVisible())
        audio->openInput();
    else
        audio->closeInput();

    if (_mode == MODE_MIDI)
        ui->levelMeter->showVelocities();
    else
    {
        ui->levelMeter->showAudioLevels(_mode == MODE_INPUT ? audio->getInputChannelCount() : 2);
        _recorder->setPeaksEnabled(this->isVisible());
    }
    updateSoundDetection();
}

void DialogRecorder::updateSoundDetection()
{
    float threshold = (_mode == MODE_INPUT ? _sliderThreshold->value() : OUTPUT_THRESHOLD);
    _recorder->setSoundDetection(std::pow(10.0f, threshold / 20.0f), static_cast<quint32>(_sliderAutoStop->value()));
}

void DialogRecorder::onAutoStartToggled(bool isChecked)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "recorder_auto_start", isChecked);
}

void DialogRecorder::onAutoStopChanged(int value)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "recorder_auto_stop", value);
    _labelAutoStop->setText(value == 0 ? tr("disabled") : tr("%1 s", "seconds").arg(value));
    updateSoundDetection();
}

void DialogRecorder::onThresholdChanged(int value)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "recorder_threshold", value);
    _labelThreshold->setText(QString::number(value) + " " + tr("dB", "unit for decibels"));
    updateSoundDetection();
}

void DialogRecorder::onAskFileNameToggled(bool isChecked)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "recorder_ask_file_name", isChecked);
}

void DialogRecorder::onChangeFolderTriggered()
{
    RecentFileManager * recentFile = ContextManager::recentFile();
    QString folder = QFileDialog::getExistingDirectory(this, tr("Folder of the recordings"),
                                                       recentFile->getLastDirectory(RecentFileManager::FILE_TYPE_RECORD));
    if (folder.isEmpty())
        return;

    // The next file is in this folder, with the same name
    QString name = QFileInfo(this->getDefaultPath()).fileName();
    recentFile->addRecentFile(RecentFileManager::FILE_TYPE_RECORD, folder + "/" + name);
}

bool DialogRecorder::isWaiting()
{
    return _isRecording && (_mode == MODE_MIDI ? _midiRecorder->isWaiting() : _recorder->isWaiting());
}

bool DialogRecorder::hasContent()
{
    return _mode == MODE_MIDI ? _midiRecorder->getNoteCount() > 0 : _recorder->getFrameCount() > 0;
}

void DialogRecorder::updateInterface()
{
    // The button on the left side starts or stops a record
    ThemeManager * theme = ContextManager::theme();
    QString shortcut = " (" + getRecordShortcut().toString(QKeySequence::NativeText) + ")";
    ui->pushRecord->setIcon(theme->getColoredSvg(_isRecording ? ":/icons/recorder_stop.svg" : ":/icons/recorder_record.svg",
                                                 QSize(32, 32), ThemeManager::LIST_TEXT));
    ui->pushRecord->setToolTip((_isRecording ? tr("Stop recording") : tr("Start recording")) + shortcut);

    // The button on the right side only pauses or resumes a record
    shortcut = " (" + getPauseShortcut().toString(QKeySequence::NativeText) + ")";
    ui->pushPlayPause->setIcon(theme->getColoredSvg(_isPaused ? ":/icons/recorder_play.svg" : ":/icons/recorder_pause.svg",
                                                    QSize(32, 32), ThemeManager::LIST_TEXT));
    ui->pushPlayPause->setToolTip((_isPaused ? tr("Resume") : tr("Pause")) + shortcut);

    // The mode doesn't change during a record
    ui->comboMode->setEnabled(!_isRecording);
    this->updateProgress();
}

void DialogRecorder::updateProgress()
{
    // There is nothing to pause as long as a record is waiting
    bool isWaiting = this->isWaiting();
    ui->pushPlayPause->setEnabled(_isRecording && !isWaiting);

    // State of the source on the right side of the record button, hidden when there is nothing to say (the elapsed time
    // being then centered)
    AudioDevice * audio = ContextManager::audio();
    int message = STATUS_COUNT;
    if (_mode == MODE_OUTPUT && !audio->isRunning())
        message = STATUS_NO_OUTPUT;
    else if (_mode == MODE_INPUT && !audio->isInputRunning())
        message = STATUS_NO_INPUT;
    else if (isWaiting)
        message = (_mode == MODE_MIDI ? STATUS_WAITING_MIDI : STATUS_WAITING_SOUND);
    QString status = (message < STATUS_COUNT ? tr(STATUS_MESSAGES[message]) : "");
    if (status != ui->labelStatus->text()) // Called periodically
    {
        ui->labelStatus->setText(status);
        ThemeManager * theme = ContextManager::theme();
        QColor color = (message >= STATUS_WAITING_SOUND) ?
                           theme->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND) :
                           theme->getFixedColor(ThemeManager::RED, ThemeManager::LIST_BACKGROUND);
        ui->labelStatus->setStyleSheet("QLabel{color:" + color.name() + "}");
        ui->labelStatus->setVisible(!status.isEmpty());
    }

    if (!_isRecording)
        return;

    // Elapsed time
    qint64 seconds = 0;
    if (_mode == MODE_MIDI)
        seconds = _midiRecorder->getDuration() / 1000000;
    else if (_recorder->getRecordSampleRate() != 0)
        seconds = _recorder->getFrameCount() / _recorder->getRecordSampleRate();
    ui->lcdNumber->display(QTime(0, 0).addSecs(static_cast<int>(seconds)).toString("HH:mm:ss"));

    // The file appears in the history as soon as it has a content
    if (!_isInHistory && this->hasContent())
    {
        _isInHistory = true;
        _files.prepend(_filePath);
        saveHistory();
        updateHistory();
        ui->history->setFirstFileBusy(true);
    }
    if (_isInHistory)
        ui->history->setFirstDetail(_mode == MODE_MIDI ? tr("Notes: %1").arg(_midiRecorder->getNoteCount()) :
                                                         Utils::getFormattedSize(_recorder->getFileSize()));
}

void DialogRecorder::onTimeout()
{
    if (_mode == MODE_MIDI)
    {
        ui->levelMeter->setVelocity(_midiRecorder->takeVelocity());

        // The silence is detected here for the MIDI signals, in the recorder for the sound
        qint64 stopDuration = _sliderAutoStop->value() * 1000000LL;
        if (_isRecording && stopDuration > 0 && _midiRecorder->getSilenceDuration() >= stopDuration)
        {
            this->stopAfterSilence();
            return;
        }
    }
    else
    {
        float left, right;
        _recorder->takePeaks(left, right);
        ui->levelMeter->setPeaks(left, right);
    }
    this->updateProgress();
}

void DialogRecorder::onInterrupted(int reason)
{
    if (!_isRecording)
        return;

    if (reason == Recorder::STOP_SILENCE)
    {
        this->stopAfterSilence();
        return;
    }
    this->stopRecord();

    QString reasonText;
    switch (reason)
    {
    case Recorder::STOP_WRITE_FAILED:
        reasonText = tr("The file cannot be written anymore: the disk may be full.");
        break;
    case Recorder::STOP_DISK_TOO_SLOW:
        reasonText = tr("The disk is too slow.");
        break;
    case Recorder::STOP_MAX_SIZE_REACHED:
        reasonText = tr("The audio file cannot exceed 4 GB.");
        break;
    case Recorder::STOP_SAMPLE_RATE_CHANGED:
        reasonText = tr("The sample rate of the audio output has changed.");
        break;
    }
    QMessageBox::warning(this, tr("Warning"), tr("The recording has been stopped and the audio recorded so far has been saved.") +
                                                  " " + reasonText);
}

void DialogRecorder::onInputChanged()
{
    if (_mode != MODE_INPUT)
        return;

    // The sample rate or the number of channels may change
    bool wasRecording = _isRecording;
    if (wasRecording)
        this->stopRecord();
    updateSource();
    updateInterface();
    if (wasRecording)
        QMessageBox::warning(this, tr("Warning"), tr("The recording has been stopped and the audio recorded so far has been saved.") +
                                                      " " + tr("The audio input has changed."));
}

void DialogRecorder::stopAfterSilence()
{
    // With the automatic start, the next record is awaited
    this->stopRecord();
    if (_checkAutoStart->isChecked())
        this->startRecord(true);
}

void DialogRecorder::startRecord(bool isChained)
{
    this->stopPlayback();

    // Nothing would be written without an audio stream
    AudioDevice * audio = ContextManager::audio();
    if (_mode == MODE_OUTPUT && !audio->isRunning())
    {
        QMessageBox::warning(this, tr("Warning"), tr("Nothing can be recorded without an audio output."));
        return;
    }
    if (_mode == MODE_INPUT && !audio->isInputRunning())
    {
        QMessageBox::warning(this, tr("Warning"), tr("Nothing can be recorded without an audio input, which is chosen in the settings."));
        return;
    }

    // File name, asked once for the records following each other
    QString filePath = this->getDefaultPath();
    if (!isChained && ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "recorder_ask_file_name", false).toBool())
    {
        QString extension = QFileInfo(filePath).suffix();
        filePath = QFileDialog::getSaveFileName(this, tr("Save a record"), filePath,
                                                (_mode == MODE_MIDI ? tr("MIDI file") : tr("Wav file")) + " (*." + extension + ")");
        if (filePath.isEmpty())
            return;
        if (QFileInfo(filePath).suffix().toLower() != extension)
            filePath.append("." + extension);
    }

    // Begin the record
    bool isWaiting = _checkAutoStart->isChecked();
    bool isStarted = false;
    switch (_mode)
    {
    case MODE_OUTPUT:
        isStarted = _recorder->startNewRecord(filePath, _synth->getSampleRate(), 2, isWaiting);
        break;
    case MODE_INPUT:
        isStarted = _recorder->startNewRecord(filePath, audio->getInputSampleRate(), audio->getInputChannelCount(), isWaiting);
        break;
    case MODE_MIDI:
        isStarted = _midiRecorder->startNewRecord(filePath, isWaiting);
        break;
    }
    if (!isStarted)
    {
        QMessageBox::warning(this, tr("Warning"), tr("Cannot write file \"%1\".").arg(QDir::toNativeSeparators(filePath)));
        return;
    }
    ContextManager::recentFile()->addRecentFile(RecentFileManager::FILE_TYPE_RECORD, filePath);
    _filePath = filePath;
    _isRecording = true;
    _isPaused = false;
    _isInHistory = false;
    ui->levelMeter->reset();
    ui->lcdNumber->display("00:00:00");
    this->updateInterface();
}

void DialogRecorder::stopRecord()
{
    // The last values are read before leaving the recording state
    this->updateProgress();

    bool isWritten = true;
    if (_mode == MODE_MIDI)
        isWritten = _midiRecorder->endRecord();
    else
    {
        _recorder->endRecord();

        // Nothing has been heard
        if (_recorder->getFrameCount() == 0)
            QFile::remove(_filePath);
    }
    bool hasContent = this->hasContent();
    _isRecording = false;
    _isPaused = false;

    // The first line is not the current record anymore
    ui->history->setFirstFileBusy(false);
    if (hasContent && !_isInHistory)
    {
        _files.prepend(_filePath);
        saveHistory();
    }
    _isInHistory = false;
    updateHistory();
    this->updateInterface();

    if (!isWritten)
        QMessageBox::warning(this, tr("Warning"), tr("Cannot write file \"%1\".").arg(QDir::toNativeSeparators(_filePath)));
}

bool DialogRecorder::confirmStop()
{
    if (!_isRecording)
        return true;

    // Nothing to lose as long as a record is waiting
    if (this->isWaiting())
    {
        this->stopRecord();
        return true;
    }

    QMessageBox msgBox(this);
    msgBox.setWindowTitle(tr("Warning"));
    msgBox.setText(tr("A recording is in progress. Do you want to stop it?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.button(QMessageBox::Yes)->setText(tr("&Yes"));
    msgBox.button(QMessageBox::No)->setText(tr("&No"));
    msgBox.setDefaultButton(QMessageBox::No);
    msgBox.setIcon(QMessageBox::Warning);
    if (msgBox.exec() == QMessageBox::Yes)
    {
        // Stopped now: the question is not asked again when the other windows are closed with the application
        this->stopRecord();
        return true;
    }

    // A button may have just been unchecked
    this->updateDisplayButtons(true);
    return false;
}

void DialogRecorder::onRecordShortcut()
{
    this->setVisible(true);
    this->on_pushRecord_clicked();
}

void DialogRecorder::on_pushRecord_clicked()
{
    if (_isRecording)
        this->stopRecord();
    else
        this->startRecord(false);
}

void DialogRecorder::on_pushPlayPause_clicked()
{
    if (_isRecording && !this->isWaiting())
    {
        _isPaused = !_isPaused;
        if (_mode == MODE_MIDI)
            _midiRecorder->pause(_isPaused);
        else
            _recorder->pause(_isPaused);
        this->updateInterface();
    }
}

void DialogRecorder::onListenRequested(QString filePath)
{
    // The MIDI player then uses the soundfonts that are open
    if (_mode == MODE_MIDI)
    {
        emit midiFileOpened(filePath);
        return;
    }

    // A second click stops the file, another file replaces it
    bool isSameFile = filePath == _playingFile;
    this->stopPlayback();
    if (isSameFile)
        return;

    if (!ContextManager::audio()->isRunning())
        QMessageBox::warning(this, tr("Warning"), tr("Nothing can be played without an audio output."));
    else if (!_filePlayer->start(filePath, _synth->getSampleRate()))
        QMessageBox::warning(this, tr("Warning"), tr("Cannot open the file"));
    else
    {
        _playingFile = filePath;
        ui->history->setPlayingFile(_playingFile);
    }
}

void DialogRecorder::onPlaybackFinished()
{
    this->stopPlayback();
}

void DialogRecorder::stopPlayback()
{
    _filePlayer->stop();
    _playingFile.clear();
    ui->history->setPlayingFile("");
}

void DialogRecorder::onFolderRequested(QString filePath)
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(filePath).path()));
}

void DialogRecorder::onDeleteRequested(QString filePath)
{
    QString fileName = QFileInfo(filePath).fileName();

    QMessageBox msgBox(this);
    msgBox.setWindowTitle(tr("Confirm deletion"));
    msgBox.setText(tr("Are you sure you want to delete file \"%1\"?").arg(fileName));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.button(QMessageBox::Yes)->setText(tr("&Yes"));
    msgBox.button(QMessageBox::No)->setText(tr("&No"));
    msgBox.setIcon(QMessageBox::Question);
    if (msgBox.exec() != QMessageBox::Yes)
        return;

    if (filePath == _playingFile)
        this->stopPlayback();
    if (QFile::moveToTrash(filePath))
    {
        _files.removeAll(filePath);
        saveHistory();
        updateHistory();
    }
    else
        QMessageBox::warning(this, tr("Warning"), tr("Cannot delete file \"%1\".").arg(fileName));
}

void DialogRecorder::onImportRequested(QString filePath)
{
    // In the soundfont displayed, the new samples being selected
    int sf2Index = TabManager::getInstance()->getCurrentSf2();
    if (sf2Index < 0)
        return;
    InteractiveSampleLoader loader(this);
    int replace = 0;
    IdList ids = loader.load(filePath, sf2Index, &replace);
    SoundfontManager::getInstance()->endEditing("command:newSmpl");
    if (!ids.isEmpty())
        TabManager::getInstance()->showElement(ids.first());
}

void DialogRecorder::onEditorDisplayed(bool isDisplayed)
{
    ui->history->setImportAvailable(isDisplayed);
}

void DialogRecorder::loadHistory()
{
    _files = ContextManager::configuration()->getValue(ConfManager::SECTION_RECENT_FILES, getHistoryKey(), QStringList()).toStringList();

    // The files can have been moved or deleted since the last time
    QStringList existingFiles;
    foreach (QString filePath, _files)
        if (QFile::exists(filePath))
            existingFiles << filePath;
    if (existingFiles != _files)
    {
        _files = existingFiles;
        saveHistory();
    }
    updateHistory();
}

void DialogRecorder::saveHistory()
{
    _files = _files.mid(0, RecorderHistory::LINE_COUNT);
    ContextManager::configuration()->setValue(ConfManager::SECTION_RECENT_FILES, getHistoryKey(), _files);
}

void DialogRecorder::updateHistory()
{
    QStringList details;
    foreach (QString filePath, _files)
        details << getDetail(filePath);
    ui->history->setFiles(_files, details);
}

QString DialogRecorder::getDetail(QString filePath)
{
    if (_mode != MODE_MIDI)
        return Utils::getFormattedSize(QFileInfo(filePath).size());

    // Number of notes of a MIDI file
    MidiFile file;
    if (!file.read(filePath))
        return "";
    int noteCount = 0;
    foreach (const MidiFile::Event &event, file.getEvents())
        if ((event.status & 0xF0) == 0x90)
            noteCount++;
    return tr("Notes: %1").arg(noteCount);
}

QString DialogRecorder::getDefaultPath()
{
    // Name of the last record without its number, in the same directory for the audio and MIDI files
    RecentFileManager * recentFile = ContextManager::recentFile();
    QString folderName = recentFile->getLastDirectory(RecentFileManager::FILE_TYPE_RECORD);
    QString name = QFileInfo(recentFile->getLastFile(RecentFileManager::FILE_TYPE_RECORD)).completeBaseName();
    if (name.isEmpty())
        name = tr("record");
    else
    {
        QRegularExpressionMatch match = QRegularExpression("^(.*)-[0-9][0-9]*$").match(name);
        if (match.hasMatch())
            name = match.captured(1);
    }

    // Find the first available name for a new file
    QString extension = (_mode == MODE_MIDI ? ".mid" : ".wav");
    QString filePath = folderName + "/" + name + extension;
    for (int suffix = 2; QFile(filePath).exists(); suffix++)
        filePath = folderName + "/" + name + "-" + QString::number(suffix) + extension;
    return filePath;
}

void DialogRecorder::updateDisplayButtons(bool isChecked)
{
    EditorToolBar::updateRecorderButtonsState(isChecked);
    Player::updateRecorderButtonsState(isChecked);
}

void DialogRecorder::showEvent(QShowEvent * event)
{
    QDialog::showEvent(event);
    if (_stateUpdate)
        this->updateDisplayButtons(true);

    // Restore the geometry (not working with Wayland)
    QByteArray geometry = ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "recorderGeometry", QByteArray()).toByteArray();
    if (!geometry.isEmpty())
        this->restoreGeometry(geometry);

    // The level is displayed as soon as the window is open
    loadHistory();
    updateSource();
    updateInterface();
    _timer.start(REFRESH_PERIOD);
}

void DialogRecorder::hideEvent(QHideEvent * event)
{
    QDialog::hideEvent(event);
    if (_stateUpdate)
        this->updateDisplayButtons(false);

    // Save the geometry
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "recorderGeometry", this->saveGeometry());

    // Stop the recording if needed
    if (_isRecording)
        this->stopRecord();

    this->stopPlayback();
    _timer.stop();
    _recorder->setPeaksEnabled(false);
    ContextManager::audio()->closeInput();
}

void DialogRecorder::closeEvent(QCloseEvent * event)
{
    if (this->confirmStop())
        QDialog::closeEvent(event);
    else
        event->ignore();
}

void DialogRecorder::keyPressEvent(QKeyEvent * event)
{
    // Prevent the escape key from closing the window
    if (event->key() != Qt::Key_Escape)
          QDialog::keyPressEvent(event);
}

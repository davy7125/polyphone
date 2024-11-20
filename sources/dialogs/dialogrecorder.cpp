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
#include <QTime>
#include <QFileInfo>
#include <QDir>
#include <QDesktopServices>
#include <QFileDialog>
#include <QKeyEvent>
#include "contextmanager.h"
#include "synth.h"
#include "editortoolbar.h"

DialogRecorder::DialogRecorder(QWidget *parent) :
    QDialog(parent, Qt::Tool | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint),
    ui(new Ui::DialogRecorder),
    _stateUpdate(true)
{
    ui->setupUi(this);
    _synth = ContextManager::audio()->getSynth();
    connect(_synth, SIGNAL(dataWritten(quint32,quint32)), this, SLOT(onDataWritten(quint32,quint32)));
    this->initialize();
}

DialogRecorder::~DialogRecorder()
{
    delete ui;
}

void DialogRecorder::initialize()
{
    // Initialize interface
    ui->lcdNumber->display("00:00:00");
    ui->pushRecord->setIcon(ContextManager::theme()->getColoredSvg(":/icons/recorder_record.svg", QSize(32, 32), ThemeManager::WINDOW_TEXT));
    ui->pushPlayPause->setEnabled(false);
    ui->pushPlayPause->setIcon(ContextManager::theme()->getColoredSvg(":/icons/recorder_play.svg", QSize(32, 32), ThemeManager::WINDOW_TEXT));

    // Initialize variables
    _isRecording = false;
    _isPaused = false;
    _currentSample = 0;
}

void DialogRecorder::onDataWritten(quint32 sampleRate, quint32 number)
{
    _currentSample += number;

    // Update display
    if (sampleRate != 0)
    {
        QTime time(0, 0);
        time = time.addMSecs(static_cast<int>(1000. * _currentSample / sampleRate));
        ui->lcdNumber->display(time.toString("HH:mm:ss"));
    }
}

void DialogRecorder::on_pushRecord_clicked()
{
    if (_isRecording)
    {
        // Stop recording
        this->initialize();
        _synth->endRecord();
    }
    else
    {
        // File name
        QString defaultPath = this->getDefaultPath();
        defaultPath = QFileDialog::getSaveFileName(this, tr("Save a record"),
                                                   defaultPath, tr("Wav file") + " (*.wav)");
        if (!defaultPath.isEmpty())
        {
            if (defaultPath.right(4).toLower() != ".wav")
                defaultPath.append(".wav");
            ContextManager::recentFile()->addRecentFile(RecentFileManager::FILE_TYPE_RECORD, defaultPath);

            // Begin the record
            ui->pushRecord->setIcon(ContextManager::theme()->getColoredSvg(":/icons/recorder_stop.svg", QSize(32, 32), ThemeManager::WINDOW_TEXT));
            ui->pushPlayPause->setIcon(ContextManager::theme()->getColoredSvg(":/icons/recorder_pause.svg", QSize(32, 32), ThemeManager::WINDOW_TEXT));
            _isPaused = false;
            _isRecording = true;
            _synth->startNewRecord(defaultPath);
            ui->pushPlayPause->setEnabled(true);
        }
    }
}

void DialogRecorder::on_pushPlayPause_clicked()
{
    if (_isRecording)
    {
        _isPaused = !_isPaused;
        ui->pushPlayPause->setIcon(ContextManager::theme()->getColoredSvg(
                                       _isPaused ? ":/icons/recorder_play.svg" : ":/icons/recorder_pause.svg",
                                       QSize(32, 32), ThemeManager::WINDOW_TEXT));
        _synth->pause(_isPaused);
    }
}

QString DialogRecorder::getDefaultPath()
{
    QString defaultPath = ContextManager::recentFile()->getLastFile(RecentFileManager::FILE_TYPE_RECORD);
    QFileInfo info(defaultPath);
    if (info.dir().exists() && defaultPath.size())
    {
        QString name = info.fileName();
        QRegularExpressionMatch match = QRegularExpression("^(.*)-[0-9][0-9]*.[wW][aA][vV]$").match(name);
        if (match.hasMatch())
            name = match.captured(1);
        else
            name = name.left(name.size() - 4);

        // Find the first available name for a new file
        QString folderName = info.dir().path();
        if (QFile(folderName + "/" + name + ".wav").exists())
        {
            int suffix = 2;
            while (QFile(folderName + "/" + name + "-" + QString::number(suffix) + ".wav").exists())
                suffix++;
            defaultPath = folderName + "/" + name + "-" + QString::number(suffix);
        }
        else
            defaultPath = folderName + "/" + name;
    }
    else
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        defaultPath = QDir::home().path() + "/" + tr("record");
#else
        defaultPath = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation) + "/" + tr("record");
#endif

    return defaultPath + ".wav";
}

void DialogRecorder::showEvent(QShowEvent * event)
{
    QDialog::showEvent(event);
    if (_stateUpdate)
        EditorToolBar::updateRecorderButtonsState(true);

    // Restore the geometry (not working with Wayland)
    QByteArray geometry = ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "recorderGeometry", QByteArray()).toByteArray();
    if (!geometry.isEmpty())
        this->restoreGeometry(geometry);
}

void DialogRecorder::hideEvent(QHideEvent * event)
{
    QDialog::hideEvent(event);
    if (_stateUpdate)
        EditorToolBar::updateRecorderButtonsState(false);

    // Save the geometry
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "recorderGeometry", this->saveGeometry());

    // Stop the recording if needed
    if (_isRecording)
        _synth->endRecord();
    this->initialize();
}

void DialogRecorder::keyPressEvent(QKeyEvent * event)
{
    // Prevent the escape key from closing the window
    if (event->key() != Qt::Key_Escape)
          QDialog::keyPressEvent(event);
}

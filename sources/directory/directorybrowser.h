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

#ifndef DIRECTORYBROWSER_H
#define DIRECTORYBROWSER_H

#include "tab.h"
#include "basetypes.h"
class QFileSystemWatcher;
class QPushButton;
class DialogKeyboard;
class DirectoryPreview;

namespace Ui {
class DirectoryBrowser;
}

class DirectoryBrowser : public Tab
{
    Q_OBJECT

public:
    // Without keyboard (player mode), the elements cannot be played (the files selected can still be played by the MIDI
    // player)
    DirectoryBrowser(DialogKeyboard * dialogKeyboard, QWidget *parent = nullptr);
    ~DirectoryBrowser();

    void initialize(QString dirPath);
    QString getDirectoryPath() { return _dirPath; }
    DirectoryPreview * getPreview() { return _preview; }

    void onActionRequired(TabAction action) override;

signals:
    void itemDoubleClicked(QString filePath, EltID id);
    void keyboardDisplayChanged(bool isDisplayed);
    void recorderDisplayChanged(bool isDisplayed);
    void midiPlayerDisplayChanged(bool isDisplayed);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void showEvent(QShowEvent * event) override;
    void hideEvent(QHideEvent * event) override;

private slots:
    void on_pushRetry_clicked();
    void onDirectoryChanged(const QString &path);
    void onContentChanged();
    void onSelectionChanged(QItemSelection selected, QItemSelection deselected);
    void onFileDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void onSmplSelectionChanged(QItemSelection selected, QItemSelection deselected);
    void onInstSelectionChanged(QItemSelection selected, QItemSelection deselected);
    void onPrstSelectionChanged(QItemSelection selected, QItemSelection deselected);
    void on_listView_activated(const QModelIndex &index);
    void onRenameRequested(QString path);
    void onDeleteRequested(QString path);
    void on_pushShowSamples_toggled(bool checked);
    void on_pushShowInstruments_toggled(bool checked);
    void on_pushShowPresets_toggled(bool checked);
    void on_pushShowKeyboard_clicked();
    void on_pushShowRecorder_clicked();
    void on_pushShowMidiPlayer_clicked();

private:
    void updateVerticalSpacer();
    void updateShowIcon(QPushButton * button);
    void setPreviewElement(EltID id);
    void focusFileList();

    Ui::DirectoryBrowser *ui;
    QString _dirPath;
    QFileSystemWatcher * _watcher;
    QMap<QString, QDateTime> _currentFiles;
    bool _hasKeyboard;
    DirectoryPreview * _preview;
};

#endif // DIRECTORYBROWSER_H

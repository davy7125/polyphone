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

#ifndef DIRECTORYPREVIEW_H
#define DIRECTORYPREVIEW_H

#include <QObject>
#include <QMutex>
#include "basetypes.h"
#include "imidilistener.h"
class AbstractInputParser;
class DialogKeyboard;
class QTimer;

// Soundfont of the file selected in the soundfont explorer, loaded in the background without tab (temporary soundfont) and
// kept until another file is selected. It is used by:
// * the virtual keyboard or a MIDI keyboard, playing the element selected (not in player mode): the soundfont is loaded at
//   the first selection of an element,
// * the MIDI player, whose channels can play the file selected: the soundfont is loaded shortly after the selection of the
//   file (files browsed quickly are not loaded) while the MIDI player asks for it.
class DirectoryPreview : public QObject, public IMidiListener
{
    Q_OBJECT

public:
    // State of the file selected, for the MIDI player
    enum FileState
    {
        FILE_NONE, // No file, or a file that cannot be loaded
        FILE_PENDING, // File being scanned or loaded
        FILE_READY // Soundfont loaded
    };

    // Without keyboard (player mode), the elements are not played. They are played only when "widget" (the explorer) is
    // visible.
    DirectoryPreview(DialogKeyboard * dialogKeyboard, QWidget * widget);
    ~DirectoryPreview() override;

    // File containing the elements (empty if none), the soundfont of the previous file being released. The file can be
    // loaded once it has been scanned successfully.
    void setFile(QString filePath, bool isScanning, bool isLoadable);
    QString getFile() { return _filePath; }

    // State of the file for the MIDI player, with the index of its soundfont when it is ready
    FileState getFileState(int &sf2Index);

    // The files selected are loaded while the MIDI player needs them
    static void setFileLoadingRequested(bool isRequested);

    // Element selected (elementUnknown if none), the soundfont being loaded at the first selection
    void setElement(EltID id);

    // Release the soundfont, loaded again if needed (for instance if the file changed)
    void release();

    // Called when the explorer is shown or hidden
    void onShown();
    void onHidden();

    // MIDI signals (thread 2)
    bool processKey(int channel, int key, int vel) override;

signals:
    void processKeyMainThread(int key, int vel);
    void fileStateChanged();

private slots:
    void onInputProcessed();
    void onProcessKeyMainThread(int key, int vel);
    void onSoundfontClosed(int indexSf2);
    void onLoadTimeout();

private:
    // A file that cannot be loaded (should not happen since it has been scanned) is not loaded again until another
    // file is selected, nothing is played
    enum Status
    {
        NOT_LOADED,
        LOADING,
        LOADED,
        FAILED
    };

    void load();
    void releaseSoundfont();
    void scheduleLoading();
    void setStatus(Status status);
    void updatePlayedElement();
    void updateKeyboardAndSynth();
    void releaseNotes();
    EltID getPlayedElement();

    DialogKeyboard * _dialogKeyboard; // Null in player mode
    QWidget * _widget;
    QString _filePath;
    bool _isScanning;
    bool _isLoadable;
    EltID _element;
    Status _status;
    AbstractInputParser * _parser; // Not null while the file is loaded
    int _sf2Index; // Soundfont loaded here or already open in a tab
    bool _isHeld; // True if the soundfont has been loaded here (temporary soundfont)
    QTimer * _loadTimer; // Loading for the MIDI player, once the selection is stable

    // Element played, also read by the MIDI thread
    QMutex _mutex;
    EltID _playedElement;

    static QList<DirectoryPreview *> s_instances;
    static bool s_isFileLoadingRequested;
    static const int LOADING_DELAY_MS;
};

#endif // DIRECTORYPREVIEW_H

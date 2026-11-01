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

// Element selected in the soundfont explorer, played with the virtual keyboard or a MIDI keyboard
// Its soundfont is loaded in the background without tab (temporary soundfont) and kept until another file is selected
class DirectoryPreview : public QObject, public IMidiListener
{
    Q_OBJECT

public:
    // The element is played only when "widget" (the explorer) is visible
    DirectoryPreview(DialogKeyboard * dialogKeyboard, QWidget * widget);
    ~DirectoryPreview() override;

    // File containing the elements (empty if none), the soundfont of the previous file being released
    void setFile(QString filePath);
    QString getFile() { return _filePath; }

    // Element selected (elementUnknown if none), the soundfont being loaded at the first selection
    void setElement(EltID id);

    // Release the soundfont, loaded again at the next selection (for instance if the file changed)
    void release();

    // Called when the explorer is shown or hidden
    void onShown();
    void onHidden();

    // MIDI signals (thread 2)
    bool processKey(int channel, int key, int vel) override;

signals:
    void processKeyMainThread(int key, int vel);

private slots:
    void onInputProcessed();
    void onProcessKeyMainThread(int key, int vel);
    void onSoundfontLoaded(int indexSf2);
    void onSoundfontClosed(int indexSf2);

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
    void setStatus(Status status);
    void updatePlayedElement();
    void updateKeyboardAndSynth();
    void releaseNotes();
    EltID getPlayedElement();

    DialogKeyboard * _dialogKeyboard;
    QWidget * _widget;
    QString _filePath;
    EltID _element;
    Status _status;
    AbstractInputParser * _parser; // Not null while the file is loaded
    int _sf2Index; // Soundfont loaded here or already open in a tab
    bool _isTemporary; // True if the soundfont has been loaded here (and must be deleted)

    // Element played, also read by the MIDI thread
    QMutex _mutex;
    EltID _playedElement;

    static const int DELETION_DELAY_MS;
};

#endif // DIRECTORYPREVIEW_H

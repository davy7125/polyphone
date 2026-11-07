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

#ifndef MIDIPLAYERSELECTION_H
#define MIDIPLAYERSELECTION_H

#include <QObject>
#include <QList>
#include <thread>
#include <atomic>

// Soundfont played by the channels of the MIDI player set to "Selection": the soundfont of the current tab, or the file
// selected in the soundfont explorer displayed (loaded without tab). With other tabs, the soundfont played is kept.
// A new soundfont is played once the samples of the presets used by the channels are loaded (by another thread), the
// previous one being played meanwhile.
class MidiPlayerSelection : public QObject
{
    Q_OBJECT

public:
    MidiPlayerSelection();
    ~MidiPlayerSelection() override;

    // The selection is followed while the window is visible and channels are set to "Selection"
    void setActive(bool isActive);

    // Programs used by these channels (bank * 128 + preset), whose samples are loaded beforehand
    void setPrograms(QList<int> programs);

    // Soundfont played (-1: none), and name of the selection (the soundfont being loaded, otherwise the one played)
    int getSoundfont() { return _sf2Index; }
    QString getName();

    // True while the soundfont selected is being scanned or loaded, or while its samples are loaded
    bool isLoading() { return _isPending || _loadingSf2Index != -1; }

    // Possibly another selection (tab, file of the soundfont explorer, end of the loading of a soundfont)
    void update();

    // The soundfont played is possibly closed
    void onSoundfontClosed(int sf2Index);

signals:
    // The channels set to "Selection" play another soundfont
    void soundfontChanged();

    // Name or loading state of the selection
    void stateChanged();

private:
    void startLoading(int sf2Index, QString fileName);
    void stopLoading();
    void onSamplesLoaded(int loadingId);
    bool areSamplesLoaded(int sf2Index);
    void setSoundfont(int sf2Index, QString fileName);
    void setName(int sf2Index, QString fileName);

    bool _isActive;
    QList<int> _programs;

    // Soundfont played (held if it has been loaded by the soundfont explorer), with the name of its file if it comes from
    // the soundfont explorer
    int _sf2Index;
    QString _fileName;

    // Name displayed: name of a file or, if empty, name of a soundfont
    int _nameSf2Index;
    QString _nameFileName;

    // File selected being scanned or loaded, its soundfont not being known yet
    bool _isPending;

    // Loading of samples, with the programs whose samples are loaded for a soundfont
    int _loadingSf2Index;
    QString _loadingFileName;
    QList<int> _loadingPrograms;
    int _loadingId;
    std::thread _thread;
    std::atomic<bool> _isCanceled;
    int _loadedSf2Index;
    QList<int> _loadedPrograms;
};

#endif // MIDIPLAYERSELECTION_H

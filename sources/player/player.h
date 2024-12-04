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

#ifndef PLAYER_H
#define PLAYER_H

#include <QItemSelection>
#include "tab.h"
#include "basetypes.h"
class AbstractInputParser;
class PlayerOptions;
class Synth;

namespace Ui {
class Player;
}

class Player : public Tab
{
    Q_OBJECT

public:
    Player(PlayerOptions * playerOptions, QWidget * parent = nullptr);
    ~Player();

    // MIDI signals
    bool processKey(int channel, int key, int vel) override;
    bool processPolyPressureChanged(int channel, int key, int pressure) override { Q_UNUSED(channel); Q_UNUSED(key); Q_UNUSED(pressure); return false; }
    bool processMonoPressureChanged(int channel, int value) override { Q_UNUSED(channel); Q_UNUSED(value); return false; }
    bool processControllerChanged(int channel, int num, int value) override { Q_UNUSED(channel); Q_UNUSED(num); Q_UNUSED(value); return false; }
    bool processBendChanged(int channel, float value) override { Q_UNUSED(channel); Q_UNUSED(value); return false; }
    bool processBendSensitivityChanged(int channel, float semitones) override { Q_UNUSED(channel); Q_UNUSED(semitones); return false; }
    bool processProgramChanged(int channel, quint16 bank, quint8 preset) override;

    static void updateRecorderButtonsState(bool isChecked);

protected:
    void tabInitializing(QString filename) override;
    void tabInError(QString errorMessage) override;
    void tabInitialized(int indexSf2) override;
    void tabUpdate(QString editingSource) override;

private slots:
    void onBankSelectionChanged(QItemSelection selected, QItemSelection deselected);
    void onPresetSelectionChanged(QItemSelection selected, QItemSelection deselected);
    void on_comboChannel_currentIndexChanged(int index);
    void on_comboMultipleSelection_currentIndexChanged(int index);
    void on_comboSelectionByKeys_currentIndexChanged(int index);
    void on_pushShowRecorder_clicked();

private:
    void customizeKeyboard();
    void scanPreset(EltID presetId, bool * keys);
    QVector<bool> getEnabledKeysForInstrument(EltID idInst);
    void stopPresetKey(EltID presetId, int key);

    Ui::Player *ui;
    PlayerOptions * _playerOptions;
    Synth * _synth;
    bool _initializing;
    int _currentSoundfontId;
    QList<EltID> _currentIds;
    int _bankPositionByBankNumber[16384];
    int _presetPositionByPresetNumber[128];
    int _currentKeyVelocities[128];
    QMap<int, QVector<bool> > _rangeByInst;

    static QList<Player *> s_instances;
    static bool s_recorderOpen;
};

#endif // PLAYER_H

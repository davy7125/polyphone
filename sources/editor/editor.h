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

#ifndef EDITOR_H
#define EDITOR_H

#include "tab.h"
#include "basetypes.h"

class PageSelector;
class AbstractInputParser;
class DialogKeyboard;

namespace Ui {
class Editor;
}

class Editor : public Tab
{
    Q_OBJECT

public:
    Editor(DialogKeyboard * dialogKeyboard);
    ~Editor() override;

    // MIDI signals
    bool processKey(int channel, int key, int vel) override;
    bool processPolyPressureChanged(int channel, int key, int pressure) override { Q_UNUSED(channel); Q_UNUSED(key); Q_UNUSED(pressure); return false; }
    bool processMonoPressureChanged(int channel, int value) override { Q_UNUSED(channel); Q_UNUSED(value); return false; }
    bool processControllerChanged(int channel, int num, int value) override { Q_UNUSED(channel); Q_UNUSED(num); Q_UNUSED(value); return false; }
    bool processBendChanged(int channel, float value) override { Q_UNUSED(channel); Q_UNUSED(value); return false; }
    bool processBendSensitivityChanged(int channel, float semitones) override { Q_UNUSED(channel); Q_UNUSED(semitones); return false; }
    bool processProgramChanged(int channel, quint16 bank, quint8 preset) override { Q_UNUSED(channel); Q_UNUSED(bank); Q_UNUSED(preset); return false; }

signals:
    void processKeyMainThread(int channel, int key, int vel);

protected:
    void showEvent(QShowEvent* event) override;
    void tabInitializing(QString filename) override;
    void tabInError(QString errorMessage) override;
    void tabInitialized(int indexSf2) override;
    void tabUpdate(QString editingSource) override;

    DialogKeyboard * _dialogKeyboard;

private slots:
    void onProcessKeyMainThread(int channel, int key, int vel);
    void onSelectionChanged(IdList ids);
    void displayOptionChanged(int displayOption);
    void customizeKeyboard();
    void onEditingDone(QString editingSource, QList<int> sf2Indexes);
    void onErrorEncountered(QString text);
    void onRootKeyChanged(int rootKey);
    void onPageHidden();

private:
    QVector<bool> getEnabledKeysForInstrument(EltID idInst);

    Ui::Editor *ui;
    PageSelector * _pageSelector;
    ElementType _currentElementType;
    IdList _currentIds;
};

#endif // EDITOR_H

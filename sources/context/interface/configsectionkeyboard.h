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

#ifndef CONFIGSECTIONKEYBOARD_H
#define CONFIGSECTIONKEYBOARD_H

#include <QWidget>
#include <QStringList>
#include "temperamentsettings.h"
#include "imidilistener.h"

namespace Ui {
class ConfigSectionKeyboard;
}

class ConfigSectionKeyboard : public QWidget, IMidiListener
{
    Q_OBJECT

public:
    explicit ConfigSectionKeyboard(QWidget *parent = nullptr);
    ~ConfigSectionKeyboard();
    void initialize();

    bool processRPNChanged(int channel, int parameter, int value, bool isRegistered, int trigger) override;

private slots:
    void on_comboFirstC_currentIndexChanged(int index);
    void initializeFirstC();
    void on_pushDefaultReferencePitch_clicked();
    void on_comboTemperament_currentIndexChanged(int index);
    void on_comboTemperamentRelativeKey_currentIndexChanged(int index);
    void on_pushDefaultTemperament_clicked();
    void onTemperamentEdited();
    void onTonalityRequested(int key);
    void on_comboTransposition_currentIndexChanged(int index);
    void on_checkPlayUnusedNotes_toggled(bool checked);
    void onSoundEngineConfigurationChanged();
    void updateTonalityNames();

private:
    void renameComboFirstC();
    void adjustTableHeight();
    int lastProgramIndex() const;
    void saveTemperament();
    void updateTemperamentWidgets();

    Ui::ConfigSectionKeyboard *ui;
    int _currentTuningBank;
    TemperamentSettings _temperament;
    bool _isSaving; // The configuration is being changed here
};

#endif // CONFIGSECTIONKEYBOARD_H

/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2017 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://polyphone-soundfonts.com                      **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef CONFIGPANEL_H
#define CONFIGPANEL_H

#include <QMessageBox>
#include <QWidget>
#include <QString>
#include "pianokeybdcustom.h"
#include "contextmanager.h"

namespace Ui
{
class ConfigPanel;
}

class ConfigPanel : public QWidget
{
    Q_OBJECT
    
public:
    ConfigPanel(QWidget *parent = NULL);
    ~ConfigPanel();

    // Initialize the interface with the parameters values
    void initializeInterface();

    // Initialisation
    void setVolume(int val);

private slots:
    void setAudioOutput(int index);
    void setWavAutoLoop(bool checked);
    void setWavRemoveBlank(bool checked);
    void setNumPortMidi(int val);
    void setSynthGain(int val);
    void on_dialRevNiveau_valueChanged(int value);
    void on_dialRevProfondeur_valueChanged(int value);
    void on_dialRevDensite_valueChanged(int value);
    void on_dialRevAttenuation_valueChanged(int value);
    void on_dialChoNiveau_valueChanged(int value);
    void on_dialChoAmplitude_valueChanged(int value);
    void on_dialChoFrequence_valueChanged(int value);
    void combinaisonChanged(int key, int numOctave, QString combinaison);
    void on_checkRepercussionStereo_clicked();
    void on_comboBufferSize_activated(int index);
    void on_comboKeyName_currentIndexChanged(int index);
    void on_comboDo_currentIndexChanged(int index);
    void on_spinDefaultVelocity_editingFinished();
    void on_comboLangue_currentIndexChanged(int index);
    void fillColors();
    void on_comboColorTheme_currentIndexChanged(int index);
    void on_pushColorWindowBackground_clicked();
    void on_pushColorButtonBackground_clicked();
    void on_pushColorSelectionBackground_clicked();
    void on_pushColorListBackground_clicked();
    void on_pushColorListAlternativeBackground_clicked();
    void on_pushColorWindowText_clicked();
    void on_pushColorButtonText_clicked();
    void on_pushColorSelectionText_clicked();
    void on_pushColorListText_clicked();

private:
    Ui::ConfigPanel * ui;

    // Misc
    bool _loaded;

    void renameComboDo();
    void initComboLanguage();
};

#endif // CONFIGPANEL_H

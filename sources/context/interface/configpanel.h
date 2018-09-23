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

    // Mapping
    QString getKeyMapped(int numOctave, PianoKeybd::Key key)
    {
        QString txt = "";
        switch (numOctave)
        {
        case 0:
            switch (key)
            {
            case PianoKeybd::KEY_C_LEFT:  txt = trUtf8("w"); break;
            case PianoKeybd::KEY_C_SHARP: txt = trUtf8("s"); break;
            case PianoKeybd::KEY_D:       txt = trUtf8("x"); break;
            case PianoKeybd::KEY_D_SHARP: txt = trUtf8("d"); break;
            case PianoKeybd::KEY_E:       txt = trUtf8("c"); break;
            case PianoKeybd::KEY_F:       txt = trUtf8("v"); break;
            case PianoKeybd::KEY_F_SHARP: txt = trUtf8("g"); break;
            case PianoKeybd::KEY_G:       txt = trUtf8("b"); break;
            case PianoKeybd::KEY_G_SHARP: txt = trUtf8("h"); break;
            case PianoKeybd::KEY_A:       txt = trUtf8("n"); break;
            case PianoKeybd::KEY_A_SHARP: txt = trUtf8("j"); break;
            case PianoKeybd::KEY_B:       txt = trUtf8(","); break;
            case PianoKeybd::KEY_C_RIGHT: txt = trUtf8(";"); break;
            }
            break;
        case 1:
            switch (key)
            {
            case PianoKeybd::KEY_C_LEFT:  txt = trUtf8("a"); break;
            case PianoKeybd::KEY_C_SHARP: txt = trUtf8("é"); break;
            case PianoKeybd::KEY_D:       txt = trUtf8("z"); break;
            case PianoKeybd::KEY_D_SHARP: txt = trUtf8("\""); break;
            case PianoKeybd::KEY_E:       txt = trUtf8("e"); break;
            case PianoKeybd::KEY_F:       txt = trUtf8("r"); break;
            case PianoKeybd::KEY_F_SHARP: txt = trUtf8("("); break;
            case PianoKeybd::KEY_G:       txt = trUtf8("t"); break;
            case PianoKeybd::KEY_G_SHARP: txt = trUtf8("-"); break;
            case PianoKeybd::KEY_A:       txt = trUtf8("y"); break;
            case PianoKeybd::KEY_A_SHARP: txt = trUtf8("è"); break;
            case PianoKeybd::KEY_B:       txt = trUtf8("u"); break;
            case PianoKeybd::KEY_C_RIGHT: txt = trUtf8("i"); break;
            }
            break;
        case 2:
            switch (key)
            {
            case PianoKeybd::KEY_C_LEFT:  txt = trUtf8("Shift+w"); break;
            case PianoKeybd::KEY_C_SHARP: txt = trUtf8("Shift+s"); break;
            case PianoKeybd::KEY_D:       txt = trUtf8("Shift+x"); break;
            case PianoKeybd::KEY_D_SHARP: txt = trUtf8("Shift+d"); break;
            case PianoKeybd::KEY_E:       txt = trUtf8("Shift+c"); break;
            case PianoKeybd::KEY_F:       txt = trUtf8("Shift+v"); break;
            case PianoKeybd::KEY_F_SHARP: txt = trUtf8("Shift+g"); break;
            case PianoKeybd::KEY_G:       txt = trUtf8("Shift+b"); break;
            case PianoKeybd::KEY_G_SHARP: txt = trUtf8("Shift+h"); break;
            case PianoKeybd::KEY_A:       txt = trUtf8("Shift+n"); break;
            case PianoKeybd::KEY_A_SHARP: txt = trUtf8("Shift+j"); break;
            case PianoKeybd::KEY_B:       txt = trUtf8("Shift+?"); break;
            case PianoKeybd::KEY_C_RIGHT: txt = trUtf8("Shift+."); break;
            }
            break;
        case 3:
            switch (key)
            {
            case PianoKeybd::KEY_C_LEFT:  txt = trUtf8("Shift+a"); break;
            case PianoKeybd::KEY_C_SHARP: txt = trUtf8("Shift+2"); break;
            case PianoKeybd::KEY_D:       txt = trUtf8("Shift+z"); break;
            case PianoKeybd::KEY_D_SHARP: txt = trUtf8("Shift+3"); break;
            case PianoKeybd::KEY_E:       txt = trUtf8("Shift+e"); break;
            case PianoKeybd::KEY_F:       txt = trUtf8("Shift+r"); break;
            case PianoKeybd::KEY_F_SHARP: txt = trUtf8("Shift+5"); break;
            case PianoKeybd::KEY_G:       txt = trUtf8("Shift+t"); break;
            case PianoKeybd::KEY_G_SHARP: txt = trUtf8("Shift+6"); break;
            case PianoKeybd::KEY_A:       txt = trUtf8("Shift+y"); break;
            case PianoKeybd::KEY_A_SHARP: txt = trUtf8("Shift+7"); break;
            case PianoKeybd::KEY_B:       txt = trUtf8("Shift+u"); break;
            case PianoKeybd::KEY_C_RIGHT: txt = trUtf8("Shift+i"); break;
            }
            break;
        default:
            break;
        }
        return ContextManager::configuration()->getValue(
                    ConfManager::SECTION_MAP, "key_" + QString::number(numOctave) + "_" + QString::number((int)key), txt).toString();
    }

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

/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef CONFIGSECTIONGENERAL_H
#define CONFIGSECTIONGENERAL_H

#include <QWidget>

namespace Ui {
class ConfigSectionGeneral;
}

class ConfigSectionGeneral : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigSectionGeneral(QWidget *parent = nullptr);
    ~ConfigSectionGeneral();

    void initialize();

private slots:
    void on_comboAudioOuput_currentIndexChanged(int index);
    void on_comboBufferSize_currentIndexChanged(int index);
    void on_comboMidiInput_currentIndexChanged(int index);

    void on_checkBoucle_toggled(bool checked);
    void on_checkBlanc_toggled(bool checked);
    void on_checkRepercussionStereo_toggled(bool checked);

private:
    void initializeAudio();
    void initializeMidi();

    Ui::ConfigSectionGeneral *ui;
};

#endif // CONFIGSECTIONGENERAL_H

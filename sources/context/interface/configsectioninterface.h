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

#ifndef CONFIGSECTIONINTERFACE_H
#define CONFIGSECTIONINTERFACE_H

#include <QWidget>

namespace Ui {
class ConfigSectionInterface;
}

class ConfigSectionInterface : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigSectionInterface(QWidget *parent = nullptr);
    ~ConfigSectionInterface();

    void initialize();

private slots:
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
    void on_comboLangue_currentIndexChanged(int index);
    void on_comboKeyName_currentIndexChanged(int index);
    void on_comboSortDivisions_currentIndexChanged(int index);

    void on_checkDecorations_clicked();

private:
    void initComboLanguage();
    void fillColors();

    Ui::ConfigSectionInterface *ui;
};

#endif // CONFIGSECTIONINTERFACE_H

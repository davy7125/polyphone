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

#ifndef TOOLCHANGEATTENUATION_GUI_H
#define TOOLCHANGEATTENUATION_GUI_H

#include "abstracttoolgui.h"

namespace Ui {
class ToolChangeAttenuation_gui;
}

class ToolChangeAttenuation_gui : public AbstractToolGui
{
    Q_OBJECT

public:
    explicit ToolChangeAttenuation_gui(QWidget *parent = nullptr);
    ~ToolChangeAttenuation_gui();

    /// Update the interface with the parameters
    void updateInterface(AbstractToolParameters * parameters, IdList ids) override;

    /// Save the parameters based on the interface
    void saveParameters(AbstractToolParameters * parameters) override;

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    static QString formatDouble(double value);

    bool _isInst;
    Ui::ToolChangeAttenuation_gui *ui;
};

#endif // TOOLCHANGEATTENUATION_GUI_H

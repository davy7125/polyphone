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

#ifndef TOOLMERGESAMPLES_GUI_H
#define TOOLMERGESAMPLES_GUI_H

#include "abstracttoolgui.h"

namespace Ui {
class ToolMergeSamples_gui;
}

class ToolMergeSamples_gui : public AbstractToolGui
{
    Q_OBJECT

public:
    explicit ToolMergeSamples_gui(QWidget *parent = nullptr);
    ~ToolMergeSamples_gui() override;

    /// Update the interface with the parameters
    void updateInterface(AbstractToolParameters * parameters, IdList ids) override;

    /// Save the parameters based on the interface
    void saveParameters(AbstractToolParameters * parameters) override;

private slots:
    void on_pushCancel_clicked();
    void on_pushOk_clicked();

private:
    Ui::ToolMergeSamples_gui *ui;
};

#endif // TOOLMERGESAMPLES_GUI_H

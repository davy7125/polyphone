/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2026 Davy Triponney                                **
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

#ifndef TOOLMISSINGSAMPLES_GUI_H
#define TOOLMISSINGSAMPLES_GUI_H

#include "abstracttoolgui.h"

namespace Ui {
class ToolMissingSamples_gui;
}

class ToolMissingSamples_gui : public AbstractToolGui
{
    Q_OBJECT

public:
    explicit ToolMissingSamples_gui(QWidget *parent = nullptr);
    ~ToolMissingSamples_gui() override;

    /// Update the interface with the parameters
    void updateInterface(AbstractToolParameters * parameters, IdList ids) override;

    /// Save the parameters based on the interface
    void saveParameters(AbstractToolParameters * parameters) override;

    /// Keys from a text such as "61, 63, 84-96" (numbers or names), false if a part is not valid (stored in "error")
    static bool parseKeys(QString text, QList<int> &keys, QString &error);

    /// Text of a list of keys, with ranges for the consecutive keys
    static QString keysToText(QList<int> keys);

private slots:
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
    void updateEnabledState();

private:
    Ui::ToolMissingSamples_gui *ui;
};

#endif // TOOLMISSINGSAMPLES_GUI_H

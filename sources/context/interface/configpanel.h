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

#ifndef CONFIGPANEL_H
#define CONFIGPANEL_H

#include "tab.h"
#include "usermanager.h"
class QLabel;

namespace Ui
{
class ConfigPanel;
}

class ConfigPanel : public Tab
{
    Q_OBJECT

public:
    ConfigPanel(QWidget *parent = NULL);
    ~ConfigPanel();

    // Initialize the interface with the parameters values
    void initializeInterface();

protected:
    bool eventFilter(QObject * watched, QEvent * event) override;

private slots:
    void onSectionClicked(int sectionNumber);
    void updateTitle();

private:
    void updateTocPosition();
    void updateBottomSpace();

    Ui::ConfigPanel * ui;
    QList<QLabel *> _sectionLabels; // Headers of the sections, except the first one
};

#endif // CONFIGPANEL_H

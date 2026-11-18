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

#ifndef TEMPERAMENTCOMBOBOX_H
#define TEMPERAMENTCOMBOBOX_H

#include <QComboBox>
class QMenu;

// Combobox whose items are shown in a menu, with a submenu for each category and separators between the items of the
// top level and the categories. The category of an item is a list of levels (QStringList) stored with the role
// CATEGORY_ROLE (top level if empty).
class TemperamentComboBox : public QComboBox
{
    Q_OBJECT

public:
    explicit TemperamentComboBox(QWidget * parent = nullptr);
    void showPopup() override;

    // All temperaments in their categories, then the custom one (data: tuning program or TemperamentSettings::CUSTOM)
    void fillTemperaments();
    int getProgram() const;
    void selectProgram(int program); // Without signal

    static const int CATEGORY_ROLE = Qt::UserRole + 1;

private:
    QMenu * getMenu(QMenu * rootMenu, QMap<QString, QMenu *> &menus, const QStringList &category);
};

#endif // TEMPERAMENTCOMBOBOX_H

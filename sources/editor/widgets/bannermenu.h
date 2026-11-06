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


#ifndef BANNERMENU_H
#define BANNERMENU_H

#include <QMenu>
#include <QMap>
class QToolButton;
class QGridLayout;
class QCheckBox;

// Menu of the button on the right side of a banner, aligned on the right side of this button. It starts with a grid of
// settings (labels, checkboxes, sliders) that can be changed without closing it.
class BannerMenu : public QMenu
{
    Q_OBJECT

public:
    // The parent is the window (not the banner, whose style would apply)
    BannerMenu(QToolButton * button, QWidget * parent);

    // Grid of the settings
    QGridLayout * getGrid() { return _grid; }

    // Add a label in a row of the grid and a checkbox in its last column, a click on the label toggling the checkbox
    QCheckBox * addCheckBox(QString text, int row, int columnCount);

protected:
    void showEvent(QShowEvent * event) override;
    bool eventFilter(QObject * watched, QEvent * event) override;

private:
    QToolButton * _button;
    QGridLayout * _grid;
    QMap<QObject *, QCheckBox *> _checkBoxes; // By label
};

#endif // BANNERMENU_H

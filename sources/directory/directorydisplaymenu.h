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

#ifndef DIRECTORYDISPLAYMENU_H
#define DIRECTORYDISPLAYMENU_H

#include <QWidget>
#include <QIcon>

namespace Ui {
class DirectoryDisplayMenu;
}

// NOT USED ANYMORE BUT MIGHT BE USEFULL LATER
// This widget displays a button and a menu is shown when clicking on it
// The elements are only checkboxes
// The menu is not closed after checking / unchecking one element
class DirectoryDisplayMenu : public QWidget
{
    Q_OBJECT

public:
    explicit DirectoryDisplayMenu(QWidget *parent = nullptr);
    ~DirectoryDisplayMenu();

    int displayOptions();

signals:
    void displayOptionsChanged(int options);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
#if QT_VERSION >= 0x060000
    void enterEvent(QEnterEvent * event) override;
#else
    void enterEvent(QEvent * event) override;
#endif
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void onMenuClosed();

private:
    void setStyle(bool hovered);

    Ui::DirectoryDisplayMenu *ui;
    QIcon _emptyIcon, _checkIcon;
    QString _textColor, _backgroundColor, _hoveredBackgroundColor;
};

#endif // DIRECTORYDISPLAYMENU_H

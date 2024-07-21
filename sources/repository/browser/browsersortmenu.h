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

#ifndef BROWSERSORTMENU_H
#define BROWSERSORTMENU_H

#include <QWidget>
#include <QIcon>

namespace Ui {
class BrowserSortMenu;
}

class BrowserSortMenu : public QWidget
{
    Q_OBJECT

public:
    explicit BrowserSortMenu(QWidget *parent = nullptr);
    ~BrowserSortMenu();

    int currentIndex();

signals:
    void currentIndexChanged(int index);

protected:
#if QT_VERSION >= 0x060000
    void enterEvent(QEnterEvent * event) override;
#else
    void enterEvent(QEvent * event) override;
#endif
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void element1Clicked();
    void element2Clicked();
    void element3Clicked();
    void element4Clicked();
    void onMenuClosed();

private:
    void elementClicked(int index);
    void setStyle(bool hovered);

    Ui::BrowserSortMenu *ui;
    QIcon _checkIcon;
    QString _textColor, _backgroundColor, _hoveredBackgroundColor;
};

#endif // BROWSERSORTMENU_H

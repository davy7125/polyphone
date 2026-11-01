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
class QMenu;

namespace Ui {
class BrowserSortMenu;
}

class BrowserSortMenu : public QWidget
{
    Q_OBJECT

public:
    // Values stored in the configuration (the order in the menu is different)
    enum SortType
    {
        SORT_DATE = 0,
        SORT_DOWNLOADS = 1,
        SORT_RATING = 2,
        SORT_TITLE = 3,
        SORT_TRENDING = 4,
        SORT_SIZE = 5
    };

    explicit BrowserSortMenu(QWidget *parent = nullptr);
    ~BrowserSortMenu();

    SortType currentSort();

signals:
    void sortChanged();

protected:
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
    void addElement(QMenu * menu, QString text, SortType sortType, int currentSort);
    void elementClicked(SortType sortType);
    void setStyle(bool hovered);

    Ui::BrowserSortMenu *ui;
    QIcon _checkIcon;
    QString _textColor, _backgroundColor, _hoveredBackgroundColor;
};

#endif // BROWSERSORTMENU_H

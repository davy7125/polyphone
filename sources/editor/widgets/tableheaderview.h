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

#ifndef TABLEHEADERVIEW_H
#define TABLEHEADERVIEW_H

#include <QHeaderView>
#include "basetypes.h"

class TableHeaderView: public QHeaderView
{
    Q_OBJECT

public:
    TableHeaderView(QWidget *parent = nullptr);
    ~TableHeaderView();

signals:
    void rightClick(QPoint pos);

protected:
    void mousePressEvent(QMouseEvent * e);

private slots:
    void mute(bool isMute);
    void activateSolo(bool unused);
    void unmuteAll(bool unused);

private:
    QPixmap _muteIcon;
    QMenu * _menu;
    QAction * _muteAction;
    int _currentSection;
    EltID _currentId;
};

#endif // TABLEHEADERVIEW_H

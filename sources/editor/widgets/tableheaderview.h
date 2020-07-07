/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2020 Davy Triponney                                **
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
class SoloManager;

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
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;
    QSize sizeHint() const;

private slots:
    void mute(bool isMute);
    void activateSolo(bool unused);
    void unmuteAll(bool unused);
    void soloOnSelection(bool isOn);

private:
    QPixmap _muteIcon;
    QMenu * _menu;
    QAction * _muteAction;
    QAction * _soloAction;
    QAction * _clearAction;
    QAction * _soloSelectionAction;
    int _currentSection;
    EltID _currentId;
    int _height;
    SoloManager * _solo;

    static const int MARGIN;
};

#endif // TABLEHEADERVIEW_H

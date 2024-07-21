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

#ifndef TABLEPAGEOVERVIEW_H
#define TABLEPAGEOVERVIEW_H

#include <QTableWidget>
#include <QHeaderView>

class TablePageOverview : public QTableWidget
{
    Q_OBJECT
public:
    explicit TablePageOverview(QWidget *parent = nullptr);
};

class OverviewTableHeaderView : public QHeaderView
{
    Q_OBJECT
public:
    explicit OverviewTableHeaderView(QWidget *parent = nullptr);

protected:
    QSize sizeHint() const override;
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const override;

private:
    int _height;
    QPixmap _arrowDown, _arrowUp;
    QColor _textColor;

    static const int MARGIN;
};

#endif // TABLEPAGEOVERVIEW_H

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

#ifndef DIRECTORYLISTHEADER_H
#define DIRECTORYLISTHEADER_H

#include <QWidget>
#include "directorylistview.h"

// Header above the list of files, aligned with the columns of each row
// Clicking on a column sorts the files, clicking again reverses the order
class DirectoryListHeader : public QWidget
{
    Q_OBJECT

public:
    explicit DirectoryListHeader(QWidget *parent = nullptr);

    void setListView(DirectoryListView * listView);

protected:
    bool event(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    int getTotalWidth() const;
    int getLabelWidth(int column) const;
    void getSectionLimits(int column, int &left, int &right) const;
    int getColumnAt(int x) const;
    void setHoveredColumn(int column);

    DirectoryListView * _listView;
    QString _texts[DirectoryListView::COLUMN_NUMBER];
    QPixmap _icons[DirectoryListView::COLUMN_NUMBER];
    QString _toolTips[DirectoryListView::COLUMN_NUMBER];
    QPixmap _arrowUp, _arrowDown;
    QColor _textColor, _backgroundColor, _hoveredBackgroundColor, _borderColor;
    int _hoveredColumn;
    int _pressedColumn;
};

#endif // DIRECTORYLISTHEADER_H

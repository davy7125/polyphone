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

#include "tablepageoverview.h"
#include <QHeaderView>

TablePageOverview::TablePageOverview(QWidget *parent) : QTableWidget(parent)
{
    connect(this->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(onSort(int)));

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
    this->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
#endif

    this->setAlternatingRowColors(true);

    // Horizontal header in bold
    this->horizontalHeader()->setHighlightSections(false);
    QFont font = this->font();
    font.setBold(true);
    this->horizontalHeader()->setFont(font);
}

void TablePageOverview::onSort(int column)
{
    Q_UNUSED(column)

    // Restore colors
    colorRows();
}

void TablePageOverview::colorRows()
{
    //    for (int row = 0; row < this->rowCount(); row++)
    //    {
    //        QColor color = (row % 2) ? QColor(255, 255, 200) : QColor(255, 255, 255);
    //        for (int col = 0; col < this->columnCount(); col++)
    //            this->item(row, col)->setBackgroundColor(color);
    //    }
}

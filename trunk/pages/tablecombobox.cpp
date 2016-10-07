/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
**                2014      Andrea Celani                                 **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://polyphone-soundfonts.com                      **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "tablecombobox.h"
#include "thememanager.h"
#include <QMouseEvent>
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>

TableComboBox::TableComboBox(QWidget* parent) : QComboBox(parent)
{
    // Nouvelle vue
    QTableView * view = new QTableView();
    view->viewport()->installEventFilter(this);
    view->horizontalHeader()->setVisible(false);
    view->verticalHeader()->setVisible(false);
    view->setShowGrid(false);
    this->setView(view);

    // Préparation du modèle
    QStandardItemModel * model = new QStandardItemModel();
    model->setColumnCount(4);
    model->setRowCount(4);
    bool isDark = ThemeManager::getInstance()->isDark(ThemeManager::LIST_BACKGROUND, ThemeManager::LIST_TEXT);
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (isDark)
                model->setItem(i, j, new QStandardItem(
                                   QIcon(QString(":/icones/courbe%1_w").arg(4*j+i+1, 2, 10, QChar('0'))), ""));
            else
                model->setItem(i, j, new QStandardItem(
                                   QIcon(QString(":/icones/courbe%1").arg(4*j+i+1, 2, 10, QChar('0'))), ""));
        }
    }
    this->setModel(model);
    view->resizeColumnsToContents();

    int width = 0;
    int height = 0;
    for (int i = 0; i < 4; i++)
    {
        width += view->columnWidth(i);
        height += view->rowHeight(i);
    }
    view->setFixedSize(width, height);
}

TableComboBox::~TableComboBox()
{
    delete this->model();
}

bool TableComboBox::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress && object == view()->viewport())
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        view()->indexAt(mouseEvent->pos());
        this->setCurrentIndex(view()->currentIndex().row());
        this->setModelColumn(view()->currentIndex().column());
        emit(clicked(view()->currentIndex().row(), view()->currentIndex().column()));
    }
    return false;
}

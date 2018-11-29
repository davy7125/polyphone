/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2018 Davy Triponney                                **
**                2014      Andrea Celani                                 **
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

#include "tablecombobox.h"
#include "contextmanager.h"
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

    // Prepare the model
    QStandardItemModel * model = new QStandardItemModel();
    this->setModel(model);
    model->setColumnCount(4);
    model->setRowCount(4);

    // Icons
    int iconWidth = 48;
    int margin = 6;
    QMap<QString, QString> replacement;
    replacement["currentColor"] = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT).name();
    replacement["secondColor"] = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name();
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            QString iconName = QString(":/icons/courbe%1.svg").arg(4*j+i+1, 2, 10, QChar('0'));
            QStandardItem * item = new QStandardItem("");
            item->setData(ContextManager::theme()->getColoredSvg(iconName, QSize(iconWidth, iconWidth), replacement), Qt::DecorationRole);
            model->setItem(i, j, item);
        }
    }

    // Size
    for (int i = 0; i < 4; i++)
    {
        view->setColumnWidth(i, iconWidth + margin);
        view->setRowHeight(i, iconWidth + margin);
    }
    view->setFixedSize(4 * (iconWidth + margin), 4 * (iconWidth + margin));

    // Highlight color
    QPalette p = view->palette();
    p.setColor(QPalette::Highlight, ContextManager::theme()->getColor(ThemeManager::LIST_ALTERNATIVE_BACKGROUND));
    view->setPalette(p);
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

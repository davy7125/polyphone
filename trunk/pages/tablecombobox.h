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
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef TABLECOMBOBOX_H
#define TABLECOMBOBOX_H

#include <QComboBox>
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QMouseEvent>

// Classe TableComboBox pour les formes de courbes
class TableComboBox : public QComboBox
{
    Q_OBJECT

public:
    TableComboBox(QWidget* parent = 0) : QComboBox(parent)
    {
        // Nouvelle vue
        QTableView * view = new QTableView();
        view->viewport()->installEventFilter(this);
        view->horizontalHeader()->setVisible(false);
        view->verticalHeader()->setVisible(false);
        view->setShowGrid(false);
        view->setFixedSize(28*4, 30*4);
        this->setView(view);

        // Préparation du modèle
        QStandardItemModel * model = new QStandardItemModel();
        model->setColumnCount(4);
        model->setRowCount(4);
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
                model->setItem(i, j, new QStandardItem(
                                   QIcon(QString(":/icones/courbe%1").arg(4*j+i+1, 2, 10, QChar('0'))),
                                   ""));
        }
        this->setModel(model);
        view->resizeColumnsToContents();
    }

    ~TableComboBox()
    {
        delete this->model();
    }

    bool eventFilter(QObject* object, QEvent* event)
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

signals:
    void clicked(int row, int column);
};

#endif // TABLECOMBOBOX_H

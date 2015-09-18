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

#include "tablewidget.h"
#include "tabledelegate.h"
#include <QApplication>
#include <QKeyEvent>
#include <QClipboard>

TableWidget::TableWidget(QWidget *parent) : QTableWidget(parent)
{
    setItemDelegate(new TableDelegate(this));
    _timer = new QTimer(this);
    connect(_timer, SIGNAL(timeout()), this, SLOT(updateColors()));
}

void TableWidget::clear()
{
    for (int i = 0; i < this->columnCount(); i++)
    {
        for (int j = 0; j < this->rowCount(); j++)
            delete this->item(j, i);
    }
    this->setColumnCount(0);
}

void TableWidget::addColumn(int column, QString title)
{
    this->insertColumn(column);
    for (int i = 0; i < this->rowCount(); i++)
        this->setItem(i, column, new QTableWidgetItem());
    this->setHorizontalHeaderItem(column, new QTableWidgetItem(title));

    // Ajout d'un élément couleur
    _listColors.insert(column, QColor(0, 0, 0));
}

void TableWidget::setID(EltID id, int colonne)
{
    QString str;
    switch(id.typeElement)
    {
    case elementInstGen: case elementInst: str = "Inst"; break;
    case elementInstSmplGen: case elementInstSmpl: str = "InstSmpl"; break;
    case elementPrstGen: case elementPrst:  str = "Prst"; break;
    case elementPrstInstGen: case elementPrstInst: str = "PrstInst"; break;
    default: break;
    }
    item(0, colonne)->setText(str);
    this->item(1, colonne)->setText(QString::number(id.indexSf2));
    this->item(2, colonne)->setText(QString::number(id.indexElt));
    this->item(3, colonne)->setText(QString::number(id.indexElt2));
}

EltID TableWidget::getID(int colonne)
{
    EltID id(elementUnknown, 0, 0, 0, 0);
    if (this->columnCount() > colonne)
    {
        if (item(0, colonne)->text() == "Inst")
            id.typeElement = elementInst;
        else if (item(0, colonne)->text() == "InstSmpl")
            id.typeElement = elementInstSmpl;
        else if (item(0, colonne)->text() == "Prst")
            id.typeElement = elementPrst;
        else if (item(0, colonne)->text() == "PrstInst")
            id.typeElement = elementPrstInst;
        id.indexSf2 = item(1, colonne)->text().toInt();
        id.indexElt = item(2, colonne)->text().toInt();
        id.indexElt2 = item(3, colonne)->text().toInt();
    }
    return id;
}

void TableWidget::setEnlighted(int colonne, bool isEnlighted)
{
    if (colonne >= this->columnCount())
        return;

    if (isEnlighted)
        _listColors[colonne] = QColor(70, 120, 210);
    else
        _listColors[colonne] = QColor(0, 0, 0);

    _timer->start(30);
}

void TableWidget::updateColors()
{
    int minChange = 40;
    bool toutPareil = true;
    for (int i = 0; i < this->columnCount(); i++)
    {
        if (this->horizontalHeaderItem(i))
        {
            QColor couleur1 = this->horizontalHeaderItem(i)->foreground().color();
            QColor couleur2 = _listColors.at(i);
            if (couleur1 != couleur2)
            {
                int deltaRouge = qMax(-minChange, qMin(minChange, couleur2.red() - couleur1.red()));
                int deltaVert = qMax(-minChange, qMin(minChange, couleur2.green() - couleur1.green()));
                int deltaBleu = qMax(-minChange, qMin(minChange, couleur2.blue() - couleur1.blue()));
                couleur1.setRed(couleur1.red() + deltaRouge);
                couleur1.setGreen(couleur1.green() + deltaVert);
                couleur1.setBlue(couleur1.blue() + deltaBleu);
                this->horizontalHeaderItem(i)->setForeground(couleur1);
            }
            if (couleur1 != couleur2)
                toutPareil = false;
        }
    }
    if (toutPareil)
        _timer->stop();
}

void TableWidget::setColumnCount(int columns)
{
    QTableWidget::setColumnCount(columns);
    _listColors.clear();
    for (int i = 0; i < columns; i++)
        _listColors << QColor(0, 0, 0);
}

void TableWidget::removeColumn(int column)
{
    QTableWidget::removeColumn(column);
    _listColors.removeAt(column);
}

void TableWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_C && (event->modifiers() & Qt::ControlModifier))
    {
        if (selectedItems().isEmpty())
        {
            // no items selected
            QApplication::clipboard()->setText("");
        }
        else
        {
            // Selected items
            // They may not be contiguous !
            QModelIndexList indexes = selectionModel()->selectedIndexes();

            // Rows before 4 are for identifiers
            int indexNumber = indexes.size();
            for (int i = indexNumber - 1; i >= 0; i--)
                if (indexes.at(i).row() < 4)
                    indexes.removeAt(i);

            QMap<int, QMap<int, QString> > mapText;
            foreach (QModelIndex index, indexes)
            {
                QString text = model()->data(index).toString();

                // Maybe data comes from the UserRole (loopmode?)
                if (!model()->data(index, Qt::UserRole).isNull())
                    text = QString::number(model()->data(index, Qt::UserRole).toInt());

                // "!" stands for "erase", empty cell in the selection
                if (text.isEmpty())
                    text = "!";
                mapText[index.row()][index.column()] = text;
            }

            int minRow = mapText.keys().first();
            int maxRow = mapText.keys().last();

            int minCol = -1;
            int maxCol = -1;
            QMap<int, QString> mapTmp;
            foreach (mapTmp, mapText.values())
            {
                int minColTmp = mapTmp.keys().first();
                int maxColTmp = mapTmp.keys().last();
                if (minCol == -1)
                    minCol = minColTmp;
                else
                    minCol = qMin(minCol, minColTmp);
                if (maxCol == -1)
                    maxCol = maxColTmp;
                else
                    maxCol = qMax(maxCol, maxColTmp);
            }

            QString selected_text;
            for (int numRow = minRow; numRow <= maxRow; numRow++)
            {
                if (numRow != minRow)
                    selected_text += "\n";

                for (int numCol = minCol; numCol <= maxCol; numCol++)
                {
                    if (numCol != minCol)
                        selected_text += "\t";

                    QString text = mapText[numRow][numCol];
                    // "?" stands for "no change", this cell was not present in the selection
                    if (text.isEmpty())
                        text = "?";
                    selected_text += text;
                }
            }

            QApplication::clipboard()->setText(selected_text);
        }
    }
    else if (event->key() == Qt::Key_V && (event->modifiers() & Qt::ControlModifier))
    {
        if (!selectedItems().isEmpty())
        {
            emit(actionBegin());

            QString selected_text = qApp->clipboard()->text();
            QStringList cells = selected_text.split(QRegExp("[\n\t]"));

            int cellrows = selected_text.count('\n') + 1;
            int cellcols = cells.size() / cellrows;
            if (cells.size() != cellrows * cellcols)
            {
                // error, uneven number of columns, probably bad data
                //QMessageBox::critical(this, "Error", "Invalid clipboard data, unable to perform paste operation.");
                return;
            }

            // Paste from the top left corner of the selected items
            QModelIndexList indexes = selectionModel()->selectedIndexes();
            int minRow = -1;
            int minCol = -1;
            foreach (QModelIndex modelIndex, indexes)
            {
                if (minRow == -1)
                    minRow = modelIndex.row();
                else
                    minRow = qMin(minRow, modelIndex.row());
                if (minCol == -1)
                    minCol = modelIndex.column();
                else
                    minCol = qMin(minCol, modelIndex.column());
            }

            // First rows are identifiers, no paste here
            if (minRow < 4)
                minRow = 4;

            for (int indRow = 0; indRow < cellrows; indRow++)
            {
                for (int indCol = 0; indCol < cellcols; indCol++)
                {
                    const QModelIndex idx = model()->index(indRow + minRow, indCol + minCol);
                    QString text = cells.takeFirst();
                    if (text != "?")
                    {
                        if (text == "!")
                            text = "";

                        if (this->rowCount() == 50 && indRow + minRow == 8)
                        {
                            bool ok;
                            int val = text.toInt(&ok);
                            if (ok && (val == 0 || val == 1 || val == 3))
                                setLoopModeImage(indRow + minRow, indCol + minCol, val);
                        }
                        else
                            model()->setData(idx, text.replace(",", "."), Qt::EditRole);
                    }
                }
            }

            emit(actionFinished());
        }
    }
    else if (event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete)
    {
        // Touche retour ou suppr (efface la cellule)
        if (!selectedItems().isEmpty())
        {
            emit(actionBegin());
            QList<QTableWidgetItem*> listItems = selectedItems();
            foreach (QTableWidgetItem * item, listItems)
            {
                item->setText("");
                item->setData(Qt::DecorationRole, QImage());
                item->setData(Qt::UserRole, QVariant());
            }
            emit(actionFinished());
        }
    }
    else
    {
        QTableWidget::keyPressEvent(event);
    }
}

void TableWidget::commitData(QWidget *editor)
{
    emit(actionBegin());
    QTableWidget::commitData(editor);
    
    QVariant value = model()->data(currentIndex(), Qt::EditRole).toString().replace(",", ".");
    int curRow = currentIndex().row();
    int curCol = currentIndex().column();
    QItemSelectionRange isr;
    Q_FOREACH (isr, selectionModel()->selection())
    {
        for (int rows = isr.top(); rows <= isr.bottom(); rows++)
        {
            for (int cols = isr.left(); cols <= isr.right(); cols++)
            {
                if (!(curRow == rows && curCol == cols))
                {
                    const QModelIndex idx = model()->index(rows, cols);

                    if (this->rowCount() == 50 && curRow == 8)
                    {
                        // Copy the data in DecorationRole and UserRole
                        model()->setData(idx, model()->data(currentIndex(), Qt::DecorationRole), Qt::DecorationRole);
                        model()->setData(idx, model()->data(currentIndex(), Qt::UserRole), Qt::UserRole);
                    }
                    else
                    {
                        // Copy the data in EditRole
                        model()->setData(idx, value, Qt::EditRole);
                    }
                }
            }
        }
    }
    emit(actionFinished());
}

void TableWidget::setLoopModeImage(int row, int column, int loopModeValue)
{
    switch (loopModeValue)
    {
    case 0:
        // no loop
        this->item(row, column)->setData(Qt::DecorationRole, QImage(":/icones/loop_off.png"));
        this->item(row, column)->setData(Qt::UserRole, 0);
        break;
    case 1:
        // loop
        this->item(row, column)->setData(Qt::DecorationRole, QImage(":/icones/loop_on.png"));
        this->item(row, column)->setData(Qt::UserRole, 1);
        break;
    case 2:
        // loop + end
        this->item(row, column)->setData(Qt::DecorationRole, QImage(":/icones/loop_on_end.png"));
        this->item(row, column)->setData(Qt::UserRole, 3);
        break;
    default:
        // no image
        this->item(row, column)->setData(Qt::DecorationRole, QImage());
        this->item(row, column)->setData(Qt::UserRole, QVariant());
        break;
    }
}

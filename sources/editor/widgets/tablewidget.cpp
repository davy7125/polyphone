/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "tablewidget.h"
#include "tabledelegate.h"
#include "contextmanager.h"
#include <QApplication>
#include <QKeyEvent>
#include <QClipboard>
#include <QPainter>
#include <QHeaderView>
#include <QScrollBar>
#include "tableheaderview.h"
#include "tableheaderviewv.h"

TableWidget::TableWidget(QWidget *parent) : QTableWidget(parent)
{
    _tableDelegate = new TableDelegate(this);
    setItemDelegate(_tableDelegate);
    setHorizontalHeader(new TableHeaderView(this));
    setVerticalHeader(new TableHeaderViewV(this));

    _timer = new QTimer(this);
    connect(_timer, SIGNAL(timeout()), this, SLOT(updateColors()));
    connect(this->horizontalHeader(), SIGNAL(sectionDoubleClicked(int)), this, SLOT(onSectionDoubleClicked(int)));
    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(onItemSelectionChanged()));

    // Style
    this->setStyleSheet(ContextManager::theme()->getTableTheme() +
                        "QTableWidget {border-top:0;border-left:0;border-right:0;}");
}

void TableWidget::clear()
{
    for (int i = 0; i < this->columnCount(); i++)
        for (int j = 0; j < this->rowCount(); j++)
            delete this->item(j, i);
    this->setColumnCount(0);
    _columnIds.clear();
    _listColors.clear();
}

void TableWidget::addColumn(int column, QString title, EltID id)
{
    _columnIds.insert(column, EltID());
    this->insertColumn(column);
    for (int i = 0; i < this->rowCount(); i++)
        this->setItem(i, column, new QTableWidgetItem());
    this->setHorizontalHeaderItem(column, new QTableWidgetItem(title));

    // Add a colored element
    QColor color = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT);
    _listColors.insert(column, color);
    this->horizontalHeaderItem(column)->setForeground(color);

    // Keep the id somewhere
    if (id.typeElement == elementInstGen)
        id.typeElement = elementInst;
    if (id.typeElement == elementInstSmplGen)
        id.typeElement = elementInstSmpl;
    if (id.typeElement == elementPrstGen)
        id.typeElement = elementPrst;
    if (id.typeElement == elementPrstInstGen)
        id.typeElement = elementPrstInst;
    _columnIds[column] = id;
    this->model()->setHeaderData(column, Qt::Horizontal, QVariant::fromValue(id), Qt::UserRole);
}

EltID TableWidget::getID(int column)
{
    return _columnIds.count() > column ? _columnIds[column] : EltID();
}

void TableWidget::setEnlighted(int colonne, bool isEnlighted)
{
    if (colonne >= this->columnCount())
        return;

    if (isEnlighted)
        _listColors[colonne] = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);
    else
        _listColors[colonne] = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT);

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

void TableWidget::removeColumn(int column)
{
    QTableWidget::removeColumn(column);
    _listColors.removeAt(column);
    _columnIds.removeAt(column);
}

void TableWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_C && (event->modifiers() & Qt::ControlModifier))
    {
        copy();
    }
    else if (event->key() == Qt::Key_V && (event->modifiers() & Qt::ControlModifier))
    {
        if (!selectedItems().isEmpty())
            paste();
    }
    else if (event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete)
    {
        // Delete the cell(s)
        if (!selectedItems().isEmpty())
            deleteCells();
    }
    else if (event->key() == Qt::Key_Space || event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        // Key enter or space: enter the cell
        if (this->state() != QAbstractItemView::EditingState && !selectedItems().isEmpty())
        {
            QTableWidgetItem * item = selectedItems()[0];
            this->edit(this->model()->index(item->row(), item->column()));
        }
    }
    else if (event->key() == Qt::Key_X && (event->modifiers() & Qt::ControlModifier))
    {
        copy();
        deleteCells();
    }
    else if (event->key() == Qt::Key_Tab)
    {
        QModelIndex index = this->currentIndex();
        bool editing = _tableDelegate->isEditing();

        QTableWidget::keyPressEvent(event);

        if (editing)
        {
            // Otherwise the next cell is selected
            this->setCurrentIndex(index);

            // Otherwise we lose the focus if we use the mouse in the combobox
            this->setFocus();
        }
    }
    else
    {
        QTableWidget::keyPressEvent(event);
    }
}

void TableWidget::wheelEvent(QWheelEvent * event)
{
    if (event->modifiers() == Qt::ShiftModifier)
    {
        event->setModifiers(Qt::NoModifier); // Otherwise it moves too fast
        QApplication::sendEvent(this->horizontalScrollBar(), event);
    }
    else
        QTableWidget::wheelEvent(event);
}

void TableWidget::commitData(QWidget *editor)
{
    emit actionBegin();
    QTableWidget::commitData(editor);

    QVariant value = model()->data(currentIndex(), Qt::EditRole).toString();
    int curRow = currentIndex().row();
    int curCol = currentIndex().column();
    QItemSelectionRange isr;

    foreach (isr, selectionModel()->selection())
    {
        for (int row = isr.top(); row <= isr.bottom(); row++)
        {
            for (int col = isr.left(); col <= isr.right(); col++)
            {
                if (!(curRow == row && curCol == col))
                {
                    const QModelIndex idx = this->model()->index(row, col);

                    if (this->isInstrumentLevel())
                    {
                        // Copy the data but not in grayed columns
                        if (!this->isGrayed(row))
                        {
                            if (curRow == 5)
                            {
                                if (row == 5)
                                {
                                    // Copy the data in UserRole
                                    model()->setData(idx, model()->data(currentIndex(), Qt::UserRole), Qt::UserRole);
                                }
                                else
                                {
                                    // Copy from the loop combobox to a regular cell => do nothing
                                }
                            }
                            else
                            {
                                if (row == 5)
                                {
                                    // Copy to a regular cell to the loop combobox => do nothing
                                }
                                else
                                {
                                    model()->setData(idx, value, Qt::EditRole);
                                }
                            }

                        }
                    }
                    else
                        model()->setData(idx, value, Qt::EditRole); // simple case
                }
            }
        }
    }
    emit actionFinished();
}

void TableWidget::setLoopModeImage(int row, int column, int loopModeValue)
{
    this->item(row, column)->setData(Qt::UserRole, loopModeValue >= 0 && loopModeValue < 4 ? loopModeValue : QVariant());
}

void TableWidget::copy()
{
    if (selectedItems().isEmpty())
    {
        // no items selected
        QApplication::clipboard()->setText("");
    }
    else
    {
        // Selected items
        // They may not be contiguous!
        QModelIndexList indexes = selectionModel()->selectedIndexes();

        // Row 0 is for selection
        int indexNumber = indexes.size();
        for (int i = indexNumber - 1; i >= 0; i--)
            if (indexes.at(i).row() == 0)
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

void TableWidget::paste()
{
    emit actionBegin();
    QString selected_text = qApp->clipboard()->text();
    QStringList cells = selected_text.split(QRegularExpression("[\n\t]"));

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

    // First row is for selection, no paste here
    if (minRow < 1)
        minRow = 1;

    int currentRow, currentCol;
    for (int indRow = 0; indRow < cellrows; indRow++)
    {
        currentRow = indRow + minRow;
        for (int indCol = 0; indCol < cellcols; indCol++)
        {
            currentCol = indCol + minCol;
            const QModelIndex idx = this->model()->index(currentRow, currentCol);
            QString text = cells.takeFirst();
            if (text != "?")
            {
                if (text == "!")
                    text = "";

                if (this->isInstrumentLevel())
                {
                    if (currentRow == 5)
                    {
                        bool ok;
                        int val = text.toInt(&ok);
                        if (ok && (val >= -1 && val <= 3))
                            setLoopModeImage(currentRow, currentCol, val);
                    }
                    else if (!isGrayed(currentRow))
                        model()->setData(idx, text, Qt::EditRole);
                }
                else
                    model()->setData(idx, text, Qt::EditRole); // Simple case
            }
        }
    }
    emit actionFinished();
}

void TableWidget::deleteCells()
{
    emit actionBegin();
    QList<QTableWidgetItem*> listItems = selectedItems();
    foreach (QTableWidgetItem * item, listItems)
    {
        item->setText("");
        item->setData(Qt::UserRole, QVariant());
    }
    emit actionFinished();
}

void TableWidget::onSectionDoubleClicked(int index)
{
    if (index >= 0) // Avoid a double click in the empty area
    {
        EltID id = getID(index);
        openElement(id);
    }
}

void TableWidget::onItemSelectionChanged()
{
    // Problematic case: background color when the first loopmode cell is selected
    // The highlight color must be displayed along with the hatching pattern
    // Only if ONE instrument is displayed
    if (_columnIds.empty())
        return;
    if (_columnIds[0].typeElement != elementInst)
        return;
    if (_columnIds.count() > 1 && _columnIds[1].typeElement != elementInstSmpl)
        return;

    if (this->item(5, 0)->isSelected())
        this->item(5, 0)->setBackground(ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND));
    else if (this->item(5, 0)->background() == ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND))
    {
        QColor color = ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND);
        QColor alternateColor = ContextManager::theme()->getColor(ThemeManager::LIST_ALTERNATIVE_BACKGROUND);
        QBrush brush(TableWidget::getPixMap(color, alternateColor));
        this->item(5, 0)->setBackground(brush);
    }
}

QPixmap TableWidget::getPixMap(QColor backgroundColor, QColor dotColor)
{
    QPixmap pix(4, 4);
    QPainter painter(&pix);
    painter.fillRect(0, 0, 4, 4, backgroundColor);
    painter.setPen(dotColor);
    painter.drawPoint(3, 0);
    painter.drawPoint(2, 1);
    painter.drawPoint(1, 2);
    painter.drawPoint(0, 3);
    return pix;
}

void TableWidget::resetModDisplay()
{
    _tableDelegate->resetModDisplay();
}

void TableWidget::updateModDisplay(int column, QList<int> rows)
{
    _tableDelegate->updateModDisplay(column, rows);
}

void TableWidget::selectCells(EltID id, QList<AttributeType> attributes)
{
    int numCol = _columnIds.indexOf(id);
    if (numCol < 0 && numCol >= this->columnCount())
        return;

    int selectionNumber = 0;
    this->blockSignals(true);
    foreach (AttributeType attribute, attributes)
    {
        if (attribute == champ_startAddrsCoarseOffset)
            attribute = champ_startAddrsOffset;
        else if (attribute == champ_endAddrsCoarseOffset)
            attribute = champ_endAddrsOffset;
        else if (attribute == champ_startloopAddrsCoarseOffset)
            attribute = champ_startloopAddrsOffset;
        else if (attribute == champ_endloopAddrsCoarseOffset)
            attribute = champ_endloopAddrsOffset;

        int numRow = getRow(attribute);
        if (numRow >= 0 && numRow < this->rowCount())
        {
            this->selectionModel()->select(this->model()->index(numRow, numCol),
                                           selectionNumber == 0 ? QItemSelectionModel::ClearAndSelect : QItemSelectionModel::Select);
            selectionNumber++;
        }
    }
    this->blockSignals(false);
}

bool TableWidget::isGrayed(int numRow)
{
    return getChamp(numRow) > champ_endOper;
}

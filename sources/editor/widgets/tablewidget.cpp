/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
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

#include "tablewidget.h"
#include "tabledelegate.h"
#include "contextmanager.h"
#include <QApplication>
#include <QKeyEvent>
#include <QClipboard>
#include <QPainter>
#include <QHeaderView>
#include "tableheaderview.h"

TableWidget::TableWidget(QWidget *parent) : QTableWidget(parent)
{
    _tableDelegate = new TableDelegate(this);
    setItemDelegate(_tableDelegate);
    _tableHeader = new TableHeaderView(this);
    setHorizontalHeader(_tableHeader);
    this->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    _timer = new QTimer(this);
    connect(_timer, SIGNAL(timeout()), this, SLOT(updateColors()));
    connect(this->horizontalHeader(), SIGNAL(sectionDoubleClicked(int)), this, SLOT(onSectionDoubleClicked(int)));
    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(onItemSelectionChanged()));

    _muteIcon = ContextManager::theme()->getColoredSvg(":/icons/volume-mute.svg", QSize(12, 12), ThemeManager::HIGHLIGHTED_BACKGROUND);
}

void TableWidget::clear()
{
    for (int i = 0; i < this->columnCount(); i++)
        for (int j = 0; j < this->rowCount(); j++)
            delete this->item(j, i);
    this->setColumnCount(0);
    _columnIds.clear();
}

void TableWidget::addColumn(int column, QString title, EltID id)
{
    _columnIds.insert(column, EltID());
    this->insertColumn(column);
    for (int i = 0; i < this->rowCount(); i++)
        this->setItem(i, column, new QTableWidgetItem());
    this->setHorizontalHeaderItem(column, new QTableWidgetItem(title));

    // Add a colored element
    QColor color = this->palette().color(QPalette::Text);
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

    if ((id.typeElement == elementInstSmpl || id.typeElement == elementPrstInst) &&
            SoundfontManager::getInstance()->get(id, champ_mute).bValue > 0)
        this->model()->setHeaderData(column, Qt::Horizontal, QVariant::fromValue(_muteIcon), Qt::DecorationRole);
}

EltID TableWidget::getID(int column)
{
    return _columnIds.count() > column ? _columnIds[column] : EltID();
}

void TableWidget::setEnlighted(int colonne, bool isEnlighted)
{
    if (colonne >= this->columnCount())
        return;

    QPalette p = this->palette();
    if (isEnlighted)
        _listColors[colonne] = p.color(QPalette::Highlight);
    else
        _listColors[colonne] = p.color(QPalette::Text);

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
    _columnIds.clear();
    for (int i = 0; i < columns; i++)
        _columnIds.append(EltID());
    _listColors.clear();
    QPalette p = this->palette();
    QColor color = p.color(QPalette::Text);
    for (int i = 0; i < columns; i++)
        _listColors << color;
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

void TableWidget::commitData(QWidget *editor)
{
    emit(actionBegin());
    QTableWidget::commitData(editor);
    
    QVariant value = model()->data(currentIndex(), Qt::EditRole).toString().replace(",", ".");
    int curRow = currentIndex().row();
    int curCol = currentIndex().column();
    QItemSelectionRange isr;

    foreach (isr, selectionModel()->selection())
    {
        for (int rows = isr.top(); rows <= isr.bottom(); rows++)
        {
            for (int cols = isr.left(); cols <= isr.right(); cols++)
            {
                if (!(curRow == rows && curCol == cols))
                {
                    const QModelIndex idx = model()->index(rows, cols);

                    if (this->rowCount() == 50 && curRow == 5)
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
    bool isDark = ContextManager::theme()->isDark(ThemeManager::LIST_BACKGROUND, ThemeManager::LIST_TEXT);
    switch (loopModeValue)
    {
    case 0:
        // no loop
        if (isDark)
            this->item(row, column)->setData(Qt::DecorationRole, QImage(":/icons/loop_off_w.png"));
        else
            this->item(row, column)->setData(Qt::DecorationRole, QImage(":/icons/loop_off.png"));
        this->item(row, column)->setData(Qt::UserRole, 0);
        break;
    case 1:
        // loop
        if (isDark)
            this->item(row, column)->setData(Qt::DecorationRole, QImage(":/icons/loop_on_w.png"));
        else
            this->item(row, column)->setData(Qt::DecorationRole, QImage(":/icons/loop_on.png"));
        this->item(row, column)->setData(Qt::UserRole, 1);
        break;
    case 3:
        // loop + end
        if (isDark)
            this->item(row, column)->setData(Qt::DecorationRole, QImage(":/icons/loop_on_end_w.png"));
        else
            this->item(row, column)->setData(Qt::DecorationRole, QImage(":/icons/loop_on_end.png"));
        this->item(row, column)->setData(Qt::UserRole, 3);
        break;
    default:
        // no image
        this->item(row, column)->setData(Qt::DecorationRole, QImage());
        this->item(row, column)->setData(Qt::UserRole, QVariant());
        break;
    }
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

    // First row is for selection, no paste here
    if (minRow < 1)
        minRow = 1;

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

                if (this->rowCount() == 50 && indRow + minRow == 5)
                {
                    bool ok;
                    int val = text.toInt(&ok);
                    if (ok && (val == -1 || val == 0 || val == 1 || val == 3))
                        setLoopModeImage(indRow + minRow, indCol + minCol, val);
                }
                else
                    model()->setData(idx, text.replace(",", "."), Qt::EditRole);
            }
        }
    }
    emit(actionFinished());
}

void TableWidget::deleteCells()
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

void TableWidget::onSectionDoubleClicked(int index)
{
    EltID id = getID(index);
    openElement(id);
}

void TableWidget::onItemSelectionChanged()
{
    if (this->rowCount() == 50) // If instrument
    {
        // Problematic case: background color when the first loopmode cell is selected
        if (this->item(5, 0)->isSelected())
            this->item(5, 0)->setBackgroundColor(this->palette().color(QPalette::Highlight));
        else if (this->item(5, 0)->backgroundColor() == this->palette().color(QPalette::Highlight))
        {
            QColor color = this->palette().color(QPalette::Base);
            QColor alternateColor = this->palette().color(QPalette::AlternateBase);
            QBrush brush(TableWidget::getPixMap(color, alternateColor));
            this->item(5, 0)->setBackground(brush);
        }
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

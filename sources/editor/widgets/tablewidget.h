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

#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QTableWidget>
#include <QTimer>
#include "soundfontmanager.h"
class TableDelegate;
class TableHeaderView;

// QTableWidget an id by column, double click triggers an event and blue effect on the header
class TableWidget : public QTableWidget
{
    Q_OBJECT

public:
    TableWidget(QWidget *parent = nullptr);
    ~TableWidget() { delete this->itemDelegate(); }
    void clear();
    void addColumn(int column, QString title, EltID id);
    EltID getID(int column);
    void setEnlighted(int colonne, bool isEnlighted);
    void setColumnCount(int columns);
    void removeColumn(int column);
    void resetModDisplay();
    void updateModDisplay(int column, QList<int> rows);
    void selectCells(EltID id, QList<AttributeType> attributes);

    // Set the image corresponding to the loop mode value
    // -1 will remove the image
    void setLoopModeImage(int row, int column, int loopModeValue);

    // Association champ - ligne (méthodes virtuelles pures)
    virtual AttributeType getChamp(int row) = 0;
    virtual int getRow(quint16 champ) = 0;

    static QPixmap getPixMap(QColor backgroundColor, QColor dotColor);

signals:
    void actionBegin();
    void actionFinished();
    void openElement(EltID id);

protected:
    void keyPressEvent(QKeyEvent *event);

protected slots:
    // Function reimplemented to fill all selected cells in the same time
    virtual void commitData(QWidget *editor);
    void onSectionDoubleClicked(int index);
    void onItemSelectionChanged();

private slots:
    void updateColors();

private:
    QTimer *_timer;
    QList<QColor> _listColors;
    TableDelegate * _tableDelegate;
    TableHeaderView * _tableHeader;
    QPixmap _muteIcon;

    void copy();
    void paste();
    void deleteCells();
    QList<EltID> _columnIds;
};

#endif // TABLEWIDGET_H

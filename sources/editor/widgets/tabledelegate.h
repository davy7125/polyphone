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

#ifndef TABLEDELEGATE_H
#define TABLEDELEGATE_H

#include <QStyledItemDelegate>
class QTableWidget;

// Redéfinition des éditeurs au sein de la table
class TableDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    TableDelegate(QTableWidget * table, QObject * parent = NULL);
    bool isEditing() { return _isEditing; }
    void resetModDisplay();
    void updateModDisplay(int column, QList<int> rows);

protected:
    QWidget * createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    void destroyEditor(QWidget * editor, const QModelIndex & index) const;
#endif

private:
    void getType(bool &isNumeric, bool &isKey, int &nbDecimales, int numRow, bool &isLoop, bool &isFixed) const;
    QTableWidget * _table;

    static const char * DECO_PROPERTY;
    mutable bool _isEditing;
    QMap<int, QList<int> > _modDisplay;
    QColor _modBorderColor, _modBorderColor2;
};

#endif // TABLEDELEGATE_H

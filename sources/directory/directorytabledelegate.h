/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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

#ifndef DIRECTORYTABLEDELEGATE_H
#define DIRECTORYTABLEDELEGATE_H

#include <QStyledItemDelegate>
#include "basetypes.h"

class DirectoryTableDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit DirectoryTableDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    // Structure pour stocker les infos de chaque sous-élément (pour la détection de clic)
    struct ItemRectInfo
    {
        QRect rect;
        int id;
    };

signals:
    void itemDoubleClicked(const QString &itemId, EltID id) const;

protected:
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

private:
    void drawListItems(QPainter *painter, const QRect &contentRect, const QModelIndex &index, const QFontMetrics &fm) const;
};

#endif // DIRECTORYTABLEDELEGATE_H

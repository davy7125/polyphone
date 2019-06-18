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

#ifndef TREEITEMDELEGATE_H
#define TREEITEMDELEGATE_H

#include <QStyledItemDelegate>

class TreeItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    TreeItemDelegate(QObject *parent);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

public slots:
    bool helpEvent(QHelpEvent* e, QAbstractItemView* view, const QStyleOptionViewItem& option, const QModelIndex& index);

private:
    void drawRoot(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index, bool expandable) const;
    void drawElement(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index,
                     const QPixmap &leftIcon, bool expandable, bool withIndent) const;

    class Icons
    {
    public:
        QPixmap getPixmap(QString name, int colorType, bool fixedColor);

    private:
        QMap<QString, QPixmap> _pixmaps;
    };

    class Colors
    {
    public:
        Colors();

        QColor _borderColor;
        QColor _alternateColor;
        QColor _textColor;
        QColor _highlightColorBackground;
        QColor _highlightColorText;
    };

    static Icons * s_icons;
    static Colors * s_colors;
    static const int MARGIN;
};

#endif // TREEITEMDELEGATE_H

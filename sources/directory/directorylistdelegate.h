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

#ifndef DIRECTORYLISTDELEGATE_H
#define DIRECTORYLISTDELEGATE_H

#include <QStyledItemDelegate>
#include "basetypes.h"
class DirectoryFileData;

class DirectoryListDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit DirectoryListDelegate(QObject *parent = nullptr);

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

signals:
    void renameRequested(QString path);
    void deleteRequested(QString path);

private:
    int getCounterWidth(const QFontMetrics &fm, const DirectoryFileData *d) const;
    void paintCounters(QPainter *painter, const QFontMetrics &fm, const DirectoryFileData *d, int x, int y, bool selected) const;
    QRect getRenameButtonRect(const QStyleOptionViewItem &option) const;
    QRect getDeleteButtonRect(const QStyleOptionViewItem &option) const;

    QColor _colorEnabled, _colorHighlighted, _colorDisabled;
    QString _dateFormat;
    QPixmap _iconSample, _iconInstrument, _iconPreset, _iconRename, _iconDelete;
    QPixmap _iconSampleSelected, _iconInstrumentSelected, _iconPresetSelected, _iconRenameSelected, _iconDeleteSelected;
    mutable QPersistentModelIndex _pressedIndex;
    mutable bool _pressedWasSelected = false;

    static const int MARGIN;
    static const int ICON_SIZE;
    static const int BUTTON_SIZE;
};

#endif // DIRECTORYLISTDELEGATE_H

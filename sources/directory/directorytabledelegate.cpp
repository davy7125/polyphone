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

#include "directorytabledelegate.h"
#include "directoryfiledata.h"
#include "directorytablemodel.h"
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>

void DirectoryTableDelegate::drawListItems(QPainter *painter, const QRect &contentRect, const QModelIndex &index, const QFontMetrics &fm) const
{
    int lineHeight = fm.height() + 2;
    int currentY = contentRect.top();
    int displayOptions = property("displayOptions").toInt();

    // Définir la couleur du texte (par défaut ou depuis le rôle Qt::TextColorRole)
    QColor textColor = Qt::black;
    painter->setPen(textColor);

    if (index.column() == 1) // Samples
    {
        QList<DirectorySampleData> samples = index.data(DirectoryTableModel::SampleListRole).value<QList<DirectorySampleData>>();
        for (const auto& sample : samples)
        {
            QString text = sample.name;
            painter->drawText(contentRect.left(), currentY, contentRect.width(), lineHeight, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextDontClip, text);
            currentY += lineHeight;
            if (currentY > contentRect.bottom())
                break; // Don't paint outside the cell
        }
    }
    else if (index.column() == 2) // Instruments
    {
        QList<DirectoryInstrumentPresetData> instruments = index.data(DirectoryTableModel::InstrumentListRole).value<QList<DirectoryInstrumentPresetData>>();
        for (const auto& inst : instruments)
        {
            QString text = inst.name;
            painter->drawText(contentRect.left(), currentY, contentRect.width(), lineHeight, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextDontClip, text);
            currentY += lineHeight;
            if (currentY > contentRect.bottom())
                break; // Don't paint outside the cell
        }
    }
    else if (index.column() == 3) // Presets
    {
        QList<DirectoryInstrumentPresetData> presets = index.data(DirectoryTableModel::PresetListRole).value<QList<DirectoryInstrumentPresetData>>();
        for (const auto& prst : presets)
        {
            QString text = prst.name;
            painter->drawText(contentRect.left(), currentY, contentRect.width(), lineHeight, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextDontClip, text);
            currentY += lineHeight;
            if (currentY > contentRect.bottom())
                break; // Don't paint outside the cell
        }
    }
}

void DirectoryTableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    // Découpe le rendu à la zone de la cellule
    painter->setClipRect(option.rect);

    // Dessine l'arrière-plan de la cellule (sélection, focus, etc.)
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index); // Initialise avec les états actuels (sélectionné, survolé...)
    opt.widget->style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);

    QRect contentRect = option.rect;
    contentRect.adjust(2, 2, -2, -2); // Ajoute un peu de marge interne

    drawListItems(painter, contentRect, index, option.fontMetrics);

    painter->restore();
}

bool DirectoryTableDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    // if (event->type() == QEvent::MouseButtonDblClick)
    // {
    //     QMouseEvent * mouseEvent = static_cast<QMouseEvent*>(event);
    //     if (mouseEvent->button() == Qt::LeftButton)
    //     {
    //         // Il faut recréer les zones de dessin pour savoir quel sous-élément a été cliqué
    //         QList<ItemRectInfo> itemRects;
    //         QFontMetrics fm = option.fontMetrics;
    //         int lineHeight = fm.height() + 2;

    //         QRect contentRect = option.rect;
    //         contentRect.adjust(2, 2, -2, -2);

    //         int displayOptions = property("displayOptions").toInt();

    //         int currentY = contentRect.top();

    //         if (index.column() == 1) // Samples
    //         {
    //             QList<DirectorySampleData> samples = index.data(DirectoryTableModel::SampleListRole).value<QList<DirectorySampleData>>();
    //             for (int i = 0; i < samples.count(); ++i)
    //             {
    //                 QRect itemRect(contentRect.left(), currentY, contentRect.width(), lineHeight);
    //                 itemRects.append({itemRect, samples[i].id});
    //                 currentY += lineHeight;
    //             }
    //         }
    //         else if (index.column() == 2) // Instruments
    //         {
    //             QList<DirectoryInstrumentPresetData> instruments = index.data(DirectoryTableModel::InstrumentListRole).value<QList<DirectoryInstrumentPresetData>>();
    //             for (int i = 0; i < instruments.count(); ++i)
    //             {
    //                 QRect itemRect(contentRect.left(), currentY, contentRect.width(), lineHeight);
    //                 itemRects.append({itemRect, instruments[i].id, i});
    //                 currentY += lineHeight;
    //             }
    //         }
    //         else if (index.column() == 3) // Presets
    //         {
    //             QList<DirectoryInstrumentPresetData> presets = index.data(DirectoryTableModel::PresetListRole).value<QList<DirectoryInstrumentPresetData>>();
    //             for (int i = 0; i < presets.count(); ++i)
    //             {
    //                 QRect itemRect(contentRect.left(), currentY, contentRect.width(), lineHeight);
    //                 itemRects.append({itemRect, presets[i].id, i});
    //                 currentY += lineHeight;
    //             }
    //         }

    //         // Vérifie si le clic est tombé dans l'un des rectangles de sous-éléments
    //         for (const auto& info : itemRects)
    //         {
    //             if (info.rect.contains(mouseEvent->pos()))
    //             {
    //                 emit itemDoubleClicked(info.id, index.column());
    //                 return true; // Événement géré
    //             }
    //         }
    //     }
    // }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

QSize DirectoryTableDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // Calcule la hauteur nécessaire en fonction du contenu
    QFontMetrics fm = option.fontMetrics;
    int lineHeight = fm.height() + 2; // Hauteur de ligne + petit espacement

    int numItems = 0;
    int numLinesPerItem = 1; // Par défaut, 1 ligne par élément (non détaillé)

    if (index.column() == 1) // Samples
    {
        QList<DirectorySampleData> samples = index.data(DirectoryTableModel::SampleListRole).value<QList<DirectorySampleData>>();
        numItems = samples.count();
        // Les échantillons restent souvent sur une ligne même en détaillé dans ce toString()
    }
    else if (index.column() == 2 || index.column() == 3) // Instruments or presets
    {
        QList<DirectoryInstrumentPresetData> items = index.data(DirectoryTableModel::InstrumentListRole).value<QList<DirectoryInstrumentPresetData>>();
        numItems = items.count();
        // Les instruments/presets restent souvent sur une ligne même en détaillé dans ce toString()
    }

    if (numItems == 0)
        return QSize(option.rect.width(), QStyledItemDelegate::sizeHint(option, index).height());

    int totalHeight = (numItems * numLinesPerItem * lineHeight) + 4; // Ajoute un peu de padding
    return QSize(option.rect.width(), qMax(QStyledItemDelegate::sizeHint(option, index).height(), totalHeight));
}

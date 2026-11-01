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

#include "midiplayerdelegate.h"
#include <QTimer>
#include <QPainter>
#include <QGuiApplication>
#include "midiplayerchannel.h"
#include "contextmanager.h"

void MidiPlayerComboBox::showEvent(QShowEvent * event)
{
    QComboBox::showEvent(event);

    // After a double click the button is still pressed, the release going to the view that received the press
    if (QGuiApplication::mouseButtons() == Qt::NoButton)
        this->showPopup();
    else if (this->parentWidget() != nullptr)
        this->parentWidget()->installEventFilter(this);
}

bool MidiPlayerComboBox::eventFilter(QObject * watched, QEvent * event)
{
    if (event->type() == QEvent::MouseButtonRelease && watched == this->parentWidget())
    {
        watched->removeEventFilter(this);
        QTimer::singleShot(0, this, [this]() { this->showPopup(); });
    }
    return QComboBox::eventFilter(watched, event);
}

void MidiPlayerComboBox::hidePopup()
{
    // Finished after a possible choice has been processed
    QComboBox::hidePopup();
    if (!_isFinished)
    {
        _isFinished = true;
        QTimer::singleShot(0, this, &MidiPlayerComboBox::finished);
    }
}

MidiPlayerDelegate::MidiPlayerDelegate(const QList<MidiPlayerChannel *> &channels, QObject * parent) :
    QStyledItemDelegate(parent),
    _channels(channels),
    _gridColor(ContextManager::theme()->getColor(ThemeManager::BORDER))
{}

void MidiPlayerDelegate::paint(QPainter * painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    // Same lines as the grid of the editor tables, without the outer ones
    painter->save();
    painter->setPen(_gridColor);
    if (index.column() < index.model()->columnCount() - 1)
        painter->drawLine(option.rect.topRight(), option.rect.bottomRight());
    if (index.row() < index.model()->rowCount() - 1)
        painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());
    painter->restore();
}

QWidget * MidiPlayerDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    if ((index.column() != COLUMN_SOUNDFONT && index.column() != COLUMN_PRESET) || index.row() >= _channels.count())
        return nullptr;

    // Soundfonts, or presets of the soundfont
    MidiPlayerChannel * channel = _channels[index.row()];
    QList<QPair<QString, int> > choices = (index.column() == COLUMN_SOUNDFONT) ?
                                              channel->getSoundfontChoices() : channel->getPresetChoices();
    MidiPlayerComboBox * combobox = new MidiPlayerComboBox(parent);
    combobox->setStyleSheet("QComboBox{combobox-popup:0}"); // List limited to the visible items, otherwise as high as the screen
    combobox->setMaxVisibleItems(20);
    for (int i = 0; i < choices.count(); i++)
        combobox->addItem(choices[i].first, choices[i].second);

    // The choice is applied and the editor closed when the list closes
    MidiPlayerDelegate * delegate = const_cast<MidiPlayerDelegate *>(this);
    connect(combobox, &MidiPlayerComboBox::finished, delegate, [delegate, combobox]() {
        emit delegate->commitData(combobox);
        emit delegate->closeEditor(combobox);
    });
    return combobox;
}

void MidiPlayerDelegate::setEditorData(QWidget * editor, const QModelIndex &index) const
{
    QComboBox * combobox = dynamic_cast<QComboBox *>(editor);
    if (combobox == nullptr || index.row() >= _channels.count())
        return;
    MidiPlayerChannel * channel = _channels[index.row()];
    int current = (index.column() == COLUMN_SOUNDFONT) ? channel->getSoundfont() : channel->getForcedProgram();
    combobox->blockSignals(true);
    combobox->setCurrentIndex(qMax(0, combobox->findData(current)));
    combobox->blockSignals(false);
}

void MidiPlayerDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex &index) const
{
    // The channel updates the text of the table
    Q_UNUSED(model)
    QComboBox * combobox = dynamic_cast<QComboBox *>(editor);
    if (combobox == nullptr || combobox->currentIndex() < 0 || index.row() >= _channels.count())
        return;
    MidiPlayerChannel * channel = _channels[index.row()];
    int value = combobox->currentData().toInt();
    if (index.column() == COLUMN_SOUNDFONT)
    {
        if (value != channel->getSoundfont())
            channel->setSoundfont(value);
    }
    else if (value != channel->getForcedProgram())
        channel->setForcedProgram(value);
}

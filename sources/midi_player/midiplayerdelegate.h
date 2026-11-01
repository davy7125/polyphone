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

#ifndef MIDIPLAYERDELEGATE_H
#define MIDIPLAYERDELEGATE_H

#include <QStyledItemDelegate>
#include <QComboBox>
class MidiPlayerChannel;

// Editor of the table of the MIDI player: its list opens when it is shown (once the click that opened the editor is
// released, otherwise the release would close it), and it is finished when the list closes (with or without a choice)
class MidiPlayerComboBox : public QComboBox
{
    Q_OBJECT

public:
    MidiPlayerComboBox(QWidget * parent = nullptr) : QComboBox(parent), _isFinished(false) {}
    void hidePopup() override;

signals:
    void finished(); // Only once

protected:
    void showEvent(QShowEvent * event) override;
    bool eventFilter(QObject * watched, QEvent * event) override;

private:
    bool _isFinished;
};

// Choice of the soundfont and of the preset of the channels in the table of the MIDI player, a row being a channel. The
// grid is drawn without the lines on the right side of the last column and at the bottom of the last row, the edges of
// the table being the ones of the window.
class MidiPlayerDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    // Columns of the table
    static const int COLUMN_CHANNEL = 0;
    static const int COLUMN_SOUNDFONT = 1;
    static const int COLUMN_PRESET = 2;
    static const int COLUMN_VOLUME = 3;

    MidiPlayerDelegate(const QList<MidiPlayerChannel *> &channels, QObject * parent = nullptr);

protected:
    void paint(QPainter * painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget * editor, const QModelIndex &index) const override;
    void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex &index) const override;

private:
    const QList<MidiPlayerChannel *> &_channels;
    QColor _gridColor;
};

#endif // MIDIPLAYERDELEGATE_H

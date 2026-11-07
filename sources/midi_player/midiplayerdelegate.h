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
class QVariantAnimation;

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
// the table being the ones of the window. While the soundfont selected in the main window is loading, a spinner turns in
// front of the soundfont of the channels following the selection (not in the list).
class MidiPlayerDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    // Columns of the table
    static constexpr int COLUMN_CHANNEL = 0;
    static constexpr int COLUMN_SOUNDFONT = 1;
    static constexpr int COLUMN_PRESET = 2;
    static constexpr int COLUMN_VOLUME = 3;

    // The parent is the table
    MidiPlayerDelegate(const QList<MidiPlayerChannel *> &channels, QObject * parent = nullptr);

    // Spinner shown or not, and the room it takes in front of the text
    void setLoading(bool isLoading);
    static int getSpinnerWidth();

protected:
    void initStyleOption(QStyleOptionViewItem * option, const QModelIndex &index) const override;
    void paint(QPainter * painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget * editor, const QModelIndex &index) const override;
    void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex &index) const override;

private slots:
    void updateSoundfontColumn();

private:
    bool isSpinnerShown(const QModelIndex &index) const;

    const QList<MidiPlayerChannel *> &_channels;
    QColor _gridColor;
    bool _isLoading;
    QVariantAnimation * _animation;
    QPixmap _spinner, _spinnerSelected; // Twice as big, for high resolution screens
    QIcon _blankIcon; // Room of the spinner in front of the text

    static const int SPINNER_SIZE;
};

#endif // MIDIPLAYERDELEGATE_H

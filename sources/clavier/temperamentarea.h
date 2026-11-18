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

#ifndef TEMPERAMENTAREA_H
#define TEMPERAMENTAREA_H

#include <QWidget>
#include "temperamentsettings.h"
class QComboBox;
class QCheckBox;
class QLabel;
class QToolButton;
class ReferencePitchEditor;
class TemperamentComboBox;
class TemperamentKeyboard;

// Tuning of the sound engine, also editable in the keyboard settings: the reference pitch (note = frequency) and
// the transposition, completed on demand (microtonal tuning) by the temperament with its tonality and the use of the
// notes not used on the left, the fine tunes of the 12 notes on the right
class TemperamentArea : public QWidget
{
    Q_OBJECT

public:
    explicit TemperamentArea(QWidget * parent = nullptr);

signals:
    // The temperaments and scales have been shown or hidden: the height changed
    void detailsToggled();

private slots:
    void updateValues();
    void updateNames();
    void onTemperamentChanged(int index);
    void onTonalityChanged(int index);
    void onKeyboardEdited();
    void onTonalityRequested(int key);
    void onTranspositionChanged(int index);
    void onPlayUnusedNotesToggled(bool checked);
    void onDetailsClicked();

private:
    void updateTemperamentWidgets();
    void updateDetails();
    void saveTemperament();

    ReferencePitchEditor * _referencePitchEditor;
    QComboBox * _comboTransposition;
    QToolButton * _buttonDetails;
    QLabel * _labelTemperament;
    TemperamentComboBox * _comboTemperament;
    QLabel * _labelTonality;
    QComboBox * _comboTonality;
    QCheckBox * _checkPlayUnusedNotes;
    TemperamentKeyboard * _keyboard;
    TemperamentSettings _temperament;
    bool _isSaving; // The configuration is being changed here
    bool _areDetailsShown; // Temperaments and scales
};

#endif // TEMPERAMENTAREA_H

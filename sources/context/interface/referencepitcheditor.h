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

#ifndef REFERENCEPITCHEDITOR_H
#define REFERENCEPITCHEDITOR_H

#include <QWidget>
class QComboBox;
class QDoubleSpinBox;
class QLabel;
class QGridLayout;

// Reference pitch of the sound engine, "note = frequency" (A4 = 440 Hz by default): the note sounds at this frequency
// whatever the temperament. Changing the note shows its frequency in the equal temperament. The configuration is read
// again when it changes elsewhere.
class ReferencePitchEditor : public QWidget
{
    Q_OBJECT

public:
    explicit ReferencePitchEditor(QWidget * parent = nullptr);

    // Back to A4 = 440 Hz
    void reset();

    // The note, "=" and the frequency placed in 3 columns of a grid (widgets of other rows aligned with the note), this
    // widget being hidden
    void moveToGrid(QGridLayout * grid, int row, int column);

public slots:
    // Names of the notes as configured: number with the name of the note, or name with the octave
    void updateNames();

private slots:
    void updateValues();
    void onKeyChanged(int index);
    void onFrequencyChanged(double value);

private:
    QComboBox * _comboKey;
    QLabel * _labelEqual;
    QDoubleSpinBox * _spinFrequency;
    bool _isSaving; // The configuration is being changed here
};

#endif // REFERENCEPITCHEDITOR_H

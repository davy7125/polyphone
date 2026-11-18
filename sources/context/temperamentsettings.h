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

#ifndef TEMPERAMENTSETTINGS_H
#define TEMPERAMENTSETTINGS_H

#include <QCoreApplication>
#include <QString>

// Temperament of the sound engine, read from and written to the configuration ("temperament", "temperament_custom"
// and "temperament_play_unused"), shared by the keyboard settings and the area of the virtual keyboard.
// The reference pitch (a note and its frequency) is independent of the temperament.
class TemperamentSettings
{
    Q_DECLARE_TR_FUNCTIONS(TemperamentSettings)

public:
    // Tuning program of the custom temperament (the others have positive indexes)
    static const int CUSTOM = -1;
    static QString getCustomName();

    TemperamentSettings();

    // Read the configuration, write the temperament
    void load();
    void save() const;

    // Tuning program (CUSTOM for the custom temperament) and tonality (0 = C), not used by the equal temperament
    int getProgram() const { return _program; }
    int getTonality() const { return _tonality; }
    bool isTonalityUsed() const { return _program != 0; }

    // Fine tunes in cents and use of the keys from C to B, the temperament being transposed to the tonality
    void getTemperament(double fineTunes[12], bool used[12]) const;

    // Changes, to be saved. The custom temperament starts from the temperament shown if it has never been defined; a
    // temperament given (transposed to the tonality) becomes the custom temperament.
    void selectProgram(int program);
    void setTonality(int tonality);
    void setTemperament(const double fineTunes[12], const bool used[12]);

    // Transposition in semitones of the keys played (keyboard and MIDI input), from -12 to 12 (saved at once)
    static int getTransposition();
    static void setTransposition(int semitones);
    static QString getTranspositionText(int semitones);
    static QString getTranspositionToolTip();

    // Notes not used played anyway (saved at once)
    static bool playsUnusedNotes();
    static void setPlayUnusedNotes(bool playUnusedNotes);

    // Reference pitch: a note (0 = C) sounding at a frequency whatever the temperament, A by default. The frequency of
    // A4 is stored, the frequency of a note being the one of its equal temperament with A4, between A#3 and A4.
    static int getReferencePitchKey();
    static double getReferencePitch(int key); // Hz
    static double getReferencePitchMinimum(int key);
    static double getReferencePitchMaximum(int key);
    static void setReferencePitch(int key, double frequency); // Saved at once

private:
    static bool parseFineTunes(const QStringList &parts, int first, double fineTunes[12], bool used[12]);
    static double getRatioToA4(int key);

    int _program;
    int _tonality;
    double _customTemperament[12]; // From the tonic to the 12th note
    bool _customUsed[12];
    int _customTonality;
    bool _customTemperamentDefined;
};

#endif // TEMPERAMENTSETTINGS_H

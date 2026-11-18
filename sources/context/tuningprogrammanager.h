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

#ifndef TUNINGPROGRAMMANAGER_H
#define TUNINGPROGRAMMANAGER_H

#include <QString>
#include <QStringList>
#include <QMap>

class TuningProgram
{
public:
    TuningProgram();

    // Configuration of the sound engine for a relative key (tonality, see TuningProgramManager::getConfiguration)
    QString getConfiguration(int relativeKey) const;

    QString _name; // Translated
    QStringList _category; // Levels, translated (empty for the equal temperament and the MIDI tuning programs)
    float _deviations[12]; // From the tonic (C) to B
    bool _used[12];
};

class TuningProgramManager
{
public:
    static TuningProgramManager * getInstance();
    static void kill();
    bool addTuningProgram(int index, QString name, const double deviations[12]);

    // Configuration of the sound engine ("temperament"): name, 12 fine tunes from the tonic (C) to B ("-" for a key
    // not used) and relative key (tonality)
    static QString getConfiguration(QString name, const double fineTunes[12], const bool used[12], int relativeKey);

    TuningProgram * getTuningProgram(int index);
    QMap<int, TuningProgram> * getAllPrograms();

private:
    TuningProgramManager();
    ~TuningProgramManager();

    bool _defaultPrograms;
    QMap<int, TuningProgram> _tuningPrograms;
    QString _settingToRestore;

    static TuningProgramManager * s_instance;
};

#endif // TUNINGPROGRAMMANAGER_H

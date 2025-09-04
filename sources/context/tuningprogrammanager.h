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
#include <QMap>

class TuningProgram
{
public:
    TuningProgram();
    bool parse(QString description);
    QString getDescription();

    QString _name;
    float _deviations[12];
};

class TuningProgramManager
{
public:
    static TuningProgramManager * getInstance();
    static void kill();
    bool addTuningProgram(int index, QString name, const double deviations[12]);

    TuningProgram * getTuningProgram(int index);
    QMap<int, TuningProgram> * getAllPrograms();
    bool hasBeenUpdated() { return _updated; }

private:
    TuningProgramManager();
    ~TuningProgramManager();

    bool _defaultPrograms;
    bool _updated;
    QMap<int, TuningProgram> _tuningPrograms;
    QString _settingToRestore;

    static TuningProgramManager * s_instance;
};

#endif // TUNINGPROGRAMMANAGER_H

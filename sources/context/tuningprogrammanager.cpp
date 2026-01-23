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

#include "tuningprogrammanager.h"
#include <QFile>
#include <QTextStream>
#include "contextmanager.h"

TuningProgramManager * TuningProgramManager::s_instance = nullptr;

TuningProgram::TuningProgram()
{

}

bool TuningProgram::parse(QString description)
{
    QStringList split = description.split(',');
    if (split.count() != 13)
        return false;

    this->_name = split[0];
    bool ok;
    for (int i = 1; i <= 12; i++)
    {
        _deviations[i - 1] = split[i].toFloat(&ok);
        if (!ok)
            return false;
    }

    return true;
}

QString TuningProgram::getDescription()
{
    QStringList parts;
    parts << _name;
    for (float v : _deviations)
        parts << QString::number(v, 'f', 2);
    return parts.join(",");
}

TuningProgramManager * TuningProgramManager::getInstance()
{
    if (s_instance == nullptr)
        s_instance = new TuningProgramManager();
    return s_instance;
}

void TuningProgramManager::kill()
{
    if (s_instance != nullptr)
    {
        delete s_instance;
        s_instance = nullptr;
    }
}

TuningProgramManager::TuningProgramManager() :
    _defaultPrograms(true),
    _updated(true)
{
    // Load the default tuning programs
    _tuningPrograms[0].parse(QObject::tr("Equal", "Equal musical temperament") + ",0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0");
    QFile file(":/misc/temperaments.csv");
    int index = 1;
    if (file.open(QIODevice::ReadOnly | QFile::Text))
    {
        QTextStream in(&file);
        QString line;
        while (!(line = in.readLine()).isEmpty())
        {
            if (_tuningPrograms[index].parse(line))
                index++;
            else
                _tuningPrograms.remove(index);
        }
    }
}

TuningProgramManager::~TuningProgramManager()
{
    if (!_settingToRestore.isEmpty())
        ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "temperament", _settingToRestore);
}

bool TuningProgramManager::addTuningProgram(int index, QString name, const double deviations[12])
{
    if (index < 0 || index > 16383)
        return false;

    if (_defaultPrograms)
    {
        _tuningPrograms.clear();
        _defaultPrograms = false;
        _settingToRestore = ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "temperament", "").toString();
    }
    _updated = true;

    _tuningPrograms[index]._name = name.isEmpty() ? QObject::tr("Tuning program %1:%2").arg(index / 128).arg(index % 128) : name;
    for (int i = 0; i < 12; i++)
        _tuningPrograms[index]._deviations[i] = static_cast<float>(deviations[i]);

    return true;
}

TuningProgram * TuningProgramManager::getTuningProgram(int index)
{
    if (_tuningPrograms.contains(index))
        return &_tuningPrograms[index];
    return nullptr;
}

QMap<int, TuningProgram> * TuningProgramManager::getAllPrograms()
{
    _updated = false;
    return &_tuningPrograms;
}

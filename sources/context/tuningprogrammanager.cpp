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
#include <QCoreApplication>
#include "contextmanager.h"
#include "temperaments.h"

TuningProgramManager * TuningProgramManager::s_instance = nullptr;

TuningProgram::TuningProgram()
{
    for (int i = 0; i < 12; i++)
    {
        _deviations[i] = 0;
        _used[i] = true;
    }
}

QString TuningProgram::getConfiguration(int relativeKey) const
{
    double fineTunes[12];
    for (int i = 0; i < 12; i++)
        fineTunes[i] = static_cast<double>(_deviations[i]);
    return TuningProgramManager::getConfiguration(_name, fineTunes, _used, relativeKey);
}

QString TuningProgramManager::getConfiguration(QString name, const double fineTunes[12], const bool used[12],
                                               int relativeKey)
{
    QStringList parts;
    parts << name;
    for (int i = 0; i < 12; i++)
        parts << (used[i] ? QString::number(fineTunes[i], 'f', 2) : "-");
    parts << QString::number(relativeKey);
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
    _defaultPrograms(true)
{
    // Equal temperament, then the temperaments and scales of the file (names and categories translated)
    _tuningPrograms[0]._name = QObject::tr("Equal", "Equal musical temperament");
    QFile file(":/misc/temperaments.csv");
    if (!file.open(QIODevice::ReadOnly))
        return;
    std::vector<TemperamentData> temperaments = Temperaments::parse(file.readAll().toStdString());
    for (size_t i = 0; i < temperaments.size(); i++)
    {
        const TemperamentData &temperament = temperaments[i];
        TuningProgram &program = _tuningPrograms[static_cast<int>(i) + 1];
        program._name = QCoreApplication::translate(Temperaments::TRANSLATION_CONTEXT, temperament.name.c_str());
        for (const QString &level : QString::fromStdString(temperament.category).split('/', Qt::SkipEmptyParts))
            program._category << QCoreApplication::translate(Temperaments::TRANSLATION_CONTEXT, level.toUtf8().constData());
        for (int key = 0; key < 12; key++)
        {
            program._deviations[key] = temperament.fineTunes[key];
            program._used[key] = temperament.used[key];
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

    TuningProgram program;
    program._name = name.isEmpty() ? QObject::tr("Tuning program %1:%2").arg(index / 128).arg(index % 128) : name;
    for (int i = 0; i < 12; i++)
        program._deviations[i] = static_cast<float>(deviations[i]);
    _tuningPrograms[index] = program;

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
    return &_tuningPrograms;
}

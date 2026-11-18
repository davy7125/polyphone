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

#include "temperamentsettings.h"
#include "contextmanager.h"
#include "tuningprogrammanager.h"
#include <QtMath>

QString TemperamentSettings::getCustomName()
{
    return tr("Custom", "custom temperament");
}

TemperamentSettings::TemperamentSettings() :
    _program(0),
    _tonality(0),
    _customTonality(0),
    _customTemperamentDefined(false)
{
    for (int key = 0; key < 12; key++)
    {
        _customTemperament[key] = 0;
        _customUsed[key] = true;
    }
}

bool TemperamentSettings::parseFineTunes(const QStringList &parts, int first, double fineTunes[12], bool used[12])
{
    // 12 fine tunes, "-" for a note not used
    for (int key = 0; key < 12; key++)
    {
        used[key] = (parts[first + key] != "-");
        fineTunes[key] = 0;
        bool ok = true;
        if (used[key])
            fineTunes[key] = parts[first + key].toDouble(&ok);
        if (!ok)
            return false;
    }
    return true;
}

void TemperamentSettings::load()
{
    // Custom temperament, kept when another one is selected: 12 fine tunes from its tonic ("-" for a note not used)
    // and its tonality
    QStringList customConfig = ContextManager::configuration()->getValue(
                ConfManager::SECTION_SOUND_ENGINE, "temperament_custom", "").toString().split(",");
    double values[12];
    bool used[12];
    if ((customConfig.size() == 12 || customConfig.size() == 13) && parseFineTunes(customConfig, 0, values, used))
    {
        for (int key = 0; key < 12; key++)
        {
            _customTemperament[key] = values[key];
            _customUsed[key] = used[key];
        }
        _customTonality = customConfig.size() == 13 ? (customConfig[12].toInt() % 12 + 12) % 12 : 0;
        _customTemperamentDefined = true;
    }

    // Current temperament: a name, 12 fine tunes from the tonic (C) to B ("-" for a note not used) and the tonality
    // (a 15th field written by a development version is ignored), equal temperament if there is no configuration
    QStringList currentConfig = ContextManager::configuration()->getValue(
                ConfManager::SECTION_SOUND_ENGINE, "temperament", "").toString().split(",");
    _program = 0;
    _tonality = 0;
    if ((currentConfig.size() != 14 && currentConfig.size() != 15) || !parseFineTunes(currentConfig, 1, values, used))
        return;
    _tonality = (currentConfig[13].toInt() % 12 + 12) % 12;

    // Temperament with the same name and the same definition, else the custom one if it is its name, else a
    // temperament having the same definition (the name of the equal temperament is translated), else custom
    QMap<int, TuningProgram> * programs = TuningProgramManager::getInstance()->getAllPrograms();
    for (int pass = 0; pass < 2; pass++)
    {
        if (pass == 1 && currentConfig[0] == getCustomName())
            break;
        for (auto i = programs->cbegin(), end = programs->cend(); i != end; ++i)
        {
            const TuningProgram &program = i.value();
            if (pass == 0 && program._name != currentConfig[0])
                continue;
            bool same = true;
            for (int key = 0; key < 12 && same; key++)
                same = (program._used[key] == used[key]) &&
                       (!used[key] || qAbs(static_cast<double>(program._deviations[key]) - values[key]) < 0.006);
            if (same)
            {
                _program = i.key();
                return;
            }
        }
    }

    // Custom temperament
    _program = CUSTOM;
    for (int key = 0; key < 12; key++)
    {
        _customTemperament[key] = values[key];
        _customUsed[key] = used[key];
    }
    _customTonality = _tonality;
    _customTemperamentDefined = true;
}

void TemperamentSettings::save() const
{
    if (_program == CUSTOM)
    {
        QStringList values;
        for (int key = 0; key < 12; key++)
            values << (_customUsed[key] ? QString::number(_customTemperament[key], 'f', 2) : "-");
        ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "temperament_custom",
                                                  values.join(",") + "," + QString::number(_customTonality));
        ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "temperament",
                                                  TuningProgramManager::getConfiguration(
                                                      getCustomName(), _customTemperament, _customUsed, _tonality));
        return;
    }

    TuningProgram * program = TuningProgramManager::getInstance()->getTuningProgram(_program);
    if (program != nullptr)
        ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "temperament",
                                                  program->getConfiguration(_tonality));
}

void TemperamentSettings::getTemperament(double fineTunes[12], bool used[12]) const
{
    // Fine tunes as defined by the temperament, transposed to the tonality: the synth shifts all of them so that the
    // note of the reference pitch keeps its pitch in the equal temperament (VoiceContext::getTemperamentFineTune), a
    // same value on all keys being thus equivalent to 0
    TuningProgram * program = (_program == CUSTOM) ? nullptr : TuningProgramManager::getInstance()->getTuningProgram(_program);
    for (int key = 0; key < 12; key++)
    {
        int degree = (key - _tonality + 12) % 12;
        if (_program == CUSTOM)
        {
            fineTunes[key] = _customTemperament[degree];
            used[key] = _customUsed[degree];
        }
        else
        {
            fineTunes[key] = program == nullptr ? 0 : static_cast<double>(program->_deviations[degree]);
            used[key] = program == nullptr ? true : program->_used[degree];
        }
    }
}

void TemperamentSettings::selectProgram(int program)
{
    if (program == CUSTOM)
    {
        if (_customTemperamentDefined)
            _tonality = _customTonality;
        else
        {
            // The custom temperament starts from the temperament shown
            double fineTunes[12];
            bool used[12];
            getTemperament(fineTunes, used);
            setTemperament(fineTunes, used);
        }
    }
    _program = program;
}

void TemperamentSettings::setTonality(int tonality)
{
    _tonality = (tonality % 12 + 12) % 12;
    if (_program == CUSTOM)
        _customTonality = _tonality;
}

void TemperamentSettings::setTemperament(const double fineTunes[12], const bool used[12])
{
    // Stored from the tonic
    for (int key = 0; key < 12; key++)
    {
        _customTemperament[(key - _tonality + 12) % 12] = fineTunes[key];
        _customUsed[(key - _tonality + 12) % 12] = used[key];
    }
    _customTonality = _tonality;
    _customTemperamentDefined = true;
    _program = CUSTOM;
}

int TemperamentSettings::getTransposition()
{
    return qBound(-12, ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "transposition", 0).toInt(), 12);
}

void TemperamentSettings::setTransposition(int semitones)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "transposition", qBound(-12, semitones, 12));
}

QString TemperamentSettings::getTranspositionText(int semitones)
{
    return (semitones > 0 ? "+" : "") + QString::number(semitones);
}

QString TemperamentSettings::getTranspositionToolTip()
{
    return tr("Transposition in semitones of the notes played with the keyboard or a MIDI input");
}

bool TemperamentSettings::playsUnusedNotes()
{
    return ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "temperament_play_unused", false).toBool();
}

void TemperamentSettings::setPlayUnusedNotes(bool playUnusedNotes)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "temperament_play_unused", playUnusedNotes);
}

int TemperamentSettings::getReferencePitchKey()
{
    return (ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "reference_pitch_key", 9).toInt() % 12 + 12) % 12;
}

double TemperamentSettings::getRatioToA4(int key)
{
    // Notes from A#3 to A4, so that the step of 0.1 Hz of A4 is not larger for the other notes
    return qPow(2.0, ((key <= 9 ? key : key - 12) - 9) / 12.0);
}

double TemperamentSettings::getReferencePitch(int key)
{
    return 0.1 * ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "reference_pitch", 4400).toInt() *
           getRatioToA4(key);
}

double TemperamentSettings::getReferencePitchMinimum(int key)
{
    return 390.0 * getRatioToA4(key);
}

double TemperamentSettings::getReferencePitchMaximum(int key)
{
    return 470.0 * getRatioToA4(key);
}

void TemperamentSettings::setReferencePitch(int key, double frequency)
{
    // Note, then frequency of A4
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "reference_pitch_key", (key % 12 + 12) % 12);
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "reference_pitch",
                                              qRound(10.0 * frequency / getRatioToA4(key)));
}

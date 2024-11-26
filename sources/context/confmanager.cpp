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

#include "confmanager.h"
#include <QFileInfo>
#include <QDir>
#include "modulatordata.h"
#include "pushstereoediting.h"
#include "synth.h"

ConfManager::ConfManager(): QObject(),
    _settings(this),
    _synthConfig(new SynthConfig())
{
    // Get the previous version installed
    QStringList version = this->getValue(SECTION_NONE, "last_version_installed", "0.0").toString().split(".");
    int v_major = 0;
    int v_minor = 0;
    int v_fix = 0;
    if (version.count() == 2)
    {
        v_major = version[0].toInt();
        v_minor = version[1].toInt();
    }
    else if (version.count() == 3)
    {
        v_major = version[0].toInt();
        v_minor = version[1].toInt();
        v_fix = version[2].toInt();
    }
    Q_UNUSED(v_fix)

    // Possible changes in the configurations
    if (v_major < 2)
    {
        // Clear everything if the previous version was < 2.0
        this->clear();
    }
    else
    {
        if (v_major == 2 && v_minor == 0)
        {
            // One key moved from "keyboard" to "midi"
            _settings.setValue("midi/velocity", _settings.value("keyboard/velocity"));
            _settings.remove("keyboard/velocity");

            // 1 key moved from the general section to "display"
            _settings.setValue("display/name_middle_c", _settings.value("name_middle_c"));
            _settings.remove("name_middle_c");

            // Everything from "map" is now in "keyboard"
            _settings.beginGroup("map");
            QStringList keys = _settings.allKeys();
            _settings.endGroup();
            foreach (QString key, keys)
                _settings.setValue("keyboard/" + key, _settings.value("map/" + key));
            _settings.remove("map");

            // "affichage" renamed in "display"
            _settings.beginGroup("affichage");
            keys = _settings.allKeys();
            _settings.endGroup();
            foreach (QString key, keys)
                _settings.setValue("display/" + key, _settings.value("affichage/" + key));
            _settings.remove("affichage");
        }

        if (v_major * 100 + v_minor <= 203)
        {
            // Remove the controller values
            for (int i = 0; i < 128; i++)
                _settings.remove("midi/CC_" + QString("%1").arg(i, 3, 10, QChar('0')));

            // username becomes email
            _settings.setValue("repository/email", _settings.value("repository/username"));
            _settings.remove("repository/username");
        }
    }

    // Special initialization here (for more speed when reading modulator_vel_to_filter)
    ModulatorData::setModulatorVelToFilterType(this->getValue(ConfManager::SECTION_SOUND_ENGINE, "modulator_vel_to_filter", 1).toInt());
}

ConfManager::~ConfManager()
{
    delete _synthConfig;
}

QVariant ConfManager::getValue(Section section, QString key, QVariant defaultValue) const
{
    return getValue(section, "", key, defaultValue);
}

QVariant ConfManager::getValue(Section section, QString subSection, QString key, QVariant defaultValue) const
{
#if defined(Q_OS_MACOS)
    // On MAC OS cannot resize window if the borders are hidden, so this option return always "true"
    if (section == ConfManager::SECTION_DISPLAY && key == "window_borders")
        return true;
#endif
    return _settings.value(getFullKey(section, subSection, key), defaultValue);
}

void ConfManager::setValue(Section section, QString key, QVariant value)
{
    setValue(section, "", key, value);
}

void ConfManager::setValue(Section section, QString subSection, QString key, QVariant value)
{
    _settings.setValue(getFullKey(section, subSection, key), value);

    // Possibly update elements
    switch (section)
    {
    case Section::SECTION_NONE:
        if (key == "stereo_modification")
            PushStereoEditing::SetState(value.toBool());
        break;
    case Section::SECTION_SOUND_ENGINE:
        emit soundEngineConfigurationChanged();
        if (key == "modulator_vel_to_filter")
            ModulatorData::setModulatorVelToFilterType(value.toInt());
        break;
    case Section::SECTION_AUDIO:
        if (key != "stereo_playback")
        {
            emit soundEngineConfigurationChanged(); // First prepare the sound engine (the buffer can be adjusted)
            emit audioServerConfigurationChanged(); // Then update the audio server configuration
        }
        break;
    case Section::SECTION_KEYBOARD:
        emit keyMapChanged();
        if (key == "octave_offset")
            emit keyboardOctaveChanged();
        break;
    case Section::SECTION_DISPLAY:
        if (key == "division_sort")
            emit divisionSortChanged();
        else if (key == "name_middle_c")
            emit interfaceChanged();
        break;
    default:
        // Nothing
        break;
    }
}

QString ConfManager::getFullKey(Section section, QString subSection, QString key) const
{
    // First part depends on the section
    QString firstPart = "";
    switch (section)
    {
    case SECTION_NONE:         firstPart = "";              break;
    case SECTION_DISPLAY:      firstPart = "display";       break;
    case SECTION_AUDIO:        firstPart = "audio";         break;
    case SECTION_BULK_RENAME:  firstPart = "bulk_rename";   break;
    case SECTION_COLORS:       firstPart = "colors";        break;
    case SECTION_KEYBOARD:     firstPart = "keyboard";      break;
    case SECTION_MIDI:         firstPart = "midi";          break;
    case SECTION_RECENT_FILES: firstPart = "recent_file";   break;
    case SECTION_SOUND_ENGINE: firstPart = "synth";         break;
    case SECTION_TOOLS:        firstPart = "tools";         break;
    case SECTION_WARNINGS:     firstPart = "warnings";      break;
    case SECTION_REPOSITORY:   firstPart = "repository";    break;
    case SECTION_EXTENSIONS:   firstPart = "extensions";    break;
    }
    if (firstPart != "")
        firstPart += "/";

    if (!subSection.isEmpty())
        firstPart += subSection + "/";

    return firstPart + key;
}

QString ConfManager::getConfigDir()
{
    return QFileInfo(_settings.fileName()).dir().path();
}

QString ConfManager::getMapping(int numOctave, Key key)
{
    QString txt = "";

    // Default value
    switch (numOctave)
    {
    case 0:
        switch (key)
        {
        case KEY_C_LEFT:  txt = tr("Z", "first key mapping for bottom left C"); break;
        case KEY_C_SHARP: txt = tr("S", "first key mapping for bottom C#"); break;
        case KEY_D:       txt = tr("X", "first key mapping for bottom D"); break;
        case KEY_D_SHARP: txt = tr("D", "first key mapping for bottom D#"); break;
        case KEY_E:       txt = tr("C", "first key mapping for bottom E"); break;
        case KEY_F:       txt = tr("V", "first key mapping for bottom F"); break;
        case KEY_F_SHARP: txt = tr("G", "first key mapping for bottom F#"); break;
        case KEY_G:       txt = tr("B", "first key mapping for bottom G"); break;
        case KEY_G_SHARP: txt = tr("H", "first key mapping for bottom G#"); break;
        case KEY_A:       txt = tr("N", "first key mapping for bottom A"); break;
        case KEY_A_SHARP: txt = tr("J", "first key mapping for bottom A#"); break;
        case KEY_B:       txt = tr("M", "first key mapping for bottom B"); break;
        case KEY_C_RIGHT: txt = tr(",", "first key mapping for bottom right C"); break;
        }
        break;
    case 1:
        switch (key)
        {
        case KEY_C_LEFT:  txt = tr("Q", "first key mapping for upper left C"); break;
        case KEY_C_SHARP: txt = tr("2", "first key mapping for upper C#"); break;
        case KEY_D:       txt = tr("W", "first key mapping for upper D"); break;
        case KEY_D_SHARP: txt = tr("3", "first key mapping for upper D#"); break;
        case KEY_E:       txt = tr("E", "first key mapping for upper E"); break;
        case KEY_F:       txt = tr("R", "first key mapping for upper F"); break;
        case KEY_F_SHARP: txt = tr("5", "first key mapping for upper F#"); break;
        case KEY_G:       txt = tr("T", "first key mapping for upper G"); break;
        case KEY_G_SHARP: txt = tr("6", "first key mapping for upper G#"); break;
        case KEY_A:       txt = tr("Y", "first key mapping for upper A"); break;
        case KEY_A_SHARP: txt = tr("7", "first key mapping for upper A#"); break;
        case KEY_B:       txt = tr("U", "first key mapping for upper B"); break;
        case KEY_C_RIGHT: txt = tr("I", "first key mapping for upper right C"); break;
        }
        break;
    case 2:
        switch (key)
        {
        case KEY_C_LEFT:  txt = tr("Shift+Z", "second key mapping for bottom left C"); break;
        case KEY_C_SHARP: txt = tr("Shift+S", "second key mapping for bottom C#"); break;
        case KEY_D:       txt = tr("Shift+X", "second key mapping for bottom D"); break;
        case KEY_D_SHARP: txt = tr("Shift+D", "second key mapping for bottom D#"); break;
        case KEY_E:       txt = tr("Shift+C", "second key mapping for bottom E"); break;
        case KEY_F:       txt = tr("Shift+V", "second key mapping for bottom F"); break;
        case KEY_F_SHARP: txt = tr("Shift+G", "second key mapping for bottom F#"); break;
        case KEY_G:       txt = tr("Shift+B", "second key mapping for bottom G"); break;
        case KEY_G_SHARP: txt = tr("Shift+H", "second key mapping for bottom G#"); break;
        case KEY_A:       txt = tr("Shift+N", "second key mapping for bottom A"); break;
        case KEY_A_SHARP: txt = tr("Shift+J", "second key mapping for bottom A#"); break;
        case KEY_B:       txt = tr("Shift+M", "second key mapping for bottom B"); break;
        case KEY_C_RIGHT: txt = tr("Shift+<", "second key mapping for bottom right C"); break;
        }
        break;
    case 3:
        switch (key)
        {
        case KEY_C_LEFT:  txt = tr("Shift+Q", "second key mapping for upper left C"); break;
        case KEY_C_SHARP: txt = tr("Shift+@", "second key mapping for upper C#"); break;
        case KEY_D:       txt = tr("Shift+W", "second key mapping for upper D"); break;
        case KEY_D_SHARP: txt = tr("Shift+#", "second key mapping for upper D#"); break;
        case KEY_E:       txt = tr("Shift+E", "second key mapping for upper E"); break;
        case KEY_F:       txt = tr("Shift+R", "second key mapping for upper F"); break;
        case KEY_F_SHARP: txt = tr("Shift+%", "second key mapping for upper F#"); break;
        case KEY_G:       txt = tr("Shift+T", "second key mapping for upper G"); break;
        case KEY_G_SHARP: txt = tr("Shift+^", "second key mapping for upper G#"); break;
        case KEY_A:       txt = tr("Shift+Y", "second key mapping for upper A"); break;
        case KEY_A_SHARP: txt = tr("Shift+&", "second key mapping for upper A#"); break;
        case KEY_B:       txt = tr("Shift+U", "second key mapping for upper B"); break;
        case KEY_C_RIGHT: txt = tr("Shift+I", "second key mapping for upper right C"); break;
        }
        break;
    default:
        break;
    }

    // The default value is possibly overridden
    QString override = this->getValue(SECTION_KEYBOARD, "key_" + QString::number(numOctave) + "_" + QString::number((int)key), txt).toString();
    return (override != "") ? override : txt;
}

void ConfManager::clear()
{
    _settings.clear();
}

SynthConfig * ConfManager::getSynthConfig()
{
    _synthConfig->choLevel = this->getValue(ConfManager::SECTION_SOUND_ENGINE, "cho_level", 0).toInt();
    _synthConfig->choDepth = this->getValue(ConfManager::SECTION_SOUND_ENGINE, "cho_depth", 0).toInt();
    _synthConfig->choFrequency = this->getValue(ConfManager::SECTION_SOUND_ENGINE, "cho_frequency", 0).toInt();
    _synthConfig->revLevel = this->getValue(ConfManager::SECTION_SOUND_ENGINE, "rev_level", 0).toInt();
    _synthConfig->revSize = this->getValue(ConfManager::SECTION_SOUND_ENGINE, "rev_size", 0).toInt();
    _synthConfig->revWidth = this->getValue(ConfManager::SECTION_SOUND_ENGINE, "rev_width", 0).toInt();
    _synthConfig->revDamping = this->getValue(ConfManager::SECTION_SOUND_ENGINE, "rev_damping", 0).toInt();
    _synthConfig->masterGain = this->getValue(ConfManager::SECTION_SOUND_ENGINE, "gain", 0).toInt();
    _synthConfig->tuningFork = this->getValue(ConfManager::SECTION_SOUND_ENGINE, "tuning_fork", 440).toInt();
    _synthConfig->temperament = this->getValue(ConfManager::SECTION_SOUND_ENGINE, "temperament", "").toString().split(",");
    return _synthConfig;
}

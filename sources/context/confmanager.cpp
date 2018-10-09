/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2017 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://polyphone-soundfonts.com                      **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "confmanager.h"
#include <QFileInfo>
#include <QDir>

ConfManager::ConfManager(): QObject(),
    _settings(this)
{}

QVariant ConfManager::getValue(Section section, QString key, QVariant defaultValue) const
{
    return _settings.value(getFullKey(section, key), defaultValue);
}

QVariant ConfManager::getToolValue(ToolType toolType, QString toolName, QString key, QVariant defaultValue) const
{
    return _settings.value(getFullKey(toolType, toolName, key), defaultValue);
}

void ConfManager::setValue(Section section, QString key, QVariant value)
{
    _settings.setValue(getFullKey(section, key), value);

    // Possibly update elements
    if (section == Section::SECTION_SOUND_ENGINE)
        emit(soundEngineConfigurationChanged());
    else if (section == Section::SECTION_AUDIO)
    {
        emit(soundEngineConfigurationChanged()); // First prepare the sound engine (the buffer can be adjusted)
        emit(audioServerConfigurationChanged()); // Then update the audio server configuration
    }
    else if (section == Section::SECTION_MAP)
        emit(keyMapChanged());
    else if (section == Section::SECTION_NONE && key == "name_middle_c")
        emit(interfaceChanged());
}

void ConfManager::setToolValue(ToolType toolType, QString toolName, QString key, QVariant value)
{
    _settings.setValue(getFullKey(toolType, toolName, key), value);
}

QString ConfManager::getFullKey(Section section, QString key) const
{
    // First part depends on the section
    QString firstPart = "";
    switch (section)
    {
    case SECTION_NONE:         firstPart = "";            break;
    case SECTION_DISPLAY:      firstPart = "affichage";   break;
    case SECTION_AUDIO:        firstPart = "audio";       break;
    case SECTION_BULK_RENAME:  firstPart = "bulk_rename"; break;
    case SECTION_COLORS:       firstPart = "colors";      break;
    case SECTION_EXPORT:       firstPart = "export";      break;
    case SECTION_KEYBOARD:     firstPart = "keyboard";    break;
    case SECTION_MAP:          firstPart = "map";         break;
    case SECTION_MIDI:         firstPart = "midi";        break;
    case SECTION_RECENT_FILES: firstPart = "recent_file"; break;
    case SECTION_SOUND_ENGINE: firstPart = "synth";       break;
    case SECTION_TOOLS:        firstPart = "tools";       break;
    case SECTION_WARNINGS:     firstPart = "warnings";    break;
    }
    if (firstPart != "")
        firstPart += "/";

    return firstPart + key;
}

QString ConfManager::getFullKey(ToolType toolType, QString toolName, QString key) const
{
    QString subSection = "";
    switch (toolType)
    {
    case TOOL_TYPE_SF2:        subSection = "sf2";        break;
    case TOOL_TYPE_SAMPLE:     subSection = "sample";     break;
    case TOOL_TYPE_INSTRUMENT: subSection = "instrument"; break;
    case TOOL_TYPE_PRESET:     subSection = "preset";     break;
    }

    return getFullKey(SECTION_TOOLS, subSection + "/" + toolName + "_" + key);
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
        case KEY_C_LEFT:  txt = trUtf8("w"); break;
        case KEY_C_SHARP: txt = trUtf8("s"); break;
        case KEY_D:       txt = trUtf8("x"); break;
        case KEY_D_SHARP: txt = trUtf8("d"); break;
        case KEY_E:       txt = trUtf8("c"); break;
        case KEY_F:       txt = trUtf8("v"); break;
        case KEY_F_SHARP: txt = trUtf8("g"); break;
        case KEY_G:       txt = trUtf8("b"); break;
        case KEY_G_SHARP: txt = trUtf8("h"); break;
        case KEY_A:       txt = trUtf8("n"); break;
        case KEY_A_SHARP: txt = trUtf8("j"); break;
        case KEY_B:       txt = trUtf8(","); break;
        case KEY_C_RIGHT: txt = trUtf8(";"); break;
        }
        break;
    case 1:
        switch (key)
        {
        case KEY_C_LEFT:  txt = trUtf8("a"); break;
        case KEY_C_SHARP: txt = trUtf8("é"); break;
        case KEY_D:       txt = trUtf8("z"); break;
        case KEY_D_SHARP: txt = trUtf8("\""); break;
        case KEY_E:       txt = trUtf8("e"); break;
        case KEY_F:       txt = trUtf8("r"); break;
        case KEY_F_SHARP: txt = trUtf8("("); break;
        case KEY_G:       txt = trUtf8("t"); break;
        case KEY_G_SHARP: txt = trUtf8("-"); break;
        case KEY_A:       txt = trUtf8("y"); break;
        case KEY_A_SHARP: txt = trUtf8("è"); break;
        case KEY_B:       txt = trUtf8("u"); break;
        case KEY_C_RIGHT: txt = trUtf8("i"); break;
        }
        break;
    case 2:
        switch (key)
        {
        case KEY_C_LEFT:  txt = trUtf8("Shift+w"); break;
        case KEY_C_SHARP: txt = trUtf8("Shift+s"); break;
        case KEY_D:       txt = trUtf8("Shift+x"); break;
        case KEY_D_SHARP: txt = trUtf8("Shift+d"); break;
        case KEY_E:       txt = trUtf8("Shift+c"); break;
        case KEY_F:       txt = trUtf8("Shift+v"); break;
        case KEY_F_SHARP: txt = trUtf8("Shift+g"); break;
        case KEY_G:       txt = trUtf8("Shift+b"); break;
        case KEY_G_SHARP: txt = trUtf8("Shift+h"); break;
        case KEY_A:       txt = trUtf8("Shift+n"); break;
        case KEY_A_SHARP: txt = trUtf8("Shift+j"); break;
        case KEY_B:       txt = trUtf8("Shift+?"); break;
        case KEY_C_RIGHT: txt = trUtf8("Shift+."); break;
        }
        break;
    case 3:
        switch (key)
        {
        case KEY_C_LEFT:  txt = trUtf8("Shift+a"); break;
        case KEY_C_SHARP: txt = trUtf8("Shift+2"); break;
        case KEY_D:       txt = trUtf8("Shift+z"); break;
        case KEY_D_SHARP: txt = trUtf8("Shift+3"); break;
        case KEY_E:       txt = trUtf8("Shift+e"); break;
        case KEY_F:       txt = trUtf8("Shift+r"); break;
        case KEY_F_SHARP: txt = trUtf8("Shift+5"); break;
        case KEY_G:       txt = trUtf8("Shift+t"); break;
        case KEY_G_SHARP: txt = trUtf8("Shift+6"); break;
        case KEY_A:       txt = trUtf8("Shift+y"); break;
        case KEY_A_SHARP: txt = trUtf8("Shift+7"); break;
        case KEY_B:       txt = trUtf8("Shift+u"); break;
        case KEY_C_RIGHT: txt = trUtf8("Shift+i"); break;
        }
        break;
    default:
        break;
    }

    // The default value is possibly overriden
    return this->getValue(SECTION_MAP, "key_" + QString::number(numOctave) + "_" + QString::number((int)key), txt).toString();
}

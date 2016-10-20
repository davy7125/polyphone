/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2016 Davy Triponney                                **
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
#include "pianokeybdcustom.h"
#include "audiodevice.h"
#include "mainwindow.h"

ConfManager * ConfManager::_instance = NULL;
PianoKeybdCustom * ConfManager::_keyboard = NULL;
AudioDevice * ConfManager::_audioDevice = NULL;
MainWindow * ConfManager::_mainWindow = NULL;

ConfManager::ConfManager(): QObject(_mainWindow),
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

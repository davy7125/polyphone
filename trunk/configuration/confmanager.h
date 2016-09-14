/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
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
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef CONFMANAGER_H
#define CONFMANAGER_H

#include <QSettings>
#include <QString>

class PianoKeybdCustom;
class AudioDevice;
class MainWindow;

class ConfManager: QObject
{
public:
    enum Section
    {
        SECTION_NONE,
        SECTION_DISPLAY,
        SECTION_AUDIO,
        SECTION_BULK_RENAME,
        SECTION_COLORS,
        SECTION_EXPORT,
        SECTION_KEYBOARD,
        SECTION_MAP,
        SECTION_MIDI,
        SECTION_RECENT_FILES,
        SECTION_SOUND_ENGINE,
        SECTION_TOOLS,
        SECTION_WARNINGS
    };

    enum ToolType
    {
        TOOL_TYPE_SF2,
        TOOL_TYPE_SAMPLE,
        TOOL_TYPE_INSTRUMENT,
        TOOL_TYPE_PRESET
    };

    /// Initialization of pointers
    static void init(PianoKeybdCustom * keyboard, AudioDevice *audioDevice, MainWindow * mainwindow)
    {
        _keyboard = keyboard;
        _audioDevice = audioDevice;
        _mainWindow = mainwindow;
    }

    /// Get an instance
    static ConfManager * getInstance()
    {
        if (_instance == NULL)
            _instance = new ConfManager();
        return _instance;
    }

    /// Destroy the singleton
    void kill()
    {
        delete _instance;
        _instance = NULL;
    }

    /// Get a value from a section
    QVariant getValue(Section section, QString key, QVariant defaultValue) const;

    /// Get a value from a tool
    QVariant getToolValue(ToolType toolType, QString toolName, QString key, QVariant defaultValue) const;

    /// Set a value in a section
    void setValue(Section section, QString key, QVariant value);

    /// Set a value in a tool
    void setToolValue(ToolType toolType, QString toolName, QString key, QVariant value);

private:
    ConfManager();
    QString getFullKey(Section section, QString key) const;
    QString getFullKey(ToolType toolType, QString toolName, QString key) const;

    static ConfManager * _instance;
    static PianoKeybdCustom * _keyboard;
    static AudioDevice * _audioDevice;
    static MainWindow * _mainWindow;

    QSettings _settings;
};

#endif // CONFMANAGER_H

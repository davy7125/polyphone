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

#ifndef CONFMANAGER_H
#define CONFMANAGER_H

#include <QSettings>
#include <QString>

class ConfManager: public QObject
{
    Q_OBJECT

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

    ConfManager();

    /// Get a value from a section
    QVariant getValue(Section section, QString key, QVariant defaultValue) const;

    /// Get a value from a tool
    QVariant getToolValue(ToolType toolType, QString toolName, QString key, QVariant defaultValue) const;

    /// Set a value in a section
    void setValue(Section section, QString key, QVariant value);

    /// Set a value in a tool
    void setToolValue(ToolType toolType, QString toolName, QString key, QVariant value);

    /// Get the configuration file directory
    QString getConfigDir();

signals:
    /// Emitted when the chorus or reverb configuration changed or the buffer size
    void soundEngineConfigurationChanged();

    /// Emitted when the audio server or the buffer size changed
    void audioServerConfigurationChanged();

    /// Emitted when the note names changed
    void interfaceChanged();

private:
    QString getFullKey(Section section, QString key) const;
    QString getFullKey(ToolType toolType, QString toolName, QString key) const;
    QSettings _settings;
};

#endif // CONFMANAGER_H

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

#ifndef CONFMANAGER_H
#define CONFMANAGER_H

#include "basetypes.h"
#include <QSettings>
#include <QString>
class SynthConfig;

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
        SECTION_KEYBOARD,
        SECTION_MIDI,
        SECTION_RECENT_FILES,
        SECTION_SOUND_ENGINE,
        SECTION_TOOLS,
        SECTION_WARNINGS,
        SECTION_REPOSITORY,
        SECTION_EXTENSIONS
    };

    enum Key
    {
        KEY_C_LEFT,
        KEY_C_SHARP,
        KEY_D,
        KEY_D_SHARP,
        KEY_E,
        KEY_F,
        KEY_F_SHARP,
        KEY_G,
        KEY_G_SHARP,
        KEY_A,
        KEY_A_SHARP,
        KEY_B,
        KEY_C_RIGHT
    };

    ConfManager();
    ~ConfManager() override;

    /// Get / set a value for a section with a possible subsection
    QVariant getValue(Section section, QString key, QVariant defaultValue) const;
    void setValue(Section section, QString key, QVariant value);
    QVariant getValue(Section section, QString subSection, QString key, QVariant defaultValue) const;
    void setValue(Section section, QString subSection, QString key, QVariant value);

    /// Get the configuration file directory
    QString getConfigDir();

    /// Get the keyboard combination associated to a specific note
    QString getMapping(int numOctave, Key key);

    /// Gather all configurations for the synth
    SynthConfig * getSynthConfig();

signals:
    /// Emitted when the chorus or reverb configuration changed or the buffer size
    void soundEngineConfigurationChanged();

    /// Emitted when the audio server or the buffer size changed
    void audioServerConfigurationChanged();

    /// Emitted when the note names changed
    void interfaceChanged();

    /// Emitted when the mapping changed
    void keyMapChanged();

    /// Emitted when the division sort type changed
    void divisionSortChanged();

    /// Emitted when the key mapping octave changed (Ctrl+1, Ctrl+2, ...)
    void keyboardOctaveChanged();

private:
    /// Clear everything
    void clear();

    QString getFullKey(Section section, QString subSection, QString key) const;
    QSettings _settings;
    SynthConfig * _synthConfig;
};

#endif // CONFMANAGER_H

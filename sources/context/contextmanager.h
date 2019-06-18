/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef CONTEXTMANAGER_H
#define CONTEXTMANAGER_H

#include "confmanager.h"
#include "keynamemanager.h"
#include "recentfilemanager.h"
#include "thememanager.h"
#include "translationmanager.h"
#include "audiodevice.h"
#include "mididevice.h"

class ContextManager: public QObject
{
    Q_OBJECT

public:
    // Access to the configuration (key / value)
    static ConfManager * configuration();

    // Way to name the keys
    static KeyNameManager * keyName();

    // Get / set the recent files and folders
    static RecentFileManager * recentFile();

    // Everything related to the theme
    static ThemeManager * theme();

    // For translating the application
    static TranslationManager * translation();

    // Link to the system for playing sounds
    static AudioDevice * audio();

    // Link to the system for receiving MIDI signals
    static MidiDevice * midi();

    // FINALLY, kill everything that has been built
    static void kill();

private:
    ContextManager();
    ~ContextManager();
    static ContextManager * s_instance;

    ConfManager * _configuration;
    KeyNameManager * _keyName;
    RecentFileManager * _recentFile;
    ThemeManager * _theme;
    TranslationManager * _translation;
    AudioDevice * _audio;
    MidiDevice * _midi;
};

#endif // CONTEXTMANAGER_H

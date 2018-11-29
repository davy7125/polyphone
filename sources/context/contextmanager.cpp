/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2018 Davy Triponney                                **
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

#include "contextmanager.h"
#include "synth.h"

ContextManager * ContextManager::s_instance = NULL;

ConfManager * ContextManager::configuration()
{
    if (s_instance == NULL)
        s_instance = new ContextManager();
    return s_instance->_configuration;
}

KeyNameManager * ContextManager::keyName()
{
    if (s_instance == NULL)
        s_instance = new ContextManager();
    return s_instance->_keyName;
}

RecentFileManager * ContextManager::recentFile()
{
    if (s_instance == NULL)
        s_instance = new ContextManager();
    return s_instance->_recentFile;
}

ThemeManager * ContextManager::theme()
{
    if (s_instance == NULL)
        s_instance = new ContextManager();
    return s_instance->_theme;
}

TranslationManager * ContextManager::translation()
{
    if (s_instance == NULL)
        s_instance = new ContextManager();
    return s_instance->_translation;
}

AudioDevice * ContextManager::audio()
{
    if (s_instance == NULL)
        s_instance = new ContextManager();
    return s_instance->_audio;
}

MidiDevice * ContextManager::midi()
{
    if (s_instance == NULL)
        s_instance = new ContextManager();
    return s_instance->_midi;
}

void ContextManager::kill()
{
    delete s_instance;
    s_instance = NULL;
}

ContextManager::ContextManager() :
    _configuration(NULL),
    _keyName(NULL),
    _recentFile(NULL),
    _theme(NULL),
    _translation(NULL),
    _audio(NULL),
    _midi(NULL)
{
    // Build the context

    // 1. Access to the configuration
    _configuration = new ConfManager();

    // 2. Themes
    _theme = new ThemeManager(_configuration);

    // 3. Recent files
    _recentFile = new RecentFileManager(_configuration);

    // 4. Key names
    _keyName = new KeyNameManager(_configuration);

    // 5. Translations
    _translation = new TranslationManager(_configuration);

    // 6. Audio device
    _audio = new AudioDevice(_configuration);
    connect(_configuration, SIGNAL(audioServerConfigurationChanged()), _audio, SLOT(initAudio()));
    connect(_configuration, SIGNAL(soundEngineConfigurationChanged()), _audio->getSynth(), SLOT(updateConfiguration()));

    // 7. Midi device
    _midi = new MidiDevice(_configuration, _audio->getSynth());
}

ContextManager::~ContextManager()
{

    delete _configuration;
}

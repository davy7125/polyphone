#include "contextmanager.h"

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

    // 7. Midi device
    _midi = new MidiDevice(_configuration);
}

ContextManager::~ContextManager()
{

    delete _configuration;
}

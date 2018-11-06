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

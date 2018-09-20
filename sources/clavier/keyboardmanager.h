#ifndef KEYBOARDMANAGER_H
#define KEYBOARDMANAGER_H

#include "pianokeybdcustom.h"

class KeyboardManager
{
public:
    /// Configure the keyboard
    static void set(PianoKeybd::KeyboardProperty keyboardProperty, QVariant value);
    static void setMapping(PianoKeybd::Key key, int numOctave, QKeySequence sequence);

    /// Get the keyboard widget
    static PianoKeybdCustom * getWidget();

private:
    static PianoKeybdCustom * _keyboard;
};

#endif // KEYBOARDMANAGER_H

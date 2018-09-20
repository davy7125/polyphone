#include "keyboardmanager.h"

PianoKeybdCustom * KeyboardManager::_keyboard = NULL;

PianoKeybdCustom * KeyboardManager::getWidget()
{
    if (_keyboard == NULL)
        _keyboard = new PianoKeybdCustom(NULL);
    return _keyboard;
}

void KeyboardManager::set(PianoKeybd::KeyboardProperty keyboardProperty, QVariant value)
{
    getWidget()->set(keyboardProperty, value);
}

void KeyboardManager::setMapping(PianoKeybd::Key key, int numOctave, QKeySequence sequence)
{
    getWidget()->setMapping(key, numOctave, sequence);
}

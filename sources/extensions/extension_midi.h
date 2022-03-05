/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2020 Davy Triponney                                **
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

#ifndef EXTENSION_MIDI_H
#define EXTENSION_MIDI_H

#include "extension_midi_dialog.h"

class ExtensionMidi: public QObject
{
    Q_OBJECT

public:
    ExtensionMidi() : QObject() {}
    virtual ~ExtensionMidi()
    {
        delete _dialog;
    }

    /// Title of the extension
    virtual QString getTitle() const = 0;

    /// Internal identifier
    virtual QString getIdentifier() const = 0;

    /// Path of the icon
    virtual QString getIconPath() const = 0;

    QDialog * getDialog()
    {
        if (_dialog == nullptr)
            _dialog = new ExtensionMidiDialog(this->getTitle(), this->getIdentifier(), this->getGui());
        return _dialog;
    }

    virtual bool processKeyOn(int channel, int key, int vel) = 0;
    virtual bool processKeyOff(int channel, int key) = 0;
    virtual bool processPolyPressureChanged(int channel, int key, int pressure) = 0;
    virtual bool processMonoPressureChanged(int channel, int value) = 0;
    virtual bool processControllerChanged(int channel, int num, int value) = 0;
    virtual bool processBendChanged(int channel, float value) = 0;
    virtual bool processBendSensitivityChanged(int channel, float semitones) = 0;

protected:
    virtual QWidget * getGui() = 0;

private:
    QDialog * _dialog = nullptr;
};

#endif // EXTENSION_MIDI_H

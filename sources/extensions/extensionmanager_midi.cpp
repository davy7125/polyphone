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

#include "extensionmanager_midi.h"
#include "extension_midi.h"

ExtensionManagerMidi::ExtensionManagerMidi()
{

}

ExtensionManagerMidi::~ExtensionManagerMidi()
{
    while (!_extensions.isEmpty())
        delete _extensions.takeFirst();
}

QString ExtensionManagerMidi::getTitle(int index)
{
    return _extensions[index]->getTitle();
}

QString ExtensionManagerMidi::getIconPath(int index)
{
    return _extensions[index]->getIconPath();
}

QDialog * ExtensionManagerMidi::getDialog(int index)
{
    return _extensions[index]->getDialog();
}

bool ExtensionManagerMidi::processKeyOn(int channel, int key, int vel)
{
    foreach (ExtensionMidi * ext, _extensions)
        if (ext->processKeyOn(channel, key, vel))
            return true;
    return false;
}

bool ExtensionManagerMidi::processKeyOff(int channel, int key)
{
    foreach (ExtensionMidi * ext, _extensions)
        if (ext->processKeyOff(channel, key))
            return true;
    return false;
}

bool ExtensionManagerMidi::processPolyPressureChanged(int channel, int key, int pressure)
{
    foreach (ExtensionMidi * ext, _extensions)
        if (ext->processPolyPressureChanged(channel, key, pressure))
            return true;
    return false;
}

bool ExtensionManagerMidi::processMonoPressureChanged(int channel, int value)
{
    foreach (ExtensionMidi * ext, _extensions)
        if (ext->processMonoPressureChanged(channel, value))
            return true;
    return false;
}

bool ExtensionManagerMidi::processControllerChanged(int channel, int num, int value)
{
    foreach (ExtensionMidi * ext, _extensions)
        if (ext->processControllerChanged(channel, num, value))
            return true;
    return false;
}

bool ExtensionManagerMidi::processBendChanged(int channel, float value)
{
    foreach (ExtensionMidi * ext, _extensions)
        if (ext->processBendChanged(channel, value))
            return true;
    return false;
}

bool ExtensionManagerMidi::processBendSensitivityChanged(int channel, float semitones)
{
    foreach (ExtensionMidi * ext, _extensions)
        if (ext->processBendSensitivityChanged(channel, semitones))
            return true;
    return false;
}

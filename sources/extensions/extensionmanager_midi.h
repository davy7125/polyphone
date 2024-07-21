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

#ifndef EXTENSIONMANAGER_MIDI_H
#define EXTENSIONMANAGER_MIDI_H

#include <QString>
#include <QList>
class QDialog;
class QWidget;
class ExtensionMidi;

class ExtensionManagerMidi
{
public:
    ExtensionManagerMidi();
    ~ExtensionManagerMidi();
    
    void store(ExtensionMidi * extension) { _extensions << extension; }
    int count() { return _extensions.count(); }
    QString getTitle(int index);
    QString getIconPath(int index);
    QDialog * getDialog(int index);

    bool processKeyOn(int channel, int key, int vel);
    bool processKeyOff(int channel, int key);
    bool processPolyPressureChanged(int channel, int key, int pressure);
    bool processMonoPressureChanged(int channel, int value);
    bool processControllerChanged(int channel, int num, int value);
    bool processBendChanged(int channel, float value);
    bool processBendSensitivityChanged(int channel, float semitones);

private:
    QList<ExtensionMidi *> _extensions;
};

#endif // EXTENSIONMANAGER_MIDI_H

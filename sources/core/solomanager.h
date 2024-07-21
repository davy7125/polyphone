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

#ifndef SOLOMANAGER_H
#define SOLOMANAGER_H

#include <QObject>
#include "basetypes.h"
class SoundfontManager;

class SoloManager : public QObject
{
    Q_OBJECT
public:
    SoloManager(SoundfontManager * sm);

    // Mute a single element
    void setMute(EltID id, bool isMute);
    bool isMute(EltID id);

    // Solo on a selection
    void activateSolo(IdList ids);

    // Unmute all
    void unmuteAll(int sf2Index);

    // Automatic solo on the current selection
    bool isSoloOnSelectionEnabled(int sf2Index);
    void setSoloOnSelection(bool enable, int sf2Index);

    // Must be called every time the selection changes
    void selectionChanged(IdList ids);

private:
    SoundfontManager * _sm;
    QList<int> _soloOnSelectionEnabled;
    QMap<int, IdList> _currentSelectionPerSoundfont;
};

#endif // SOLOMANAGER_H

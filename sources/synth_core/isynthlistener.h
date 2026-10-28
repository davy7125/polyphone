/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2026 Davy Triponney                                **
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

#ifndef ISYNTHLISTENER_H
#define ISYNTHLISTENER_H

#include <cstdint>

// Notifications of the voices playing a sample alone (key -1), for the editor
// Called by the thread computing the voices
class ISynthListener
{
public:
    virtual ~ISynthListener() {}

    /// Current position in the sample, 0 when it is finished
    virtual void onSamplePositionChanged(uint32_t position) = 0;

    /// The reading identified by token is finished
    virtual void onSampleReadFinished(int token) = 0;
};

#endif // ISYNTHLISTENER_H

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

#ifndef SAMPLETRIMMING_H
#define SAMPLETRIMMING_H

#include "basetypes.h"
class SoundfontManager;

// Samples of a soundfont cropped (tools "Trim to end of loop" and "Remove blank at start", samples loaded)
class SampleTrimming
{
public:
    /// Remove the data after the end of the loop of a sample, except the 8 values used for the interpolation at the
    /// loop point
    static void trimToLoop(SoundfontManager * sm, EltID idSmpl);

    /// Remove the blank at the start of samples (the channels of a stereo sample being given together): the position
    /// where the sound starts is the earliest of all samples, so that the sound of no channel is cut
    static void trimStart(SoundfontManager * sm, IdList idsSmpl);
};

#endif // SAMPLETRIMMING_H

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

#ifndef FASTMATHS_H
#define FASTMATHS_H

#include <QtGlobal>

class FastMaths
{
public:
    static void initialize();

    static float fastSin(float value); // Range [0; 1] for [0; pi]
    static float fastCos(float value); // Range [0; 1] for [0; pi]
    static float fastPow10(float value); // Range [-102.4; 102.3]
    static float fastPow2(float p)
    {
        float offset = (p < 0) ? 1.0f : 0.0f;
        float clipp = (p < -126) ? -126.0f : p;
        int w = static_cast<int>(clipp);
        float z = clipp - static_cast<float>(w) + offset;
        union { quint32 i; float f; } v =
            { static_cast<quint32> ( (1 << 23) * (clipp + 121.2740575f + 27.7280233f / (4.84252568f - z) - 1.49012907f * z) ) };
        return v.f;
    }

private:
    static float getSinValue(float value); // Range [0; 0.5] for [0; pi / 2]
    static float s_sin_table[2048];
    static float s_pow10_table[2048];
};

#endif // FASTMATHS_H

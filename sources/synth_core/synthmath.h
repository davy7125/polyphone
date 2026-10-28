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

#ifndef SYNTHMATH_H
#define SYNTHMATH_H

#include <cstdint>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_LN2
#define M_LN2 0.69314718055994530942
#endif

// Helpers giving exactly the results of the Qt functions used before by the synth
// (qRound, qMin, qMax, qAbs, also for the sign of zero), so that the sound does not change
class SynthMath
{
public:
    // Like qRound: the halfway cases are rounded away from zero
    static int round(double value) { return static_cast<int>(value >= 0.0 ? value + 0.5 : value - 0.5); }

    // Rounding to a fixed size integer
    static int8_t round8(double value) { return static_cast<int8_t>(value > 0 ? (value + 0.5) : (value - 0.5)); }
    static int16_t round16(double value) { return static_cast<int16_t>(value > 0 ? (value + 0.5) : (value - 0.5)); }
    static int32_t round32(double value) { return static_cast<int32_t>(value > 0 ? (value + 0.5) : (value - 0.5)); }

    template <typename T> static const T &min(const T &a, const T &b) { return (a < b) ? a : b; }
    template <typename T> static const T &max(const T &a, const T &b) { return (a < b) ? b : a; }
    template <typename T> static T abs(const T &t) { return t >= 0 ? t : -t; }

    // Sinc table windowed by a Kaiser-Bessel window, for the resampling
    static void fillSincTable(float * table, int order, int subdivisions, double kaiserBesserBeta);

    // Modified Bessel function of the first kind (order 0)
    static double besselI0(double x);
};

#endif // SYNTHMATH_H

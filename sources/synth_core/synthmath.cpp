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

#include "synthmath.h"

void SynthMath::fillSincTable(float * table, int order, int subdivisions, double kaiserBesserBeta)
{
    double v, posI, posIshifted;
    double multiplier = 1.0 / besselI0(kaiserBesserBeta);
    double twoInvOrder = 2.0 / (double)order;
    double dTmp;
    double invSubDivisions = 1.0 / (double)subdivisions;

    // i2: Offset in terms of fractional samples ('subsamples')
    for (int i2 = 0; i2 < subdivisions; ++i2)
    {
        // i: Offset in terms of whole samples
        for (int i = 0; i < order; ++i)
        {
            posI = (double)i + (double)(subdivisions - i2 - 1) * invSubDivisions;
            posIshifted = posI - (double)order / 2.0;

            // sinc(0) cannot be calculated straightforward (limit needed for 0/0)
            if (fabs(posIshifted) > 0.000001)
            {
                v = sin(posIshifted * M_PI) / (posIshifted * M_PI);

                // Kaiser-bessel window
                dTmp = twoInvOrder * posI - 1.;
                v *= besselI0(kaiserBesserBeta * sqrt(1. - dTmp * dTmp)) * multiplier;
            }
            else
                v = 1.0;

            table[i2 * order + i] = (float)v;
        }
    }
}

double SynthMath::besselI0(double x)
{
    // Computation based on Abramowitz & Stegun work
    double ax = std::fabs(x);
    double y;
    double result;

    if (ax < 3.75)
    {
        // Approximation for |x| < 3.75
        y = (x / 3.75);
        y *= y;
        result = 1.0 + y * (3.5156229 +
                            y * (3.0899424 +
                                 y * (1.2067492 +
                                      y * (0.2659732 +
                                           y * (0.0360768 +
                                                y * (0.0045813))))));
    }
    else
    {
        // Approximation for |x| >= 3.75
        y = 3.75 / ax;
        result = (std::exp(ax) / std::sqrt(ax)) *
                 (0.39894228 +
                  y * (0.01328592 +
                       y * (0.00225319 +
                            y * (-0.00157565 +
                                 y * (0.00916281 +
                                      y * (-0.02057706 +
                                           y * (0.02635537 +
                                                y * (-0.01647633 +
                                                     y * (0.00392377)))))))));
    }

    return result;
}

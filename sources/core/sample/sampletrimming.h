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
#include <QMap>
#include <limits>
class SoundfontManager;

// Samples of a soundfont cropped (tool "Trim", samples loaded)
// The channels of a stereo sample are given together so that they keep the same length
class SampleTrimming
{
public:
    /// Threshold for an automatic detection of the background noise
    static constexpr double AUTOMATIC_THRESHOLD = std::numeric_limits<double>::quiet_NaN();

    /// Remove the blank at the start of samples: the position where the sound starts is the earliest of all channels,
    /// so that the sound of no channel is cut. Threshold in dBFS or AUTOMATIC_THRESHOLD. Return the number of values
    /// removed
    static quint32 trimStart(SoundfontManager * sm, IdList idsSmpl, double thresholdDb = AUTOMATIC_THRESHOLD);

    /// Remove the blank at the end of samples, replaced by a fade-out prolonging the decay of the sound (never before the
    /// end of a loop and the 8 values after it). Threshold in dBFS or AUTOMATIC_THRESHOLD. Return the number of values
    /// removed in each sample
    static QList<quint32> trimEnd(SoundfontManager * sm, IdList idsSmpl, double thresholdDb = AUTOMATIC_THRESHOLD);

    /// True if data can be removed after the end of the loop of a sample
    static bool hasDataAfterLoop(SoundfontManager * sm, EltID idSmpl);

    /// Remove the data after the end of the loop of a sample, except the 8 values used for the interpolation at the
    /// loop point. Return the number of values removed
    static quint32 trimToLoop(SoundfontManager * sm, EltID idSmpl);

    /// Divisions of the instruments using each sample of a soundfont (key: index of the sample)
    static QMap<int, QList<EltID> > getDivisionsBySample(SoundfontManager * sm, int indexSf2);

    /// Update the offsets of divisions after values have been removed at the start or at the end of their sample, so
    /// that they still target the same sound
    static void updateStartOffsets(SoundfontManager * sm, const QList<EltID> &divisions, quint32 removed);
    static void updateEndOffsets(SoundfontManager * sm, const QList<EltID> &divisions, quint32 removed);

private:
    static void updateOffset(SoundfontManager * sm, EltID idDiv, AttributeType champFine, AttributeType champCoarse,
                             qint64 removed, bool start);
    static AttributeValue getInheritedValue(SoundfontManager * sm, EltID idDiv, AttributeType champ);
};

#endif // SAMPLETRIMMING_H

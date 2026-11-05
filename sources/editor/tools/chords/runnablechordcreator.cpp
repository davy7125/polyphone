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

#include "runnablechordcreator.h"
#include "toolchords.h"

RunnableChordCreator::RunnableChordCreator(ToolChords * tool, EltID idInst, ChordInfo ci, quint32 key, quint32 minKey, bool loop, bool stereo, int side) :
    AbstractRunnableMixer(idInst, loop, stereo, side),
    _tool(tool),
    _ci(ci),
    _key(key),
    _minKey(minKey)
{}

void RunnableChordCreator::run()
{
    // Get the different pitches with the corresponding attenuation
    QMap<int, int> pitches = getChordKeys(_key, _ci);

    // Minimum attenuation for all parts
    QList<double> keys;
    foreach (int pitch, pitches.keys())
        keys << pitch;
    double attMini = getMinimumAttenuation(keys);

    // Mix of the parts
    QVector<float> vData(SAMPLE_DURATION * SAMPLE_RATE, 0.0f);
    foreach (int pitch, pitches.keys())
        if (pitch <= 120)
            addKey(vData, pitch, attMini, 0.1 * pitches[pitch], pitches.count());

    EltID idSmpl = createSample(vData, static_cast<int>(_key));
    emit _tool->elementProcessed(idSmpl, _key, _minKey, attMini);
}

// List of all possible keys with their corresponding attenuation
QMap<int, int> RunnableChordCreator::getChordKeys(quint32 key, ChordInfo& chordInfo)
{
    QMap<int, int> chordKeys;
    int shift = 0;

    // Root key (not impacted by the octave or inversion)
    if (chordInfo.chordType1 == 1)
        chordKeys[static_cast<int>(key)] = chordInfo.chordType1Attenuation;

    // Third
    switch (chordInfo.chordType3)
    {
    case 1: shift = 4; break; // Major
    case 2: shift = 3; break; // Minor
    default: shift = 0; break;
    }
    if (shift != 0)
        chordKeys[key + shift + 12 * (chordInfo.octave - (chordInfo.chordType3Inversion ? 1 : 0))] = chordInfo.chordType3Attenuation;

    // Fifth
    switch (chordInfo.chordType5)
    {
    case 1: shift = 7; break; // Perfect
    case 2: shift = 6; break; // Diminished
    case 3: shift = 8; break; // Augmented
    default: shift = 0; break;
    }
    if (shift != 0)
        chordKeys[key + shift + 12 * (chordInfo.octave - (chordInfo.chordType5Inversion ? 1 : 0))] = chordInfo.chordType5Attenuation;

    // Seventh
    switch (chordInfo.chordType7)
    {
    case 1: shift = 10; break; // Minor
    case 2: shift = 11; break; // Major
    case 3: shift = 9; break; // Diminished
    default: shift = 0; break;
    }
    if (shift != 0)
        chordKeys[key + shift + 12 * (chordInfo.octave - (chordInfo.chordType7Inversion ? 1 : 0))] = chordInfo.chordType7Attenuation;

    // Ninth
    switch (chordInfo.chordType9)
    {
    case 1: shift = 14; break; // Major
    case 2: shift = 13; break; // Minor
    default: shift = 0; break;
    }
    if (shift != 0)
        chordKeys[key + shift + 12 * (chordInfo.octave - (chordInfo.chordType9Inversion ? 1 : 0))] = chordInfo.chordType9Attenuation;

    return chordKeys;
}

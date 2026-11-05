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

#ifndef ABSTRACTRUNNABLEMIXER_H
#define ABSTRACTRUNNABLEMIXER_H

#include <QRunnable>
#include <QVector>
#include "basetypes.h"

// Base of the tasks creating a sample by mixing an instrument played at several keys (chords, mixtures)
class AbstractRunnableMixer: public QRunnable
{
public:
    AbstractRunnableMixer(EltID idInst, bool loop, bool stereo, int side);

protected:
    // Lowest attenuation (dB) of the divisions playing the keys
    double getMinimumAttenuation(const QList<double> &keys);

    // Add to data the instrument played at a key: the attenuation of the division relative to minAttenuation and an
    // additional attenuation (dB) are applied if the division has an attenuation, then the result is divided by divisor
    void addKey(QVector<float> &data, double key, double minAttenuation, double extraAttenuation, double divisor);

    // Loop the data if required and create the sample, return its id
    EltID createSample(QVector<float> data, int rootKey);

    static const double SAMPLE_DURATION; // In seconds
    static const quint32 SAMPLE_RATE; // In samples per second

    EltID _idInst;

private:
    EltID closestSample(EltID idInst, double pitch, double &ecart, int cote, EltID &idInstSmpl);

    bool _loop;
    bool _stereo;
    int _side;
};

#endif // ABSTRACTRUNNABLEMIXER_H

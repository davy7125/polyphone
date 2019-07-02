/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
**                2014      Andrea Celani                                 **
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
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef LIVEEQ_H
#define LIVEEQ_H

#include <QVector>
#include <QMutex>
#include "lib/Iir_2.h"

class LiveEQ
{
public:
    LiveEQ();

    // Initialize the sample rate
    void setSampleRate(quint32 sampleRate);

    // Configuration of the equalizer
    void on();
    void off();
    void setValues(QVector<int> values);

    // Filter data
    void filterData(float * dataR, float * dataL, quint32 len);

private:
    quint32 _sampleRate;
    bool _isOn;
    QVector<Iir::Butterworth::BandPass<4> > _passBandsR, _passBandsL;
    QVector<double> _coeff;
    QMutex _mutex;
    int _crossFade;
    static int CROSSFADE_LENGTH;
    static double CORRECTION;
};

#endif // LIVEEQ_H

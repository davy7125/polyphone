/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "liveeq.h"
#include <qmath.h>

int LiveEQ::CROSSFADE_LENGTH = 1000;
float LiveEQ::CORRECTION = 0.85f;

LiveEQ::LiveEQ() :
    _isOn(false),
    _crossFade(0)
{

}

void LiveEQ::setSampleRate(unsigned int sampleRate)
{
    _sampleRate = sampleRate;

    // Prepare 10 pass bands per channel
    _passBandsR[0].setup(_sampleRate, 32, 2*12);
    _passBandsR[1].setup(_sampleRate, 64, 2*20);
    _passBandsR[2].setup(_sampleRate, 125, 2*41);
    _passBandsR[3].setup(_sampleRate, 250, 2*84);
    _passBandsR[4].setup(_sampleRate, 500, 2*166);
    _passBandsR[5].setup(_sampleRate, 1000, 2*334);
    _passBandsR[6].setup(_sampleRate, 2000, 2*666);
    _passBandsR[7].setup(_sampleRate, 4000, 2*1334);
    _passBandsR[8].setup(_sampleRate, 8000, 2*2666);
    _passBandsR[9].setup(_sampleRate, 16000, 2*5334);

    _passBandsL[0].setup(_sampleRate, 32, 2*12);
    _passBandsL[1].setup(_sampleRate, 64, 2*20);
    _passBandsL[2].setup(_sampleRate, 125, 2*41);
    _passBandsL[3].setup(_sampleRate, 250, 2*84);
    _passBandsL[4].setup(_sampleRate, 500, 2*166);
    _passBandsL[5].setup(_sampleRate, 1000, 2*334);
    _passBandsL[6].setup(_sampleRate, 2000, 2*666);
    _passBandsL[7].setup(_sampleRate, 4000, 2*1334);
    _passBandsL[8].setup(_sampleRate, 8000, 2*2666);
    _passBandsL[9].setup(_sampleRate, 16000, 2*5334);
}

void LiveEQ::on()
{
    _isOn = true;
}

void LiveEQ::off()
{
    _isOn = false;
}

void LiveEQ::setValues(int values[10])
{
    for (int i = 0; i < 10; i++)
        _coeff[i] = static_cast<float>(qPow(10.0, 0.1 * static_cast<double>(values[i])));
}

void LiveEQ::filterData(float * dataR, float * dataL, unsigned int len)
{
    if (!_isOn && _crossFade == 0)
        return;

    // Filter
    int j;
    for (quint32 i = 0; i < len; i++)
    {
        if (_isOn && _crossFade < CROSSFADE_LENGTH)
            _crossFade++;
        else if (!_isOn && _crossFade > 0)
            _crossFade--;
        float pos = static_cast<float>(_crossFade) / static_cast<float>(CROSSFADE_LENGTH);

        float value = dataR[i];
        float fTmp = 0;
        for (j = 0; j < 10; ++j)
            fTmp += _coeff[j] * _passBandsR[j].filter(value);
        dataR[i] = (1.0f - pos) * dataR[i] + pos * fTmp * CORRECTION;

        value = dataL[i];
        fTmp = 0;
        for (j = 0; j < 10; ++j)
            fTmp += _coeff[j] * _passBandsL[j].filter(value);
        dataL[i] = (1.0f - pos) * dataL[i] + pos * fTmp * CORRECTION;
    }
}

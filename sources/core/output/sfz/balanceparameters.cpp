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

#include "balanceparameters.h"
#include "soundfontmanager.h"
#include "sfzparamlist.h"

BalanceParameters::BalanceParameters(bool isMono) :
    _isMono(isMono),
    _leftUsed(false),
    _leftPan(0),
    _leftAttenuation(0),
    _rightUsed(false),
    _rightPan(0),
    _rightAttenuation(0),
    _pan(0),
    _width(100),
    _position(0),
    _volume(0)
{

}

void BalanceParameters::storeAttenuationAndPan(SoundfontManager * sf2, EltID idInstSmpl, int channel, SfzParamList * paramInst, SfzParamList * paramPrst)
{
    // Compute the attenuation
    double attenuationDb = 0;
    if (sf2->isSet(idInstSmpl, champ_initialAttenuation))
    {
        attenuationDb = Attribute::toRealValue(champ_initialAttenuation, false, sf2->get(idInstSmpl, champ_initialAttenuation));

        // Merge the prst value
        for (int i = 0; i < paramPrst->attributeCount(); i++)
        {
            if (paramPrst->getAttribute(i) == champ_initialAttenuation)
            {
                attenuationDb += paramPrst->getValue(i);

                // Limit
                AttributeValue value = Attribute::fromRealValue(champ_initialAttenuation, false, attenuationDb);
                attenuationDb = Attribute::toRealValue(champ_initialAttenuation, false, value);
            }
        }
    }
    else
    {
        // Take the default attenuation of the inst (prst value already merged)
        for (int i = 0; i < paramInst->attributeCount(); i++)
        {
            if (paramInst->getAttribute(i) == champ_initialAttenuation)
                attenuationDb = paramInst->getValue(i);
        }
    }

    // Compute the pan
    double pan = 0;
    if (sf2->isSet(idInstSmpl, champ_pan))
    {
        pan = Attribute::toRealValue(champ_pan, false, sf2->get(idInstSmpl, champ_pan));

        // Merge the prst value
        for (int i = 0; i < paramPrst->attributeCount(); i++)
        {
            if (paramPrst->getAttribute(i) == champ_pan)
            {
                pan += paramPrst->getValue(i);

                // Limit
                AttributeValue value = Attribute::fromRealValue(champ_pan, false, pan);
                pan = Attribute::toRealValue(champ_pan, false, value);
            }
        }
    }
    else
    {
        // Take the default pan of the inst (prst value already merged)
        for (int i = 0; i < paramInst->attributeCount(); i++)
        {
            if (paramInst->getAttribute(i) == champ_pan)
                pan = paramInst->getValue(i);
        }
    }

    // Divide by 50 => between -1 and 1
    pan *= 0.02;

    if (channel == -1)
    {
        _leftUsed = true;
        _leftPan = pan;
        _leftAttenuation = attenuationDb;
    }
    else
    {
        _rightUsed = true;
        _rightPan = pan;
        _rightAttenuation = attenuationDb;
    }
}

void BalanceParameters::computeSfzParameters()
{
    // Case when the sound is mono
    if (_isMono)
    {
        if (_rightUsed)
        {
            _pan = toInt(100. * _rightPan + 0.5);
            _volume = -_rightAttenuation;
        }
        else if (_leftUsed)
        {
            _pan = toInt(100. * _leftPan + 0.5);
            _volume = -_leftAttenuation;
        }
        return;
    }

    // Case when we use only one channel
    if (!_leftUsed)
    {
        _pan = _rightPan;
        _volume = -_rightAttenuation;
        return;
    }

    if (!_rightUsed)
    {
        _pan = _leftPan;
        _volume = -_leftAttenuation;
        return;
    }

    // Position / width
    _width = toInt((_rightPan - _leftPan) * 50); // Value between -100 and 100
    int delta = 100 - qAbs(_width);
    if (_leftPan < _rightPan)
        _position = toInt((((_leftPan + 1) * 50) / delta - 0.5) * 200);
    else
        _position = toInt((((_rightPan + 1) * 50) / delta - 0.5) * 200);

    // Coeff of each audio channel in each output
    double coefLeft = dBtoGain(-_leftAttenuation);
    double coefRight = dBtoGain(-_rightAttenuation);

    // Global gain
    double gain = sqrt(coefLeft * coefLeft + coefRight * coefRight);
    _volume = 0.1 * toInt(10. * log10(gain) * 20.);

    // Global pan
    double tmp = coefLeft / gain;
    double panRad = tmp >= 1 ? 0 : acos(tmp);
    _pan = toInt(100. * (panRad / M_PI * 4 - 1));
}

int BalanceParameters::toInt(double value)
{
    if (value > 0.5)
        return static_cast<int>(value + 0.5);
    if (value < -0.5)
        return static_cast<int>(value - 0.5);
    return 0;
}

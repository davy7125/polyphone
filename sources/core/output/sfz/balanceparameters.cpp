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

void BalanceParameters::getAttenuationAndPan(SoundfontManager * sf2, EltID idInstSmpl, int channel)
{
    double attenuationDb =
        sf2->isSet(idInstSmpl, champ_initialAttenuation) ?
            0.1 * DB_SF2_TO_REAL_DB * sf2->get(idInstSmpl, champ_initialAttenuation).shValue : 0.;
    double pan =
        sf2->isSet(idInstSmpl, champ_pan) ?
            0.002 * sf2->get(idInstSmpl, champ_pan).shValue : 0.; // Divide by 500 => between -1 and 1

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

    // Coeff of each audio channel in each output
    double coefLeftToLeft = dBtoGain(-_leftAttenuation) * cos((_leftPan + 1.) * M_PI / 4.);
    double coefLeftToRight = dBtoGain(-_leftAttenuation) * sin((_leftPan + 1.) * M_PI / 4.);
    double coefRightToLeft = dBtoGain(-_rightAttenuation) * cos((_rightPan + 1.) * M_PI / 4.);
    double coefRightToRight = dBtoGain(-_rightAttenuation) * sin((_rightPan + 1.) * M_PI / 4.);

    // Global gain
    double gain = sqrt(coefLeftToLeft * coefLeftToLeft + coefRightToLeft * coefRightToLeft +
        coefLeftToRight * coefLeftToRight + coefRightToRight * coefRightToRight);
    _volume = 0.1 * toInt(10. * log10(gain) * 20.);

    // Global pan
    double panRad = acos(sqrt(coefLeftToLeft * coefLeftToLeft + coefRightToLeft * coefRightToLeft) / gain);
    _pan = toInt(100. * (panRad / M_PI * 4 - 1));

    // Cancel the pan and gain on the coefficients
    if (coefLeftToLeft > 0 || coefRightToLeft > 0)
    {
        coefLeftToLeft /= gain * cos(panRad);
        coefRightToLeft /= gain * cos(panRad);
    }
    if (coefLeftToRight > 0 || coefRightToRight > 0)
    {
        coefLeftToRight /= gain * sin(panRad);
        coefRightToRight /= gain * sin(panRad);
    }

    // It's now possible to find the width and position
    double positionLeft = acos(coefLeftToLeft) / M_PI * 2;
    double positionRight = acos(coefRightToLeft) / M_PI * 2;
    _width = toInt(100. * (positionRight - positionLeft));
    double delta = 1. - qAbs(positionLeft - positionRight);
    if (delta > 0)
    {
        _position = positionLeft < positionRight ?
            toInt(200. * (positionLeft / delta - 0.5)) : toInt(200. * (positionRight / delta - 0.5));
    }
}

int BalanceParameters::toInt(double value)
{
    if (value > 0.5)
        return static_cast<int>(value + 0.5);
    if (value < -0.5)
        return static_cast<int>(value - 0.5);
    return 0;
}

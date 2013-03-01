#include "oscsinus.h"

OscSinus::OscSinus(qint32 sampleRate, double delay) :
    _sampleRate(sampleRate),
    _previousFreq(-1),
    _delayTime(delay * sampleRate),
    _currentDelay(0)
{
}

// Générateur Gordon-Smith
void OscSinus::getSinus(double * data, qint32 len, double freq)
{
    // Attente
    qint64 total = qMin(_delayTime - _currentDelay, len);
    for (int i = 0; i < total; i++)
        data[i] = 0;
    _currentDelay += total;

    // Sinus
    if (total != len)
    {
        if (_previousFreq == -1)
        {
            // Initialisation du système
            _previousFreq = freq;
            computeEpsilon(freq, _theta, _epsilon);
            _posPrec = qSin(-_theta);
            _posPrecQuad = qCos(-_theta);
        }
        if (_previousFreq != freq)
        {
            double theta2, epsilon2;
            computeEpsilon(freq, theta2, epsilon2);
            _previousFreq = freq;

            double progEpsilon;
            for (int i = total; i < len; i++)
            {
                progEpsilon = (double)(len - i) / (len - total) * _epsilon
                        + (double)(i - total) / (len - total) * epsilon2;
                _posPrecQuad -= progEpsilon * _posPrec;
                _posPrec     += progEpsilon * _posPrecQuad;
                data[i] = _posPrec;
            }

            // Mise à jour valeurs
            _theta = theta2;
            _epsilon = epsilon2;
            _previousFreq = freq;
        }
        else
        {
            for (int i = total; i < len; i++)
            {
                _posPrecQuad -= _epsilon * _posPrec;
                _posPrec     += _epsilon * _posPrecQuad;
                data[i] = _posPrec;
            }
        }
    }
}

void OscSinus::computeEpsilon(double freq, double &theta, double &epsilon)
{
    theta = 2. * M_PI * freq / _sampleRate;
    epsilon = 2. * qSin(theta / 2.);
}

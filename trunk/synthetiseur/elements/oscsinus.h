#ifndef OSCSINUS_H
#define OSCSINUS_H

#include "sound.h"

class OscSinus
{
public:
    OscSinus(qint32 sampleRate, double delay = 0);
    void getSinus(double *data, qint32 len, double freq);

private:
    // Attributs privés
    qint32 _sampleRate;
    double _previousFreq;
    qint32 _delayTime, _currentDelay;
    double _posPrec, _posPrecQuad;
    double _theta, _epsilon;

    // Méthode privée
    void computeEpsilon(double freq, double &theta, double &epsilon);
};

#endif // OSCSINUS_H

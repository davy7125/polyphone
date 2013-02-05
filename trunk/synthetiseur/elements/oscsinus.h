#ifndef OSCSINUS_H
#define OSCSINUS_H

#include "sound.h"

class OscSinus
{
public:
    OscSinus(qint32 sampleRate);
    void getSinus(char * data, qint32 len, double freq);
private:
    // Attributs privés
    qint32 _sampleRate;
    double _previousFreq;
    qint32 _currentPos;
    QByteArray _buffer;
    // Méthodes privées
    void initializeBuffer();
};

#endif // OSCSINUS_H

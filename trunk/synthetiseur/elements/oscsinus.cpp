#include "oscsinus.h"

OscSinus::OscSinus(qint32 sampleRate) :
    _sampleRate(sampleRate),
    _previousFreq(-1),
    _currentPos(0)
{
}

void OscSinus::getSinus(char * data, qint32 len, double freq)
{
    if (freq != _previousFreq)
    {
        _previousFreq = freq;
        this->initializeBuffer();
    }
    qint64 total = 0;
    while (len - total > 0)
    {
        const qint64 chunk = qMin((_buffer.size() - _currentPos), (int)(len - total));
        memcpy(data + total, _buffer.constData() + _currentPos, chunk);
        _currentPos = (_currentPos + chunk) % _buffer.size();
        total += chunk;
    }
}

void OscSinus::initializeBuffer()
{
    // Dimensionnement du buffer en fonction de la fréquence
    qint64 lengthT;
    double freqAjustee = 0;
    if (_previousFreq < 100)
    {
        // Longueur de 10 périodes
        lengthT = (double)(10.0 * _sampleRate) / _previousFreq;
        // Ajustement fréquence
        freqAjustee = (double)(10.0 * _sampleRate) / lengthT;
    }
    else if (_previousFreq < 1000)
    {
        // Longueur de 100 périodes
        lengthT = (double)(100.0 * _sampleRate) / _previousFreq;
        // Ajustement fréquence
        freqAjustee = (double)(100.0 * _sampleRate) / lengthT;
    }
    else
    {
        // Longueur de 1000 périodes
        lengthT = (double)(1000.0 * _sampleRate) / _previousFreq;
        // Ajustement fréquence
        freqAjustee = (double)(1000.0 * _sampleRate) / lengthT;
    }
    qint64 length = 4 * lengthT;
    _buffer.resize(length);
    // Création du sinus
    qint32 *ptr = (qint32 *)_buffer.data();
    double x;
    for (int i = 0; i < lengthT; i++)
    {
        x = qSin((2. * M_PI * freqAjustee * i) / _sampleRate);
        ptr[i] = static_cast<qint32>(x * 300000000);
    }
    // Positionnement au départ
    _currentPos = 0;
}

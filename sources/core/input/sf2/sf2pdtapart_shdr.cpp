#include "sf2pdtapart_shdr.h"
#include <QDataStream>

Sf2PdtaPart_shdr::Sf2PdtaPart_shdr() :
    _isValid(false),
    _start(0),
    _end(0),
    _startLoop(0),
    _endLoop(0),
    _sampleRate(0),
    _originalPitch(0),
    _correction(0)
{

}

QDataStream & operator >> (QDataStream &in, Sf2PdtaPart_shdr &shdr)
{
    // Name
    int length = 20;
    QByteArray buffer(length, Qt::Uninitialized);
    if (in.readRawData(buffer.data(), length) != length)
        return in;
    shdr._name = QString::fromLatin1(buffer);

    // Positions
    in >> shdr._start >> shdr._end >> shdr._startLoop >> shdr._endLoop;

    // Pitch
    in >> shdr._sampleRate >> shdr._originalPitch >> shdr._correction;

    // Link
    in >> shdr._wSampleLink >> shdr._sfSampleType;

    shdr._isValid = true;
    return in;
}

QDataStream & operator << (QDataStream &out, const Sf2PdtaPart_shdr &shdr)
{
    return out;
}

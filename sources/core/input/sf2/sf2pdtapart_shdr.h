#ifndef SF2PDTAPART_SHDR_H
#define SF2PDTAPART_SHDR_H

#include "sf2basetypes.h"
#include <QString>
class QDataStream;

class Sf2PdtaPart_shdr
{
public:
    Sf2PdtaPart_shdr();

    bool _isValid;

    QString _name;
    quint32Reversed _start;
    quint32Reversed _end;
    quint32Reversed _startLoop;
    quint32Reversed _endLoop;
    quint32Reversed _sampleRate;
    quint8 _originalPitch;
    qint8 _correction;
    quint16Reversed _wSampleLink;
    quint16Reversed _sfSampleType;
};

// Extension methods for QDataStream to serialize / deserialize
QDataStream & operator >> (QDataStream &in, Sf2PdtaPart_shdr &shdr);
QDataStream & operator << (QDataStream &out, const Sf2PdtaPart_shdr &shdr);

#endif // SF2PDTAPART_SHDR_H

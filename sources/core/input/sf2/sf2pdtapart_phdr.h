#ifndef SF2PDTAPART_PHDR_H
#define SF2PDTAPART_PHDR_H

#include "sf2basetypes.h"
#include <QString>
class QDataStream;

class Sf2PdtaPart_phdr
{
public:
    Sf2PdtaPart_phdr();

    bool _isValid;

    QString _name;
    quint16Reversed _preset;
    quint16Reversed _bank;
    quint16Reversed _pBagIndex;
    quint32Reversed _library;
    quint32Reversed _genre;
    quint32Reversed _morphology;
};

// Extension methods for QDataStream to serialize / deserialize
QDataStream & operator >> (QDataStream &in, Sf2PdtaPart_phdr &phdr);
QDataStream & operator << (QDataStream &out, const Sf2PdtaPart_phdr &phdr);

#endif // SF2PDTAPART_PHDR_H

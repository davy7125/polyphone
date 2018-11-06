#ifndef SF2PDTAPART_BAG_H
#define SF2PDTAPART_BAG_H

#include "sf2basetypes.h"
class QDataStream;

class Sf2PdtaPart_bag
{
public:
    Sf2PdtaPart_bag();

    bool _isValid;

    quint16Reversed _genIndex;
    quint16Reversed _modIndex;
};

// Extension methods for QDataStream to serialize / deserialize
QDataStream & operator >> (QDataStream &in, Sf2PdtaPart_bag &bag);
//QDataStream & operator << (QDataStream &out, const Sf2PdtaPart_bag &bag);

#endif // SF2PDTAPART_BAG_H

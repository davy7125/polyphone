#ifndef SF2PDTAPART_MOD_H
#define SF2PDTAPART_MOD_H

#include "sf2basetypes.h"
class QDataStream;

class Sf2PdtaPart_mod
{
public:
    Sf2PdtaPart_mod();

    bool _isValid;

    SFModulator _sfModSrcOper;
    quint16Reversed _sfModDestOper;
    qint16Reversed _modAmount;
    SFModulator _sfModAmtSrcOper;
    quint16Reversed _sfModTransOper;
};

// Extension methods for QDataStream to serialize / deserialize
QDataStream & operator >> (QDataStream &in, Sf2PdtaPart_mod &mod);
//QDataStream & operator << (QDataStream &out, const Sf2PdtaPart_mod &mod);

#endif // SF2PDTAPART_MOD_H

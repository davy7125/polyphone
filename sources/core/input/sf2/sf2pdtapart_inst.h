#ifndef SF2PDTAPART_INST_H
#define SF2PDTAPART_INST_H

#include "sf2basetypes.h"
#include <QString>
class QDataStream;

class Sf2PdtaPart_inst
{
public:
    Sf2PdtaPart_inst();

    bool _isValid;

    QString _name;
    quint16Reversed _iBagIndex;
};

// Extension methods for QDataStream to serialize / deserialize
QDataStream & operator >> (QDataStream &in, Sf2PdtaPart_inst &inst);
//QDataStream & operator << (QDataStream &out, const Sf2PdtaPart_inst &inst);

#endif // SF2PDTAPART_INST_H

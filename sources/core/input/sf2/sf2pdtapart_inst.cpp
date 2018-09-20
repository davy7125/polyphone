#include "sf2pdtapart_inst.h"
#include <QDataStream>

Sf2PdtaPart_inst::Sf2PdtaPart_inst() :
    _isValid(false),
    _iBagIndex(0)
{

}

QDataStream & operator >> (QDataStream &in, Sf2PdtaPart_inst &inst)
{
    // Name
    int length = 20;
    QByteArray buffer(length, Qt::Uninitialized);
    if (in.readRawData(buffer.data(), length) != length)
        return in;
    inst._name = QString::fromLatin1(buffer);

    // Min index for pBag
    in >> inst._iBagIndex;

    inst._isValid = true;
    return in;
}

QDataStream & operator << (QDataStream &out, const Sf2PdtaPart_inst &inst)
{
    return out;
}

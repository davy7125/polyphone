#include "sf2pdtapart_gen.h"
#include <QDataStream>

Sf2PdtaPart_gen::Sf2PdtaPart_gen() :
    _isValid(false),
    _sfGenOper(0),
    _genAmount(0)
{

}

QDataStream & operator >> (QDataStream &in, Sf2PdtaPart_gen &gen)
{
    in >> gen._sfGenOper >> gen._genAmount;

    gen._isValid = true;
    return in;
}

QDataStream & operator << (QDataStream &out, const Sf2PdtaPart_gen &gen)
{
    return out;
}

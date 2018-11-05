#include "sf2pdtapart_bag.h"
#include <QDataStream>

Sf2PdtaPart_bag::Sf2PdtaPart_bag() :
    _isValid(false),
    _genIndex(0),
    _modIndex(0)
{

}

QDataStream & operator >> (QDataStream &in, Sf2PdtaPart_bag &bag)
{
    in >> bag._genIndex;
    in >> bag._modIndex;

    bag._isValid = true;
    return in;
}


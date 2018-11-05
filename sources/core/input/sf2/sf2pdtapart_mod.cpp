#include "sf2pdtapart_mod.h"
#include <QDataStream>

Sf2PdtaPart_mod::Sf2PdtaPart_mod() :
    _isValid(false),
    _sfModDestOper(0),
    _modAmount(0),
    _sfModTransOper(0)
{
    _sfModSrcOper.CC = 0;
    _sfModSrcOper.D = 0;
    _sfModSrcOper.Index = 0;
    _sfModSrcOper.P = 0;
    _sfModSrcOper.Type = 0;

    _sfModAmtSrcOper.CC = 0;
    _sfModAmtSrcOper.D = 0;
    _sfModAmtSrcOper.Index = 0;
    _sfModAmtSrcOper.P = 0;
    _sfModAmtSrcOper.Type = 0;
}

QDataStream & operator >> (QDataStream &in, Sf2PdtaPart_mod &mod)
{
    in >> mod._sfModSrcOper >> mod._sfModDestOper >> mod._modAmount >> mod._sfModAmtSrcOper >> mod._sfModTransOper;

    mod._isValid = true;
    return  in;
}

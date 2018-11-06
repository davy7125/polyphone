#ifndef SF2PDTAPART_H
#define SF2PDTAPART_H

#include "sf2basetypes.h"
#include "sf2pdtapart_phdr.h"
#include "sf2pdtapart_bag.h"
#include "sf2pdtapart_mod.h"
#include "sf2pdtapart_gen.h"
#include "sf2pdtapart_inst.h"
#include "sf2pdtapart_shdr.h"
#include <QList>
class QDataStream;

class Sf2PdtaPart
{
public:
    Sf2PdtaPart();

    bool _isValid;

    char _LIST[4]; // Should be "LIST"
    char _pdta[4]; // Should be "pdta"
    quint32Reversed _pdtaSize; // Size of the section pdta

    char _phdr[4]; // Should be "phdr"
    quint32Reversed _phdrSize; // Size of the section phdr
    QList<Sf2PdtaPart_phdr> _phdrs;

    char _pbag[4]; // Should be "pbag"
    quint32Reversed _pbagSize; // Size of the section pbag
    QList<Sf2PdtaPart_bag> _pbags;

    char _pmod[4]; // Should be "pmod"
    quint32Reversed _pmodSize; // Size of the section pmod
    QList<Sf2PdtaPart_mod> _pmods;

    char _pgen[4]; // Should be "pgen"
    quint32Reversed _pgenSize; // Size of the section pgen
    QList<Sf2PdtaPart_gen> _pgens;

    char _inst[4]; // Should be "inst"
    quint32Reversed _instSize; // Size of the section inst
    QList<Sf2PdtaPart_inst> _insts;

    char _ibag[4]; // Should be "ibag"
    quint32Reversed _ibagSize; // Size of the section ibag
    QList<Sf2PdtaPart_bag> _ibags;

    char _imod[4]; // Should be "imod"
    quint32Reversed _imodSize; // Size of the section imod
    QList<Sf2PdtaPart_mod> _imods;

    char _igen[4]; // Should be "igen"
    quint32Reversed _igenSize; // Size of the section igen
    QList<Sf2PdtaPart_gen> _igens;

    char _shdr[4]; // Should be "shdr"
    quint32Reversed _shdrSize; // Size of the section shdr
    QList<Sf2PdtaPart_shdr> _shdrs;
};

// Extension methods for QDataStream to serialize / deserialize
QDataStream & operator >> (QDataStream &in, Sf2PdtaPart &pdta);
//QDataStream & operator << (QDataStream &out, const Sf2PdtaPart &pdta);

#endif // SF2PDTAPART_H

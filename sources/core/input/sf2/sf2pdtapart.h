/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program. If not, see http://www.gnu.org/licenses/.    **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef SF2PDTAPART_H
#define SF2PDTAPART_H

#include "basetypes.h"
#include "sf2pdtapart_phdr.h"
#include "sf2pdtapart_bag.h"
#include "sf2pdtapart_mod.h"
#include "sf2pdtapart_gen.h"
#include "sf2pdtapart_inst.h"
#include "sf2pdtapart_shdr.h"
#include <QList>

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

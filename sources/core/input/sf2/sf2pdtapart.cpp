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

#include "sf2pdtapart.h"
#include <QDataStream>

Sf2PdtaPart::Sf2PdtaPart() :
    _isValid(false),
    _pdtaSize(0)
{

}

QDataStream & operator >> (QDataStream &in, Sf2PdtaPart &pdta)
{
    // 4 char, should be "LIST"
    if (in.readRawData(pdta._LIST, 4) != 4)
        return in;

    // 4 char, should be "pdta"
    if (in.readRawData(pdta._pdta, 4) != 4)
        return in;

    // Size of the section "pdta"
    in >> pdta._pdtaSize;

    /// Read PHDR

    // 4 char, should be "phdr"
    if (in.readRawData(pdta._phdr, 4) != 4)
        return in;

    // Size of the section "phdr"
    in >> pdta._phdrSize;
    if (pdta._phdrSize.value % 38 != 0)
        return in;

    for (unsigned int i = 0; i < pdta._phdrSize.value / 38; i++)
    {
        pdta._phdrs << Sf2PdtaPart_phdr();
        in >> pdta._phdrs[i];
    }

    /// Read PBAG

    // 4 char, should be "pbag"
    if (in.readRawData(pdta._pbag, 4) != 4)
        return in;

    // Size of the section "pbag"
    in >> pdta._pbagSize;
    if (pdta._pbagSize.value % 4 != 0)
        return in;

    for (unsigned int i = 0; i < pdta._pbagSize.value / 4; i++)
    {
        pdta._pbags << Sf2PdtaPart_bag();
        in >> pdta._pbags[i];
    }

    /// Read PMOD

    // 4 char, should be "pmod"
    if (in.readRawData(pdta._pmod, 4) != 4)
        return in;

    // Size of the section "pmod"
    in >> pdta._pmodSize;
    if (pdta._pmodSize.value % 10 != 0)
        return in;

    for (unsigned int i = 0; i < pdta._pmodSize.value / 10; i++)
    {
        pdta._pmods << Sf2PdtaPart_mod();
        in >> pdta._pmods[i];
    }

    /// Read PGEN

    // 4 char, should be "pgen"
    if (in.readRawData(pdta._pgen, 4) != 4)
        return in;

    // Size of the section "pgen"
    in >> pdta._pgenSize;
    if (pdta._pgenSize.value % 4 != 0)
        return in;

    for (unsigned int i = 0; i < pdta._pgenSize.value / 4; i++)
    {
        pdta._pgens << Sf2PdtaPart_gen();
        in >> pdta._pgens[i];
    }

    /// Read INST

    // 4 char, should be "inst"
    if (in.readRawData(pdta._inst, 4) != 4)
        return in;

    // Size of the section "inst"
    in >> pdta._instSize;
    if (pdta._instSize.value % 22 != 0)
        return in;

    for (unsigned int i = 0; i < pdta._instSize.value / 22; i++)
    {
        pdta._insts << Sf2PdtaPart_inst();
        in >> pdta._insts[i];
    }

    /// Read IBAG

    // 4 char, should be "ibag"
    if (in.readRawData(pdta._ibag, 4) != 4)
        return in;

    // Size of the section "ibag"
    in >> pdta._ibagSize;
    if (pdta._ibagSize.value % 4 != 0)
        return in;

    for (unsigned int i = 0; i < pdta._ibagSize.value / 4; i++)
    {
        pdta._ibags << Sf2PdtaPart_bag();
        in >> pdta._ibags[i];
    }

    /// Read IMOD

    // 4 char, should be "imod"
    if (in.readRawData(pdta._imod, 4) != 4)
        return in;

    // Size of the section "imod"
    in >> pdta._imodSize;
    if (pdta._imodSize.value % 10 != 0)
        return in;

    for (unsigned int i = 0; i < pdta._imodSize.value / 10; i++)
    {
        pdta._imods << Sf2PdtaPart_mod();
        in >> pdta._imods[i];
    }

    /// Read IGEN

    // 4 char, should be "igen"
    if (in.readRawData(pdta._igen, 4) != 4)
        return in;

    // Size of the section "igen"
    in >> pdta._igenSize;
    if (pdta._igenSize.value % 4 != 0)
        return in;

    for (unsigned int i = 0; i < pdta._igenSize.value / 4; i++)
    {
        pdta._igens << Sf2PdtaPart_gen();
        in >> pdta._igens[i];
    }

    /// READ SHDR

    // 4 char, should be "shdr"
    if (in.readRawData(pdta._shdr, 4) != 4)
        return in;

    // Size of the section "shdr"
    in >> pdta._shdrSize;
    if (pdta._shdrSize.value % 46 != 0)
        return in;

    for (unsigned int i = 0; i < pdta._shdrSize.value / 46; i++)
    {
        pdta._shdrs << Sf2PdtaPart_shdr();
        in >> pdta._shdrs[i];
    }

    pdta._isValid = true;
    return in;
}

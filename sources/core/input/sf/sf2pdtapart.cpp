/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "sf2pdtapart.h"
#include "sf2sdtapart.h"
#include <QDataStream>
#include "sound.h"

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

    // Size of the section "pdta"
    in >> pdta._pdtaSize;

    // 4 char, should be "pdta"
    if (in.readRawData(pdta._pdta, 4) != 4)
        return in;

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

    /// Read SHDR

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

quint32 Sf2PdtaPart::prepareBeforeWritingData(Sf2SdtaPart* sdtaPart, bool isSf3)
{
    // Constant strings
    memcpy(_LIST, "LIST", 4);
    memcpy(_pdta, "pdta", 4);
    memcpy(_phdr, "phdr", 4);
    memcpy(_pbag, "pbag", 4);
    memcpy(_pmod, "pmod", 4);
    memcpy(_pgen, "pgen", 4);
    memcpy(_inst, "inst", 4);
    memcpy(_ibag, "ibag", 4);
    memcpy(_imod, "imod", 4);
    memcpy(_igen, "igen", 4);
    memcpy(_shdr, "shdr", 4);

    // Sample properties
    quint32 position = 0;
    for (int i = 0; i < sdtaPart->_sounds.count(); i++)
    {
        Sound* sound = sdtaPart->_sounds[i];
        char* rawData = nullptr;
        quint32 rawDataLength = 0;
        if (isSf3)
            sound->getRawData(rawData, rawDataLength);

        _shdrs[i]._start.value = position;
        if (isSf3)
            _shdrs[i]._end.value = position + (rawDataLength > 0 ? rawDataLength : 2 * sound->getUInt32(champ_dwLength));
        else
            _shdrs[i]._end.value = position + sound->getUInt32(champ_dwLength);
        _shdrs[i]._startLoop.value = (isSf3 ? 0 : position) + sound->getUInt32(champ_dwStartLoop);
        _shdrs[i]._endLoop.value = (isSf3 ? 0 : position) + sound->getUInt32(champ_dwEndLoop);
        _shdrs[i]._sfSampleType.value |= (rawDataLength > 0 ? 0x10 : 0x00);

        // Go on
        position = _shdrs[i]._end.value;
        if (rawDataLength > 0)
        {
            // Possible padding
            if (rawDataLength % 4 != 0)
                position += 4 - (rawDataLength % 4);
        }
        else
        {
            // 46 zeros for non-compressed samples
            position += isSf3 ?  2 * 46 : 46;
        }
    }

    // Sizes
    _phdrSize.value = 38 * _phdrs.count();
    _pbagSize.value = 4 * _pbags.count();
    _pmodSize.value = 10 * _pmods.count();
    _pgenSize.value = 4 * _pgens.count();
    _instSize.value = 22 * _insts.count();
    _ibagSize.value = 4 * _ibags.count();
    _imodSize.value = 10 * _imods.count();
    _igenSize.value = 4 * _igens.count();
    _shdrSize.value = 46 * _shdrs.count();

    // Total size
    _pdtaSize = _phdrSize.value + _pbagSize.value + _pmodSize.value +
                _pgenSize.value + _instSize.value + _ibagSize.value +
                _imodSize.value +  _igenSize.value + _shdrSize.value +
                8 * 9 + 4;
    return 8 + _pdtaSize.value;
}

QDataStream & operator << (QDataStream &out, Sf2PdtaPart &pdta)
{
    // "LIST"
    if (out.writeRawData(pdta._LIST, 4) != 4)
        return out;

    // Size of the section "pdta"
    out << pdta._pdtaSize;

    // "pdta"
    if (out.writeRawData(pdta._pdta, 4) != 4)
        return out;

    /// Write PHDR

    // "phdr"
    if (out.writeRawData(pdta._phdr, 4) != 4)
        return out;

    // Size of the section "phdr"
    out << pdta._phdrSize;

    for (unsigned int i = 0; i < pdta._phdrs.count(); i++)
        out << pdta._phdrs[i];

    /// Write PBAG

    // "pbag"
    if (out.writeRawData(pdta._pbag, 4) != 4)
        return out;

    // Size of the section "pbag"
    out << pdta._pbagSize;

    for (unsigned int i = 0; i < pdta._pbags.count(); i++)
        out << pdta._pbags[i];

    /// Write PMOD

    // "pmod"
    if (out.writeRawData(pdta._pmod, 4) != 4)
        return out;

    // Size of the section "pmod"
    out << pdta._pmodSize;

    for (unsigned int i = 0; i < pdta._pmods.count(); i++)
        out << pdta._pmods[i];

    /// Write PGEN

    // "pgen"
    if (out.writeRawData(pdta._pgen, 4) != 4)
        return out;

    // Size of the section "pgen"
    out << pdta._pgenSize;

    for (unsigned int i = 0; i < pdta._pgens.count(); i++)
        out << pdta._pgens[i];

    /// Write INST

    // "inst"
    if (out.writeRawData(pdta._inst, 4) != 4)
        return out;

    // Size of the section "inst"
    out << pdta._instSize;

    for (unsigned int i = 0; i < pdta._insts.count(); i++)
        out << pdta._insts[i];

    /// Write IBAG

    // "ibag"
    if (out.writeRawData(pdta._ibag, 4) != 4)
        return out;

    // Size of the section "ibag"
    out << pdta._ibagSize;

    for (unsigned int i = 0; i < pdta._ibags.count(); i++)
        out << pdta._ibags[i];

    /// Write IMOD

    // "imod"
    if (out.writeRawData(pdta._imod, 4) != 4)
        return out;

    // Size of the section "imod"
    out << pdta._imodSize;

    for (unsigned int i = 0; i < pdta._imods.count(); i++)
        out << pdta._imods[i];

    /// Write IGEN

    // "igen"
    if (out.writeRawData(pdta._igen, 4) != 4)
        return out;

    // Size of the section "igen"
    out << pdta._igenSize;

    for (unsigned int i = 0; i < pdta._igens.count(); i++)
        out << pdta._igens[i];

    /// Write SHDR

    // "shdr"
    if (out.writeRawData(pdta._shdr, 4) != 4)
        return out;

    // Size of the section "shdr"
    out << pdta._shdrSize;

    for (unsigned int i = 0; i < pdta._shdrs.count(); i++)
        out << pdta._shdrs[i];

    pdta._isValid = true;
    return out;
}

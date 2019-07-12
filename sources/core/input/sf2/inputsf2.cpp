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

#include "inputsf2.h"
#include "soundfontmanager.h"
#include <QFile>
#include <QDataStream>
#include "sf2header.h"
#include "sf2sdtapart.h"
#include "sf2pdtapart.h"

InputSf2::InputSf2() : AbstractInput() {}

void InputSf2::processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int &sf2Index, QString &tempFilePath)
{
    Q_UNUSED(tempFilePath)

    // Keep the variables
    _sm = sm;
    _filename = fileName;

    // Open the file
    QFile fi(fileName);
    if (!fi.exists())
    {
        success = false;
        error = trUtf8("Cannot find file \"%1\".").arg(fileName);
        return;
    }

    if (!fi.open(QIODevice::ReadOnly))
    {
        success = false;
        error = trUtf8("Access denied for reading file \"%1\".").arg(fileName);
        return;
    }

    // Parse the file
    QDataStream stream(&fi);
    this->parse(stream, success, error, sf2Index);
    fi.close();
}

void InputSf2::parse(QDataStream &stream, bool &success, QString &error, int &sf2Index)
{
    // Parse the different parts of the file
    Sf2Header header;
    Sf2SdtaPart sdtaPart;
    Sf2PdtaPart pdtaPart;
    stream >> header >> sdtaPart >> pdtaPart;

    if (!header._isValid)
    {
        success = false;
        error = "invalid header";
        return;
    }

    if (!sdtaPart._isValid)
    {
        success = false;
        error = "corrupted file (sdta)";
        return;
    }

    if (!pdtaPart._isValid)
    {
        success = false;
        error = "corrupted file (pdta)";
        return;
    }

    fillSf2(header, sdtaPart, pdtaPart, success, error, sf2Index);
}

void InputSf2::fillSf2(Sf2Header &header, Sf2SdtaPart &sdtaPart, Sf2PdtaPart &pdtaPart, bool &success, QString &error, int &sf2Index)
{
    Q_UNUSED(error)

    // Create a new soundfont
    sf2Index = _sm->add(EltID(elementSf2));
    EltID id(elementSf2, sf2Index);

    /// General data

    _sm->set(id, champ_ISNG, header.getInfo("isng"));
    _sm->set(id, champ_name, header.getInfo("INAM"));
    _sm->set(id, champ_IROM, header.getInfo("irom"));
    _sm->set(id, champ_ICRD, header.getInfo("ICRD"));
    _sm->set(id, champ_IENG, header.getInfo("IENG"));
    _sm->set(id, champ_IPRD, header.getInfo("IPRD"));
    _sm->set(id, champ_ICOP, header.getInfo("ICOP"));
    _sm->set(id, champ_ICMT, header.getInfo("ICMT"));
    _sm->set(id, champ_ISFT, header.getInfo("ISFT"));
    AttributeValue value;
    value.sfVerValue = header.getVersion("ifil");
    _sm->set(id, champ_IFIL, value);
    value.sfVerValue = header.getVersion("iver");
    _sm->set(id, champ_IVER, value);

    // Mode 16 ou 24 bits au chargement
    if (sdtaPart._startSm24Offset > 0)
    {
        value.wValue = 24;
        _sm->set(id, champ_wBpsInit, value);
        _sm->set(id, champ_wBpsSave, value);
    }
    else
    {
        value.wValue = 16;
        _sm->set(id, champ_wBpsInit, value);
        _sm->set(id, champ_wBpsSave, value);
    }

    /// Samples

    id = EltID(elementSmpl, sf2Index);
    for (int i = 0; i < pdtaPart._shdrs.count() - 1; i++) // Terminal sample (EOS) is not read
    {
        Sf2PdtaPart_shdr SHDR = pdtaPart._shdrs[i];

        id.indexElt = _sm->add(id);
        _sm->set(id, champ_name, SHDR._name);

        // Sample properties
        value.bValue = SHDR._originalPitch;
        _sm->set(id, champ_byOriginalPitch, value);
        value.cValue = SHDR._correction;
        _sm->set(id, champ_chPitchCorrection, value);
        value.wValue = SHDR._wSampleLink.value;
        _sm->set(id, champ_wSampleLink, value);
        value.sfLinkValue = (SFSampleLink)SHDR._sfSampleType.value;
        _sm->set(id, champ_sfSampleType, value);
        value.wValue = 1;
        _sm->set(id, champ_wChannel, value);
        value.dwValue = SHDR._sampleRate.value;
        _sm->set(id, champ_dwSampleRate, value);
        _sm->set(id, champ_filenameForData, _filename);

        // Start / end / length of the sample
        value.dwValue = SHDR._end.value - SHDR._start.value;
        _sm->set(id, champ_dwLength, value);
        value.dwValue = SHDR._start.value * 2 + (20 + header._infoSize.value + sdtaPart._startSmplOffset);
        _sm->set(id, champ_dwStart16, value);
        if (sdtaPart._startSm24Offset > 0)
        {
            value.dwValue = SHDR._start.value + sdtaPart._startSm24Offset;
            _sm->set(id, champ_dwStart24, value);
            value.wValue = 24;
            _sm->set(id, champ_bpsFile, value);
        }
        else
        {
            value.dwValue = 0;
            _sm->set(id, champ_dwStart24, value);
            value.wValue = 16;
            _sm->set(id, champ_bpsFile, value);
        }

        // Loop
        value.dwValue = SHDR._startLoop.value - SHDR._start.value;
        _sm->set(id, champ_dwStartLoop, value);
        value.dwValue = SHDR._endLoop.value - SHDR._start.value;
        _sm->set(id, champ_dwEndLoop, value);
    }

    /// Instruments

    EltID id2;
    id = EltID(elementInst, sf2Index);
    id2.indexSf2 = sf2Index;
    quint16 bagmin, bagmax, modmin, modmax, genmin, genmax;
    int l, global;
    for (int i = 0; i < pdtaPart._insts.count() - 1; i++) // Terminal instrument (EOI) is not read
    {
        Sf2PdtaPart_inst inst = pdtaPart._insts[i];

        l = 0;
        id.indexElt = _sm->add(id);
        id2.indexElt = id.indexElt;
        _sm->set(id, champ_name, inst._name.trimmed());

        // Indexes
        bagmin = inst._iBagIndex.value;
        bagmax = pdtaPart._insts[i + 1]._iBagIndex.value;

        // Foreach ibag
        for (int j = bagmin; j < bagmax; j++)
        {
            Sf2PdtaPart_bag bag = pdtaPart._ibags[j];

            // Indexes of IMOD and IGEN
            modmin = bag._modIndex.value;
            genmin = bag._genIndex.value;
            if (j < pdtaPart._ibags.count() - 1)
            {
                modmax = pdtaPart._ibags[j + 1]._modIndex.value;
                genmax = pdtaPart._ibags[j + 1]._genIndex.value;
            }
            else
            {
                modmax = pdtaPart._imods.count();
                genmax = pdtaPart._igens.count();
            }

            // Global zone?
            global = 1;
            for (int k = genmin; k < genmax; k++)
                if (pdtaPart._igens[k]._sfGenOper.value == champ_sampleID)
                    global = 0;
            id2.indexElt2 = -1;

            // Parameters
            if (global)
                id2.typeElement = elementInst;
            else
            {
                // Add a sample to an instrument
                id2.typeElement = elementInstSmpl;
                id2.indexElt2 = _sm->add(id2);
            }
            for (int k = genmin; k < genmax; k++)
            {
                value.wValue = pdtaPart._igens[k]._genAmount.value;
                _sm->set(id2, (AttributeType)pdtaPart._igens[k]._sfGenOper.value, value);
            }

            // Modulators
            for (unsigned int k = modmin; k < modmax; k++)
            {
                id2.indexMod = -1;
                if (global)
                {
                    // Add a mod to an instrument
                    id2.typeElement = elementInstMod;
                    id2.indexMod = _sm->add(id2);
                }
                else
                {
                    // Add a mod to a sample linked to an instrument
                    id2.typeElement = elementInstSmplMod;
                    id2.indexMod = _sm->add(id2);
                }

                Sf2PdtaPart_mod mod = pdtaPart._imods[k];
                value.sfModValue = mod._sfModSrcOper;
                _sm->set(id2, champ_sfModSrcOper, value);
                value.wValue = mod._sfModDestOper.value;
                _sm->set(id2, champ_sfModDestOper, value);
                value.shValue = mod._modAmount.value;
                _sm->set(id2, champ_modAmount, value);
                value.sfModValue = mod._sfModAmtSrcOper;
                _sm->set(id2, champ_sfModAmtSrcOper, value);
                value.sfTransValue = (SFTransform)mod._sfModTransOper.value;
                _sm->set(id2, champ_sfModTransOper, value);
                value.wValue = l;
                _sm->set(id2, champ_indexMod, value);
                l++;
            }
        }
    }

    /// Presets

    id.typeElement = elementPrst;
    id.indexSf2 = sf2Index;
    id.indexElt = -1;
    for (int i = 0; i < pdtaPart._phdrs.count() - 1; i++) // Terminal preset (EOP) is not read
    {
        Sf2PdtaPart_phdr prst = pdtaPart._phdrs[i];

        l = 0;
        id.indexElt = _sm->add(id);
        id2.indexElt = id.indexElt;
        _sm->set(id, champ_name, prst._name.trimmed());
        value.wValue = prst._preset.value;
        _sm->set(id, champ_wPreset, value);
        value.wValue = prst._bank.value;
        _sm->set(id, champ_wBank, value);
        value.dwValue = prst._library.value;
        _sm->set(id, champ_dwLibrary, value);
        value.dwValue = prst._genre.value;
        _sm->set(id, champ_dwGenre, value);
        value.dwValue = prst._morphology.value;
        _sm->set(id, champ_dwMorphology, value);

        // Indexes
        bagmin = prst._pBagIndex.value;
        bagmax = pdtaPart._phdrs[i + 1]._pBagIndex.value;

        // Foreach pbag
        for (int j = bagmin; j < bagmax; j++)
        {
            Sf2PdtaPart_bag bag = pdtaPart._pbags[j];

            // Indexes of PMOD and PGEN
            modmin = bag._modIndex.value;
            genmin = bag._genIndex.value;
            if (j < pdtaPart._pbags.count() - 1)
            {
                modmax = pdtaPart._pbags[j + 1]._modIndex.value;
                genmax = pdtaPart._pbags[j + 1]._genIndex.value;
            }
            else
            {
                modmax = pdtaPart._pmods.count();
                genmax = pdtaPart._pgens.count();
            }

            // Global zone?
            global = 1;
            for (int k = genmin; k < genmax; k++)
                if (pdtaPart._pgens[k]._sfGenOper.value == champ_instrument)
                    global = 0;

            // Parameters
            id2.indexElt2 = -1;
            if (global)
                id2.typeElement = elementPrst;
            else
            {
                // Add an instrument to a preset
                id2.typeElement = elementPrstInst;
                id2.indexElt2 = _sm->add(id2);
            }
            for (int k = genmin; k < genmax; k++)
            {
                value.wValue = pdtaPart._pgens[k]._genAmount.value;
                _sm->set(id2, (AttributeType)pdtaPart._pgens[k]._sfGenOper.value, value);
            }

            // Modulators
            for (int k = modmin; k < modmax; k++)
            {
                id2.indexMod = -1;
                if (global)
                {
                    // Add a mod to a preset
                    id2.typeElement = elementPrstMod;
                    id2.indexMod = _sm->add(id2);
                }
                else
                {
                    // Add a mod to an instrument linked to a preset
                    id2.typeElement = elementPrstInstMod;
                    id2.indexMod = _sm->add(id2);
                }

                Sf2PdtaPart_mod mod = pdtaPart._pmods[k];
                value.sfModValue = mod._sfModSrcOper;
                _sm->set(id2, champ_sfModSrcOper, value);
                value.wValue = mod._sfModDestOper.value;
                _sm->set(id2, champ_sfModDestOper, value);
                value.shValue = mod._modAmount.value;
                _sm->set(id2, champ_modAmount, value);
                value.sfModValue = mod._sfModAmtSrcOper;
                _sm->set(id2, champ_sfModAmtSrcOper, value);
                value.sfTransValue = (SFTransform)mod._sfModTransOper.value;
                _sm->set(id2, champ_sfModTransOper, value);
                value.wValue = l;
                _sm->set(id2, champ_indexMod, value);
                l++;
            }
        }
    }

    success = true;
}

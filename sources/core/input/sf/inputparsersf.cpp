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


#include "inputparsersf.h"
#include "soundfontmanager.h"
#include "sf2filesource.h"
#include <QFile>

InputParserSf::InputParserSf() : AbstractInputParser() {}

void InputParserSf::processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int &sf2Index, QString &tempFilePath)
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
        error = tr("Cannot find file \"%1\".").arg(fileName);
        return;
    }

    if (!fi.open(QIODevice::ReadOnly))
    {
        success = false;
        error = tr("Access denied for reading file \"%1\".").arg(fileName);
        return;
    }

    // Read the structure (the sample data is read when needed)
    Sf2FileSource source(fi);
    Sf2Reader reader;
    success = reader.read(source);
    fi.close();
    if (success)
        fillSf2(reader, sf2Index);
    else
        error = QString::fromStdString(reader.getError());
}

void InputParserSf::fillSf2(const Sf2Reader &reader, int &sf2Index)
{
    // Create a new soundfont
    sf2Index = _sm->add(EltID(elementSf2));
    EltID id(elementSf2, sf2Index);

    /// General data

    _sm->set(id, champ_ISNG, QString::fromStdString(reader.getInfo("isng")));
    _sm->set(id, champ_name, QString::fromStdString(reader.getInfo("INAM")));
    _sm->set(id, champ_IROM, QString::fromStdString(reader.getInfo("irom")));
    _sm->set(id, champ_ICRD, QString::fromStdString(reader.getInfo("ICRD")));
    _sm->set(id, champ_IENG, QString::fromStdString(reader.getInfo("IENG")));
    _sm->set(id, champ_IPRD, QString::fromStdString(reader.getInfo("IPRD")));
    _sm->set(id, champ_ICOP, QString::fromStdString(reader.getInfo("ICOP")));
    _sm->set(id, champ_ICMT, QString::fromStdString(reader.getInfo("ICMT")));
    _sm->set(id, champ_ISFT, QString::fromStdString(reader.getInfo("ISFT")));
    AttributeValue value;
    value.sfVerValue = reader.getVersion("ifil");
    _sm->set(id, champ_IFIL, value);
    value.sfVerValue = reader.getVersion("iver");
    _sm->set(id, champ_IVER, value);

    // Mode 16 or 24 bits at loading
    value.wValue = reader.has24BitSamples() ? 24 : 16;
    _sm->set(id, champ_wBpsInit, value);
    _sm->set(id, champ_wBpsSave, value);

    /// Samples

    id = EltID(elementSmpl, sf2Index);
    for (const Sf2Reader::Sample &sample : reader.getSamples())
    {
        id.indexElt = _sm->add(id);
        _sm->set(id, champ_name, QString::fromStdString(sample.name));
        _sm->set(id, champ_filenameForData, _filename);

        // Sample type + link
        Sound * sound = _sm->getSound(id);
        value.wValue = sample.sampleLink;
        _sm->set(id, champ_wSampleLink, value);
        value.cValue = sample.isCompressed ? 1 : 0;
        sound->set(champ_rawDataAvailable, value);
        value.sfLinkValue = sample.sampleType;
        _sm->set(id, champ_sfSampleType, value);

        // Sample properties
        value.bValue = sample.originalPitch;
        sound->set(champ_byOriginalPitch, value);
        value.cValue = sample.correction;
        sound->set(champ_chPitchCorrection, value);
        value.wValue = 1;
        sound->set(champ_wChannel, value);
        value.dwValue = sample.sampleRate;
        sound->set(champ_dwSampleRate, value);

        // Length, position of the data (16 and possibly 24 bits) and loop
        value.dwValue = sample.length;
        sound->set(champ_dwLength, value);
        value.dwValue = sample.start16;
        sound->set(champ_dwStart16, value);
        value.dwValue = sample.start24;
        sound->set(champ_dwStart24, value);
        value.wValue = sample.start24 > 0 ? 24 : 16;
        sound->set(champ_bpsFile, value);
        value.dwValue = sample.loopStart;
        sound->set(champ_dwStartLoop, value);
        value.dwValue = sample.loopEnd;
        sound->set(champ_dwEndLoop, value);
    }

    /// Instruments and presets

    addElements(reader.getInstruments(), sf2Index, false);
    addElements(reader.getPresets(), sf2Index, true);
}

void InputParserSf::addElements(const std::vector<Sf2Reader::Element> &elements, int sf2Index, bool isPreset)
{
    EltID id(isPreset ? elementPrst : elementInst, sf2Index);
    AttributeValue value;
    for (const Sf2Reader::Element &element : elements)
    {
        id.indexElt = _sm->add(id);
        _sm->set(id, champ_name, QString::fromStdString(element.name));
        if (isPreset)
        {
            value.wValue = element.preset;
            _sm->set(id, champ_wPreset, value);
            value.wValue = element.bank;
            _sm->set(id, champ_wBank, value);
            value.dwValue = element.library;
            _sm->set(id, champ_dwLibrary, value);
            value.dwValue = element.genre;
            _sm->set(id, champ_dwGenre, value);
            value.dwValue = element.morphology;
            _sm->set(id, champ_dwMorphology, value);
        }

        // Global zone, then the divisions
        addZone(id, element.globalZone, isPreset ? elementPrstMod : elementInstMod);
        EltID idDivision(isPreset ? elementPrstInst : elementInstSmpl, sf2Index, id.indexElt);
        for (const Sf2Reader::Zone &zone : element.zones)
        {
            idDivision.indexElt2 = _sm->add(idDivision);
            addZone(idDivision, zone, isPreset ? elementPrstInstMod : elementInstSmplMod);
        }
    }
}

void InputParserSf::addZone(EltID id, const Sf2Reader::Zone &zone, ElementType modulatorType)
{
    // Attributes
    for (int champ = 0; champ < champ_endOper; champ++)
        if (zone.attributeSet[champ])
            _sm->set(id, static_cast<AttributeType>(champ), zone.attributeValues[champ]);

    // Modulators
    id.typeElement = modulatorType;
    AttributeValue value;
    for (const ModulatorData &modulator : zone.modulators)
    {
        id.indexMod = _sm->add(id);
        value.sfModValue = modulator.srcOper;
        _sm->set(id, champ_sfModSrcOper, value);
        value.wValue = modulator.destOper;
        _sm->set(id, champ_sfModDestOper, value);
        value.shValue = modulator.amount;
        _sm->set(id, champ_modAmount, value);
        value.sfModValue = modulator.amtSrcOper;
        _sm->set(id, champ_sfModAmtSrcOper, value);
        value.sfTransValue = modulator.transOper;
        _sm->set(id, champ_sfModTransOper, value);
        value.wValue = modulator.index;
        _sm->set(id, champ_indexMod, value);
    }
}

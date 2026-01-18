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

#include "outputsf.h"
#include "sfindexconverter.h"
#include "soundfontmanager.h"
#include <QFile>
#include <QFileInfo>
#include "utils.h"
#include "../../input/sf/sf2header.h"
#include "../../input/sf/sf2sdtapart.h"
#include "../../input/sf/sf2pdtapart.h"

OutputSf::OutputSf() : AbstractOutput() {}

void OutputSf::processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int sf2Index, QMap<QString, QVariant> & options)
{
    _sm = sm;

    // Get the quality value if a compression of all uncompressed samples is required
    EltID id(elementSf2, sf2Index);
    bool isSf3 = _sm->get(id, champ_IFIL).sfVerValue.wMajor == 3;
    double qualityValue = -1.0;
    if (isSf3 && options.contains("quality"))
    {
        int quality = options["quality"].toInt();
        switch (quality)
        {
        case 0: qualityValue = 0.2; break;
        case 2: qualityValue = 1.0; break;
        default: qualityValue = 0.6; break;
        }
    }

    // Check that we don't save over another soundfont already open
    foreach (int i, _sm->getSiblings(id))
    {
        if (i != sf2Index && QString::compare(_sm->getQstr(EltID(elementSf2, i), champ_filenameInitial), fileName, Qt::CaseSensitive) == 0)
        {
            success = false;
            error = tr("Please close file before overriding it.");
            return;
        }
    }

    // Do we override the current file?
    id.indexSf2 = sf2Index;
    if (_sm->getQstr(id, champ_filenameForData) == fileName)
    {
        // Use a temporary file
        QString filenameTmp = fileName.left(fileName.length() - 4) + "_tmp";
        QString extension = isSf3 ? ".sf3" : ".sf2";
        if (QFile(filenameTmp + extension).exists())
        {
            int index = 1;
            while (QFile(filenameTmp + "-" + QString::number(index) + extension).exists())
                index++;
            filenameTmp = filenameTmp + "-" + QString::number(index);
        }
        filenameTmp += extension;

        // Save the file
        this->save(filenameTmp, success, error, sf2Index, qualityValue);
        if (!success)
            return;

        // Delete the initial file
        if (!QFile(fileName).remove())
        {
            error = tr("Couldn't delete file \"%1\".").arg(fileName);
            success = false;
            _sm->clearNewEditing();
            _sm->markAsSaved(sf2Index);
            return;
        }

        // Rename the tmp file
        if (!QFile(filenameTmp).rename(fileName))
        {
            error = tr("Couldn't rename file \"%1\".").arg(filenameTmp);
            success = false;
            _sm->clearNewEditing();
            _sm->markAsSaved(sf2Index);
            return;
        }

        // Update the source for data
        _sm->set(id, champ_filenameInitial, fileName);
        _sm->set(id, champ_filenameForData, fileName);
        id.typeElement = elementSmpl;
        foreach (int i, _sm->getSiblings(id))
        {
            id.indexElt = i;
            _sm->set(id, champ_filenameForData, fileName);
        }
    }
    else
    {
        // Just save the file
        this->save(fileName, success, error, sf2Index, qualityValue);
    }

    _sm->clearNewEditing();
    _sm->markAsSaved(sf2Index);
}

void OutputSf::save(QString fileName, bool &success, QString &error, int sf2Index, double qualityValue)
{
    EltID id(elementSf2, sf2Index, 0, 0, 0);
    bool isSf3 = _sm->get(id, champ_IFIL).sfVerValue.wMajor == 3;

    // Update the editing software
    _sm->set(id, champ_ISFT, QString("Polyphone"));

    // Prepare the data to write
    Sf2Header header;
    Sf2SdtaPart sdtaPart;
    Sf2PdtaPart pdtaPart;
    fillSf2(sf2Index, &header, &sdtaPart, &pdtaPart);
    header.prepareBeforeWritingData(&sdtaPart, &pdtaPart, isSf3, qualityValue);

    // Write everything
    QFile fi(fileName);
    if (!fi.open(QIODevice::WriteOnly))
    {
        success = false;
        error = tr("Cannot create file \"%1\"").arg(fileName);
        return;
    }
    QDataStream stream(&fi);
    stream << header << sdtaPart << pdtaPart;
    fi.close();

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

    // Save fileName
    id.typeElement = elementSf2;
    _sm->set(id, champ_filenameInitial, fileName);
    _sm->set(id, champ_filenameForData, fileName);
    id.typeElement = elementSmpl;
    foreach (int i, _sm->getSiblings(id))
    {
        id.indexElt = i;
        _sm->set(id, champ_filenameForData, fileName);
    }

    // Sample resolution
    id.typeElement = elementSf2;
    AttributeValue val = _sm->get(id, champ_wBpsSave);
    _sm->set(id, champ_wBpsInit, val);
    id.typeElement = elementSmpl;
    foreach (int i, _sm->getSiblings(id))
    {
        id.indexElt = i;
        if (_sm->get(id, champ_bpsFile).wValue != val.wValue)
            _sm->set(id, champ_bpsFile, val);
    }

    success = true;
    error = "";
}

void OutputSf::fillSf2(int sf2Index, Sf2Header * header, Sf2SdtaPart * sdtaPart, Sf2PdtaPart * pdtaPart)
{
    EltID id(elementSf2, sf2Index);
    QString strTmp;
    AttributeValue valTmp;
    quint32 dwTmp, nGen, nMod;
    QList<int> indexes;

    //////////////
    /// HEADER ///
    //////////////

    // Mandatory fields
    header->_versions["ifil"] = _sm->get(id, champ_IFIL).sfVerValue;
    header->_infos["isng"] = _sm->getQstr(id, champ_ISNG).left(255);
    if (header->_infos["isng"].isEmpty())
        header->_infos["isng"] = "Generic";
    header->_infos["INAM"] = _sm->getQstr(id, champ_name).left(255);

    // Optional fields
    strTmp = _sm->getQstr(id, champ_IROM);
    if (!strTmp.isEmpty())
        header->_infos["irom"] = strTmp.left(255);
    valTmp = _sm->get(id, champ_IVER);
    if (valTmp.sfVerValue.wMinor != 0 || valTmp.sfVerValue.wMajor != 0)
        header->_versions["iver"] = valTmp.sfVerValue;
    strTmp = _sm->getQstr(id, champ_ICRD);
    if (!strTmp.isEmpty())
        header->_infos["ICRD"] = strTmp.left(255);
    strTmp = _sm->getQstr(id, champ_IENG);
    if (!strTmp.isEmpty())
        header->_infos["IENG"] = strTmp.left(255);
    strTmp = _sm->getQstr(id, champ_IPRD);
    if (!strTmp.isEmpty())
        header->_infos["IPRD"] = strTmp.left(255);
    strTmp = _sm->getQstr(id, champ_ICOP);
    if (!strTmp.isEmpty())
        header->_infos["ICOP"] = strTmp.left(255);
    strTmp = _sm->getQstr(id, champ_ICMT);
    if (!strTmp.isEmpty())
    {
        // The comment can be encoded as UTF-8 or Latin1
        QByteArray commentData = strTmp.toLatin1();
        if (QString::fromLatin1(commentData) != strTmp)
        {
            // utf8 special characters
            commentData = strTmp.toUtf8();
            header->_infos["ICMT"] = QString::fromUtf8(commentData.left(65536));
        }
        else
            header->_infos["ICMT"] = QString::fromLatin1(commentData.left(65536));
    }
    strTmp = _sm->getQstr(id, champ_ISFT);
    if (!strTmp.isEmpty())
        header->_infos["ISFT"] = strTmp.left(255);

    ////////////
    /// SDTA ///
    ////////////

    // Samples
    id.typeElement = elementSmpl;
    indexes = _sm->getSiblings(id);
    for (int i = 0; i < indexes.count(); i++)
    {
        id.indexElt = indexes[i];
        sdtaPart->_sounds.append(_sm->getSound(id));
    }
    sdtaPart->_sample24bits = _sm->get(id, champ_wBpsSave).wValue == 24;

    ////////////
    /// PDTA ///
    ////////////

    SfIndexConverter converterSmpl(EltID(elementSmpl, id.indexSf2));
    SfIndexConverter converterInst(EltID(elementInst, id.indexSf2));

    // Presets
    id.typeElement = elementPrst;
    indexes = _sm->getSiblings(id);
    pdtaPart->_phdrs.resize(indexes.count() + 1);
    dwTmp = 0;
    for (int i = 0; i < indexes.count(); i++)
    {
        id.indexElt = indexes[i];

        // Name
        pdtaPart->_phdrs[i]._name = _sm->getQstr(id, champ_name).left(20);
        if (pdtaPart->_phdrs[i]._name.isEmpty())
            pdtaPart->_phdrs[i]._name = QString("preset %1").arg(i + 1);

        // Properties
        pdtaPart->_phdrs[i]._preset = _sm->get(id, champ_wPreset).wValue;
        pdtaPart->_phdrs[i]._bank = _sm->get(id, champ_wBank).wValue;
        pdtaPart->_phdrs[i]._library = _sm->get(id, champ_dwLibrary).dwValue;
        pdtaPart->_phdrs[i]._genre = _sm->get(id, champ_dwGenre).dwValue;
        pdtaPart->_phdrs[i]._morphology = _sm->get(id, champ_dwMorphology).dwValue;
        pdtaPart->_phdrs[i]._pBagIndex = dwTmp;

        // Update the bag index (global bag included)
        EltID idChild(elementPrstInst, id.indexSf2, id.indexElt);
        dwTmp += _sm->getSiblings(idChild).count() + 1;
    }
    pdtaPart->_phdrs[indexes.count()]._name = "EOP";
    pdtaPart->_phdrs[indexes.count()]._pBagIndex = dwTmp;

    // Preset bags
    pdtaPart->_pbags.resize(dwTmp + 1);
    dwTmp = 0;
    nGen = 0;
    nMod = 0;
    for (int i = 0; i < indexes.count(); i++)
    {
        id.indexElt = indexes[i];

        // Global division
        pdtaPart->_pbags[dwTmp]._genIndex = nGen;
        pdtaPart->_pbags[dwTmp]._modIndex = nMod;

        // Update the indexes
        EltID idCount(elementPrstGen, id.indexSf2, id.indexElt);
        nGen += _sm->getSiblings(idCount).count();
        idCount.typeElement = elementPrstMod;
        nMod += _sm->getSiblings(idCount).count();
        dwTmp++;

        EltID idChild(elementPrstInst, id.indexSf2, id.indexElt);
        foreach (int j, _sm->getSiblings(idChild))
        {
            idChild.indexElt2 = j;

            // Instrument division
            pdtaPart->_pbags[dwTmp]._genIndex = nGen;
            pdtaPart->_pbags[dwTmp]._modIndex = nMod;

            // Update the indexes
            EltID idCount(elementPrstInstGen, idChild.indexSf2, idChild.indexElt, idChild.indexElt2);
            nGen += _sm->getSiblings(idCount).count();
            idCount.typeElement = elementPrstInstMod;
            nMod += _sm->getSiblings(idCount).count();
            dwTmp++;
        }
    }
    pdtaPart->_pbags[pdtaPart->_pbags.count() - 1]._genIndex = nGen;
    pdtaPart->_pbags[pdtaPart->_pbags.count() - 1]._modIndex = nMod;

    // Preset mods
    pdtaPart->_pmods.resize(nMod + 1);
    dwTmp = 0;
    for (int i = 0; i < indexes.count(); i++)
    {
        id.indexElt = indexes[i];

        // Global division
        EltID idMod(elementPrstMod, id.indexSf2, id.indexElt);
        loadMods(idMod, pdtaPart->_pmods, dwTmp);

        // Instrument divisions
        EltID idChild(elementPrstInst, id.indexSf2, id.indexElt);
        idMod.typeElement = elementPrstInstMod;
        foreach (int j, _sm->getSiblings(idChild))
        {
            idMod.indexElt2 = j;
            loadMods(idMod, pdtaPart->_pmods, dwTmp);
        }
    }

    // Preset gens
    pdtaPart->_pgens.resize(nGen + 1);
    dwTmp = 0;
    for (int i = 0; i < indexes.count(); i++)
    {
        id.indexElt = indexes[i];

        // Global division
        EltID idGen(elementPrstGen, id.indexSf2, id.indexElt);
        loadGens(idGen, id, pdtaPart->_pgens, dwTmp, converterInst);

        // Instrument divisions
        EltID idChild(elementPrstInst, id.indexSf2, id.indexElt);
        idGen.typeElement = elementPrstInstGen;
        foreach (int j, _sm->getSiblings(idChild))
        {
            idChild.indexElt2 = j;
            idGen.indexElt2 = j;
            loadGens(idGen, idChild, pdtaPart->_pgens, dwTmp, converterInst);
        }
    }

    // Instruments
    id.typeElement = elementInst;
    indexes = _sm->getSiblings(id);
    pdtaPart->_insts.resize(indexes.count() + 1);
    dwTmp = 0;
    for (int i = 0; i < indexes.count(); i++)
    {
        id.indexElt = indexes[i];

        // Name
        pdtaPart->_insts[i]._name = _sm->getQstr(id, champ_name).left(20);
        if (pdtaPart->_insts[i]._name.isEmpty())
            pdtaPart->_insts[i]._name = QString("instrument %1").arg(i + 1);

        // Properties
        pdtaPart->_insts[i]._iBagIndex = dwTmp;

        // Update the bag index (global bag included)
        EltID idChild(elementInstSmpl, id.indexSf2, id.indexElt);
        dwTmp += _sm->getSiblings(idChild).count() + 1;
    }
    pdtaPart->_insts[indexes.count()]._name = "EOI";
    pdtaPart->_insts[indexes.count()]._iBagIndex = dwTmp;

    // Instrument bags
    pdtaPart->_ibags.resize(dwTmp + 1);
    dwTmp = 0;
    nGen = 0;
    nMod = 0;
    for (int i = 0; i < indexes.count(); i++)
    {
        id.indexElt = indexes[i];

        // Global division
        pdtaPart->_ibags[dwTmp]._genIndex = nGen;
        pdtaPart->_ibags[dwTmp]._modIndex = nMod;

        // Update the indexes
        EltID idCount(elementInstGen, id.indexSf2, id.indexElt);
        nGen += _sm->getSiblings(idCount).count();
        idCount.typeElement = elementInstMod;
        nMod += _sm->getSiblings(idCount).count();
        dwTmp++;

        EltID idChild(elementInstSmpl, id.indexSf2, id.indexElt);
        foreach (int j, _sm->getSiblings(idChild))
        {
            idChild.indexElt2 = j;

            // Instrument division
            pdtaPart->_ibags[dwTmp]._genIndex = nGen;
            pdtaPart->_ibags[dwTmp]._modIndex = nMod;

            // Update the indexes
            EltID idCount(elementInstSmplGen, idChild.indexSf2, idChild.indexElt, idChild.indexElt2);
            nGen += _sm->getSiblings(idCount).count();
            idCount.typeElement = elementInstSmplMod;
            nMod += _sm->getSiblings(idCount).count();
            dwTmp++;
        }
    }
    pdtaPart->_ibags[pdtaPart->_ibags.count() - 1]._genIndex = nGen;
    pdtaPart->_ibags[pdtaPart->_ibags.count() - 1]._modIndex = nMod;

    // Instruments mods
    pdtaPart->_imods.resize(nMod + 1);
    dwTmp = 0;
    for (int i = 0; i < indexes.count(); i++)
    {
        id.indexElt = indexes[i];

        // Global division
        EltID idMod(elementInstMod, id.indexSf2, id.indexElt);
        loadMods(idMod, pdtaPart->_imods, dwTmp);

        // Sample divisions
        EltID idChild(elementInstSmpl, id.indexSf2, id.indexElt);
        idMod.typeElement = elementInstSmplMod;
        foreach (int j, _sm->getSiblings(idChild))
        {
            idMod.indexElt2 = j;
            loadMods(idMod, pdtaPart->_imods, dwTmp);
        }
    }

    // Instrument gens
    pdtaPart->_igens.resize(nGen + 1);
    dwTmp = 0;
    for (int i = 0; i < indexes.count(); i++)
    {
        id.indexElt = indexes[i];

        // Global division
        EltID idGen(elementInstGen, id.indexSf2, id.indexElt);
        loadGens(idGen, id, pdtaPart->_igens, dwTmp, converterSmpl);

        // Sample divisions
        EltID idChild(elementInstSmpl, id.indexSf2, id.indexElt);
        idGen.typeElement = elementInstSmplGen;
        foreach (int j, _sm->getSiblings(idChild))
        {
            idChild.indexElt2 = j;
            idGen.indexElt2 = j;
            loadGens(idGen, idChild, pdtaPart->_igens, dwTmp, converterSmpl);
        }
    }

    // Samples
    id.typeElement = elementSmpl;
    indexes = _sm->getSiblings(id);
    pdtaPart->_shdrs.resize(indexes.count() + 1);
    for (int i = 0; i < indexes.count(); i++)
    {
        id.indexElt = indexes[i];

        // Name
        pdtaPart->_shdrs[i]._name = _sm->getQstr(id, champ_name).left(20);
        if (pdtaPart->_shdrs[i]._name.isEmpty())
            pdtaPart->_shdrs[i]._name = QString("sample %1").arg(i + 1);

        // Properties
        // dwStart, dwEnd, dwStartLoop, dwEndLoop => initiliazed later
        pdtaPart->_shdrs[i]._sampleRate = _sm->get(id, champ_dwSampleRate).dwValue;
        pdtaPart->_shdrs[i]._originalPitch = _sm->get(id, champ_byOriginalPitch).bValue;
        pdtaPart->_shdrs[i]._correction = _sm->get(id, champ_chPitchCorrection).cValue;
        pdtaPart->_shdrs[i]._wSampleLink = converterSmpl.getIndexOf(_sm->get(id, champ_wSampleLink).wValue, false);
        pdtaPart->_shdrs[i]._sfSampleType = _sm->get(id, champ_sfSampleType).sfLinkValue;
    }
    pdtaPart->_shdrs[indexes.count()]._name = "EOS";
}

void OutputSf::loadMods(EltID idMod, QList<Sf2PdtaPart_mod> &mods, quint32 &index)
{
    SfIndexConverter converterMod2(idMod);
    foreach (int i, _sm->getSiblings(idMod))
    {
        idMod.indexMod = i;
        mods[index]._sfModSrcOper = _sm->get(idMod, champ_sfModSrcOper).sfModValue;
        mods[index]._sfModDestOper = converterMod2.getIndexOf(_sm->get(idMod, champ_sfModDestOper).wValue, true);
        mods[index]._modAmount = _sm->get(idMod, champ_modAmount).wValue;
        mods[index]._sfModAmtSrcOper = _sm->get(idMod, champ_sfModAmtSrcOper).sfModValue;
        mods[index]._sfModTransOper = _sm->get(idMod, champ_sfModTransOper).wValue;
        index++;
    }
}

void OutputSf::loadGens(EltID idGen, EltID idDiv, QList<Sf2PdtaPart_gen> &gens, quint32 &index, SfIndexConverter &indexConverter)
{
    QList<int> attributes = _sm->getSiblings(idGen);

    // First the key range, then the velocity range
    if (attributes.contains((int)champ_velRange))
    {
        attributes.removeAll(champ_velRange);
        attributes.prepend(champ_velRange);
    }
    if (attributes.contains((int)champ_keyRange))
    {
        attributes.removeAll(champ_keyRange);
        attributes.prepend(champ_keyRange);
    }

    // At the end the sample or instrument id
    if (attributes.contains((int)champ_sampleID))
    {
        attributes.removeAll(champ_sampleID);
        attributes.append(champ_sampleID);
    }
    if (attributes.contains((int)champ_instrument))
    {
        attributes.removeAll(champ_instrument);
        attributes.append(champ_instrument);
    }

    foreach (int attribute, attributes)
    {
        gens[index]._sfGenOper = attribute;
        gens[index]._genAmount =
            (attribute == champ_sampleID || attribute == champ_instrument) ?
                indexConverter.getIndexOf(_sm->get(idDiv, (AttributeType)attribute).wValue, false) :
                _sm->get(idDiv, (AttributeType)attribute).wValue;
        index++;
    }
}

void OutputSf::convertTo16bit(QVector<float> dataSrc, QByteArray &dataDest)
{
    const float * data = dataSrc.constData();
    int length = dataSrc.size();

    dataDest.resize(2 * length);
    qint16 * data16 = reinterpret_cast<qint16 *>(dataDest.data());

    for (int i = 0; i < length; i++)
        data16[i] = (Utils::floatToInt24(data[i]) >> 8);
}

void OutputSf::convertTo24bit(QVector<float> dataSrc, QByteArray &dataDest)
{
    const float * data = dataSrc.constData();
    int length = dataSrc.size();

    // Get only the last 8 bits of the 24 bits value
    dataDest.resize(length);
    char * dataChar = dataDest.data();
    for (int i = 0; i < length; i++)
        dataChar[i] = (Utils::floatToInt24(data[i]) & 0xFF);
}

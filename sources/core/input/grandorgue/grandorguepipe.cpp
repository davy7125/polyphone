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

#include "grandorguepipe.h"
#include "grandorguedatathrough.h"
#include "soundfontmanager.h"
#include <QFile>
#include <QDebug>

GrandOrguePipe::GrandOrguePipe(QString rootDir, GrandOrgueDataThrough * godt) :
    _rootDir(rootDir),
    _godt(godt),
    _relativePath(""),
    _error(""),
    _gain(0),
    _tuning(0)
{

}

void GrandOrguePipe::readData(QString key, QString value)
{
    if (key == "gain")
    {
        bool ok = false;
        _gain = value.toDouble(&ok);
        if (!ok)
        {
            qDebug() << "couldn't read pipe gain:" << value;
            _gain = 0;
        }
    }
    else if (key == "amplitudelevel")
    {
        bool ok = false;
        int amplitude = value.toInt(&ok);
        if (ok)
            this->mergeAmplitude(amplitude);
        else
            qDebug() << "couldn't read pipe amplitude:" << value;
    }
    else if (key == "pitchtuning")
    {
        bool ok = false;
        float fValue = value.toFloat(&ok);
        if (ok)
        {
            if (fValue < -0.5)
                _tuning = static_cast<int>(fValue - 0.5f);
            else if (fValue > 0.5)
                _tuning = static_cast<int>(fValue + 0.5f);
            else
                _tuning = 0;
        }
        else
        {
            qDebug() << "couldn't read pipe tuning:" << value;
            _tuning = 0;
        }
    }
    else if (key == "#")
    {
        // TODO: read a reference to another pipe such as:
        // Pipe002=REF:001:001:002

        _relativePath = value;
        if (!QFile::exists(_rootDir + "/" + _relativePath))
        {
            qDebug() << "couldn't find file:" << _rootDir + "/" + _relativePath;
            _relativePath = "";
        }
    }
    else
        _properties[key] = value;
}

void GrandOrguePipe::mergeAmplitude(int amplitude)
{
    // Translate into a gain in dB
    double coef = 0.01 * static_cast<double>(amplitude);
    _gain += 20. * log10(coef);
}

bool GrandOrguePipe::isValid()
{
    return !_relativePath.isEmpty() && _error.isEmpty();
}

void GrandOrguePipe::process(EltID parent, int key)
{
    if (!this->isValid())
        return;

    // ATTACK

    QList<int> sampleIds = this->getSampleIds(parent.indexSf2, _relativePath);
    SoundfontManager * sm = SoundfontManager::getInstance();
    for (int i = 0; i < sampleIds.size(); i++)
    {
        // Create one InstSmpl
        EltID idInstSmpl(elementInstSmpl, parent.indexSf2, parent.indexElt);
        idInstSmpl.indexElt2 = sm->add(idInstSmpl);

        // Link to the sample
        AttributeValue val;
        val.wValue = sampleIds[i];
        sm->set(idInstSmpl, champ_sampleID, val);

        // Pan
        if (sampleIds.size() == 2)
        {
            val.shValue = (i == 0 ? -500 : 500);
            sm->set(idInstSmpl, champ_pan, val);
        }

        // Attenuation
        val.wValue = static_cast<quint16>(10. * (_gain - _godt->getMaxGain()) / DB_SF2_TO_REAL_DB + 0.5);
        sm->set(idInstSmpl, champ_initialAttenuation, val);

        // Tuning
        int fineTune = _tuning % 100;
        int coarseTune = _tuning / 100;
        if (fineTune > 50)
        {
            fineTune -= 100;
            coarseTune += 1;
        }
        else if (fineTune < -50)
        {
            fineTune += 100;
            coarseTune -= 1;
        }
        val.shValue = fineTune;
        sm->set(idInstSmpl, champ_fineTune, val);
        val.shValue = coarseTune;
        sm->set(idInstSmpl, champ_coarseTune, val);

        // Loop + end?
        if (_properties.contains("loadrelease") && _properties["loadrelease"].toLower() == "y")
        {
            val.wValue = 3;
            sm->set(idInstSmpl, champ_sampleModes, val);
        }

        // Rootkey and keyrange
        val.wValue = key;
        sm->set(idInstSmpl, champ_overridingRootKey, val);
        val.rValue.byLo = key;
        val.rValue.byHi = key;
        sm->set(idInstSmpl, champ_keyRange, val);

        // Short release time (0.1s)
        val.shValue = -3980;
        sm->set(idInstSmpl, champ_releaseVolEnv, val);
    }

    // RELEASE

    QString releaseFilePath = getReleaseFilePath();
    if (!releaseFilePath.isEmpty())
    {
        QList<int> sampleIds = this->getSampleIds(parent.indexSf2, releaseFilePath);
        SoundfontManager * sm = SoundfontManager::getInstance();
        for (int i = 0; i < sampleIds.size(); i++)
        {
            // Create one InstSmpl
            EltID idInstSmpl(elementInstSmpl, parent.indexSf2, parent.indexElt);
            idInstSmpl.indexElt2 = sm->add(idInstSmpl);

            // Link to the sample
            AttributeValue val;
            val.wValue = sampleIds[i];
            sm->set(idInstSmpl, champ_sampleID, val);

            // Pan
            if (sampleIds.size() == 2)
            {
                val.shValue = (i == 0 ? -500 : 500);
                sm->set(idInstSmpl, champ_pan, val);
            }

            // Attenuation
            val.wValue = static_cast<quint16>(10. * (_gain - _godt->getMaxGain()) / DB_SF2_TO_REAL_DB + 0.5);
            sm->set(idInstSmpl, champ_initialAttenuation, val);

            // Tuning
            int fineTune = _tuning % 100;
            int coarseTune = _tuning / 100;
            if (fineTune > 50)
            {
                fineTune -= 100;
                coarseTune += 1;
            }
            else if (fineTune < -50)
            {
                fineTune += 100;
                coarseTune -= 1;
            }
            val.shValue = fineTune;
            sm->set(idInstSmpl, champ_fineTune, val);
            val.shValue = coarseTune;
            sm->set(idInstSmpl, champ_coarseTune, val);

            // Release mode
            val.wValue = 2;
            sm->set(idInstSmpl, champ_sampleModes, val);

            // Rootkey and keyrange
            val.wValue = key;
            sm->set(idInstSmpl, champ_overridingRootKey, val);
            val.rValue.byLo = key;
            val.rValue.byHi = key;
            sm->set(idInstSmpl, champ_keyRange, val);

            // Long release time
            val.shValue = 8000;
            sm->set(idInstSmpl, champ_releaseVolEnv, val);

            // Short attack time (0.1s)
            val.shValue = -3980;
            sm->set(idInstSmpl, champ_attackVolEnv, val);
        }
    }
}

QList<int> GrandOrguePipe::getSampleIds(int sf2Id, QString relativeFilePath)
{
    // Samples already loaded?
    QString filePath = _rootDir + "/" + relativeFilePath;
    QList<int> sampleIndex = _godt->getSf2SmplId(filePath);
    if (!sampleIndex.empty())
        return sampleIndex;

    // Otherwise load a new sample
    Sound sound;
    if (!sound.setFileName(filePath, false))
        _error = sound.getError();
    quint32 nChannels = sound.getUInt32(champ_wChannels);
    QString name = QFileInfo(filePath).completeBaseName();
    if (name.length() < 16)
    {
        // Include the last characters of the relative path with name
        QString relPath = relativeFilePath.left(relativeFilePath.lastIndexOf("."));
        int pos = relPath.lastIndexOf("./");
        if (pos >= 0)
            relPath = relPath.mid(relPath.lastIndexOf("./") + 2);
        name = relPath.right(19);
    }
    QString name2 = name;

    // Possibly adapt the name
    int suffixNumber = 0;
    if (nChannels == 2)
    {
        while ((_godt->sampleNameExists(getName(name, 20, suffixNumber, "L")) ||
                _godt->sampleNameExists(getName(name, 20, suffixNumber, "R"))) &&
               suffixNumber < 100)
        {
            suffixNumber++;
        }
        name2 = getName(name, 20, suffixNumber, "L");
        name = getName(name, 20, suffixNumber, "R");
        _godt->storeSampleName(name);
        _godt->storeSampleName(name2);
    }
    else
    {
        while (_godt->sampleNameExists(getName(name, 20, suffixNumber)) && suffixNumber < 100)
        {
            suffixNumber++;
        }
        name = getName(name, 20, suffixNumber);
        _godt->storeSampleName(name);
    }

    // Create a new sample for each channel
    SoundfontManager * sm = SoundfontManager::getInstance();
    EltID idElt(elementSmpl, sf2Id);
    AttributeValue val;
    bool hasLoop = false;
    for (quint32 numChannel = 0; numChannel < nChannels; numChannel++)
    {
        idElt.indexElt = sm->add(idElt);
        sampleIndex << idElt.indexElt;
        if (nChannels == 2)
        {
            if (numChannel == 0)
            {
                // Gauche
                sm->set(idElt, champ_name, name2);
                val.wValue = idElt.indexElt + 1;
                sm->set(idElt, champ_wSampleLink, val);
                val.sfLinkValue = leftSample;
                sm->set(idElt, champ_sfSampleType, val);
            }
            else
            {
                // Droite
                sm->set(idElt, champ_name, name);
                val.wValue = idElt.indexElt - 1;
                sm->set(idElt, champ_wSampleLink, val);
                val.sfLinkValue = rightSample;
                sm->set(idElt, champ_sfSampleType, val);
            }
        }
        else
        {
            sm->set(idElt, champ_name, name);
            val.wValue = 0;
            sm->set(idElt, champ_wSampleLink, val);
            val.sfLinkValue = monoSample;
            sm->set(idElt, champ_sfSampleType, val);
        }
        sm->set(idElt, champ_filenameForData, filePath);
        val.dwValue = sound.getUInt32(champ_dwStart16);
        sm->set(idElt, champ_dwStart16, val);
        val.dwValue = sound.getUInt32(champ_dwStart24);
        sm->set(idElt, champ_dwStart24, val);
        val.wValue = numChannel;
        sm->set(idElt, champ_wChannel, val);
        val.dwValue = sound.getUInt32(champ_dwLength);
        sm->set(idElt, champ_dwLength, val);
        val.dwValue = sound.getUInt32(champ_dwSampleRate);
        sm->set(idElt, champ_dwSampleRate, val);
        val.dwValue = sound.getUInt32(champ_dwStartLoop);
        quint32 startLoop = val.dwValue;
        sm->set(idElt, champ_dwStartLoop, val);
        val.dwValue = sound.getUInt32(champ_dwEndLoop);
        hasLoop |= (startLoop != val.dwValue);
        sm->set(idElt, champ_dwEndLoop, val);
        val.bValue = (quint8)sound.getUInt32(champ_byOriginalPitch);
        sm->set(idElt, champ_byOriginalPitch, val);
        val.cValue = (char)sound.getInt32(champ_chPitchCorrection);
        sm->set(idElt, champ_chPitchCorrection, val);
    }

    _godt->setSf2SmplId(filePath, sampleIndex, hasLoop);
    return sampleIndex;
}

QString GrandOrguePipe::getName(QString name, int maxCharacters, int suffixNumber, QString suffix)
{
    int suffixSize = suffix.size();

    // Cas où la taille du suffix est supérieure au nombre de caractères max
    if (suffixSize > maxCharacters)
        return name.left(maxCharacters);

    // Cas où un numéro n'est pas nécessaire
    if (suffixNumber == 0)
        return name.left(maxCharacters - suffixSize) + suffix;

    QString suffixNum = QString::number(suffixNumber);
    int suffixNumSize = suffixNum.length() + 1;

    // Cas où le suffix numérique est trop grand
    if (suffixNumSize > 3 || maxCharacters - suffixSize < suffixNumSize)
        return name.left(maxCharacters - suffixSize) + suffix;

    return name.left(maxCharacters - suffixNumSize - suffixSize) + suffix + "-" + suffixNum;
}

QString GrandOrguePipe::getReleaseFilePath()
{
    if (!_properties.contains("release001"))
        return "";

    QString relativeFilePath = _properties["release001"];
    if (!QFile::exists(_rootDir + "/" + relativeFilePath))
    {
        qDebug() << "couldn't find release file:" << _rootDir + "/" + relativeFilePath;
        return "";
    }

    return relativeFilePath;
}

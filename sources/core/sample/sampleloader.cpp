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

#include "sampleloader.h"
#include "soundfontmanager.h"
#include "sampletrimming.h"
#include "sound.h"
#include <QFileInfo>

SampleLoader::SampleLoader() :
    _sm(SoundfontManager::getInstance()),
    _trimStart(false),
    _trimEnd(false)
{

}

void SampleLoader::setProcessing(bool trimStart, bool trimEnd)
{
    _trimStart = trimStart;
    _trimEnd = trimEnd;
}

// Replace status
// -1: duplicate all
//  0: duplicate
//  1: replace
//  2: replace all
//  3: ignore
//  4: ignore all

IdList SampleLoader::load(QString path, int numSf2, int *replace)
{
    EltID id(elementSmpl, numSf2);
    IdList loadedSmpl;

    loading(path);
    QFileInfo qFileInfo(path);

    // Get information about a sample
    Sound sound;
    if (!sound.setFileName(path))
    {
        showError(sound.getError());
        return loadedSmpl;
    }
    unsigned int nChannels = sound.getUInt32(champ_wChannels);

    // Add a sample
    SoundfontManager * sm = SoundfontManager::getInstance();
    AttributeValue val;
    QString nom = qFileInfo.completeBaseName();

    // Replacement?
    int indexL = -1;
    int indexR = -1;
    QString qStr3 = "";
    if (*replace != -1)
    {
        foreach (int j, sm->getSiblings(id))
        {
            id.indexElt = j;
            if (nChannels == 2)
            {
                if (sm->getQstr(id, champ_name).compare(nom.left(19).append("L")) == 0)
                {
                    indexL = j;
                    qStr3 = QObject::tr("Sample \"%1L\" already exists.<br />Replace?").arg(nom.left(19));
                }
                else if (sm->getQstr(id, champ_name).compare(nom.left(19).append("R")) == 0)
                {
                    indexR = j;
                    qStr3 = QObject::tr("Sample \"%1R\" already exists.<br />Replace?").arg(nom.left(19));
                }
            }
            else
            {
                if (sm->getQstr(id, champ_name).compare(nom.left(20)) == 0)
                {
                    indexL = j;
                    qStr3 = QObject::tr("Sample \"%1\" already exists.<br />Replace?").arg(nom.left(20));
                }
            }
        }
        if (*replace != 2 && *replace != 4 && (indexL != -1 || indexR != -1))
        {
            // Replacement?
            *replace = askReplacement(qStr3, *replace);
        }
    }

    // Adjust the name, if already used
    if (*replace == 0 || *replace == -1)
    {
        QStringList listSampleName;
        foreach (int j, sm->getSiblings(id))
        {
            id.indexElt = j;
            listSampleName << sm->getQstr(id, champ_name);
        }

        int suffixNumber = 0;
        if (nChannels == 1)
        {
            while (listSampleName.contains(getName(nom, 20, suffixNumber), Qt::CaseInsensitive) && suffixNumber < 100)
            {
                suffixNumber++;
            }
            nom = getName(nom, 20, suffixNumber);
        }
        else
        {
            while ((listSampleName.contains(getName(nom, 19, suffixNumber) + "L", Qt::CaseInsensitive) ||
                    listSampleName.contains(getName(nom, 19, suffixNumber) + "R", Qt::CaseInsensitive)) &&
                   suffixNumber < 100)
            {
                suffixNumber++;
            }
            nom = getName(nom, 19, suffixNumber);
        }
    }

    for (unsigned int j = 0; j < nChannels; j++)
    {
        if (*replace < 3 || (nChannels == 2 && j == 0 && indexL == -1) ||
                (nChannels == 2 && j == 1 && indexR == -1) ||
                (nChannels == 1 && indexL == -1)) // Don't ignore
        {
            if (((nChannels == 2 && j == 0 && indexL != -1) ||
                 (nChannels == 2 && j == 1 && indexR != -1) ||
                 (nChannels == 1 && indexL != -1)) && (*replace == 2 || *replace == 1))
            {
                if ((nChannels == 2 && j == 0 && indexL != -1) || (nChannels == 1 && indexL != -1))
                    id.indexElt = indexL;
                else
                    id.indexElt = indexR;

                // Update data
                val.wValue = static_cast<quint16>(j);
                sound.set(champ_wChannel, val);
                sm->setData(id, sound.getDataFloat(true));
            }
            else
            {
                // Add a sample
                id.indexElt = sm->add(id);
                if (nChannels == 2)
                {
                    if (j == 0)
                    {
                        // Left
                        sm->set(id, champ_name, nom.left(19).append("L"));
                        val.wValue = static_cast<quint16>(id.indexElt) + 1;
                        sm->set(id, champ_wSampleLink, val);
                        val.sfLinkValue = leftSample;
                        sm->set(id, champ_sfSampleType, val);
                    }
                    else
                    {
                        // Right
                        sm->set(id, champ_name, nom.left(19).append("R"));
                        val.wValue = static_cast<quint16>(id.indexElt) - 1;
                        sm->set(id, champ_wSampleLink, val);
                        val.sfLinkValue = rightSample;
                        sm->set(id, champ_sfSampleType, val);
                    }
                }
                else
                {
                    sm->set(id, champ_name, QString(nom.left(20)));
                    val.wValue = 0;
                    sm->set(id, champ_wSampleLink, val);
                    val.sfLinkValue = monoSample;
                    sm->set(id, champ_sfSampleType, val);
                }
                sm->set(id, champ_filenameForData, path);
                val.dwValue = sound.getUInt32(champ_dwStart16);
                sm->set(id, champ_dwStart16, val);
                val.dwValue = sound.getUInt32(champ_dwStart24);
                sm->set(id, champ_dwStart24, val);
            }

            loadedSmpl << id;

            // Configuration du sample
            val.wValue = static_cast<quint16>(j);
            sm->set(id, champ_wChannel, val);
            val.dwValue = sound.getUInt32(champ_dwLength);
            sm->set(id, champ_dwLength, val);
            val.dwValue = sound.getUInt32(champ_dwSampleRate);
            sm->set(id, champ_dwSampleRate, val);
            val.dwValue = sound.getUInt32(champ_dwStartLoop);
            sm->set(id, champ_dwStartLoop, val);
            val.dwValue = sound.getUInt32(champ_dwEndLoop);
            sm->set(id, champ_dwEndLoop, val);
            val.bValue = static_cast<quint8>(sound.getUInt32(champ_byOriginalPitch));
            sm->set(id, champ_byOriginalPitch, val);
            val.cValue = static_cast<char>(sound.getInt32(champ_chPitchCorrection));
            sm->set(id, champ_chPitchCorrection, val);
        }
    }

    // Automatically remove the blank at the end and at the start? (the noise before the sound is a reference for the
    // end)
    if (_trimEnd)
        SampleTrimming::trimEnd(_sm, loadedSmpl);
    if (_trimStart)
        SampleTrimming::trimStart(_sm, loadedSmpl);

    return loadedSmpl;
}

QString SampleLoader::getName(QString name, int maxCharacters, int suffixNumber)
{
    if (suffixNumber == 0)
        return name.left(maxCharacters);
    QString suffix = QString::number(suffixNumber);
    int suffixSize = suffix.length();
    if (suffixSize > 3 || maxCharacters < suffixSize + 1)
        return name.left(maxCharacters);
    return name.left(maxCharacters - suffixSize - 1) + "-" + suffix;
}

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

#include "sound.h"
#include <QMessageBox>
#include <QLibrary>
#include <QCoreApplication>
#include <QFileInfo>
#include <QApplication>
#include "qmath.h"
#include <QFile>
#include "contextmanager.h"
#include "stream_decoder.h"
#include "sampleutils.h"
#include "samplereader.h"
#include "samplereaderfactory.h"

Sound::Sound(QString filename, bool tryFindRootkey) :
    _reader(nullptr)
{
    // Initialize data
    _sm24.clear();
    _smpl.clear();
    if (!filename.isEmpty())
        this->setFileName(filename, tryFindRootkey);
}

Sound::~Sound()
{
    delete _reader;
}

void Sound::setFileName(QString qStr, bool tryFindRootKey)
{
    _fileName = qStr;

    // Initialize the reader
    if (_reader != nullptr)
        delete _reader;
    _reader = SampleReaderFactory::getSampleReader(_fileName);

    // Get information about the sample
    if (_reader != nullptr)
    {
        SampleReader::SampleReaderResult result = _reader->getInfo(_info);
        switch (result)
        {
        case SampleReader::FILE_CORRUPT:
            QMessageBox::warning(QApplication::activeWindow(), QObject::trUtf8("Warning"),
                                 QObject::trUtf8("Corrupted file: \"%1\"").arg(_fileName));
            break;
        case SampleReader::FILE_NOT_FOUND:
            QMessageBox::warning(QApplication::activeWindow(), QObject::trUtf8("Warning"),
                                 QObject::trUtf8("Cannot find file \"%1\"").arg(_fileName));
            break;
        case SampleReader::FILE_NOT_READABLE:
            QMessageBox::warning(QApplication::activeWindow(), QObject::trUtf8("Warning"),
                                 QObject::trUtf8("Cannot open file \"%1\"").arg(_fileName));
            break;
        default:
            break;
        }

        // Add default start and end loop if not specified
        if (_info.dwStartLoop == 0 && _info.dwEndLoop == 0 && _info.dwLength != 0)
            _info.dwEndLoop = _info.dwLength - 1;
    }
    else
        _info.reset();

    if (!_info.pitchDefined && tryFindRootKey)
        determineRootKey();
}

QByteArray Sound::getData(quint16 wBps)
{
    // Copie des données dans data, résolution wBps
    // wBps = 16 : chargement smpl, 16 bits de poids fort
    // wBps =  8 : chargement sm24, 8 bits suivant le 16 de poids fort
    // wBps = 24 : chargement 24 bits de poids fort
    // wBps = 32 : chargement en 32 bits

    if (_reader != nullptr)
    {
        // Possibly load 16 bits
        if (_smpl.isEmpty() && wBps != 8)
            _reader->getData16(_smpl);

        // Possibly load the 8 extra bits
        if (_sm24.isEmpty() && wBps != 16)
        {
            if (_info.wBpsFile > 16)
                _reader->getExtraData24(_sm24);
            else
            {
                // Fill with 0
                _sm24.resize(static_cast<int>(_info.dwLength));
                _sm24.fill(0);
            }
        }
    }

    // Check the length and possibly append '0'
    if (wBps != 8)
    {
        unsigned int length = static_cast<unsigned int>(_smpl.size());
        if (length != _info.dwLength * 2)
        {
            _smpl.resize(static_cast<int>(_info.dwLength) * 2);
            for (unsigned int i = length; i < _info.dwLength * 2; i++)
                _smpl[i] = 0;
        }
    }
    if (wBps != 16)
    {
        unsigned int length = static_cast<unsigned int>(_sm24.size());
        if (length != _info.dwLength)
        {
            _sm24.resize(static_cast<int>(_info.dwLength));
            for (unsigned int i = length; i < _info.dwLength; i++)
                _sm24[i] = 0;
        }
    }

    // Compute the result
    QByteArray baRet;
    switch (wBps)
    {
    case 8:
        // Load the 8 extra bits
        baRet = this->_sm24;
        break;
    case 16:
        // Load 16 bits
        baRet = this->_smpl;
        break;
    case 24:
        // Concat 16 bits and 8 bits
        baRet.resize(static_cast<int>(_info.dwLength) * 3);
        {
            char * cDest = baRet.data();
            char * cFrom = _smpl.data();
            char * cFrom24 = _sm24.data();
            unsigned int len = _info.dwLength;
            for (unsigned int i = 0; i < len; i++)
            {
                cDest[3*i] = cFrom24[i];
                cDest[3*i+1] = cFrom[2*i];
                cDest[3*i+2] = cFrom[2*i+1];
            }
        }
        break;
    case 32:
        // Concat 16 bits, 8 bits, and null values
        baRet.resize(static_cast<int>(_info.dwLength) * 4);
        {
            char * cDest = baRet.data();
            char * cFrom = _smpl.data();
            char * cFrom24 = _sm24.data();
            unsigned int len = _info.dwLength;
            for (unsigned int i = 0; i < len; i++)
            {
                cDest[4*i] = 0;
                cDest[4*i+1] = cFrom24[i];
                cDest[4*i+2] = cFrom[2*i];
                cDest[4*i+3] = cFrom[2*i+1];
            }
        }
        break;
    default:
        QMessageBox::warning(QApplication::activeWindow(), QObject::trUtf8("Warning"), "Error in Sound::getData.");
    }

    return baRet;
}

quint32 Sound::getUInt32(AttributeType champ)
{
    quint32 result = 0;
    switch (champ)
    {
    case champ_dwStart16:
        result = _info.dwStart;
        break;
    case champ_dwStart24:
        result = _info.dwStart2;
        break;
    case champ_dwLength:
        result = _info.dwLength;
        break;
    case champ_dwStartLoop:
        result = _info.dwStartLoop;
        break;
    case champ_dwEndLoop:
        result = _info.dwEndLoop;
        break;
    case champ_dwSampleRate:
        result = _info.dwSampleRate;
        break;
    case champ_bpsFile:
        result = _info.wBpsFile;
        break;
    case champ_wChannel:
        result = _info.wChannel;
        break;
    case champ_wChannels:
        result = _info.wChannels;
        break;
    case champ_byOriginalPitch:
        result = _info.dwRootKey;
        break;
    case champ_pitchDefined:
        result = (_info.pitchDefined ? 1 : 0);
        break;
    default:
        break;
    }
    return result;
}

qint32 Sound::getInt32(AttributeType champ)
{
    qint32 result = 0;
    switch (champ)
    {
    case champ_chPitchCorrection:
        result = _info.iFineTune;
        break;
    default:
        break;
    }
    return result;
}

void Sound::setData(QByteArray data, quint16 wBps)
{
    if (wBps == 8)
    {
        // Remplacement des données 17-24 bits
        this->_sm24 = data;
    }
    else if (wBps == 16)
    {
        // Remplacement des données 16 bits
        this->_smpl = data;
    }
    else
        QMessageBox::warning(QApplication::activeWindow(), "warning", "In Sound::setData, forbidden operation");
}

void Sound::set(AttributeType champ, AttributeValue value)
{
    switch (champ)
    {
    case champ_dwStart16:
        // modification du départ
        _info.dwStart = value.dwValue;
        break;
    case champ_dwStart24:
        // modification du départ
        _info.dwStart2 = value.dwValue;
        break;
    case champ_dwLength:
        // modification de la longueur
        _info.dwLength = value.dwValue;
        break;
    case champ_dwStartLoop:
        // modification du début de la boucle
        _info.dwStartLoop = value.dwValue;
        break;
    case champ_dwEndLoop:
        // modification de la fin de la boucle
        _info.dwEndLoop = value.dwValue;
        break;
    case champ_dwSampleRate:
        // modification de l'échantillonnage
        _info.dwSampleRate = value.dwValue;
        break;
    case champ_wChannel:
        // modification du canal utilisé
        _info.wChannel = value.wValue;
        break;
    case champ_wChannels:
        // modification du nombre de canaux
        _info.wChannels = value.wValue;
        break;
    case champ_bpsFile:
        // modification de la résolution
        _info.wBpsFile = value.wValue;
        if (value.wValue < 24)
            this->_sm24.clear();
        break;
    case champ_byOriginalPitch:
        // Modification de la note en demi tons
        _info.dwRootKey = value.bValue;
        break;
    case champ_chPitchCorrection:
        // Modification de la note en centième de ton
        _info.iFineTune = value.cValue;
        break;
    default:
        break;
    }
}

void Sound::setRam(bool ram)
{
    if (ram)
    {
        // Load the 16 strong bits
        if (this->_smpl.isEmpty())
            this->_smpl = this->getData(16);

        // Load the extra 8 bits
        if (this->_sm24.isEmpty() && _info.wBpsFile >= 24)
            this->_sm24 = this->getData(8);
    }
    else
    {
        // Clear data
        this->_smpl.clear();
        this->_sm24.clear();
    }
}

void Sound::determineRootKey()
{
    // Try to find the root key with the help of the file name
    QFileInfo fileInfo(_fileName);
    QString nameNoExtension = fileInfo.baseName();

    unsigned int key = 0;
    bool keyFound = false;
    for (unsigned int i = 0; i <= 127; i++)
    {
        // First search the textual name of the key
        QString nomNote = ContextManager::keyName()->getKeyName(i, true);

        // Recherche de la note dans le nom de fichier
        if (nameNoExtension.toUpper().contains(nomNote))
        {
            key = i;
            keyFound = true;
        }
    }

    if (!keyFound)
    {
        // Then search the numeric name of the key
        QString name = nameNoExtension;
        QStringList listeNum = name.replace(QRegExp("[^0-9]"), "-").split("-", QString::SkipEmptyParts);
        if (listeNum.size())
        {
            // 0n étudie le dernier
            bool ok;
            unsigned int numNote = listeNum.last().toUInt(&ok);
            if (ok && numNote < 128)
                key = numNote;
        }
    }

    if (keyFound)
    {
        _info.dwRootKey = key;
        _info.pitchDefined = true;
    }
}

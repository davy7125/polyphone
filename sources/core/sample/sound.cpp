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
#include "FLAC/stream_decoder.h"
#include "sampleutils.h"
#include "samplereader.h"
#include "samplereaderfactory.h"

Sound::Sound(QString filename, bool tryFindRootkey) :
    _reader(nullptr)
{
    // Initialize data
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
            QMessageBox::warning(QApplication::activeWindow(), QObject::tr("Warning"),
                                 QObject::tr("Corrupted file: \"%1\"").arg(_fileName));
            break;
        case SampleReader::FILE_NOT_FOUND:
            QMessageBox::warning(QApplication::activeWindow(), QObject::tr("Warning"),
                                 QObject::tr("Cannot find file \"%1\"").arg(_fileName));
            break;
        case SampleReader::FILE_NOT_READABLE:
            QMessageBox::warning(QApplication::activeWindow(), QObject::tr("Warning"),
                                 QObject::tr("Cannot open file \"%1\"").arg(_fileName));
            break;
        default:
            break;
        }

        // Add default start and end loop if not specified
        if (_info.loops.empty())
            _info.loops << QPair<quint32, quint32>(0, _info.dwLength > 0 ? _info.dwLength - 1 : 0);
    }
    else
        _info.reset();

    if (!_info.pitchDefined && tryFindRootKey)
        determineRootKey();
}

QVector<float> Sound::getData(bool forceReload)
{
    if (_reader != nullptr)
    {
        if (forceReload)
        {
            _smpl.clear();
            _reader->getInfo(_info);
        }

        if (_smpl.isEmpty())
            _reader->getData(_smpl);
    }

    return _smpl;
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
        result = _info.loops.empty() ? 0 : _info.loops[0].first;
        break;
    case champ_dwEndLoop:
        result = _info.loops.empty() ? 0 : _info.loops[0].second;
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
        result = -_info.iFineTune; // The correction is the opposite value
        break;
    default:
        break;
    }
    return result;
}

void Sound::setData(QVector<float> data)
{
    _smpl = data;
    _info.dwLength = data.size();
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
        if (_info.loops.empty())
            _info.loops << QPair<quint32, quint32>(value.dwValue, value.dwValue);
        else
            _info.loops[0].first = value.dwValue;
        break;
    case champ_dwEndLoop:
        // modification de la fin de la boucle
        if (_info.loops.empty())
            _info.loops << QPair<quint32, quint32>(value.dwValue, value.dwValue);
        else
            _info.loops[0].second = value.dwValue;
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
        break;
    case champ_byOriginalPitch:
        // Modification de la note en demi tons
        _info.dwRootKey = value.bValue;
        break;
    case champ_chPitchCorrection:
        // Modification de la note en centième de ton
        _info.iFineTune = -value.cValue; // Fine tune is the opposite of correction
        break;
    default:
        break;
    }
}

void Sound::setRam(bool ram)
{
    if (ram)
    {
        if (_smpl.isEmpty())
            _smpl = this->getData();
    }
    else
    {
        // Clear data
        _smpl.clear();
    }
}

void Sound::determineRootKey()
{
    // Try to find the root key with the help of the file name
    QFileInfo fileInfo(_fileName);
    QString nameNoExtension = fileInfo.baseName().toLower();

    // First attempt: find a note name (standard taken being C4 = 60)
    QRegularExpression keyNameRegex("([a-g])([#b]?)(-?[0-9]+)");
    bool keyFound = false;
    unsigned int key = 0;
    QRegularExpressionMatch match = keyNameRegex.match(nameNoExtension);
    if (match.hasMatch())
    {
        switch (match.captured(1)[0].toLatin1())
        {
        case 'c':
            key = 0;
            break;
        case 'd':
            key = 2;
            break;
        case 'e':
            key = 4;
            break;
        case 'f':
            key = 5;
            break;
        case 'g':
            key = 7;
            break;
        case 'a':
            key = 9;
            break;
        case 'b':
            key = 11;
            break;
        default:
            break;
        }
        if (match.captured(2) == "b")
            key--;
        else if (match.captured(2) == "#")
            key++;
        key += 12 * (1 + match.captured(3).toInt());

        if (key > 0 && key < 128)
            keyFound = true;
    }

    // Second attempt: search the numeric name of the key
    if (!keyFound)
    {
        QStringList listeNum = nameNoExtension.replace(QRegularExpression("[^0-9]"), "-").split("-", Qt::SkipEmptyParts);
        if (!listeNum.isEmpty())
        {
            // We study the last part
            key = listeNum.last().toUInt();
            if (key < 128)
                keyFound = true;
        }
    }

    if (keyFound)
    {
        _info.dwRootKey = key;
        _info.pitchDefined = true;
    }
}

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

#include "sound.h"
#include <QFileInfo>
#include "samplereader.h"
#include "samplereaderfactory.h"

Sound::Sound() :
    _fileName(""),
    _error(""),
    _data(nullptr),
    _reader(nullptr)
{}

Sound::~Sound()
{
    delete _reader;
    if (_data != nullptr)
    {
        delete [] _data;
        _data = nullptr;
    }
}

bool Sound::setFileName(QString qStr, bool tryFindRootKey)
{
    _fileName = qStr;
    bool isOk = false;

    // Initialize the reader
    if (_reader != nullptr)
        delete _reader;
    _reader = SampleReaderFactory::getSampleReader(_fileName);

    // Get information about the sample
    if (_reader != nullptr)
    {
        SampleReader::SampleReaderResult result = _reader->getInfo(&_info);
        switch (result)
        {
        case SampleReader::FILE_CORRUPT:
            _error = QObject::tr("Corrupted file: \"%1\"").arg(_fileName);
            break;
        case SampleReader::FILE_NOT_FOUND:
            _error = QObject::tr("Cannot find file \"%1\"").arg(_fileName);
            break;
        case SampleReader::FILE_NOT_READABLE:
            _error = QObject::tr("Cannot open file \"%1\"").arg(_fileName);
            break;
        case SampleReader::FILE_NOT_SUPPORTED:
            _error = QObject::tr("File not supported: \"%1\"").arg(_fileName);
            break;
        case SampleReader::FILE_OK:
            isOk = true;
            break;
        }
    }
    else
        _info.reset();

    if (!_info.pitchDefined && tryFindRootKey)
        determineRootKey();

    return isOk;
}

float * Sound::getData(quint32 &length, bool forceReload, bool getCopy)
{
    if (_reader != nullptr)
    {
        SampleReader::SampleReaderResult result;
        if (forceReload)
        {
            // Current length / channel
            quint32 oldLength = _info.dwLength;
            quint16 channel = _info.wChannel;

            // Update the details and keep the channel number
            _reader->getInfo(&_info);
            _info.wChannel = channel;

            // Write over the current data or replace the pointer
            float * newData = _reader->getData(result);
            quint32 newLength = _info.dwLength;
            _info.dwLength = oldLength;
            this->setData(newData, newLength);
        }
        else if (_data == nullptr)
        {
            _data = _reader->getData(result);
        }
    }

    length = _data == nullptr ? 0 : _info.dwLength;

    if (getCopy && _data != nullptr)
    {
        float * copy = new float[_info.dwLength];
        memcpy(copy, _data, _info.dwLength * sizeof(float));
        return copy;
    }

    return _data;
}

QVector<float> Sound::getDataVector(bool forceReload)
{
    quint32 length;
    float * data = this->getData(length, forceReload, false);
    QVector<float> v;
    if (length > 0)
    {
        v.reserve(length);
        std::copy(data, data + length, std::back_inserter(v));
    }
    return v;
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
    // Create a copy of the data
    float * copy = new float[data.size()];
    memcpy(copy, data.constData(), data.size() * sizeof(float));
    this->setData(copy, data.size());
}

void Sound::setData(float * newData, quint32 length)
{
    if (_data != nullptr && length == _info.dwLength)
    {
        // Copy data into the existing array (as a precaution if _data is being read by a voice in the synth)
        memcpy(_data, newData, length * sizeof(float));

        delete [] newData;
    }
    else
    {
        if (_data != nullptr)
            delete [] _data;
        _data = newData;
    }

    _info.dwLength = length;
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

void Sound::loadInRam()
{
    if (_data == nullptr)
    {
        SampleReader::SampleReaderResult result;
        _data = _reader->getData(result);
    }
}

void Sound::determineRootKey()
{
    // Try to find the root key with the help of the file name
    QFileInfo fileInfo(_fileName);
    QString nameNoExtension = fileInfo.completeBaseName().toLower();

    // First attempt: find a note name (standard taken being C4 = 60)
    static QRegularExpression keyNameRegex("([a-g])([#b]?)(-?[0-9]+)");
    bool keyFound = false;
    unsigned int key = 0;
    QRegularExpressionMatch match = keyNameRegex.match(nameNoExtension);
    if (match.hasMatch())
    {
        QString captured = match.captured(1);
        switch (captured[0].toLatin1())
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
        static QRegularExpression reg = QRegularExpression("[^0-9]");
        QStringList listeNum = nameNoExtension.replace(reg, "-").split("-", Qt::SkipEmptyParts);
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

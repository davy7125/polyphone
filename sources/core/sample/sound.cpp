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
#include "sampleutils.h"

Sound::Sound() :
    _fileName(""),
    _data16(nullptr),
    _data24(nullptr),
    _reader(nullptr)
{}

Sound::~Sound()
{
    delete _reader;
    if (_data16 != nullptr)
    {
        delete [] _data16;
        _data16 = nullptr;
    }
    if (_data24 != nullptr)
    {
        delete [] _data24;
        _data24 = nullptr;
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

void Sound::getData(quint32 &sampleLength, qint16* &data16, quint8* &data24, bool forceReload, bool getCopy)
{
    loadSample(forceReload);
    sampleLength = (_data16 == nullptr) ? 0 : _info.dwLength;

    if (getCopy && _data16 != nullptr)
    {
        qint16* copy16 = new qint16[_info.dwLength];
        memcpy(copy16, _data16, _info.dwLength * sizeof(qint16));
        data16 = copy16;
    }
    else
        data16 = _data16;

    if (getCopy && _data24 != nullptr)
    {
        quint8* copy24 = new quint8[_info.dwLength];
        memcpy(copy24, _data24, _info.dwLength * sizeof(quint8));
        data24 = copy24;
    }
    else
        data24 = _data24;
}

QVector<float> Sound::getDataFloat(bool forceReload)
{
    quint32 sampleLength;
    qint16 * data16;
    quint8 * data24;
    this->getData(sampleLength, data16, data24, forceReload, false);
    return SampleUtils::int24ToFloat(data16, data24, sampleLength);
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

void Sound::setData(qint16 * data16, quint8 * data24, quint32 length)
{
    if (_data16 != nullptr && length == _info.dwLength)
    {
        if (data16 == nullptr)
            _data16 = nullptr;
        else
        {
            // Copy data into the existing array (as a precaution if _data16 is being read by a voice in the synth)
            memcpy(_data16, data16, length * sizeof(qint16));
            delete [] data16;
        }
    }
    else
    {
        if (_data16 != nullptr)
            delete [] _data16;
        _data16 = data16;
    }

    if (_data24 != nullptr && length == _info.dwLength)
    {
        if (data24 == nullptr)
            _data24 = nullptr;
        else
        {
            // Copy data into the existing array (as a precaution if _data16 is being read by a voice in the synth)
            memcpy(_data24, data24, length * sizeof(quint8));
            delete [] data24;
        }
    }
    else
    {
        if (_data24 != nullptr)
            delete [] _data24;
        _data24 = data24;
    }

    _info.dwLength = length;
}

void Sound::setDataFloat(QVector<float> data)
{
    qint16 * data16;
    quint8 * data24;
    SampleUtils::floatToInt24(data, data16, data24);
    setData(data16, data24, data.length());
}

void Sound::set(AttributeType champ, AttributeValue value)
{
    switch (champ)
    {
    case champ_dwStart16:
        // Sample start (16 bits)
        _info.dwStart = value.dwValue;
        break;
    case champ_dwStart24:
        // Sample start (extra 8 bits)
        _info.dwStart2 = value.dwValue;
        break;
    case champ_dwLength:
        // Sample length (complete length is sample length * bytes per sample)
        _info.dwLength = value.dwValue;
        break;
    case champ_dwStartLoop:
        // Loop start
        if (_info.loops.empty())
            _info.loops << QPair<quint32, quint32>(value.dwValue, value.dwValue);
        else
            _info.loops[0].first = value.dwValue;
        break;
    case champ_dwEndLoop:
        // Loop end
        if (_info.loops.empty())
            _info.loops << QPair<quint32, quint32>(value.dwValue, value.dwValue);
        else
            _info.loops[0].second = value.dwValue;
        break;
    case champ_dwSampleRate:
        // Sample rate
        _info.dwSampleRate = value.dwValue;
        break;
    case champ_wChannel:
        // Channel used (left / right)
        _info.wChannel = value.wValue;
        break;
    case champ_wChannels:
        // Number of channels
        _info.wChannels = value.wValue;
        break;
    case champ_bpsFile:
        // Resolution (16 / 24)
        _info.wBpsFile = value.wValue;
        break;
    case champ_byOriginalPitch:
        // Root key in semitones
        _info.dwRootKey = value.bValue;
        break;
    case champ_chPitchCorrection:
        // Root key correction in cents
        _info.iFineTune = -value.cValue; // Fine tune is the opposite of correction
        break;
    default:
        break;
    }
}

void Sound::loadSample(bool forceReload)
{
    // The sample is maybe already loaded
    if ((_data16 != nullptr && !forceReload) || _reader == nullptr)
        return;

    if (forceReload)
    {
        // Current length / channel
        quint32 oldLength = _info.dwLength;
        quint16 channel = _info.wChannel;

        // Update the details and keep the channel number
        _reader->getInfo(&_info);
        _info.wChannel = channel;

        // Write over the current data or replace the pointer
        qint16 * newData16;
        quint8 * newData24;
        _reader->getData(newData16, newData24);
        quint32 newLength = _info.dwLength;
        _info.dwLength = oldLength;
        this->setData(newData16, newData24, newLength);
    }
    else if (_data16 == nullptr)
    {
        _reader->getData(_data16, _data24);
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

/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
**                2014      Andrea Celani                                 **
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

#include "recorder.h"
#include <QThread>

Recorder::Recorder() : QObject(),
    _recordFile(nullptr),
    _isRecording(false),
    _isWritingInStream(0),
    _dataWav(nullptr)
{}

Recorder::~Recorder()
{
    delete [] _dataWav;
}

void Recorder::setBufferSize(quint32 bufferSize)
{
    delete [] _dataWav;
    _dataWav = new float[2 * bufferSize]; // Stereo
}

void Recorder::startNewRecord(QString fileName, quint32 sampleRate)
{
    if (_recordFile)
        this->endRecord();

    _sampleRate = sampleRate;
    _recordFile = new QFile(fileName);
    if (_recordFile->open(QIODevice::WriteOnly))
    {
        // Create header
        quint32 dwTemp = 0;
        quint16 wTemp;
        _recordStream.setDevice(_recordFile);
        _recordStream.setByteOrder(QDataStream::LittleEndian);
        _recordLength = 0;

        // Write it
        _recordStream.writeRawData("RIFF", 4);
        _recordStream << static_cast<quint32>(_recordLength + 18 + 4 + 8 + 8);
        _recordStream.writeRawData("WAVE", 4);

        ///////////// BLOC FMT /////////////
        _recordStream.writeRawData("fmt ", 4);
        dwTemp = 18;
        _recordStream << dwTemp;
        // Compression code
        wTemp = 3;
        _recordStream << wTemp;
        // Number of channels
        wTemp = 2;
        _recordStream << wTemp;
        // Sample rate
        dwTemp = _sampleRate;
        _recordStream << dwTemp;
        // Average byte per second
        dwTemp *= 2 * 4;
        _recordStream << dwTemp;
        // Block align
        wTemp = 2 * 4;
        _recordStream << wTemp;
        // Significants bits per smpl
        _recordStream << static_cast<quint16>(32);
        // Extra format bytes
        wTemp = 0;
        _recordStream << wTemp;

        ///////////// BLOC DATA /////////////
        _recordStream.writeRawData("data", 4);
        _recordStream << _recordLength;

        _isRecording = true;
    }
    else
    {
        delete _recordFile;
        _recordFile = nullptr;
    }
}

void Recorder::pause(bool isOn)
{
    _isRecording = !isOn;
}

void Recorder::endRecord()
{
    _isRecording = false;

    // Possibly wait if another thread is writing
    while (!_isWritingInStream.testAndSetRelaxed(0, 1))
        QThread::msleep(2);

    if (_recordFile)
    {
        // Adjust file dimensions
        _recordFile->seek(4);
        _recordStream << static_cast<quint32>(_recordLength + 18 + 4 + 8 + 8);
        _recordFile->seek(42);
        _recordStream << _recordLength;

        // Close file
        _recordStream.setDevice(nullptr);
        _recordFile->close();
        delete _recordFile;
        _recordFile = nullptr;
    }

    // We are not writing anymore
    _isWritingInStream.storeRelaxed(0);
}

void Recorder::process(float * dataL, float * dataR, quint32 maxlen)
{
    // If we are not recording or if someone else is writing, do nothing
    if (!_isRecording || !_recordFile || !_isWritingInStream.testAndSetRelaxed(0, 1))
        return;

    // Interleave and write
    for (quint32 i = 0; i < maxlen; i++)
    {
        _dataWav[2 * i] = dataL[i];
        _dataWav[2 * i + 1] = dataR[i];
    }
    _recordStream.writeRawData(reinterpret_cast<char*>(_dataWav), static_cast<int>(maxlen * 2 * sizeof(float)));

    // Prise en compte de l'avance
    _recordLength += maxlen * 2 * sizeof(float);
    emit dataWritten(_sampleRate, maxlen);

    // We are not writing anymore
    _isWritingInStream.storeRelaxed(0);
}

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

#include "samplewriterwav.h"
#include "sampleutils.h"
#include "utils.h"

SampleWriterWav::SampleWriterWav(QString fileName) :
    _fileName(fileName)
{

}

void SampleWriterWav::write(Sound * sound)
{
    write(sound, nullptr);
}

void SampleWriterWav::write(Sound * leftSound, Sound * rightSound)
{
    bool deleteLeft = false;
    bool deleteRight = false;

    // Get left data
    quint32 sampleLengthLeft;
    qint16 * data16Left = nullptr;
    quint8 * data24Left = nullptr;
    leftSound->getData(sampleLengthLeft, data16Left, data24Left, false, false);
    quint32 dwSmplRate = leftSound->getUInt32(champ_dwSampleRate);
    quint16 wBps = leftSound->getUInt32(champ_bpsFile);

    // Possible right data
    quint32 sampleLengthRight = 0;
    qint16 * data16Right = nullptr;
    quint8 * data24Right = nullptr;
    if (rightSound != nullptr)
    {
        rightSound->getData(sampleLengthRight, data16Right, data24Right, false, false);

        // Maybe the sample rates don't match
        if (rightSound->getUInt32(champ_dwSampleRate) > dwSmplRate)
        {
            // Resample sound left
            QVector<float> fTmp = SampleUtils::int24ToFloat(data16Left, data24Left, sampleLengthLeft);
            fTmp = SampleUtils::resampleMono(fTmp, dwSmplRate, rightSound->getUInt32(champ_dwSampleRate));
            sampleLengthLeft = fTmp.length();
            SampleUtils::floatToInt24(fTmp, data16Left, data24Left);
            deleteLeft = true;
            dwSmplRate = rightSound->getUInt32(champ_dwSampleRate);
        }
        else if (rightSound->getUInt32(champ_dwSampleRate) < dwSmplRate)
        {
            // Resample sound right
            QVector<float> fTmp = SampleUtils::int24ToFloat(data16Right, data24Right, sampleLengthRight);
            fTmp = SampleUtils::resampleMono(fTmp, rightSound->getUInt32(champ_dwSampleRate), dwSmplRate);
            sampleLengthRight = fTmp.length();
            SampleUtils::floatToInt24(fTmp, data16Right, data24Right);
            deleteRight = true;
        }

        // Keep the best resolution
        if (rightSound->getUInt32(champ_bpsFile) > wBps)
            wBps = rightSound->getUInt32(champ_bpsFile);
    }

    // Data as arrays of integers
    QByteArray channel1;
    QByteArray channel2;
    if (wBps > 16)
    {
        wBps = 24;
        channel1 = convertTo24bit(data16Left, data24Left, sampleLengthLeft);
        channel2 = convertTo24bit(data16Right, data24Right, sampleLengthRight);
    }
    else
    {
        wBps = 16;
        channel1 = convertTo16bit(data16Left, sampleLengthLeft);
        channel2 = convertTo16bit(data16Right, sampleLengthRight);
    }

    if (deleteLeft)
    {
        delete [] data16Left;
        delete [] data24Left;
    }

    if (deleteRight)
    {
        delete [] data16Right;
        delete [] data24Right;
    }

    // Interleave data
    QByteArray baData;
    if (channel2.isEmpty())
    {
        baData = channel1;
    }
    else
    {
        if (channel1.size() < channel2.size())
        {
            // Add channel1 data
            QByteArray baTemp;
            baTemp.resize(channel2.size() - channel1.size());
            baTemp.fill(0);
            channel1.append(baTemp);
        }
        else if (channel1.size() > channel2.size())
        {
            // Add channel2 data
            QByteArray baTemp;
            baTemp.resize(channel1.size() - channel2.size());
            baTemp.fill(0);
            channel2.append(baTemp);
        }
        baData = from2MonoTo1Stereo(channel1, channel2, wBps);
    }

    /////////////////////////////////////////

    // Création d'un fichier
    QFile fi(_fileName);
    if (!fi.open(QIODevice::WriteOnly))
        return;

    // Ecriture
    quint32 dwTemp;
    quint16 wTemp;
    QDataStream out(&fi);
    out.setByteOrder(QDataStream::LittleEndian);
    quint32 dwTailleFmt = 18;
    quint32 dwTailleSmpl = 36;
    bool withLoop = (leftSound->getUInt32(champ_dwEndLoop) != 0);
    if (withLoop)
        dwTailleSmpl += 24;
    quint32 dwLength = static_cast<quint32>(baData.size());
    dwTemp = dwLength + dwTailleFmt + dwTailleSmpl + 12 + 8 + 8;

    // Entete
    out.writeRawData("RIFF", 4);
    out << dwTemp;
    out.writeRawData("WAVE", 4);

    ///////////// BLOC FMT /////////////
    out.writeRawData("fmt ", 4);
    out << dwTailleFmt;
    // Compression code
    wTemp = 1; // no compression
    out << wTemp;
    // Number of channels
    quint16 wChannels = (channel2.isEmpty() ? 1 : 2);
    out << wChannels;
    // Sample rate
    out << dwSmplRate;
    // Average byte per second
    dwTemp = dwSmplRate * wChannels * wBps / 8;
    out << dwTemp;
    // Block align
    wTemp = wChannels * wBps / 8;
    out << wTemp;
    // Significants bits per smpl
    out << wBps;
    // Extra format bytes
    wTemp = 0;
    out << wTemp;

    ///////////// BLOC SMPL /////////////
    out.writeRawData("smpl", 4);
    out << dwTailleSmpl;
    dwTemp = 0;
    out << dwTemp; // manufacturer
    out << dwTemp; // product
    dwTemp = 1000000000 / dwSmplRate;
    out << dwTemp; // smpl period

    // Note et correction
    int fineTune = -leftSound->getInt32(champ_chPitchCorrection);
    dwTemp = leftSound->getUInt32(champ_byOriginalPitch);
    if (fineTune > 100)
    {
        dwTemp = qMin(127u, dwTemp + 1);
        out << dwTemp;
        dwTemp = static_cast<quint32>(static_cast<double>(fineTune - 100) / 50. * 0x80000000 + 0.5);
        out << dwTemp;
    }
    else if (fineTune < 0)
    {
        dwTemp = qMax(0, (int)dwTemp - 1);
        out << dwTemp;
        dwTemp = static_cast<quint32>(static_cast<double>(fineTune + 100) / 50. * 0x80000000 + 0.5);
        out << dwTemp;
    }
    else
    {
        out << dwTemp;
        dwTemp = static_cast<quint32>(static_cast<double>(fineTune) / 50. * 0x80000000 + 0.5);
        out << dwTemp;
    }
    dwTemp = 0;
    out << dwTemp; // smpte format
    out << dwTemp; // smpte offset
    if (withLoop)
    {
        dwTemp = 1;
        out << dwTemp; // nombre boucles
        dwTemp = 0;
        out << dwTemp; // taille sampler data (après les boucles)
        dwTemp = 1;
        out << dwTemp; // CUE point id
        dwTemp = 0;
        out << dwTemp; // type
        dwTemp = leftSound->getUInt32(champ_dwStartLoop);
        out << dwTemp; // début boucle
        dwTemp = leftSound->getUInt32(champ_dwEndLoop) - 1;
        out << dwTemp; // fin boucle
        dwTemp = 0;
        out << dwTemp; // fraction
        out << dwTemp; // play count (infinite)
    }
    else
    {
        dwTemp = 0;
        out << dwTemp; // nombre boucles
        out << dwTemp; // taille sampler data (après les boucles)
    }

    ///////////// BLOC DATA /////////////
    out.writeRawData("data", 4);
    out << dwLength;
    out.writeRawData(baData.constData(), baData.size());

    // Fermeture du fichier
    fi.close();
}

QByteArray SampleWriterWav::convertTo16bit(qint16 * data16, quint32 length)
{
    QByteArray dataDest;
    if (data16 != nullptr && length > 0)
    {
        dataDest.resize(2 * length);
        qint16 * dataDest16 = reinterpret_cast<qint16 *>(dataDest.data());
        memcpy(dataDest16, data16, length * sizeof(qint16));
    }
    return dataDest;
}

QByteArray SampleWriterWav::convertTo24bit(qint16 * data16, quint8 * data24, quint32 length)
{
    QByteArray dataDest;
    if (data16 != nullptr && length > 0)
    {
        dataDest.resize(3 * length);
        char * dataDestChar = dataDest.data();
        for (quint32 i = 0; i < length; i++)
        {
            dataDestChar[3 * i] = (data24 == nullptr ? 0 : data24[i]);
            dataDestChar[3 * i + 1] = (data16[i] & 0xFF);
            dataDestChar[3 * i + 2] = (data16[i] >> 8);
        }
    }
    return dataDest;
}

QByteArray SampleWriterWav::from2MonoTo1Stereo(QByteArray baData1, QByteArray baData2, quint16 wBps)
{
    int size;

    // Si tailles différentes, on complète le petit avec des 0
    if (baData1.size() < baData2.size())
    {
        QByteArray baTmp;
        baTmp.fill(0, baData2.size() - baData1.size());
        baData1.append(baTmp);
    }
    else if (baData2.size() < baData1.size())
    {
        QByteArray baTmp;
        baTmp.fill(0, baData1.size() - baData2.size());
        baData2.append(baTmp);
    }
    size = baData1.size();

    // Assemblage
    QByteArray baRet;
    baRet.resize(2 * size);
    char * cDest = baRet.data();
    const char * cFrom1 = baData1.constData();
    const char * cFrom2 = baData2.constData();
    if (wBps == 24)
    {
        for (int i = 0; i < size/3; i++)
        {
            cDest[6*i] = cFrom1[3*i];
            cDest[6*i+1] = cFrom1[3*i+1];
            cDest[6*i+2] = cFrom1[3*i+2];
            cDest[6*i+3] = cFrom2[3*i];
            cDest[6*i+4] = cFrom2[3*i+1];
            cDest[6*i+5] = cFrom2[3*i+2];
        }
    }
    else // 16 bits
    {
        for (int i = 0; i < size/2; i++)
        {
            cDest[4*i] = cFrom1[2*i];
            cDest[4*i+1] = cFrom1[2*i+1];
            cDest[4*i+2] = cFrom2[2*i];
            cDest[4*i+3] = cFrom2[2*i+1];
        }
    }
    return baRet;
}

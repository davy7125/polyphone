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

#include "samplewriterwav.h"
#include "sampleutils.h"
#include "utils.h"

SampleWriterWav::SampleWriterWav(QString fileName) :
    _fileName(fileName)
{

}

void SampleWriterWav::write(Sound * sound)
{
    // Data as an array of integers
    InfoSound info = sound->getInfo();
    QByteArray baData;
    if (info.wBpsFile > 16)
    {
        info.wBpsFile = 24;
        convertTo24bit(sound->getData(), baData);
    }
    else
    {
        info.wBpsFile = 16;
        convertTo16bit(sound->getData(), baData);
    }

    // Exportation d'un sample mono au format wav
    info.wChannels = 1;
    write(baData, info);
}

void SampleWriterWav::write(Sound *leftSound, Sound *rightSound)
{
    QVector<float> dataLeft = leftSound->getData();
    QVector<float> dataRight = rightSound->getData();

    // Sample rate (max of the 2 channels)
    quint32 dwSmplRate = leftSound->getInfo().dwSampleRate;
    if (rightSound->getInfo().dwSampleRate > dwSmplRate)
    {
        // Adapt sound 1
        dataLeft = SampleUtils::resampleMono(dataLeft, dwSmplRate, rightSound->getInfo().dwSampleRate);
        dwSmplRate = rightSound->getInfo().dwSampleRate;
    }
    else if (rightSound->getInfo().dwSampleRate < dwSmplRate)
    {
        // Adapt sound 2
        dataRight = SampleUtils::resampleMono(dataRight, rightSound->getInfo().dwSampleRate, dwSmplRate);
    }

    // Data as arrays of integers
    quint16 wBps = leftSound->getInfo().wBpsFile;
    if (rightSound->getInfo().wBpsFile > wBps)
        wBps = rightSound->getInfo().wBpsFile;

    QByteArray channel1;
    QByteArray channel2;
    if (wBps > 16)
    {
        wBps = 24;
        convertTo24bit(dataLeft, channel1);
        convertTo24bit(dataRight, channel2);
    }
    else
    {
        wBps = 16;
        convertTo16bit(dataLeft, channel1);
        convertTo16bit(dataRight, channel2);
    }

    // Taille et mise en forme des données
    quint32 dwLength = channel1.size();
    if (dwLength < (unsigned)channel2.size())
    {
        // On complète chanel1
        QByteArray baTemp;
        baTemp.resize(channel2.size() - dwLength);
        baTemp.fill(0);
        channel1.append(baTemp);
        dwLength = channel2.size();
    }
    else if (dwLength > (unsigned)channel2.size())
    {
        // On complète chanel2
        QByteArray baTemp;
        baTemp.resize(dwLength - channel2.size());
        baTemp.fill(0);
        channel2.append(baTemp);
    }

    // Assemblage des canaux
    QByteArray baData = from2MonoTo1Stereo(channel1, channel2, wBps);

    // Création d'un fichier
    InfoSound info = leftSound->getInfo();
    info.wBpsFile = wBps;
    info.dwSampleRate = dwSmplRate;
    info.wChannels = 2;
    write(baData, info);
}

void SampleWriterWav::write(QByteArray &baData, InfoSound &info)
{
    // Création d'un fichier
    QFile fi(_fileName);
    if (!fi.open(QIODevice::WriteOnly))
        return;

    bool withLoop = !info.loops.empty();

    // Ecriture
    quint32 dwTemp;
    quint16 wTemp;
    QDataStream out(&fi);
    out.setByteOrder(QDataStream::LittleEndian);
    quint32 dwTailleFmt = 18;
    quint32 dwTailleSmpl = 36;
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
    wTemp = info.wChannels;
    out << wTemp;
    // Sample rate
    out << static_cast<quint32>(info.dwSampleRate);
    // Average byte per second
    dwTemp = info.dwSampleRate * info.wChannels * info.wBpsFile / 8;
    out << dwTemp;
    // Block align
    wTemp = info.wChannels * info.wBpsFile / 8;
    out << wTemp;
    // Significants bits per smpl
    out << info.wBpsFile;
    // Extra format bytes
    wTemp = 0;
    out << wTemp;

    ///////////// BLOC SMPL /////////////
    out.writeRawData("smpl", 4);
    out << dwTailleSmpl;
    dwTemp = 0;
    out << dwTemp; // manufacturer
    out << dwTemp; // product
    dwTemp = 1000000000 / info.dwSampleRate;
    out << dwTemp; // smpl period

    // Note et correction
    if (info.iFineTune > 100)
    {
        dwTemp = qMin(127u, info.dwRootKey + 1);
        out << dwTemp;
        dwTemp = static_cast<quint32>(static_cast<double>(info.iFineTune - 100) / 50. * 0x80000000 + 0.5);
        out << dwTemp;
    }
    else if (info.iFineTune < 0)
    {
        dwTemp = qMax(0u, info.dwRootKey - 1);
        out << dwTemp;
        dwTemp = static_cast<quint32>(static_cast<double>(info.iFineTune + 100) / 50. * 0x80000000 + 0.5);
        out << dwTemp;
    }
    else
    {
        dwTemp = info.dwRootKey;
        out << dwTemp;
        dwTemp = static_cast<quint32>(static_cast<double>(info.iFineTune) / 50. * 0x80000000 + 0.5);
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
        dwTemp = info.loops[0].first;
        out << dwTemp; // début boucle
        dwTemp = info.loops[0].second - 1;
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

void SampleWriterWav::convertTo16bit(QVector<float> dataSrc, QByteArray &dataDest)
{
    const float * data = dataSrc.constData();
    int length = dataSrc.size();

    dataDest.resize(2 * length);
    qint16 * data16 = reinterpret_cast<qint16 *>(dataDest.data());
    for (int i = 0; i < length; i++)
        data16[i] = (Utils::floatToInt24(data[i]) >> 8);
}

void SampleWriterWav::convertTo24bit(QVector<float> dataSrc, QByteArray &dataDest)
{
    const float * data = dataSrc.constData();
    int length = dataSrc.size();

    dataDest.resize(3 * length);
    char * dataChar = dataDest.data();
    for (int i = 0; i < length; i++)
    {
        qint32 tmp = Utils::floatToInt24(data[i]);
        dataChar[3 * i] = tmp & 0xFF;
        dataChar[3 * i + 1] = (tmp >> 8) & 0xFF;
        dataChar[3 * i + 2] = tmp >> 16;
    }
}

QByteArray SampleWriterWav::from2MonoTo1Stereo(QByteArray baData1, QByteArray baData2, quint16 wBps, bool bigEndian)
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
    if (wBps == 32)
    {
        if (bigEndian)
        {
            for (int i = 0; i < size/4; i++)
            {
                cDest[8*i] = cFrom1[4*i+3];
                cDest[8*i+1] = cFrom1[4*i+2];
                cDest[8*i+2] = cFrom1[4*i+1];
                cDest[8*i+3] = cFrom1[4*i];
                cDest[8*i+4] = cFrom2[4*i+3];
                cDest[8*i+5] = cFrom2[4*i+2];
                cDest[8*i+6] = cFrom2[4*i+1];
                cDest[8*i+7] = cFrom2[4*i];
            }
        }
        else
        {
            for (int i = 0; i < size/4; i++)
            {
                cDest[8*i] = cFrom1[4*i];
                cDest[8*i+1] = cFrom1[4*i+1];
                cDest[8*i+2] = cFrom1[4*i+2];
                cDest[8*i+3] = cFrom1[4*i+3];
                cDest[8*i+4] = cFrom2[4*i];
                cDest[8*i+5] = cFrom2[4*i+1];
                cDest[8*i+6] = cFrom2[4*i+2];
                cDest[8*i+7] = cFrom2[4*i+3];
            }
        }
    }
    else if (wBps == 24)
    {
        if (bigEndian)
        {
            for (int i = 0; i < size/3; i++)
            {
                cDest[6*i] = cFrom1[3*i+2];
                cDest[6*i+1] = cFrom1[3*i+1];
                cDest[6*i+2] = cFrom1[3*i];
                cDest[6*i+3] = cFrom2[3*i+2];
                cDest[6*i+4] = cFrom2[3*i+1];
                cDest[6*i+5] = cFrom2[3*i];
            }
        }
        else
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
    }
    else // 16 bits
    {
        if (bigEndian)
        {
            for (int i = 0; i < size/2; i++)
            {
                cDest[4*i] = cFrom1[2*i+1];
                cDest[4*i+1] = cFrom1[2*i];
                cDest[4*i+2] = cFrom2[2*i+1];
                cDest[4*i+3] = cFrom2[2*i];
            }
        }
        else
        {
            for (int i = 0; i < size/2; i++)
            {
                cDest[4*i] = cFrom1[2*i];
                cDest[4*i+1] = cFrom1[2*i+1];
                cDest[4*i+2] = cFrom2[2*i];
                cDest[4*i+3] = cFrom2[2*i+1];
            }
        }
    }
    return baRet;
}

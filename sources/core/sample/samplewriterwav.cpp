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

SampleWriterWav::SampleWriterWav(QString fileName) :
    _fileName(fileName)
{

}

void SampleWriterWav::write(Sound * sound)
{
    // Exportation d'un sample mono au format wav
    quint16 wBps = sound->getInfo().wBpsFile;
    if (wBps > 16)
        wBps = 24;
    else
        wBps = 16;
    QByteArray baData = sound->getData(wBps);

    // Création d'un fichier
    InfoSound info = sound->getInfo();
    info.wBpsFile = wBps;
    info.wChannels = 1;
    write(baData, info);
}

void SampleWriterWav::write(Sound *leftSound, Sound *rightSound)
{
    // Exportation d'un sample stereo au format wav
    // bps (max des 2 canaux)
    quint16 wBps = leftSound->getInfo().wBpsFile;
    if (rightSound->getInfo().wBpsFile > wBps)
        wBps = rightSound->getInfo().wBpsFile;
    if (wBps > 16)
        wBps = 24;
    else
        wBps = 16;

    // Récupération des données
    QByteArray channel1 = leftSound->getData(wBps);
    QByteArray channel2 = rightSound->getData(wBps);

    // sample rate (max des 2 canaux)
    quint32 dwSmplRate = leftSound->getInfo().dwSampleRate;
    if (rightSound->getInfo().dwSampleRate > dwSmplRate)
    {
        // Ajustement son1
        channel1 = SampleUtils::resampleMono(channel1, dwSmplRate, rightSound->getInfo().dwSampleRate, wBps);
        dwSmplRate = rightSound->getInfo().dwSampleRate;
    }
    else if (rightSound->getInfo().dwSampleRate < dwSmplRate)
    {
        // Ajustement son2
        channel2 = SampleUtils::resampleMono(channel2, rightSound->getInfo().dwSampleRate, dwSmplRate, wBps);
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
    QByteArray baData = SampleUtils::from2MonoTo1Stereo(channel1, channel2, wBps);

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

    bool withLoop = (info.dwStartLoop != info.dwEndLoop);

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
    if (info.iFineTune > 50)
    {
        dwTemp = qMin((quint32)127, info.dwRootKey + 1);
        out << dwTemp;
        dwTemp = qRound(((double)(info.iFineTune - 50) / 50.) * 2147483648.);
        out << dwTemp;
    }
    else if (info.iFineTune < -50)
    {
        dwTemp = qMax((quint32)0, info.dwRootKey - 1);
        out << dwTemp;
        dwTemp = qRound(((double)(info.iFineTune + 50) / 50.) * 2147483648.);
        out << dwTemp;
    }
    else
    {
        dwTemp = info.dwRootKey;
        out << dwTemp;
        dwTemp = qRound(((double)info.iFineTune / 50.) * 2147483648.);
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
        dwTemp = info.dwStartLoop;
        out << dwTemp; // début boucle
        dwTemp = info.dwEndLoop-1;
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

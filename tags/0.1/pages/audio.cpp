/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013 Davy Triponney                                     **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
****************************************************************************/

#include "audio.h"

// Constructeur, destructeur, initialisation
Audio::Audio(QObject * parent) : QIODevice(parent)
{
    this->m_buffer.resize(90000);
    this->avance = 20000;
    this->audioOutput = NULL;
    // Connexion
    this->connect(this, SIGNAL(askForData()), SLOT(generateData()));
    this->connect(this, SIGNAL(readFinished()), SLOT(finished2()));
}
Audio::~Audio()
{
    this->close();
    delete this->audioOutput;
}
void Audio::initAudio(int numServeur)
{
    // Ouverture serveur son (doit être fait après la création, une fois dans le bon thread
    // Format audio à l'écoute
    format.setFrequency(48000);
    format.setChannels(2);
    format.setSampleSize(24);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);
    // Serveur son
    QAudioDeviceInfo m_audioOutputDevice;
    QList<QAudioDeviceInfo> listeAudio = QAudioDeviceInfo::availableDevices (QAudio::AudioOutput);
    if (listeAudio.count() > numServeur)
        m_audioOutputDevice = listeAudio.takeAt(numServeur);
    else
        m_audioOutputDevice = QAudioDeviceInfo(QAudioDeviceInfo::defaultOutputDevice());
    if (!m_audioOutputDevice.isFormatSupported(format))
        format = m_audioOutputDevice.nearestFormat(format);
    this->audioOutput = new QAudioOutput(m_audioOutputDevice, format);
    // Réglage buffer (latence)
    this->audioOutput->setBufferSize(19200);
}

// Méthodes virtuelles pures à implémenter
qint64 Audio::readData(char *data, qint64 len)
{
    if (this->currentBufferLength < len)
    {
        // Chargement d'urgence
        this->generateData(len - this->currentBufferLength);
    }
    qint64 writeLen = qMin(len, this->currentBufferLength);
    qint64 total = 0;
    while (writeLen - total > 0)
    {
        const qint64 chunk = qMin((m_buffer.size() - this->posLecture), writeLen - total);
        memcpy(data + total, m_buffer.constData() + this->posLecture, chunk);
        this->posLecture = (this->posLecture + chunk) % m_buffer.size();
        total += chunk;
    }
    this->currentBufferLength -= total;
    if (currentBufferLength < this->avance)
        emit(askForData());
    return total;
}
qint64 Audio::readSinus(char *data, qint64 maxlen)
{
    qint64 total = 0;
    while (maxlen - total > 0)
    {
        const qint64 chunk = qMin((m_bufferSinus.size() - this->posLectureSinus), maxlen - total);
        memcpy(data + total, m_bufferSinus.constData() + this->posLectureSinus, chunk);
        this->posLectureSinus = (this->posLectureSinus + chunk) % m_bufferSinus.size();
        total += chunk;
    }
    return total;
}
qint64 Audio::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);
    return 0;
}
qint64 Audio::bytesAvailable() const
{
    return this->currentBufferLength;
}

// Génération des données
void Audio::generateSinus(double frequency)
{
    // Dimensionnement du buffer en fonction de la fréquence
    qint64 lengthT;
    if (frequency < 100)
    {
        // Longueur de 10 périodes
        lengthT = (double)(10.0 * format.frequency()) / frequency;
        // Ajustement fréquence
        frequency = (double)(10.0 * format.frequency()) / lengthT;
    }
    else if (frequency < 1000)
    {
        // Longueur de 100 périodes
        lengthT = (double)(100.0 * format.frequency()) / frequency;
        // Ajustement fréquence
        frequency = (double)(100.0 * format.frequency()) / lengthT;
    }
    else
    {
        // Longueur de 1000 périodes
        lengthT = (double)(1000.0 * format.frequency()) / frequency;
        // Ajustement fréquence
        frequency = (double)(1000.0 * format.frequency()) / lengthT;
    }
    qint64 length = 4 * lengthT;
    m_bufferSinus.resize(length);
    // Création du sinus
    unsigned char * ptr = reinterpret_cast<unsigned char *>(m_bufferSinus.data());
    qreal x;
    for (int i = 0; i < lengthT; i++)
    {
        x = qSin(2 * M_PI * frequency * qreal(i) / format.frequency());
        qint32 value = static_cast<qint32>(x * 300000000);
        qToLittleEndian<qint32>(value, ptr+4*i);
    }
}
void Audio::generateData(int nbData)
{
    if (this->finDemandee && this->attenuationFin < 0.000000001)
        QTimer::singleShot(200, this, SLOT(finished2()));
    // Nombre d'échantillons à mettre dans le buffer
    int nbFinal;
    if (nbData)
        nbFinal = nbData / 6 + 1;
    else
        nbFinal = (80 * this->dwSmplRate) / 1000; // 80 milli secondes
    // Nombre d'échantillons à lire au départ
    int nbInit = 0;
    // Fréquence d'échantillonnage initiale fictive (pour accordage)
    double bpsInit = (double)this->dwSmplRate * pow(2, (double)this->tune/1200);
    if (bpsInit != this->format.frequency())
        nbInit = qMax(100.0, (nbFinal * bpsInit) / this->format.frequency());
    else
        nbInit = qMax(100, nbFinal);
    //// CONSTITUTION DES DONNEES ////
    // Canal droit
    bool bRet = false; qint64 iRet = 0;
    QByteArray baDataD = takeData(this->baDataDroite, this->currentSmpl1, nbInit,
                                  this->startLoop1, this->endLoop1, this->boucle,
                                  bRet, iRet);
    if (!bRet)
    {
        // Au bout du sample
        this->lectureD = false;
        if (!this->lectureG) emit(readFinished());
    }
    else
    {
        // Mise à jour position
        this->currentSmpl1 = iRet;
        if (this->typeSmpl == 1 || this->typeSmpl == 0)
            emit(setCurrentPos(iRet));
    }
    // Canal gauche
    QByteArray baDataG = takeData(this->baDataGauche, this->currentSmpl2, nbInit,
                                  this->startLoop2, this->endLoop2, this->boucle,
                                  bRet, iRet);
    if (!bRet)
    {
        // Au bout du sample
        this->lectureG = false;
        if (!this->lectureD) emit(readFinished());
    }
    else
    {
        // Mise à jour position
        this->currentSmpl2 = iRet;
        if (this->typeSmpl == 2)
            emit(setCurrentPos(iRet));
    }
    // Ajustement du volume
    qint32 * dataD = (qint32*)baDataD.data();
    qint32 * dataG = (qint32*)baDataG.data();
    for (int i = 0; i < baDataD.size()/4; i++)
    {
        if (this->finDemandee)
        {
            this->attenuationFin *= 0.994;
        }
        dataD[i] *= volume * attenuationFin;
        dataG[i] *= volume * attenuationFin;
    }
    // Resampling
    if (bpsInit != this->format.frequency())
    {
        double decalage = this->d;
        // Récupération des dernières valeurs avant resampling
        qint32 * dataD = (qint32*)baDataD.data();
        qint32 * dataG = (qint32*)baDataG.data();
        qint32 finD = dataD[baDataD.size()/4-1];
        qint32 finG = dataG[baDataG.size()/4-1];
        baDataD = Sound::resampleMono(baDataD, bpsInit, this->format.frequency(), 32, this->valPrec1, decalage);
        baDataG = Sound::resampleMono(baDataG, bpsInit, this->format.frequency(), 32, this->valPrec2, this->d);
        // Mise à jour des valeurs précédentes
        this->valPrec1 = finD;
        this->valPrec2 = finG;
    }
    // Association sinus
    if (this->sinus)
    {
        QByteArray baDataSin = baDataD;
        readSinus(baDataSin.data(), baDataSin.size());
        qint32 * dataSin = (qint32 *)baDataSin.data();
        qint32 * dataD = (qint32 *)baDataD.data();
        qint32 * dataG = (qint32 *)baDataG.data();
        // Addition
        for (int i = 0; i < baDataD.size()/4; i++)
        {
            dataD[i] += dataSin[i];
            dataG[i] += dataSin[i];
        }
    }
    // signed / unsigned
    if (this->format.sampleType() == QAudioFormat::UnSignedInt)
    {
        // Conversion signed => unsigned
        const qint32 * dataD = (const qint32 *)baDataD.data();
        const qint32 * dataG = (const qint32 *)baDataG.data();
        quint32 * dataDu = (quint32 *)baDataD.data();
        quint32 * dataGu = (quint32 *)baDataG.data();
        for (int i = 0; i < baDataD.size()/4; i++)
        {
            dataDu[i] = (qint64)dataD[i] + 2147483648LL;
            dataGu[i] = (qint64)dataG[i] + 2147483648LL;
        }
    }
    // Association des canaux, prise en compte endianness et sample size
    QByteArray baData;
    if (this->stereo)
    {
        // Ecoute en stéréo
        baDataD = Sound::bpsConversion(baDataD, 32, this->format.sampleSize());
        baDataG = Sound::bpsConversion(baDataG, 32, this->format.sampleSize());
        baData = Sound::from2MonoTo1Stereo(baDataG, baDataD, this->format.sampleSize(),
                                           this->format.byteOrder() == QAudioFormat::BigEndian);
    }
    else if (this->typeSmpl == 0 || this->typeSmpl == 1)
    {
        // Ecoute du canal droit
        baDataD = Sound::bpsConversion(baDataD, 32, this->format.sampleSize());
        baData = Sound::from2MonoTo1Stereo(baDataD, baDataD, this->format.sampleSize(),
                                           this->format.byteOrder() == QAudioFormat::BigEndian);
    }
    else
    {
        // Ecoute du canal gauche
        baDataG = Sound::bpsConversion(baDataG, 32, this->format.sampleSize());
        baData = Sound::from2MonoTo1Stereo(baDataG, baDataG, this->format.sampleSize(),
                                           this->format.byteOrder() == QAudioFormat::BigEndian);
    }
    //// ECRITURE SUR LE BUFFER ////
    if ((this->posEcriture + baData.size()) > this->m_buffer.size())
    {
        qint64 firstData = this->m_buffer.size() - this->posEcriture;
        memcpy(this->m_buffer.data() + this->posEcriture, baData.data(), firstData);
        memcpy(this->m_buffer.data(), baData.data() + firstData, baData.size() - firstData);
    }
    else
        memcpy(m_buffer.data() + this->posEcriture, baData.data(), baData.size());
    this->posEcriture = (this->posEcriture + baData.size()) % this->m_buffer.size();
    this->currentBufferLength += baData.size();
}

// Modification des paramètres
void Audio::setPlayParameter(int startLoop1, int endLoop1, int typeSmpl,
                             int startLoop2, int endLoop2, bool boucle, bool sinus,
                             int tune, int rootKey, int volume, int stereo)
{
    this->startLoop1 = startLoop1;
    this->startLoop2 = startLoop2;
    this->endLoop1 = endLoop1;
    this->endLoop2 = endLoop2;
    this->boucle = boucle;
    this->sinus = sinus;
    this->tune = tune;
    this->typeSmpl = typeSmpl;
    this->stereo = stereo;
    this->setVolume(volume);
    this->setRootKey(rootKey);
}
void Audio::setStartLoop(int index)
{
    if (typeSmpl == 1)
        this->startLoop1 = index;
    else if (typeSmpl == 2)
        this->startLoop2 = index;
    else
    {
        this->startLoop1 = index;
        this->startLoop2 = index;
    }
}
void Audio::setEndLoop(int index)
{
    if (typeSmpl == 1)
        this->endLoop1 = index;
    else if (typeSmpl == 2)
        this->endLoop2 = index;
    else
    {
        this->endLoop1 = index;
        this->endLoop2 = index;
    }
}
void Audio::setBoucle(int iVal) {this->boucle = iVal;}
void Audio::setSinus(int iVal) {this->sinus = iVal;}
void Audio::setTune(int iTune) {this->tune = iTune;}
void Audio::setStereo(int iVal) {this->stereo = iVal;}
void Audio::setRootKey(int iKey)
{
    // Construction d'un sinus
    this->rootKey = iKey;
    this->posLectureSinus = 0;
    // Fréquence
    double f = 440.0 * pow(2.0,(double)(iKey-69)/12);
    this->generateSinus(f);
}
void Audio::setVolume(int iVol) {this->volume = (double)(iVol*iVol)/2500;}

// Modification des données de bases à écouter
void Audio::setData(QByteArray * baDataDroite, QByteArray * baDataGauche, quint32 dwSmplRate)
{
    // Données et échantillonnage
    this->baDataDroite = QByteArray(baDataDroite->constData(), baDataDroite->size());
    this->baDataGauche = QByteArray(baDataGauche->constData(), baDataGauche->size());
    this->dwSmplRate = dwSmplRate;
    // Vérification que les données ont la même longueur
    if (this->baDataDroite.size() < this->baDataGauche.size())
    {
        int delta = this->baDataGauche.size() - this->baDataDroite.size();
        QByteArray baTmp(delta, 0);
        this->baDataDroite.append(baTmp);
    }
    else if (this->baDataGauche.size() < this->baDataDroite.size())
    {
        int delta = this->baDataDroite.size() - this->baDataGauche.size();
        QByteArray baTmp(delta, 0);
        this->baDataGauche.append(baTmp);
    }
}

// Début, fin de l'écoute
void Audio::start()
{
    // Ouverture en lecture seule
    this->open(QIODevice::ReadOnly);
    QThread::currentThread()->setPriority(QThread::HighPriority);
    // Initialisation des variables
    this->posLecture = 0;
    this->posEcriture = 0;
    this->posLectureSinus = 0;
    this->currentBufferLength = 0;
    this->currentSmpl1 = 0;
    this->currentSmpl2 = 0;
    this->lectureD = true;
    this->lectureG = true;
    this->finDemandee = false;
    this->d = 0;
    this->valPrec1 = 0;
    this->valPrec2 = 0;
    this->attenuationFin = 1;
    // Ajout de données dans le buffer
    this->generateData();
    // Début du son, mode pull
    this->audioOutput->start(this);
}
void Audio::stop()
{
    this->finDemandee = true;
}

void Audio::finished2()
{
    QThread::currentThread()->setPriority(QThread::LowPriority);
    this->audioOutput->stop();
    this->audioOutput->reset();
    emit(setCurrentPos(-1)); // Effacement du curseur sur le graphique
    emit(sigFinished());
}

void Audio::finished3()
{
    QThread::currentThread()->setPriority(QThread::LowPriority);
    this->audioOutput->stop();
    this->audioOutput->reset();
    emit(setCurrentPos(-1)); // Effacement du curseur sur le graphique
    emit(sigFinished2());
}

// UTILITAIRES
QByteArray Audio::takeData(QByteArray baData, qint64 posInit, qint64 nbRead,
                           qint64 loopStart, qint64 loopEnd, bool boucle,
                           bool &end, qint64 &posFinal)
{
    const char * data = baData.constData();
    QByteArray baDataRet;
    baDataRet.resize(nbRead * 4);
    char * dataRet = baDataRet.data();
    if (boucle && loopStart != loopEnd)
    {
        // Boucle
        if (posInit >= loopEnd) posInit = loopStart;
        qint64 total = 0;
        while (nbRead - total > 0)
        {
            const qint64 chunk = qMin(loopEnd - posInit, nbRead - total);
            memcpy(dataRet + 4 * total, data + 4 * posInit, 4 * chunk);
            posInit += chunk;
            if (posInit >= loopEnd) posInit = loopStart;
            total += chunk;
        }
        posFinal = posInit;
        end = true;
    }
    else
    {
        // Pas de boucle
        if (baData.size()/4 - posInit < nbRead)
        {
            baDataRet.fill(0);
            memcpy(dataRet, data + 4 * posInit, baData.size() - 4 * posInit);
            posFinal = baData.size() / 4;
            end = false;
        }
        else
        {
            memcpy(dataRet, data + 4 * posInit, 4 * nbRead);
            posFinal = posInit + nbRead;
            end = true;
        }
    }
    return baDataRet;
}

/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
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
***************************************************************************/

#include "sound.h"
#include <QMessageBox>
#include "config.h"
#include <QLibrary>
#include <QCoreApplication>
#include <QFileInfo>
#include "qmath.h"
#include <QFile>

QWidget * Sound::_parent = NULL;

// Constructeur / destructeur
Sound::Sound(QString filename, bool tryFindRootkey)
{
    this->sm24.clear();
    this->smpl.clear();
    _info.wChannel = 0;
    _info.wChannels = 0;
    _info.dwLength = 0;
    _info.dwStart = 0;
    _info.dwStart2 = 0;
    _info.dwSampleRate = 0;
    _info.dwStartLoop = 0;
    _info.dwEndLoop = 0;
    _info.dwNote = 0;
    _info.iCent = 0;
    _info.wBpsFile = 0;
    if (!filename.isEmpty())
        this->setFileName(filename, tryFindRootkey);
}
Sound::~Sound() {}

////////////////////////////////////////////////////////////////////////////
//////////////////////////   METHODES PUBLIQUES   //////////////////////////
////////////////////////////////////////////////////////////////////////////

QByteArray Sound::getData(quint16 wBps)
{
    // Copie des données dans data, résolution wBps
    // wBps = 16 : chargement smpl, 16 bits de poids fort
    // wBps =  8 : chargement sm24, 8 bits suivant le 16 de poids fort
    // wBps = 24 : chargement 24 bits de poids fort
    // wBps = 32 : chargement en 32 bits
    QByteArray baRet;
    switch (wBps)
    {
    case 8:
        // chargement des 8 bits de poids faible
        if (!this->sm24.isEmpty())
           baRet = this->sm24;
        else if (_info.wBpsFile >= 24)
        {
            // lecture fichier
            QFile *fi = new QFile(fileName);
            if (!fi->exists())
            {
                // Si impossible à ouvrir : pas de message d'erreur et remplissage 0
                baRet.resize(_info.dwLength);
                baRet.fill(0);
            }
            else
            {
                fi->open(QFile::ReadOnly | QFile::Unbuffered);
                switch (this->getFileType())
                {
                case fileSf2:
                    baRet = getDataSf2(fi, 1);
                    break;
                case fileWav:
                    baRet = getDataWav(fi, 1);
                    break;
                case fileCustom1:{
                    typedef QByteArray (*GetDataSound)(QFile*, quint16, InfoSound);
                    GetDataSound getDataSound = (GetDataSound) QLibrary::resolve(QCoreApplication::applicationDirPath() + "/extension_lecture",
                                                                             "getDataSound");
                    if (getDataSound)
                        baRet = getDataSound(fi, 1, _info);
                    else
                    {
                        baRet.resize(_info.dwLength);
                        baRet.fill(0);
                    }
                    }break;
                default:
                    QMessageBox::warning(_parent, QObject::trUtf8("Attention"), QObject::trUtf8("Fichier non pris en charge."));
                }
                // Fermeture du fichier
                fi->close();
            }
            delete fi;
        }
        else
        {
            // remplissage 0
            baRet.resize(_info.dwLength);
            baRet.fill(0);
        }
        break;
    case 16:
        // chargement des 16 bits de poids fort
        if (!this->smpl.isEmpty())
            baRet = this->smpl;
        else
        {
            // lecture fichier
            QFile *fi = new QFile(fileName);
            if (!fi->exists())
            {
                // Si impossible à ouvrir : pas de message d'erreur et remplissage 0
                baRet.resize(_info.dwLength*2);
                baRet.fill(0);
            }
            else
            {
                fi->open(QFile::ReadOnly | QFile::Unbuffered);
                switch (this->getFileType())
                {
                case fileSf2:
                    baRet = getDataSf2(fi, 2);
                    break;
                case fileWav:
                    baRet = getDataWav(fi, 2);
                    break;
                case fileCustom1:{
                    typedef QByteArray (*GetDataSound)(QFile*, quint16, InfoSound);
                    GetDataSound getDataSound = (GetDataSound) QLibrary::resolve(QCoreApplication::applicationDirPath() + "/extension_lecture",
                                                                             "getDataSound");
                    if (getDataSound)
                        baRet = getDataSound(fi, 2, _info);
                    else
                    {
                        baRet.resize(_info.dwLength*2);
                        baRet.fill(0);
                    }
                    }break;
                default:
                    QMessageBox::warning(_parent, QObject::trUtf8("Attention"), QObject::trUtf8("Fichier non pris en charge."));
                }
                // Fermeture du fichier
                fi->close();
            }
            delete fi;
        }
        break;
    case 24:
        // chargement des 24 bits
        if (!this->smpl.isEmpty() && !this->sm24.isEmpty() && _info.wBpsFile >= 24)
        {
            // copie 24 bits
            baRet.resize(_info.dwLength*3);
            char * cDest = baRet.data();
            char * cFrom = this->smpl.data();
            char * cFrom24 = this->smpl.data();
            int len = (int)_info.dwLength;
            for (int i = 0; i < len; i++)
            {
                cDest[3*i] = cFrom24[i];
                cDest[3*i+1] = cFrom[2*i];
                cDest[3*i+2] = cFrom[2*i+1];
            }
        }
        else if (!this->smpl.isEmpty() && _info.wBpsFile < 24)
        {
            // copie 16 bits
            baRet.resize(_info.dwLength*3);
            char * cDest = baRet.data();
            char * cFrom = this->smpl.data();
            int len = (int)_info.dwLength;
            for (int i = 0; i < len; i++)
            {
                cDest[3*i] = 0;
                cDest[3*i+1] = cFrom[2*i];
                cDest[3*i+2] = cFrom[2*i+1];
            }
        }
        else
        {
            // lecture fichier
            QFile *fi = new QFile(fileName);
            if (!fi->exists())
            {
                // Si impossible à ouvrir : pas de message d'erreur et remplissage 0
                baRet.resize(_info.dwLength*3);
                baRet.fill(0);
            }
            else
            {
                fi->open(QFile::ReadOnly | QFile::Unbuffered);
                switch (this->getFileType())
                {
                case fileSf2:
                    baRet = getDataSf2(fi, 3);
                    break;
                case fileWav:
                    baRet = getDataWav(fi, 3);
                    break;
                case fileCustom1:{
                    typedef QByteArray (*GetDataSound)(QFile*, quint16, InfoSound);
                    GetDataSound getDataSound = (GetDataSound) QLibrary::resolve(QCoreApplication::applicationDirPath() + "/extension_lecture",
                                                                             "getDataSound");
                    if (getDataSound)
                        baRet = getDataSound(fi, 3, _info);
                    else
                    {
                        baRet.resize(_info.dwLength*3);
                        baRet.fill(0);
                    }
                    }break;
                default:
                    QMessageBox::warning(_parent, QObject::trUtf8("Attention"), QObject::trUtf8("Fichier non pris en charge."));
                }
                // Fermeture du fichier
                fi->close();
            }
            delete fi;
        }
        break;
    case 32:
        // chargement en 32 bits
        if (!this->smpl.isEmpty() && !this->sm24.isEmpty() && _info.wBpsFile >= 24)
        {
            // copie 24 bits
            baRet.resize(_info.dwLength*4);
            char * cDest = baRet.data();
            char * cFrom = this->smpl.data();
            char * cFrom24 = this->smpl.data();
            int len = (int)_info.dwLength;
            for (int i = 0; i < len; i++)
            {
                cDest[4*i] = 0;
                cDest[4*i+1] = cFrom24[i];
                cDest[4*i+2] = cFrom[2*i];
                cDest[4*i+3] = cFrom[2*i+1];
            }
        }
        else if (!this->smpl.isEmpty() && _info.wBpsFile < 24)
        {
            // copie 16 bits
            baRet.resize(_info.dwLength*4);
            char * cDest = baRet.data();
            char * cFrom = this->smpl.data();
            int len = (int)_info.dwLength;
            for (int i = 0; i < len; i++)
            {
                cDest[4*i] = 0;
                cDest[4*i+1] = 0;
                cDest[4*i+2] = cFrom[2*i];
                cDest[4*i+3] = cFrom[2*i+1];
            }
        }
        else
        {
            // lecture fichier
            QFile *fi = new QFile(fileName);
            if (!fi->exists())
            {
                // Si impossible à ouvrir : pas de message d'erreur et remplissage 0
                baRet.resize(_info.dwLength*4);
                baRet.fill(0);
            }
            else
            {
                fi->open(QFile::ReadOnly | QFile::Unbuffered);
                switch (this->getFileType())
                {
                case fileSf2:
                    baRet = getDataSf2(fi, 4);
                    break;
                case fileWav:
                    baRet = getDataWav(fi, 4);
                    break;
                case fileCustom1:{
                    typedef QByteArray (*GetDataSound)(QFile*, quint16, InfoSound);
                    GetDataSound getDataSound = (GetDataSound) QLibrary::resolve(QCoreApplication::applicationDirPath() + "/extension_lecture",
                                                                             "getDataSound");
                    if (getDataSound)
                        baRet = getDataSound(fi, 4, _info);
                    else
                    {
                        baRet.resize(_info.dwLength*4);
                        baRet.fill(0);
                    }
                    }break;
                default:
                    QMessageBox::warning(_parent, QObject::trUtf8("Attention"), QObject::trUtf8("Fichier non pris en charge."));
                }
                // Fermeture du fichier
                fi->close();
            }
            delete fi;
        }
        break;
    default:
        QMessageBox::warning(_parent, QObject::trUtf8("Attention"), QObject::trUtf8("Erreur dans Sound::getData."));
    }
    return baRet;
}
quint32 Sound::get(Champ champ)
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
        result = _info.dwNote;
        break;
    case champ_chPitchCorrection:
        result = _info.iCent;
        break;
    default:
        break;
    }
    return result;
}
QString Sound::getFileName() {return this->fileName;}

void Sound::setData(QByteArray data, quint16 wBps)
{
    if (wBps == 8)
    {
        // Remplacement des données 17-24 bits
        this->sm24 = data;
    }
    else if (wBps == 16)
    {
        // Remplacement des données 16 bits
        this->smpl = data;
    }
    else
        QMessageBox::warning(_parent, "warning", "In Sound::setData, forbidden operation");
}
void Sound::set(Champ champ, Valeur value)
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
            this->sm24.clear();
        break;
    case champ_byOriginalPitch:
        // Modification de la note en demi tons
        _info.dwNote = value.bValue;
        break;
    case champ_chPitchCorrection:
        // Modification de la note en centième de ton
        _info.iCent = value.cValue;
        break;
    default:
        break;
    }
}
void Sound::setFileName(QString qStr, bool tryFindRootKey)
{
    this->fileName = qStr;

    // Récupération des infos sur le son
    this->getInfoSound(tryFindRootKey);
}
void Sound::setRam(bool ram)
{
    if (ram)
    {
        if (this->smpl.isEmpty())
        {
            // Chargement des données en ram, 16 bits de poids fort
            this->smpl = this->getData(16);
        }
        if (this->sm24.isEmpty() && _info.wBpsFile >= 24)
        {
            // chargement des 8 bits de poids faible
            this->sm24 = this->getData(8);
        };
    }
    else
    {
        // libération des données
        this->smpl.clear();
        this->sm24.clear();
    }
}

void Sound::exporter(QString fileName, Sound son)
{
    // Exportation d'un sample mono au format wav
    quint16 wBps = son._info.wBpsFile;
    if (wBps > 16)
        wBps = 24;
    else
        wBps = 16;
    QByteArray baData = son.getData(wBps);

    // Création d'un fichier
    InfoSound info = son._info;
    info.wBpsFile = wBps;
    info.wChannels = 1;
    exporter(fileName, baData, info);
}
void Sound::exporter(QString fileName, Sound son1, Sound son2)
{
    // Exportation d'un sample stereo au format wav
    // bps (max des 2 canaux)
    quint16 wBps = son1._info.wBpsFile;
    if (son2._info.wBpsFile > wBps)
        wBps = son2._info.wBpsFile;
    if (wBps > 16)
        wBps = 24;
    else
        wBps = 16;
    // Récupération des données
    QByteArray channel1 = son1.getData(wBps);
    QByteArray channel2 = son2.getData(wBps);
    // sample rate (max des 2 canaux)
    quint32 dwSmplRate = son1._info.dwSampleRate;
    if (son2._info.dwSampleRate > dwSmplRate)
    {
        // Ajustement son1
        channel1 = resampleMono(channel1, dwSmplRate, son2._info.dwSampleRate, wBps);
        dwSmplRate = son2._info.dwSampleRate;
    }
    else if (son2._info.dwSampleRate < dwSmplRate)
    {
        // Ajustement son2
        channel2 = resampleMono(channel2, son2._info.dwSampleRate, dwSmplRate, wBps);
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
    InfoSound info = son1._info;
    info.wBpsFile = wBps;
    info.dwSampleRate = dwSmplRate;
    info.wChannels = 2;
    exporter(fileName, baData, info);
}

////////////////////////////////////////////////////////////////////////////
//////////////////////////    METHODES PRIVEES    //////////////////////////
////////////////////////////////////////////////////////////////////////////

Sound::FileType Sound::getFileType()
{
    QFileInfo fileInfo = this->fileName;
    QString ext = fileInfo.suffix().toLower();

    QString extCustom = "";
    typedef QString (*MyPrototype)();
    MyPrototype myFunction = (MyPrototype) QLibrary::resolve(QCoreApplication::applicationDirPath() + "/extension_lecture",
                                                             "getExtension");
    if (myFunction)
        extCustom = myFunction();

    if (ext.compare("sf2") == 0 || ext.compare("sf3") == 0)
        return fileSf2;
    else if (ext.compare("wav") == 0)
        return fileWav;
    else if (ext.compare(extCustom) == 0 && !extCustom.isEmpty())
        return fileCustom1;
    else
        return fileUnknown;
}

void Sound::getInfoSound(bool tryFindRootkey)
{
    // Chargement des caractéristiques du sample
    switch (this->getFileType())
    {
    case fileWav:
        getInfoSoundWav(tryFindRootkey);
        break;
    case fileSf2:
        _info.wChannel = 0;
        _info.wChannels = 1;
        break;
    case fileCustom1:{
        typedef bool (*GetInfoSound)(QString, InfoSound*);
        GetInfoSound getInfoSound = (GetInfoSound) QLibrary::resolve(QCoreApplication::applicationDirPath() + "/extension_lecture",
                                                                 "getInfoSound");
        if (getInfoSound)
        {
            if (!getInfoSound(this->fileName, &_info))
            {
                _info.wFormat = 0;
                _info.dwLength = 0;
                _info.wChannel = 0;
                _info.wChannels = 0;
                _info.dwStart = 0;
                _info.dwStart2 = 0;
                _info.dwSampleRate = 0;
                _info.dwStartLoop = 0;
                _info.dwEndLoop = 0;
                _info.dwNote = 0;
                _info.iCent = 0;
                _info.wBpsFile = 0;
            }
        }
        }break;
    default:
        _info.wFormat = 0;
        _info.dwLength = 0;
        _info.wChannel = 0;
        _info.wChannels = 0;
        _info.dwStart = 0;
        _info.dwStart2 = 0;
        _info.dwSampleRate = 0;
        _info.dwStartLoop = 0;
        _info.dwEndLoop = 0;
        _info.dwNote = 0;
        _info.iCent = 0;
        _info.wBpsFile = 0;
    }
}

void Sound::getInfoSoundWav(bool tryFindRootkey)
{
    // Caractéristiques d'un fichier wav
    _info.wFormat = 0;
    _info.dwLength = 0;
    _info.dwSampleRate = 0;
    _info.dwStart = 0;
    _info.wBpsFile = 0;
    _info.wChannels = 0;
    _info.dwStartLoop = 0;
    _info.dwEndLoop = 0;
    _info.dwNote = 60; // Par défaut do central
    _info.iCent = 0;
    QFile fi(fileName);
    if (!fi.exists())
    {
        QMessageBox::warning(_parent, QObject::trUtf8("Attention"),
                             QObject::trUtf8("Impossible d'ouvrir le fichier") + " " + fileName);
        return;
    }
    fi.open(QFile::ReadOnly | QFile::Unbuffered);
    QByteArray baData = fi.readAll();
    fi.close();
    this->getInfoSoundWav(baData, tryFindRootkey);
}

void Sound::getInfoSoundWav(QByteArray baData, bool tryFindRootkey)
{
    int taille, pos;
    if (strcmp("RIFF", baData.left(4)))
    {
        QMessageBox::warning(_parent, QObject::trUtf8("Attention"), QObject::trUtf8("Le fichier est corrompu."));
        return;
    }
    // Taille totale du fichier - 8 octets
    taille = readDWORD(baData, 4);
    if (taille == 0)
    {
        QMessageBox::warning(_parent, QObject::trUtf8("Attention"), QObject::trUtf8("Le fichier est corrompu."));
        return;
    }
    taille = taille + 8;
    if (strcmp("WAVE", baData.mid(8, 4)))
    {
        QMessageBox::warning(_parent, QObject::trUtf8("Attention"), QObject::trUtf8("Le fichier est corrompu."));
        return;
    }
    pos = 12;
    bool rootKeyOk = false;
    while (pos < baData.size()-8)
    {
        QString section = baData.mid(pos, 4);
        pos += 4;
        int sectionSize = readDWORD(baData, pos);
        if (sectionSize % 2 != 0)
            sectionSize++;
        pos += 4;

        if (section == "fmt ")
        {
            // informations concernant le signal audio
            if (sectionSize < 16 || sectionSize > 40)
            {
                QMessageBox::warning(_parent, QObject::trUtf8("Attention"), QObject::trUtf8("Le fichier est corrompu."));
                return;
            }
            _info.wFormat = readWORD(baData, pos);
            _info.wChannels = readWORD(baData, pos + 2);
            _info.dwSampleRate = readDWORD(baData, pos + 4);
            _info.wBpsFile = readWORD(baData, pos + 14);
            if (_info.wBpsFile < 16)
                QMessageBox::warning(_parent, QObject::trUtf8("Attention"),
                                     QObject::trUtf8("Résolution insuffisante"));
        }
        else if (section == "smpl")
        {
            // informations sur le sample
            if (sectionSize >= 36)
            {
                // numéro note
                _info.dwNote = readDWORD(baData, pos + 12);
                if (_info.dwNote > 127)
                    _info.dwNote = 127;
                rootKeyOk = true;

                // accordage
                _info.iCent = (int)readDWORD(baData, pos + 16);
                _info.iCent = qRound((double)_info.iCent / 2147483648. * 50.);
            }
            if (sectionSize >= 60)
            {
                // boucle
                _info.dwStartLoop = readDWORD(baData, pos + 44);
                _info.dwEndLoop = readDWORD(baData, pos + 48) + 1;
            }
        }
        else if (section == "data")
        {
            if (sectionSize == 0) sectionSize = baData.size() - pos;
            _info.dwStart = pos;
            if (_info.wBpsFile != 0 && _info.wChannels != 0)
                _info.dwLength = qMin(sectionSize, baData.size() - pos) / (_info.wBpsFile * _info.wChannels / 8);
        }

        // Mise à jour de la position
        pos += sectionSize;
    }
    if (!rootKeyOk && tryFindRootkey)
        determineRootKey();
}

void Sound::determineRootKey()
{
    // Détermination de la note à partir du nom de fichier
    int note = -1;
    for (int i = 0; i <= 127; i++)
    {
        // Nom de la note
        QString nomNote = Config::getInstance()->getKeyName(i, true);

        // Recherche de la note dans le nom de fichier
        if (fileName.toUpper().contains(nomNote))
            note = i;
    }

    if (note == -1)
    {
        // Recherche du numéro de la note
        QString name = fileName;
        QStringList listeNum = name.replace(QRegExp("[^0-9]"), "-").split("-", QString::SkipEmptyParts);
        if (listeNum.size())
        {
            // 0n étudie le dernier
            bool ok;
            int numNote = listeNum.last().toInt(&ok);
            if (ok)
            {
                if (numNote >= 0 && numNote < 128)
                    note = numNote;
            }
        }
    }

    if (note != -1)
        _info.dwNote = note;
}

QByteArray Sound::getDataSf2(QFile * fi, quint16 byte)
{
    QByteArray baRet;

    // Fichier sf2
    switch (byte)
    {
    case 1:
        baRet.resize(_info.dwLength);

        // Chargement 8 bits poids faible
        if (_info.wBpsFile >= 24 && _info.dwLength)
        {
            // Copie des données
            fi->seek(_info.dwStart2);  // saut
            baRet = fi->read(_info.dwLength);
        }
        else
        {
            // Remplissage de 0
            baRet.fill(0);
        }
        break;
    case 2:
        baRet.resize(_info.dwLength*2);

        // Chargement 16 bits poids fort
        if (_info.dwLength)
        {
            // Copie des données
            fi->seek(_info.dwStart);  // saut
            baRet = fi->read(_info.dwLength*2);
        }
        else
        {
            // remplissage de 0
            baRet.fill(0);
        }
        break;
    case 3:
        baRet.resize(_info.dwLength*3);

        // Chargement des 24 bits
        if (_info.dwLength)
        {
            char * data = baRet.data();
            QByteArray baTmp;

            // Assemblage des données, partie 16 bits
            fi->seek(_info.dwStart);  // saut
            baTmp.resize(_info.dwLength*2);
            baTmp = fi->read(_info.dwLength*2);
            const char * dataTmp = baTmp.constData();
            for (quint32 i = 0; i < _info.dwLength; i++)
            {
                data[3*i+1] = dataTmp[2*i];
                data[3*i+2] = dataTmp[2*i+1];
            }
            // Partie 24 bits
            if (_info.wBpsFile >= 24)
            {
                fi->seek(_info.dwStart2);  // saut
                baTmp.resize(_info.dwLength);
                baTmp = fi->read(_info.dwLength);
                dataTmp = baTmp.constData();
                for (quint32 i = 0; i < _info.dwLength; i++)
                    data[3*i] = dataTmp[i];
            }
            else
            {
                // Remplissage de 0
                for (quint32 i = 0; i < _info.dwLength; i++)
                    data[3*i] = 0;
            }
        }
        else
        {
            // remplissage de 0
            baRet.fill(0);
        }
        break;
    case 4:
        baRet.resize(_info.dwLength*4);

        // Chargement des 24 bits
        if (_info.dwLength)
        {
            char * data = baRet.data();
            QByteArray baTmp;

            // Assemblage des données, partie 16 bits
            fi->seek(_info.dwStart);  // saut
            baTmp.resize(_info.dwLength*2);
            baTmp = fi->read(_info.dwLength*2);
            const char * dataTmp = baTmp.constData();
            for (quint32 i = 0; i < _info.dwLength; i++)
            {
                data[4*i+2] = dataTmp[2*i];
                data[4*i+3] = dataTmp[2*i+1];
            }
            // Partie 24 bits
            if (_info.wBpsFile >= 24)
            {
                fi->seek(_info.dwStart2);  // saut
                baTmp.resize(_info.dwLength);
                baTmp = fi->read(_info.dwLength);
                dataTmp = baTmp.constData();
                for (quint32 i = 0; i < _info.dwLength; i++)
                    data[4*i+1] = dataTmp[i];
            }
            else
            {
                // Remplissage de 0
                for (quint32 i = 0; i < _info.dwLength; i++)
                    data[4*i+1] = 0;
            }
            // Partie 32 bits
            for (quint32 i = 0; i < _info.dwLength; i++)
                data[4*i] = 0;
        }
        else
        {
            // remplissage de 0
            baRet.fill(0);
        }
        break;
    }
    return baRet;
}
QByteArray Sound::getDataWav(QFile * fi, quint16 byte)
{
    // Fichier wav
    int byte_fichier = _info.wBpsFile / 8;

    // Saut de l'entête + positionnement sur le canal
    fi->seek(_info.dwStart + _info.wChannel * byte_fichier);

    // Lecture des données
    QByteArray baTmp = fi->read(_info.dwLength * byte_fichier * _info.wChannels);
    return this->getDataWav(baTmp, byte);
}

QByteArray Sound::getDataWav(QByteArray baData, quint16 byte)
{
    // Fichier wav
    QByteArray baRet;
    baRet.resize(_info.dwLength * byte);
    char * data = baRet.data();
    int a, b;
    int byte_fichier = _info.wBpsFile / 8;
    if (_info.wFormat == 3 && byte_fichier == 4)
    {
        // conversion WAVE_FORMAT_IEEE_FLOAT -> PCM 32
        float * dataF = (float *)baData.data();
        qint32 * data32 = (qint32 *)baData.data();
        for (int i = 0; i < baData.size() / 4; i++)
            data32[i] = (qint32) (dataF[i] * 2147483647);
    }
    const char * dataTmp = baData.constData();
    b = _info.wChannels * byte_fichier;

    // remplissage data
    switch (byte)
    {
    case 1:
        // 8 bits (après 16)
        if (byte_fichier < 3)
            baRet.fill(0); // Résolution insuffisante, remplissage de zéros
        else
        {
            a = byte_fichier - 3;
            for (quint32 i = 0; i < _info.dwLength; i++)
                data[i] = dataTmp[a + b * i];
        }
        break;
    case 2:
        // 16 bits
        if (byte_fichier < 2)
        {
            // Résolution insuffisante
            for (quint32 i = 0; i < _info.dwLength; i++)
            {
                data[byte*i] = 0;
                data[byte*i+1] = dataTmp[b * i];
            }
        }
        else
        {
            a = byte_fichier - byte;
            for (quint32 i = 0; i < _info.dwLength; i++)
            {
                data[byte*i] = dataTmp[a + b * i];
                data[byte*i+1] = dataTmp[1 + a + b * i];
            }
        }
        break;
    case 3:
        // 24 bits
        if (byte_fichier == 1)
        {
            for (quint32 i = 0; i < _info.dwLength; i++)
            {
                data[byte*i] = 0;
                data[byte*i+1] = 0;
                data[byte*i+2] = dataTmp[b * i];
            }
        }
        else if (byte_fichier == 2)
        {
            for (quint32 i = 0; i < _info.dwLength; i++)
            {
                data[byte*i] = 0;
                data[byte*i+1] = dataTmp[b * i];
                data[byte*i+2] = dataTmp[1 + b * i];
            }
        }
        else
        {
            a = byte_fichier - byte;
            for (quint32 i = 0; i < _info.dwLength; i++)
            {
                data[byte*i] = dataTmp[a + b * i];
                data[byte*i+1] = dataTmp[1 + a + b * i];
                data[byte*i+2] = dataTmp[2 + a + b * i];
            }
        }
        break;
    case 4:
        // 32 bits
        if (byte_fichier == 1)
        {
            for (quint32 i = 0; i < _info.dwLength; i++)
            {
                data[byte*i] = 0;
                data[byte*i+1] = 0;
                data[byte*i+2] = 0;
                data[byte*i+3] = dataTmp[b * i];
            }
        }
        else if (byte_fichier == 2)
        {
            for (quint32 i = 0; i < _info.dwLength; i++)
            {
                data[byte*i] = 0;
                data[byte*i+1] = 0;
                data[byte*i+2] = dataTmp[b * i];
                data[byte*i+3] = dataTmp[1 + b * i];
            }
        }
        else if (byte_fichier == 3)
        {
            for (quint32 i = 0; i < _info.dwLength; i++)
            {
                data[byte*i] = 0;
                data[byte*i+1] = dataTmp[b * i];
                data[byte*i+2] = dataTmp[1 + b * i];
                data[byte*i+3] = dataTmp[2 + b * i];
            }
        }
        else
        {
            a = byte_fichier - byte;
            for (quint32 i = 0; i < _info.dwLength; i++)
            {
                data[byte*i] = dataTmp[a + b * i];
                data[byte*i+1] = dataTmp[1 + a + b * i];
                data[byte*i+2] = dataTmp[2 + a + b * i];
                data[byte*i+3] = dataTmp[3 + a + b * i];
            }
        }
        break;
    }
    return baRet;
}

void Sound::exporter(QString fileName, QByteArray baData, InfoSound info)
{
    // Création d'un fichier
    QFile fi(fileName);
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
    quint32 dwLength = baData.size();
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
    out << (quint32)info.dwSampleRate;
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
    if (info.iCent > 50)
    {
        dwTemp = qMin((quint32)127, info.dwNote + 1);
        out << dwTemp;
        dwTemp = qRound(((double)(info.iCent - 50) / 50.) * 2147483648.);
        out << dwTemp;
    }
    else if (info.iCent < -50)
    {
        dwTemp = qMax((quint32)0, info.dwNote - 1);
        out << dwTemp;
        dwTemp = qRound(((double)(info.iCent + 50) / 50.) * 2147483648.);
        out << dwTemp;
    }
    else
    {
        dwTemp = info.dwNote;
        out << dwTemp;
        dwTemp = qRound(((double)info.iCent / 50.) * 2147483648.);
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

////////////////////////////////////////////////////////////////////////////
//////////////////////////       UTILITAIRES      //////////////////////////
////////////////////////////////////////////////////////////////////////////

QByteArray Sound::resampleMono(QByteArray baData, double echInit, qint32 echFinal, quint16 wBps)
{
    // Paramètres
    double alpha = 3;
    qint32 nbPoints = 10;

    // Préparation signal d'entrée
    baData = bpsConversion(baData, wBps, 32);
    if (echFinal < echInit)
    {
        // Filtre passe bas (voir sinc filter)
        baData = Sound::bandFilter(baData, 32, echInit, echFinal/2, 0, -1);
    }
    qint32 sizeInit = baData.size() / 4;
    qint32 * dataI = (qint32*)baData.data();
    double * data = new double[sizeInit]; // utilisation de new sinon possibilité de dépasser une limite de mémoire
    for (int i = 0; i < sizeInit; i++)
        data[i] = (double)dataI[i] / 2147483648.;

    // Création fenêtre Kaiser-Bessel 2048 points
    double kbdWindow[2048];
    KBDWindow(kbdWindow, 2048, alpha);

    // Nombre de points à trouver
    qint32 sizeFinal = (double)(sizeInit - 1.0) * (double)echFinal / echInit + 1;
    double * dataRet = new double[sizeFinal]; // utilisation de new : même raison

    // Calcul des points par interpolation à bande limitée
    double pos, delta;
    qint32 pos1, pos2;
    double * sincCoef = new double[1 + 2 * nbPoints];
    double valMax = 0;
    for (qint32 i = 0; i < sizeFinal; i++)
    {
        // Position à interpoler
        pos = (echInit * i) / (double)echFinal;
        // Calcul des coefs
        for (qint32 j = -nbPoints; j <= nbPoints; j++)
        {
            delta = pos - floor(pos);
            // Calcul sinus cardinal
            sincCoef[j + nbPoints] = sinc(M_PI * ((double)j - delta));
            // Application fenêtre
            delta = (double)(j + nbPoints - delta) / (1 + 2 * nbPoints) * 2048;
            pos1 = qMax(0., qMin(floor(delta), 2047.)) + .5;
            pos2 = qMax(0., qMin(ceil (delta), 2047.)) + .5;
            sincCoef[j + nbPoints] *= kbdWindow[pos1] * (ceil((delta)) - delta)
                    + kbdWindow[pos2] * (1. - ceil((delta)) + delta);
        }
        // Valeur
        dataRet[i] = 0;
        for (int j = qMax(0, (qint32)pos - nbPoints); j <= qMin(sizeInit-1, (qint32)pos + nbPoints); j++)
            dataRet[i] += sincCoef[j - (qint32)pos + nbPoints] * data[j];

        valMax = qMax(valMax, qAbs(dataRet[i]));
    }
    delete [] sincCoef;

    // Passage qint32 et limitation si besoin
    QByteArray baRet;
    baRet.resize(sizeFinal * 4);
    qint32 * dataRetI = (qint32*)baRet.data();
    double coef;
    if (valMax > 1)
        coef = 2147483648. / valMax;
    else
        coef = 2147483648LL;
    for (int i = 0; i < sizeFinal; i++)
        dataRetI[i] = dataRet[i] * coef;

    delete dataRet;
    delete data;

    // Filtre passe bas après resampling
    baRet = Sound::bandFilter(baRet, 32, echFinal, echFinal/2, 0, -1);
    baRet = bpsConversion(baRet, 32, wBps);
    return baRet;
}
QByteArray Sound::bandFilter(QByteArray baData, quint16 wBps, double dwSmplRate, double fBas, double fHaut, int ordre)
{
    /******************************************************************************
     ***********************    passe_bande_frequentiel    ************************
     ******************************************************************************
     * But :
     *  - filtre un signal par un filtre passe-bande de Butterworth
     * Entrees :
     *  - baData : tableau contenant les donnees a filtrer
     *  - dwSmplRate : frequence d'echantillonnage du signal
     *  - fHaut : frequence de coupure du passe-haut
     *  - fBas : frequence de coupure du passe-bas
     *  - ordre : ordre du filtre
     * Sorties :
     *  - tableau contenant les donnees apres filtrage
     ******************************************************************************/

    // Paramètres valides ?
    if (!dwSmplRate || (fHaut <= 0 && fBas <= 0) || 2 * fHaut > dwSmplRate || 2 * fBas > dwSmplRate)
    {
        // Controle des fréquences de coupures (il faut que Fc<Fe/2 )
        QMessageBox::warning(_parent, "warning", "Sound::bandFilter, bad values");
    }
    else
    {
        unsigned long size;
        // Conversion de baData en complexes
        Complex * cpxData;
        cpxData = fromBaToComplex(baData, wBps, size);
        // Calculer la fft du signal
        Complex * fc_sortie_fft = FFT(cpxData, size);
        delete [] cpxData;
        // Convoluer par le filtre Butterworth d'ordre 4, applique dans le sens direct et retrograde
        // pour supprimer la phase (Hr4 * H4 = Gr4 * G4 = (G4)^2)
        double d_gain_ph, d_gain_pb;
        if (fHaut <= 0)
        {
            double pos;
            // Filtre passe bas uniquement
            if (ordre == -1)
            {
                // "Mur de brique"
                for (unsigned long i = 0; i < (size+1)/2; i++)
                {
                    pos = i / ((double)size-1);
                    fc_sortie_fft[i] *= (pos * dwSmplRate) < fBas;
                    fc_sortie_fft[size-1-i] *= (pos * dwSmplRate) < fBas;
                }
            }
            else
            {
                for (unsigned long i = 0; i < (size+1)/2; i++)
                {
                    pos = i / ((double)size-1);
                    d_gain_pb = 1.0/(1.0 + pow(pos * dwSmplRate / fBas, 2 * ordre));
                    fc_sortie_fft[i] *= d_gain_pb;
                    fc_sortie_fft[size-1-i] *= d_gain_pb;
                }
            }
        }
        else if (fBas <= 0)
        {
            double pos;
            // Filtre passe haut uniquement
            if (ordre == -1)
            {
                // "Mur de brique"
                for (unsigned long i = 0; i < (size+1)/2; i++)
                {
                    pos = i / ((double)size-1);
                    fc_sortie_fft[i] *= (pos * dwSmplRate) > fHaut;
                    fc_sortie_fft[size-1-i] *= (pos * dwSmplRate) > fHaut;
                }
            }
            else
            {
                for (unsigned long i = 0; i < (size+1)/2; i++)
                {
                    pos = i / ((double)size-1);
                    d_gain_ph = 1 - (1.0 / (1.0 + pow((pos * dwSmplRate) / fHaut, 2 * ordre)));
                    fc_sortie_fft[i] *= d_gain_ph;
                    fc_sortie_fft[size-1-i] *= d_gain_ph;
                }
            }
        }
        else
        {
            double pos;
            // Filtre passe bande
            for (unsigned long i = 0; i < (size+1)/2; i++)
            {
                pos = i / ((double)size-1);
                d_gain_ph = 1 - (1.0 / (1.0 + pow((pos * dwSmplRate) / fHaut, 2 * ordre)));
                d_gain_pb = 1.0/(1.0 + pow(pos * dwSmplRate / fBas, 2 * ordre));
                fc_sortie_fft[i] *= d_gain_ph * d_gain_pb;
                fc_sortie_fft[size-1-i] *= d_gain_ph * d_gain_pb;
            }
        }
        // Calculer l'ifft du signal
        cpxData = IFFT(fc_sortie_fft, size);
        delete [] fc_sortie_fft;
        // Prise en compte du facteur d'echelle
        for (unsigned long i = 0; i < size; i++)
            cpxData[i].real(cpxData[i].real() / size);
        // Retour en QByteArray
        QByteArray baRet;
        baRet = fromComplexToBa(cpxData, (long int)baData.size() * 8 / wBps, wBps);
        delete [] cpxData;
        return baRet;
    }
    return baData;
}

QByteArray Sound::EQ(QByteArray baData, quint32 dwSmplRate, quint16 wBps, int i1, int i2, int i3, int i4, int i5,
                         int i6, int i7, int i8, int i9, int i10)
{
    unsigned long size;
    // Conversion de baData en complexes
    Complex * cpxData;
    cpxData = fromBaToComplex(baData, wBps, size);
    // Calculer la fft du signal
    Complex * fc_sortie_fft = FFT(cpxData, size);
    delete [] cpxData;
    // Filtrage
    double freq;
    double gain;
    for (unsigned long i = 0; i < (size+1)/2; i++)
    {
        freq = (double)(i * dwSmplRate) / (size-1);
        gain = gainEQ(freq, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10);
        fc_sortie_fft[i] *= gain;
        fc_sortie_fft[size-1-i] *= gain;
    }
    // Calculer l'ifft du signal
    cpxData = IFFT(fc_sortie_fft, size);
    delete [] fc_sortie_fft;
    // Prise en compte du facteur d'echelle
    for (unsigned long i = 0; i < size; i++)
        cpxData[i].real(cpxData[i].real() / size);
    // Retour en QByteArray
    QByteArray baRet;
    baRet = fromComplexToBa(cpxData, baData.size() * 8 / wBps, wBps);
    delete [] cpxData;
    return baRet;
}

Complex * Sound::FFT(Complex * x, int N)
{
    Complex* out = new Complex[N];
    Complex* scratch = new Complex[N];
    Complex* twiddles = new Complex [N];
    int k;
    for (k = 0; k != N; ++k)
    {
        twiddles[k].real(cos(-2.0 * M_PI * k / N));
        twiddles[k].imag(sin(-2.0 * M_PI * k / N));
    }
    FFT_calculate(x, N, out, scratch, twiddles);
    delete [] twiddles;
    delete [] scratch;
    return out;
}
Complex * Sound::IFFT(Complex * x, int N)
{
    Complex * out = new Complex[N];
    Complex * scratch = new Complex[N];
    Complex * twiddles = new Complex [N];
    int k;
    for (k = 0; k != N; ++k)
    {
        twiddles[k].real(cos(2.0 * M_PI * k / N));
        twiddles[k].imag(sin(2.0 * M_PI * k / N));
    }
    FFT_calculate(x, N, out, scratch, twiddles);
    delete [] twiddles;
    delete [] scratch;
    return out;
}
void Sound::bpsConversion(char *cDest, const char *cFrom, qint32 size, quint16 wBpsInit, quint16 wBpsFinal, bool bigEndian)
{
    // Conversion entre formats 32, 24 et 16 bits
    // Particularité : demander format 824 bits renvoie les 8 bits de poids faible
    //                 dans les 24 bits de poids fort

    // Remplissage
    switch (wBpsInit)
    {
    case 8:
        switch (wBpsFinal)
        {
        case 824:
            // Remplissage de 0
            for (int i = 0; i < size; i++)
                cDest[i] = 0;
            break;
        case 8:
            for (int i = 0; i < size; i++)
                cDest[i] = cFrom[i];
            break;
        case 16:
            if (bigEndian)
            {
                for (int i = 0; i < size; i++)
                {
                    cDest[2*i+1] = 0;
                    cDest[2*i] = cFrom[i];
                }
            }
            else
            {
                for (int i = 0; i < size; i++)
                {
                    cDest[2*i] = 0;
                    cDest[2*i+1] = cFrom[i];
                }
            }
            break;
        case 24:
            if (bigEndian)
            {
                for (int i = 0; i < size; i++)
                {
                    cDest[3*i+2] = 0;
                    cDest[3*i+1] = 0;
                    cDest[3*i] = cFrom[i];
                }
            }
            else
            {
                for (int i = 0; i < size; i++)
                {
                    cDest[3*i] = 0;
                    cDest[3*i+1] = 0;
                    cDest[3*i+2] = cFrom[i];
                }
            }
            break;
        case 32:
            if (bigEndian)
            {
                for (int i = 0; i < size; i++)
                {
                    cDest[4*i+3] = 0;
                    cDest[4*i+2] = 0;
                    cDest[4*i+1] = 0;
                    cDest[4*i] = cFrom[i];
                }
            }
            else
            {
                for (int i = 0; i < size; i++)
                {
                    cDest[4*i] = 0;
                    cDest[4*i+1] = 0;
                    cDest[4*i+2] = 0;
                    cDest[4*i+3] = cFrom[i];
                }
            }
            break;
        }
        break;
    case 16:
        switch (wBpsFinal)
        {
        case 824:
            // Remplissage de 0
            for (int i = 0; i < size/2; i++)
                cDest[i] = 0;
            break;
        case 8:
            for (int i = 0; i < size/2; i++)
                cDest[i] = cFrom[2*i+1];
            break;
        case 16:
            if (bigEndian)
            {
                for (int i = 0; i < size/2; i++)
                {
                    cDest[2*i+1] = cFrom[2*i];
                    cDest[2*i] = cFrom[2*i+1];
                }
            }
            else
                for (int i = 0; i < size; i++)
                    cDest[i] = cFrom[i];
            break;
        case 24:
            if (bigEndian)
            {
                for (int i = 0; i < size/2; i++)
                {
                    cDest[3*i+2] = 0;
                    cDest[3*i+1] = cFrom[2*i];
                    cDest[3*i] = cFrom[2*i+1];
                }
            }
            else
            {
                for (int i = 0; i < size/2; i++)
                {
                    cDest[3*i] = 0;
                    cDest[3*i+1] = cFrom[2*i];
                    cDest[3*i+2] = cFrom[2*i+1];
                }
            }
            break;
        case 32:
            if (bigEndian)
            {
                for (int i = 0; i < size/2; i++)
                {
                    cDest[4*i+3] = 0;
                    cDest[4*i+2] = 0;
                    cDest[4*i+1] = cFrom[2*i];
                    cDest[4*i] = cFrom[2*i+1];
                }
            }
            else
            {
                for (int i = 0; i < size/2; i++)
                {
                    cDest[4*i] = 0;
                    cDest[4*i+1] = 0;
                    cDest[4*i+2] = cFrom[2*i];
                    cDest[4*i+3] = cFrom[2*i+1];
                }
            }
            break;
        }
        break;
    case 24:
        switch (wBpsFinal)
        {
        case 824:
            // 8 bits de poids faible
            for (int i = 0; i < size/3; i++)
                cDest[i] = cFrom[3*i];
            break;
        case 8:
            for (int i = 0; i < size/3; i++)
                cDest[i] = cFrom[3*i+2];
            break;
        case 16:
            if (bigEndian)
            {
                for (int i = 0; i < size/3; i++)
                {
                    cDest[2*i+1] = cFrom[3*i+1];
                    cDest[2*i] = cFrom[3*i+2];
                }
            }
            else
            {
                for (int i = 0; i < size/3; i++)
                {
                    cDest[2*i] = cFrom[3*i+1];
                    cDest[2*i+1] = cFrom[3*i+2];
                }
            }
            break;
        case 24:
            if (bigEndian)
            {
                for (int i = 0; i < size/3; i++)
                {
                    cDest[3*i+2] = cFrom[3*i];
                    cDest[3*i+1] = cFrom[3*i+1];
                    cDest[3*i] = cFrom[3*i+2];
                }
            }
            else
                for (int i = 0; i < size; i++)
                    cDest[i] = cFrom[i];
            break;
        case 32:
            if (bigEndian)
            {
                for (int i = 0; i < size/3; i++)
                {
                    cDest[4*i+3] = 0;
                    cDest[4*i+2] = cFrom[3*i];
                    cDest[4*i+1] = cFrom[3*i+1];
                    cDest[4*i] = cFrom[3*i+2];
                }
            }
            else
            {
                for (int i = 0; i < size/3; i++)
                {
                    cDest[4*i] = 0;
                    cDest[4*i+1] = cFrom[3*i];
                    cDest[4*i+2] = cFrom[3*i+1];
                    cDest[4*i+3] = cFrom[3*i+2];
                }
            }
            break;
        }
        break;
    case 32:
        switch (wBpsFinal)
        {
        case 824:
            // 8 bits poids faible après 16
            for (int i = 0; i < size/4; i++)
                cDest[i] = cFrom[4*i+1];
            break;
        case 8:
            for (int i = 0; i < size/4; i++)
                cDest[i] = cFrom[4*i+3];
            break;
        case 16:
            if (bigEndian)
            {
                for (int i = 0; i < size/4; i++)
                {
                    cDest[2*i+1] = cFrom[4*i+2];
                    cDest[2*i] = cFrom[4*i+3];
                }
            }
            else
            {
                for (int i = 0; i < size/4; i++)
                {
                    cDest[2*i] = cFrom[4*i+2];
                    cDest[2*i+1] = cFrom[4*i+3];
                }
            }
            break;
        case 24:
            if (bigEndian)
            {
                for (int i = 0; i < size/4; i++)
                {
                    cDest[3*i+2] = cFrom[4*i+1];
                    cDest[3*i+1] = cFrom[4*i+2];
                    cDest[3*i] = cFrom[4*i+3];
                }
            }
            else
            {
                for (int i = 0; i < size/4; i++)
                {
                    cDest[3*i] = cFrom[4*i+1];
                    cDest[3*i+1] = cFrom[4*i+2];
                    cDest[3*i+2] = cFrom[4*i+3];
                }
            }
            break;
        case 32:
            if (bigEndian)
            {
                for (int i = 0; i < size/4; i++)
                {
                    cDest[4*i+3] = cFrom[4*i];
                    cDest[4*i+2] = cFrom[4*i+1];
                    cDest[4*i+1] = cFrom[4*i+2];
                    cDest[4*i] = cFrom[4*i+3];
                }
            }
            else
                for (int i = 0; i < size; i++)
                    cDest[i] = cFrom[i];
            break;
        }
        break;
    }
}

QByteArray Sound::bpsConversion(QByteArray baData, quint16 wBpsInit, quint16 wBpsFinal, bool bigEndian)
{
    // Conversion entre formats 32, 24 et 16 bits
    // Particularité : demander format 824 bits renvoie les 8 bits de poids faible
    //                 dans les 24 bits de poids fort
    int size = baData.size();
    // Données de retour
    QByteArray baRet;
    // Redimensionnement
    int i = 1;
    int j = 1;
    switch (wBpsInit)
    {
    case 16: i = 2; break;
    case 24: i = 3; break;
    case 32: i = 4; break;
    default: i = 1;
    }
    switch (wBpsFinal)
    {
    case 16: j = 2; break;
    case 24: j = 3; break;
    case 32: j = 4; break;
    default: j = 1;
    }
    baRet.resize((size * j) / i);
    // Remplissage
    char * cDest = baRet.data();
    const char * cFrom = baData.constData();
    bpsConversion(cDest, cFrom, size, wBpsInit, wBpsFinal, bigEndian);
    return baRet;
}

QByteArray Sound::from2MonoTo1Stereo(QByteArray baData1, QByteArray baData2, quint16 wBps, bool bigEndian)
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
    else
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

Complex * Sound::fromBaToComplex(QVector<float> fData, long unsigned int &size)
{
    // Nombre de données (puissance de 2 la plus proche)
    int nb = ceil(qLn(fData.size()) / 0.69314718056);
    size = 1;
    for (int i = 0; i < nb; i++)
        size *= 2;

    // Création et remplissage d'un tableau de complexes
    Complex * cpxData = new Complex[size];

    // Remplissage
    for (int i = 0; i < fData.size(); i++)
    {
        cpxData[i].real(fData[i]);
        cpxData[i].imag(0);
    }

    // On complète avec des 0
    for (unsigned int i = fData.size(); i < size; i++)
    {
        cpxData[i].real(0);
        cpxData[i].imag(0);
    }

    return cpxData;
}

Complex * Sound::fromBaToComplex(QByteArray baData, quint16 wBps, long unsigned int &size)
{
    Complex * cpxData;
    // Création et remplissage d'un tableau de complexes
    if (wBps == 16)
    {
        qint16 * data = (qint16 *)baData.data();

        // Nombre de données (puissance de 2 la plus proche)
        int nb = ceil(qLn(baData.size()/2) / 0.69314718056 /* ln(2) */);
        size = 1;
        for (int i = 0; i < nb; i++) size *= 2;

        // Remplissage
        cpxData = new Complex[size];
        for (int i = 0; i < baData.size()/2; i++)
        {
            cpxData[i].real(data[i]);
            cpxData[i].imag(0);
        }

        // On complète avec des 0
        for (unsigned int i = baData.size()/2; i < size; i++)
        {
            cpxData[i].real(0);
            cpxData[i].imag(0);
        }
    }
    else
    {
        // Passage 32 bits si nécessaire
        if (wBps == 24)
            baData = bpsConversion(baData, 24, 32);
        qint32 * data = (qint32 *)baData.data();

        // Nombre de données (puissance de 2 la plus proche)
        int nb = ceil(qLn(baData.size()/4) / 0.69314718056 /* ln(2) */);
        size = 1;
        for (int i = 0; i < nb; i++) size *= 2;

        // Remplissage
        cpxData = new Complex[size];
        for (int i = 0; i < baData.size()/4; i++)
        {
            cpxData[i].real(data[i]);
            cpxData[i].imag(0);
        }

        // On complète avec des 0
        for (unsigned int i = baData.size()/4; i < size; i++)
        {
            cpxData[i].real(0);
            cpxData[i].imag(0);
        }
    }
    return cpxData;
}

QByteArray Sound::fromComplexToBa(Complex * cpxData, int size, quint16 wBps)
{
    QByteArray baData;
    if (wBps == 16)
    {
        // Calcul du maximum
        quint64 valMax = 0;
        for (int i = 0; i < size; i++)
            valMax = qMax(valMax, (quint64)qAbs(cpxData[i].real()));
        // Atténuation si dépassement de la valeur max
        double att = 1;
        if (valMax > 32700)
            att = 32700. / valMax;
        // Conversion qint16
        baData.resize(size*2);
        qint16 * data = (qint16 *)baData.data();
        for (int i = 0; i < size; i++)
            data[i] = (qint16)(cpxData[i].real() * att);
    }
    else
    {
        // Calcul du maximum
        quint64 valMax = 0;
        for (int i = 0; i < size; i++)
            valMax = qMax(valMax, (quint64)qAbs(cpxData[i].real()));
        // Atténuation si dépassement de la valeur max
        double att = 1;
        if (valMax > 2147483000)
            att = 2147483000. / valMax;
        // Conversion qint32
        baData.resize(size*4);
        qint32 * data = (qint32 *)baData.data();
        for (int i = 0; i < size; i++)
            data[i] = (qint32)(cpxData[i].real() * att);
        // Conversion 24 bits
        if (wBps == 24)
            baData = bpsConversion(baData, 32, 24);
    }
    return baData;
}

QByteArray Sound::normaliser(QByteArray baData, double dVal, quint16 wBps, double &db)
{
    // Conversion 32 bits si nécessaire
    if (wBps != 32)
        baData = bpsConversion(baData, wBps, 32);
    // Calcul valeur max
    qint32 * data = (qint32 *)baData.data();
    qint32 valMax = 0;
    for (int i = 0; i < baData.size()/4; i++) valMax = qMax(valMax, qAbs(data[i]));
    // Calcul amplification
    double mult = dVal * (double)2147483648LL / valMax;
    db = 20.0 * log10(mult);
    // Amplification
    for (int i = 0; i < baData.size()/4; i++) data[i] *= mult;
    // Conversion format d'origine si nécessaie
    if (wBps != 32)
        baData = bpsConversion(baData, 32, wBps);
    return baData;
}

QByteArray Sound::multiplier(QByteArray baData, double dMult, quint16 wBps, double &db)
{
    // Conversion 32 bits si nécessaire
    if (wBps != 32)
        baData = bpsConversion(baData, wBps, 32);
    // Calcul amplification
    db = 20.0 * log10(dMult);
    // Amplification
    qint32 * data = (qint32 *)baData.data();
    for (int i = 0; i < baData.size()/4; i++) data[i] *= dMult;
    // Conversion format d'origine si nécessaie
    if (wBps != 32)
        baData = bpsConversion(baData, 32, wBps);
    return baData;
}

QByteArray Sound::enleveBlanc(QByteArray baData, double seuil, quint16 wBps, quint32 &pos)
{
    // Conversion 32 bits si nécessaire
    if (wBps != 32)
        baData = bpsConversion(baData, wBps, 32);

    // Calcul de la moyenne des valeurs absolues
    int size = baData.size() / 4;
    QVector<float> fData;
    fData.resize(size);

    qint32 * iData = (qint32*)baData.data();
    for (int i = 0; i < size; i++)
        fData[i] = (float)iData[i];

    for (int i = 0; i < size; i++)
        fData[i] = qAbs(fData[i]);
    float median = mediane(fData);

    // Calcul du nombre d'éléments à sauter
    while ((signed)pos < size - 1 && (iData[pos] < seuil * median))
        pos++;

    // Saut
    if ((signed)pos < baData.size()/4 - 1)
        baData = baData.mid(pos * 4, baData.size() - 4 * pos);

    // Conversion format d'origine si nécessaie
    if (wBps != 32)
        baData = bpsConversion(baData, 32, wBps);
    return baData;
}

void Sound::regimePermanent(QByteArray baData, quint32 dwSmplRate, quint16 wBps, qint32 &posStart, qint32 &posEnd)
{
    // Recherche d'un régiment permanent (sans attaque ni release)
    if (wBps != 32)
        baData = bpsConversion(baData, wBps, 32);
    qint32 size = baData.size() / 4;

    QVector<float> fData;
    fData.resize(size);
    qint32 * iData = (qint32*)baData.data();
    for (int i = 0; i < size; i++)
        fData[i] = (float)iData[i];

    regimePermanent(fData, dwSmplRate, posStart, posEnd);
}

void Sound::regimePermanent(QVector<float> fData, quint32 dwSmplRate, qint32 &posStart, qint32 &posEnd)
{
    int size = fData.size();

    // Recherche fine
    regimePermanent(fData, dwSmplRate, posStart, posEnd, 10, 1.05);
    if (posEnd - posStart < size / 2)
    {
        // Recherche grossière
        regimePermanent(fData, dwSmplRate, posStart, posEnd, 7, 1.2);
        if (posEnd - posStart < size / 2)
        {
            // Recherche très grossière
            regimePermanent(fData, dwSmplRate, posStart, posEnd, 4, 1.35);
            if (posEnd - posStart < size / 2)
            {
                // moitié du milieu
                posStart = size / 4;
                posEnd = size * 3 / 4;
            }
        }
    }
}

QVector<float> Sound::correlation(const QVector<float> fData, quint32 dwSmplRate, qint32 fMin, qint32 fMax, qint32 &dMin)
{
    // Décalage max (fréquence basse)
    qint32 dMax = qRound((double)dwSmplRate / fMin);
    int size = fData.size();
    if (dMax >= size / 2)
        dMax = size / 2 - 1;

    // Décalage min (fréquence haute)
    dMin = qRound((double)dwSmplRate / fMax);

    // Calcul de la corrélation
    QVector<float> vectCorrel;
    if (dMax - dMin + 1 <= 0)
        return vectCorrel;
    vectCorrel.resize(dMax - dMin + 1);

    double qTmp;
    for (int i = dMin; i <= dMax; i++)
    {
        // Mesure de la ressemblance
        qTmp = 0;
        for (int j = 0; j < size - dMax; j++)
            qTmp += (double)qAbs(fData[j] - fData[j+i]);
        vectCorrel[i - dMin] = (double)qTmp / (size - dMax);
    }

    return vectCorrel;
}

float Sound::correlation(QVector<float> fData1, QVector<float> fData2)
{
    if (fData1.size() != fData2.size() || fData1.size() == 0)
        return 0;

    // Mesure ressemblance
    double somme = 0;
    for (int i = 0; i < fData1.size(); i++)
        somme += qAbs(fData1[i] - fData2[i]);

    // Normalisation et retour
    return somme / fData1.size();
}

QByteArray Sound::bouclage(QByteArray baData, quint32 dwSmplRate, qint32 &loopStart, qint32 &loopEnd, quint16 wBps)
{
    if (wBps != 32)
        baData = bpsConversion(baData, wBps, 32);

    // Conversion en float
    qint32 size = baData.size() / 4;
    QVector<float> fData;
    fData.resize(size);
    qint32 * iData = (qint32*)baData.data();
    for (int i = 0; i < size; i++)
        fData[i] = (float)iData[i];

    // Recherche du régime permament
    qint32 posStart = loopStart;
    if (posStart == loopEnd || loopEnd - posStart < (signed)dwSmplRate / 4)
        regimePermanent(fData, dwSmplRate, posStart, loopEnd);
    if (loopEnd - posStart < (signed)dwSmplRate / 4)
        return QByteArray();

    // Extraction du segment B de 0.05s à la fin du régime permanent
    qint32 longueurSegmentB = 0.05 * dwSmplRate;
    QVector<float> segmentB = fData.mid((loopEnd - longueurSegmentB), longueurSegmentB);

    // Calcul des corrélations
    qint32 nbCor = (loopEnd - posStart) / 2 - 2 * longueurSegmentB;
    QVector<float> vectCorrelations;
    vectCorrelations.resize(nbCor);
    for (int i = 0; i < nbCor; i++)
        vectCorrelations[i] = correlation(segmentB, fData.mid((i + longueurSegmentB + posStart), longueurSegmentB));

    // Recherche des meilleures corrélations
    QList<int> meilleuresCorrel = findMins(vectCorrelations, 1, 0.9);
    if (meilleuresCorrel.isEmpty())
        return QByteArray();

    // calcul de posStartLoop
    qint32 posStartLoop = 2 * longueurSegmentB + meilleuresCorrel[0] + posStart;

    // Longueur du crossfade pour bouclage (augmente avec l'incohérence)
    int longueurBouclage = qMin(meilleuresCorrel[0] + 2 * longueurSegmentB,
                                qRound(dwSmplRate * 4 * (double)(2147483647. - vectCorrelations[meilleuresCorrel[0]]) / 2147483647));

    // Bouclage avec crossfade
    for (int i = 0; i < longueurBouclage; i++)
    {
        double dTmp = (double)i/(longueurBouclage - 1);
        iData[loopEnd - longueurBouclage + i] =
                (1. - dTmp) * fData[loopEnd - longueurBouclage + i] +
                dTmp * fData[posStartLoop - longueurBouclage + i];
    }

    // Récupération de 8 valeurs
    QByteArray baTmp;
    baTmp.resize(4*8);
    qint32 * dataTmp = (qint32 *)baTmp.data();
    for (int i = 0; i < 8; i++)
        dataTmp[i] = fData[posStartLoop+i];

    // Coupure et ajout de 8 valeurs
    baData = baData.left(loopEnd * 4).append(baTmp);

    // Modification de loopStart et renvoi des données
    loopStart = posStartLoop;
    if (wBps != 32)
        baData = bpsConversion(baData, 32, wBps);
    return baData;
}

QByteArray Sound::sifflements(QByteArray baData, quint32 dwSmplRate, quint16 wBps, double fCut, double fMax, double raideur)
{
    // Conversion 32 bits si nécessaire
    if (wBps != 32)
        baData = bpsConversion(baData, wBps, 32);
    // Elimination des fréquences hautes du son
    unsigned long size;
    // Conversion de baData en complexes
    Complex * cpxData;
    cpxData = fromBaToComplex(baData, 32, size);
    // Calculer la fft du signal
    Complex * fc_sortie_fft = FFT(cpxData, size);
    delete [] cpxData;
    double pos;
    // Ajustement raideur
    raideur += 1;
    // Module carré maxi de la FFT
    double moduleMax = 0;
    for (unsigned long i = 0; i < (size+1)/2; i++)
    {
        pos = i / ((double)size-1.0);
        if (pos * dwSmplRate < fCut)
        {
            double module = sqrt(fc_sortie_fft[i].imag() * fc_sortie_fft[i].imag() +
                            fc_sortie_fft[i].real() * fc_sortie_fft[i].real());
            moduleMax = qMax(moduleMax, module);
            module = sqrt(fc_sortie_fft[size-1-i].imag() * fc_sortie_fft[size-1-i].imag() +
                          fc_sortie_fft[size-1-i].real() * fc_sortie_fft[size-1-i].real());
            moduleMax = qMax(moduleMax, module);
        }
    }
    for (unsigned long i = 0; i < (size+1)/2; i++)
    {
        pos = i / ((double)size-1);
        if (pos * dwSmplRate > fMax)
        {
            fc_sortie_fft[i] *= 0;
            fc_sortie_fft[size-1-i] *= 0;
        }
        else if (pos * dwSmplRate > fCut)
        {
            double module = sqrt(fc_sortie_fft[i].imag() * fc_sortie_fft[i].imag() +
                                 fc_sortie_fft[i].real() * fc_sortie_fft[i].real());
            double moduleMaxPos = (1.0 - (double)(pos * dwSmplRate - fCut) / (fMax - fCut));
            moduleMaxPos = moduleMax * pow(moduleMaxPos, raideur);
            if (module > moduleMaxPos && module != 0)
            {
                fc_sortie_fft[i] *= moduleMaxPos / module;
                fc_sortie_fft[size-1-i] *= moduleMaxPos / module;
            }
        }
    }

    // Calculer l'ifft du signal
    cpxData = IFFT(fc_sortie_fft, size);
    delete [] fc_sortie_fft;
    // Prise en compte du facteur d'echelle
    for (unsigned long i = 0; i < size; i++)
        cpxData[i].real(cpxData[i].real() / size);
    // Retour en QByteArray
    QByteArray baRet = fromComplexToBa(cpxData, (long int)baData.size() * 8 / 32, 32);
    delete [] cpxData;
    // retour wBps si nécessaire
    if (wBps != 32)
        baRet = bpsConversion(baRet, 32, wBps);
    return baRet;
}

QList<int> Sound::findMins(QVector<float> vectData, int maxNb, double minFrac)
{
    if (vectData.isEmpty())
        return QList<int>();

    // Calcul mini maxi
    float mini = vectData[0], maxi = vectData[0];
    for (qint32 i = 1; i < vectData.size(); i++)
    {
        if (vectData[i] < mini)
            mini = vectData[i];
        if (vectData[i] > maxi)
            maxi = vectData[i];
    }

    // Valeur à ne pas dépasser
    float valMax = maxi - minFrac * (maxi - mini);

    // Recherche des indices de tous les creux
    QMap<int, float> mapCreux;
    for (int i = 1; i < vectData.size() - 1; i++)
        if (vectData[i-1] > vectData[i] && vectData[i+1] > vectData[i] && vectData[i] < valMax)
            mapCreux[i] = vectData[i];

    // Sélection des plus petits creux
    QList<float> listCreux = mapCreux.values();
    qSort(listCreux);
    QList<int> listRet;
    for (int i = 0; i < qMin(maxNb, listCreux.size()); i++)
        listRet << mapCreux.key(listCreux.at(i));

    return listRet;
}

QList<quint32> Sound::findMax(QVector<float> vectData, int maxNb, double minFrac)
{
    if (vectData.isEmpty())
        return QList<quint32>();

    // Calcul mini maxi
    float mini = vectData[0], maxi = vectData[0];
    for (qint32 i = 1; i < vectData.size(); i++)
    {
        if (vectData[i] < mini)
            mini = vectData[i];
        if (vectData[i] > maxi)
            maxi = vectData[i];
    }

    // Valeur à dépasser
    float valMin = mini + minFrac * (maxi - mini);

    // Recherche des indices de tous les pics
    QMap<int, float> mapPics;
    for (int i = 1; i < vectData.size() - 1; i++)
        if (vectData[i-1] < vectData[i] && vectData[i+1] < vectData[i] && vectData[i] > valMin)
            mapPics[i] = vectData[i];

    // Sélection des plus grands pics
    QList<float> listPics = mapPics.values();
    qSort(listPics);
    QList<quint32> listRet;
    for (int i = listPics.size() - 1; i >= qMax(0, listPics.size() - maxNb); i--)
        listRet << (quint32)mapPics.key(listPics.at(i));

    return listRet;
}

qint32 Sound::max(QByteArray baData, quint16 wBps)
{
    if (wBps != 32)
        baData = bpsConversion(baData, wBps, 32);
    qint32 * data = (qint32 *)baData.data();
    qint32 maxi = data[0];
    for (int i = 1; i < baData.size()/4; i++)
    {
        if (data[i] > maxi)
            maxi = data[i];
    }
    return maxi;
}

// UTILITAIRES, PARTIE PRIVEE

void Sound::FFT_calculate(Complex * x, long N /* must be a power of 2 */,
        Complex * X, Complex * scratch, Complex * twiddles)
{
    int k, m, n;
    int skip;
    bool evenIteration = N & 0x55555555;
    Complex* E;
    Complex* Xp, * Xp2, * Xstart;
    if (N == 1)
    {
        X[0] = x[0];
        return;
    }
    E = x;
    for (n = 1; n < N; n *= 2)
    {
        Xstart = evenIteration? scratch : X;
        skip = N/(2 * n);
        /* each of D and E is of length n, and each element of each D and E is
        separated by 2*skip. The Es begin at E[0] to E[skip - 1] and the Ds
        begin at E[skip] to E[2*skip - 1] */
        Xp = Xstart;
        Xp2 = Xstart + N/2;
        for(k = 0; k != n; k++)
        {
            double tim = twiddles[k * skip].imag();
            double tre = twiddles[k * skip].real();
            for (m = 0; m != skip; ++m)
            {
                Complex* D = E + skip;
                /* twiddle *D to get dre and dim */
                double dre = D->real() * tre - D->imag() * tim;
                double dim = D->real() * tim + D->imag() * tre;
                Xp->real(E->real() + dre);
                Xp->imag(E->imag() + dim);
                Xp2->real(E->real() - dre);
                Xp2->imag(E->imag() - dim);
                ++Xp;
                ++Xp2;
                ++E;
            }
            E += skip;
        }
        E = Xstart;
        evenIteration = !evenIteration;
    }
}

double Sound::moyenne(QByteArray baData, quint16 wBps)
{
    if (baData.size())
        return somme(baData, wBps) / (baData.size() / (wBps/8));
    else
        return 0;
}

double Sound::moyenneCarre(QByteArray baData, quint16 wBps)
{
    //return sommeAbs(baData, wBps) / (baData.size() / (wBps/8));
    return (double)qSqrt(sommeCarre(baData, wBps)) / (baData.size() / (wBps/8));
}

float Sound::mediane(QVector<float> data)
{
    float * arr = data.data();
    qint32 n = data.size();
    qint32 low, high;
    qint32 median;
    qint32 middle, ll, hh;
    float qTmp;
    low = 0 ; high = n-1 ; median = (low + high) / 2;
    for (;;)
    {
        if (high <= low) // One element only
            return arr[median] ;

        if (high == low + 1)
        {  // Two elements only
            if (arr[low] > arr[high])
            {
                qTmp = arr[low];
                arr[low] = arr[high];
                arr[high] = qTmp;
            }
            return arr[median] ;
        }
        // Find median of low, middle and high items; swap into position low
        middle = (low + high) / 2;
        if (arr[middle] > arr[high])
        {
            qTmp = arr[middle];
            arr[middle] = arr[high];
            arr[high] = qTmp;
        }
        if (arr[low] > arr[high])
        {
            qTmp = arr[low];
            arr[low] = arr[high];
            arr[high] = qTmp;
        }
        if (arr[middle] > arr[low])
        {
            qTmp = arr[middle];
            arr[middle] = arr[low];
            arr[low] = qTmp;
        }
        // Swap low item (now in position middle) into position (low+1)
        qTmp = arr[middle];
        arr[middle] = arr[low+1];
        arr[low+1] = qTmp;
        // Nibble from each end towards middle, swapping items when stuck
        ll = low + 1;
        hh = high;
        for (;;)
        {
            do ll++; while (arr[low] > arr[ll]) ;
            do hh--; while (arr[hh]  > arr[low]) ;
            if (hh < ll)
                break;
            qTmp = arr[ll];
            arr[ll] = arr[hh];
            arr[hh] = qTmp;
        }
        // Swap middle item (in position low) back into correct position
        qTmp = arr[low];
        arr[low] = arr[hh];
        arr[hh] = qTmp;
        // Re-set active partition
        if (hh <= median)
            low = ll;
        if (hh >= median)
            high = hh - 1;
    }
}

qint64 Sound::somme(QByteArray baData, quint16 wBps)
{
    if (wBps != 32)
        baData = bpsConversion(baData, wBps, 32);
    qint32 n = baData.size() / 4;
    qint32 * arr = (qint32 *)baData.data();
    qint64 valeur = 0;
    for (int i = 0; i < n; i++)
        valeur += arr[i];
    return valeur;
}

qint64 Sound::sommeCarre(QByteArray baData, quint16 wBps)
{
    if (wBps != 32)
        baData = bpsConversion(baData, wBps, 32);
    qint32 n = baData.size() / 4;
    qint32 * arr = (qint32 *)baData.data();
    qint64 valeur = 0;
    for (int i = 0; i < n; i++)
        valeur += (arr[i]/47000) * (arr[i]/47000);
    return valeur;
}

double Sound::gainEQ(double freq, int i1, int i2, int i3, int i4, int i5, int i6, int i7, int i8, int i9, int i10)
{
    int x1 = 0;
    int x2 = 1;
    int y1 = 0;
    int y2 = 1;
    if (freq < 32)
    {
        x1 = 32; x2 = 64;
        y1 = qMin(i1, i2); y2 = i2;
    }
    else if (freq < 64)
    {
        x1 = 32; x2 = 64;
        y1 = i1; y2 = i2;
    }
    else if (freq < 125)
    {
        x1 = 64; x2 = 125;
        y1 = i2; y2 = i3;
    }
    else if (freq < 250)
    {
        x1 = 125; x2 = 250;
        y1 = i3; y2 = i4;
    }
    else if (freq < 500)
    {
        x1 = 250; x2 = 500;
        y1 = i4; y2 = i5;
    }
    else if (freq < 1000)
    {
        x1 = 500; x2 = 1000;
        y1 = i5; y2 = i6;
    }
    else if (freq < 2000)
    {
        x1 = 1000; x2 = 2000;
        y1 = i6; y2 = i7;
    }
    else if (freq < 4000)
    {
        x1 = 2000; x2 = 4000;
        y1 = i7; y2 = i8;
    }
    else if (freq < 8000)
    {
        x1 = 4000; x2 = 8000;
        y1 = i8; y2 = i9;
    }
    else if (freq < 16000)
    {
        x1 = 8000; x2 = 16000;
        y1 = i9; y2 = i10;
    }
    else
    {
        x1 = 8000; x2 = 16000;
        y1 = i9; y2 = qMin(i9, i10);
    }
    double a = (double)(y1 - y2) / (x1 - x2);
    double b = (double)y2 - a * x2;
    // Gain en dB
    double val = a * freq + b;
    // Conversion
    return pow(10.0, val / 20);
}

void Sound::regimePermanent(QVector<float> data, quint32 dwSmplRate, qint32 &posStart, qint32 &posEnd, int nbOK, double coef)
{
    // Calcul de la moyenne des valeurs absolues sur une période de 0.05 s à chaque 10ième de seconde
    qint32 sizePeriode = dwSmplRate / 10;
    int len = data.size();
    if (len < sizePeriode)
    {
        posStart = 0;
        posEnd = len - 1;
        return;
    }
    qint32 nbValeurs = (len - sizePeriode) / (dwSmplRate / 20);
    QVector<float> tableauMoyennes;
    tableauMoyennes.resize(nbValeurs);
    for (int i = 0; i < nbValeurs; i++)
    {
        float valTmp = 0;
        for (int j = 0; j < sizePeriode; j++)
            valTmp += qAbs(data[(dwSmplRate / 20) * i + j]);
        data[i] = valTmp / sizePeriode;
    }

    // Calcul de la médiane des valeurs
    qint32 median = mediane(tableauMoyennes);
    posStart = 0;
    posEnd = nbValeurs - 1;
    int count = 0;
    while (count < nbOK && posStart <= posEnd)
    {
        if (data[posStart] < coef * median && data[posStart] > (double)median / coef)
            count++;
        else
            count = 0;
        posStart++;
    }
    posStart -= count-2;
    count = 0;
    while (count < nbOK && posEnd > 0)
    {
        if (data[posEnd] < coef * median && data[posEnd] > (double)median / coef)
            count++;
        else
            count = 0;
        posEnd--;
    }
    posEnd += count-2;

    // Conversion position
    posStart *= dwSmplRate / 20;
    posEnd *= dwSmplRate / 20;
    posEnd += sizePeriode;
}

double Sound::sinc(double x)
{
    double epsilon0 = 0.32927225399135962333569506281281311031656150598474e-9L;
    double epsilon2 = qSqrt(epsilon0);
    double epsilon4 = qSqrt(epsilon2);

    if (qAbs(x) >= epsilon4)
        return(qSin(x)/x);
    else
    {
        // x très proche de 0, développement limité ordre 0
        double result = 1;
        if (qAbs(x) >= epsilon0)
        {
            double x2 = x*x;

            // x un peu plus loin de 0, développement limité ordre 2
            result -= x2 / 6.;

            if (qAbs(x) >= epsilon2)
            {
                // x encore plus loin de 0, développement limité ordre 4
                result += (x2 * x2) / 120.;
            }
        }
        return(result);
    }
}

// Keser-Bessel window
void Sound::KBDWindow(double* window, int size, double alpha)
{
    double sumvalue = 0.;
    int i;

    for (i = 0; i < size/2; i++)
    {
        sumvalue += BesselI0(M_PI * alpha * sqrt(1. - pow(4.0*i/size - 1., 2)));
        window[i] = sumvalue;
    }

    // need to add one more value to the nomalization factor at size/2:
    sumvalue += BesselI0(M_PI * alpha * sqrt(1. - pow(4.0*(size/2) / size-1., 2)));

    // normalize the window and fill in the righthand side of the window:
    for (i = 0; i < size/2; i++)
    {
        window[i] = sqrt(window[i]/sumvalue);
        window[size-1-i] = window[i];
    }
}

double Sound::BesselI0(double x)
{
    double denominator;
    double numerator;
    double z;

    if (x == 0.0)
        return 1.0;
    else
    {
        z = x * x;
        numerator = (z* (z* (z* (z* (z* (z* (z* (z* (z* (z* (z* (z* (z*
                                                                     (z* 0.210580722890567e-22  + 0.380715242345326e-19 ) +
                                                                     0.479440257548300e-16) + 0.435125971262668e-13 ) +
                                                             0.300931127112960e-10) + 0.160224679395361e-7  ) +
                                                     0.654858370096785e-5)  + 0.202591084143397e-2  ) +
                                             0.463076284721000e0)   + 0.754337328948189e2   ) +
                                     0.830792541809429e4)   + 0.571661130563785e6   ) +
                             0.216415572361227e8)   + 0.356644482244025e9   ) +
                     0.144048298227235e10);
        denominator = (z*(z*(z-0.307646912682801e4)+
                          0.347626332405882e7)-0.144048298227235e10);
    }

    return -numerator/denominator;
}

// Reconnaissance de liens R - L dans les noms de samples
int Sound::lastLettersToRemove(QString str1, QString str2)
{
    str1 = str1.toLower();
    str2 = str2.toLower();
    int nbLetters = 0;

    int size = 0;
    if (str1.size() == str2.size())
        size = str1.size();
    else return 0;

    if (str1.left(size - 2).compare(str2.left(size - 2)) != 0)
        return 0;

    QString fin1_3 = str1.right(3);
    QString fin2_3 = str2.right(3);
    QString fin1_2 = str1.right(2).left(1);
    QString fin2_2 = str2.right(2).left(1);
    QString fin1_1 = str1.right(1);
    QString fin2_1 = str2.right(1);

    if ((fin1_3.compare("(r)") == 0 && fin2_3.compare("(l)") == 0) ||
        (fin1_3.compare("(l)") == 0 && fin2_3.compare("(r)") == 0))
        nbLetters = 3;
    else if (((fin1_1.compare("r") == 0 && fin2_1.compare("l") == 0) ||
              (fin1_1.compare("l") == 0 && fin2_1.compare("r") == 0)) &&
             str1.left(size - 1).compare(str2.left(size - 1)) == 0)
    {
        nbLetters = 1;
        if ((fin1_2.compare("-") == 0 && fin2_2.compare("-") == 0) ||
            (fin1_2.compare("_") == 0 && fin2_2.compare("_") == 0) ||
            (fin1_2.compare(".") == 0 && fin2_2.compare(".") == 0) ||
            (fin1_2.compare(" ") == 0 && fin2_2.compare(" ") == 0))
            nbLetters = 2;
    }

    return nbLetters;
}

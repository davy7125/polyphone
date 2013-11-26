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
***************************************************************************/

#ifndef SOUND_H
#define SOUND_H

#include "sf2_types.h"
#include <complex>
using namespace std;
typedef complex<double> Complex;

#define PI  3.1415926535897932

class Sound
{
public:

    // constructeur, destructeur
    Sound(QString filename = "", bool tryFindRootkey = true);
    ~Sound();

    // méthodes publiques
    static void setParent(QWidget * parent) { _parent = parent; }
    QByteArray getData(WORD wBps);
    DWORD get(Champ champ);
    QString getFileName();
    void set(Champ champ, Valeur value);
    void setFileName(QString qStr, bool tryFindRootKey = true);
    void setData(QByteArray data, WORD wBps);
    void setRam(bool ram);
    static void exporter(QString fileName, Sound son);
    static void exporter(QString fileName, Sound son1, Sound son2);
    // utilitaires
    static QByteArray resampleMono(QByteArray data, double echInit, qint32 echFinal, WORD wBps);
    static QByteArray bandFilter(QByteArray baData, WORD wBps, double dwSmplRate, double fBas, double fHaut, int ordre);
    static QByteArray EQ(QByteArray baData, DWORD dwSmplRate, WORD wBps, int i1, int i2, int i3, int i4, int i5,
                         int i6, int i7, int i8, int i9, int i10);
    static QByteArray bpsConversion(QByteArray baData, WORD wBpsInit, WORD wBpsFinal, bool bigEndian = false);
    static void bpsConversion(char *cDest, const char *cFrom, qint32 size, WORD wBpsInit, WORD wBpsFinal, bool bigEndian = false);
    static QByteArray from2MonoTo1Stereo(QByteArray baData1, QByteArray baData2, WORD wBps, bool bigEndian = false);
    static Complex * fromBaToComplex(QByteArray baData, WORD wBps, unsigned long &size);
    static QByteArray fromComplexToBa(Complex * cpxData, int size, WORD wBps);
    static Complex * FFT(Complex * x, int N); // N must be a power of 2
    static Complex * IFFT(Complex * x, int N); // N must be a power of 2
    static QByteArray normaliser(QByteArray baData, double dVal, WORD wBps, double &db);
    static QByteArray multiplier(QByteArray baData, double dMult, WORD wBps, double &db);
    static QByteArray enleveBlanc(QByteArray baData, double seuil, WORD wBps, quint32 &pos);
    static void regimePermanent(QByteArray baData, DWORD dwSmplRate, WORD wBps, qint32 &posStart, qint32 &posEnd);
    static QByteArray correlation(QByteArray baData, DWORD dwSmplRate, WORD wBps, qint32 fMin, qint32 fMax, qint32 &dMin);
    static qint32 correlation(QByteArray baData1, QByteArray baData2, WORD wBps);
    static QByteArray bouclage(QByteArray baData, DWORD dwSmplRate, qint32 &loopStart, qint32 &loopEnd, WORD wBps);
    static QByteArray sifflements(QByteArray baData, DWORD dwSmplRate, WORD wBps, double fCut, double fMax, double raideur);
    static quint32 * findMins(QByteArray baCorrel, WORD wBps, int nb, double minFrac = 0);
    static quint32 * findMax(QByteArray baData, WORD wBps, int nb, double minFrac = 0);
    static qint32 max(QByteArray baData, WORD wBps);
    static double moyenneCarre(QByteArray baData, WORD wBps);
    static int lastLettersToRemove(QString str1, QString str2);
private:
    // Structure de données pour informations sur un sample
    typedef struct infoSound
    {
        WORD wFormat;
        DWORD dwStart;
        DWORD dwStart2; // pour sf2 : les données en 24 bits sont sur 2 blocs
        DWORD dwLength;
        DWORD dwSampleRate;
        WORD wChannels;
        WORD wBpsFile;
        DWORD dwStartLoop;
        DWORD dwEndLoop;
        DWORD dwNote;
        WORD wChannel;
        int iCent;
    }InfoSound;
    // Type de fichier
    typedef enum
    {
        fileUnknown = 0,
        fileSf2,
        fileWav,
        fileCustom1
    }FileType;
    // Attributs privés
    QString fileName;
    InfoSound info;
    QByteArray smpl;
    QByteArray sm24;
    // Méthodes privées
    FileType getFileType();
    void getInfoSound(bool tryFindRootkey);
    QByteArray getDataSf2(QFile *fi, WORD byte);
    QByteArray getDataWav(QFile *fi, WORD byte);
    QByteArray getDataWav(QByteArray baData, WORD byte);
    void determineRootKey();
    void getInfoSoundWav(bool tryFindRootkey);
    void getInfoSoundWav(QByteArray baData, bool tryFindRootkey);
    static void exporter(QString fileName, QByteArray baData, InfoSound info);
    static void FFT_calculate(Complex * x, long N /* must be a power of 2 */,
            Complex * X, Complex * scratch, Complex * twiddles);
    static double moyenne(QByteArray baData, WORD wBps);
    static double gainEQ(double freq, int i1, int i2, int i3, int i4, int i5, int i6, int i7, int i8, int i9, int i10);
    static qint32 mediane(QByteArray baData, WORD wBps);
    static qint64 somme(QByteArray baData, WORD wBps);
    static qint64 sommeCarre(QByteArray baData, WORD wBps);
    static void regimePermanent(QByteArray baData, DWORD dwSmplRate, WORD wBps, qint32 &posStart,
                                qint32 &posEnd, int nbOK, double coef);
    static double sinc(double x);
    static void KBDWindow(double* window, int size, double alpha);
    static double BesselI0(double x);

    static QWidget * _parent;
};

#endif // SOUND_H

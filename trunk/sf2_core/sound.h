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

#ifndef SOUND_H
#define SOUND_H

#include "sf2_types.h"

class QFile;
class QWidget;

class Complex
{
public:
    void imag(double value) { _imag = value; }
    void real(double value) { _real = value; }
    double imag() { return _imag; }
    double real() { return _real; }

    Complex operator *= (const double factor)
    {
        _real *= factor;
        _imag *= factor;
        return *this;
    }
private:
    double _real, _imag;
};


class Sound
{
public:
    Sound(QString filename = "", bool tryFindRootkey = true);
    ~Sound();

    // Méthodes publiques
    static void setParent(QWidget * parent) { _parent = parent; }
    QByteArray getData(quint16 wBps);
    quint32 get(Champ champ);
    QString getFileName();
    void set(Champ champ, Valeur value);
    void setFileName(QString qStr, bool tryFindRootKey = true);
    void setData(QByteArray data, quint16 wBps);
    void setRam(bool ram);
    static void exporter(QString fileName, Sound son);
    static void exporter(QString fileName, Sound son1, Sound son2);

    // Utilitaires
    static QByteArray resampleMono(QByteArray data, double echInit, qint32 echFinal, quint16 wBps);
    static QByteArray bandFilter(QByteArray baData, quint16 wBps, double dwSmplRate, double fBas, double fHaut, int ordre);
    static QByteArray EQ(QByteArray baData, quint32 dwSmplRate, quint16 wBps, int i1, int i2, int i3, int i4, int i5,
                         int i6, int i7, int i8, int i9, int i10);
    static QByteArray bpsConversion(QByteArray baData, quint16 wBpsInit, quint16 wBpsFinal, bool bigEndian = false);
    static void bpsConversion(char *cDest, const char *cFrom, qint32 size, quint16 wBpsInit, quint16 wBpsFinal, bool bigEndian = false);
    static QByteArray from2MonoTo1Stereo(QByteArray baData1, QByteArray baData2, quint16 wBps, bool bigEndian = false);
    static Complex * fromBaToComplex(QByteArray baData, quint16 wBps, unsigned long &size);
    static Complex * fromBaToComplex(QVector<float> fData, unsigned long &size);
    static QByteArray fromComplexToBa(Complex * cpxData, int size, quint16 wBps);
    static Complex * FFT(Complex * x, int N); // N must be a power of 2
    static Complex * IFFT(Complex * x, int N); // N must be a power of 2
    static QByteArray normaliser(QByteArray baData, double dVal, quint16 wBps, double &db);
    static QByteArray multiplier(QByteArray baData, double dMult, quint16 wBps, double &db);
    static QByteArray enleveBlanc(QByteArray baData, double seuil, quint16 wBps, quint32 &pos);
    static void regimePermanent(QByteArray baData, quint32 dwSmplRate, quint16 wBps, qint32 &posStart, qint32 &posEnd);
    static void regimePermanent(QVector<float> fData, quint32 dwSmplRate, qint32 &posStart, qint32 &posEnd);
    static QVector<float> correlation(const QVector<float> fData, quint32 dwSmplRate, qint32 fMin, qint32 fMax, qint32 &dMin);
    static float correlation(QVector<float> fData1, QVector<float> fData2);
    static QByteArray bouclage(QByteArray baData, quint32 dwSmplRate, qint32 &loopStart, qint32 &loopEnd, quint16 wBps);
    static QByteArray sifflements(QByteArray baData, quint32 dwSmplRate, quint16 wBps, double fCut, double fMax, double raideur);
    static QList<int> findMins(QVector<float> vectData, int maxNb, double minFrac = 0);
    static QList<quint32> findMax(QVector<float> vectData, int maxNb, double minFrac = 0);
    static qint32 max(QByteArray baData, quint16 wBps);
    static double moyenneCarre(QByteArray baData, quint16 wBps);
    static int lastLettersToRemove(QString str1, QString str2);

private:
    // Structure de données pour informations sur un sample
    typedef struct infoSound
    {
        quint16 wFormat;
        quint32 dwStart;
        quint32 dwStart2; // pour sf2 : les données en 24 bits sont sur 2 blocs
        quint32 dwLength;
        quint32 dwSampleRate;
        quint16 wChannels;
        quint16 wBpsFile;
        quint32 dwStartLoop;
        quint32 dwEndLoop;
        quint32 dwNote;
        quint16 wChannel;
        int iCent;
    } InfoSound;

    // Type de fichier
    typedef enum
    {
        fileUnknown = 0,
        fileSf2,
        fileWav,
        fileCustom1
    } FileType;

    // Attributs privés
    QString fileName;
    InfoSound _info;
    QByteArray smpl;
    QByteArray sm24;

    // Méthodes privées
    FileType getFileType();
    void getInfoSound(bool tryFindRootkey);
    QByteArray getDataSf2(QFile *fi, quint16 byte);
    QByteArray getDataWav(QFile *fi, quint16 byte);
    QByteArray getDataWav(QByteArray baData, quint16 byte);
    void determineRootKey();
    void getInfoSoundWav(bool tryFindRootkey);
    void getInfoSoundWav(QByteArray baData, bool tryFindRootkey);
    static void exporter(QString fileName, QByteArray baData, InfoSound info);
    static void FFT_calculate(Complex * x, long N /* must be a power of 2 */,
            Complex * X, Complex * scratch, Complex * twiddles);
    static double moyenne(QByteArray baData, quint16 wBps);
    static double gainEQ(double freq, int i1, int i2, int i3, int i4, int i5, int i6, int i7, int i8, int i9, int i10);
    static float mediane(QVector<float> data);
    static qint64 somme(QByteArray baData, quint16 wBps);
    static qint64 sommeCarre(QByteArray baData, quint16 wBps);
    static void regimePermanent(QVector<float> data, quint32 dwSmplRate, qint32 &posStart, qint32 &posEnd, int nbOK, double coef);
    static double sinc(double x);
    static void KBDWindow(double* window, int size, double alpha);
    static double BesselI0(double x);

    static QWidget * _parent;
};

#endif // SOUND_H

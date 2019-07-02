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

#ifndef SAMPLEUTILS_H
#define SAMPLEUTILS_H

#include "basetypes.h"

class SampleUtils
{
public:
    SampleUtils();

    static QByteArray resampleMono(QByteArray data, double echInit, qint32 echFinal, quint16 wBps);
    static QByteArray bandFilter(QByteArray baData, quint16 wBps, double dwSmplRate, double fBas, double fHaut, int ordre);
    static QByteArray cutFilter(QByteArray baData, quint32 dwSmplRate, QVector<double> dValues, quint16 wBps, int maxFreq);
    static QByteArray EQ(QByteArray baData, quint32 dwSmplRate, quint16 wBps, int i1, int i2, int i3, int i4, int i5,
                         int i6, int i7, int i8, int i9, int i10);
    static QByteArray bpsConversion(QByteArray baData, quint16 wBpsInit, quint16 wBpsFinal, bool bigEndian = false);
    static void bpsConversion(char *cDest, const char *cFrom, qint32 size, quint16 wBpsInit, quint16 wBpsFinal, bool bigEndian = false);
    static QByteArray from2MonoTo1Stereo(QByteArray baData1, QByteArray baData2, quint16 wBps, bool bigEndian = false);
    static Complex * fromBaToComplex(QByteArray baData, quint16 wBps, unsigned long &size);
    static Complex * fromBaToComplex(QVector<float> fData, unsigned long &size);
    static QByteArray fromComplexToBa(Complex * cpxData, int size, quint16 wBps);
    static QVector<float> getFourierTransform(QVector<float> input);
    static QByteArray normaliser(QByteArray baData, double dVal, quint16 wBps, double &db);
    static QByteArray multiplier(QByteArray baData, double dMult, quint16 wBps, double &db);
    static QByteArray enleveBlanc(QByteArray baData, double seuil, quint16 wBps, quint32 &pos);
    static void regimePermanent(QByteArray baData, quint32 dwSmplRate, quint16 wBps, qint32 &posStart, qint32 &posEnd);
    static void regimePermanent(QVector<float> fData, quint32 dwSmplRate, qint32 &posStart, qint32 &posEnd);
    static QVector<float> correlation(const QVector<float> fData, quint32 dwSmplRate, qint32 fMin, qint32 fMax, qint32 &dMin);
    static float correlation(QVector<float> fData1, QVector<float> fData2);
    static QByteArray bouclage(QByteArray baData, quint32 dwSmplRate, qint32 &loopStart, qint32 &loopEnd, quint16 wBps);
    static QList<int> findMins(QVector<float> vectData, int maxNb, double minFrac = 0);
    static QList<quint32> findMax(QVector<float> vectData, int maxNb, double minFrac = 0);
    static qint32 max(QByteArray baData, quint16 wBps);
    static double moyenneCarre(QByteArray baData, quint16 wBps);
    static int lastLettersToRemove(QString str1, QString str2);

private:
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
    static Complex * FFT(Complex * x, int N); // N must be a power of 2
    static Complex * IFFT(Complex * x, int N); // N must be a power of 2
};

#endif // SAMPLEUTILS_H

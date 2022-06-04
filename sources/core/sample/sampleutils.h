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

    static QVector<float> resampleMono(QVector<float> vData, double echInit, quint32 echFinal);
    static QVector<float> bandFilter(QVector<float> vData, double dwSmplRate, double fBas, double fHaut, int ordre);
    static QVector<float> cutFilter(QVector<float> vData, quint32 dwSmplRate, QVector<double> dValues, int maxFreq);
    static QVector<float> EQ(QVector<float> vData, quint32 dwSmplRate, QVector<int> eqGains);
    static Complex * fromFloatToComplex(QVector<float> fData, quint32 &size);
    static QVector<float> fromComplexToFloat(Complex * cpxData, int size);
    static QVector<float> getFourierTransform(QVector<float> input);
    static QVector<float> normalize(QVector<float> vData, float dVal, float &db);
    static QVector<float> multiply(QVector<float> vData, float dMult, float &db);
    static void removeBlankStep1(QVector<float> vData, quint32 &pos1, quint32 &pos2);
    static QVector<float> removeBlankStep2(QVector<float> vData, quint32 pos);
    static void regimePermanent(QVector<float> fData, quint32 dwSmplRate, quint32 &posStart, quint32 &posEnd);
    static QVector<float> correlation(const float *fData, quint32 size, quint32 dwSmplRate, quint32 fMin, quint32 fMax, quint32 &dMin);
    static float correlation(const float *fData1, const float *fData2, quint32 length, float *bestValue);
    static bool loopStep1(QVector<float> vData, quint32 dwSmplRate, quint32 &loopStart, quint32 &loopEnd, quint32 &loopCrossfadeLength);
    static QVector<float> loopStep2(QVector<float> vData, quint32 loopStart, quint32 loopEnd, quint32 loopCrossfadeLength);
    static QList<quint32> findMins(QVector<float> vectData, int maxNb, float minFrac = 0);
    static QList<quint32> findMax(QVector<float> vectData, int maxNb, float minFrac = 0);
    static float max(QVector<float> vData);
    static float meanSquare(QVector<float> vData);
    static int lastLettersToRemove(QString str1, QString str2);

    // Compute the quality of a loop
    // If the result if < 0.05 it can be considered as OK
    // If > 0.150 => you will probably hear the loop point
    static float computeLoopQuality(QVector<float> vData, quint32 loopStart, quint32 loopEnd);

private:
    static void FFT_calculate(Complex * x, quint32 N /* must be a power of 2 */,
                              Complex * X, Complex * scratch, Complex * twiddles);
    static float mean(QVector<float> vData);
    static double gainEQ(double freq, QVector<int> eqGains);
    static float median(QVector<float> vData);
    static float sum(QVector<float> vData);
    static float sumSquare(QVector<float> vData);
    static void regimePermanent(QVector<float> data, quint32 dwSmplRate, quint32 &posStart, quint32 &posEnd, quint32 nbOK, float coef);
    static double sinc(double x);
    static void KBDWindow(double* window, int size, double alpha);
    static double BesselI0(double x);
    static Complex * FFT(Complex * x, quint32 N); // N must be a power of 2
    static Complex * IFFT(Complex * x, quint32 N); // N must be a power of 2
    static float getDiffForLoopQuality(const float *data, quint32 pos1, quint32 pos2);
};

#endif // SAMPLEUTILS_H

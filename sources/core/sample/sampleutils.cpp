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

#include "sampleutils.h"
#include "utils.h"
#include <QMessageBox>

QVector<float> SampleUtils::int24ToFloat(const qint16 * data16, const quint8 * data24, quint32 length)
{
    QVector<float> result;
    result.resize(length, 0.f);
    if (data24 == nullptr)
    {
        for (quint32 i = 0; i < length; i++)
            result[i] = Utils::int24ToFloat(data16[i] << 8);
    }
    else
    {
        for (quint32 i = 0; i < length; i++)
            result[i] = Utils::int24ToFloat(data16[i] << 8 | data24[i]);
    }
    return result;
}

void SampleUtils::floatToInt24(const QVector<float> data, qint16 *& data16, quint8 *& data24)
{
    data16 = new qint16[data.length()];
    data24 = new quint8[data.length()];
    int iTmp;
    for (int i = 0; i < data.length(); i++)
    {
        iTmp = Utils::floatToInt24(data[i]);
        data16[i] = (iTmp >> 8);
        if (data24 != nullptr)
            data24[i] = iTmp & 0xFF;
    }
}

QVector<float> SampleUtils::resampleMono(QVector<float> vData, double echInit, quint32 echFinal)
{
    // Paramètres
    double alpha = 3;
    qint32 nbPoints = 10;

    // Préparation signal d'entrée
    if (echFinal < echInit)
    {
        // Filtre passe bas (voir sinc filter)
        vData = SampleUtils::bandFilter(vData, echInit, echFinal / 2, 0, -1);
    }
    quint32 sizeInit = static_cast<quint32>(vData.size());
    const float * dataF = vData.constData();

    // Création fenêtre Kaiser-Bessel 2048 points
    double kbdWindow[2048];
    KBDWindow(kbdWindow, 2048, alpha);

    // Nombre de points à trouver
    quint32 sizeFinal = static_cast<quint32>(1. + (sizeInit - 1.0) * echFinal / echInit);
    QVector<float> dataRet(sizeFinal);

    // Calcul des points par interpolation à bande limitée
    float pos, delta;
    qint32 pos1, pos2;
    float * sincCoef = new float[1 + 2 * static_cast<quint32>(nbPoints)];
    float valMax = 0;
    for (quint32 i = 0; i < sizeFinal; i++)
    {
        // Position à interpoler
        pos = (echInit * i) / echFinal;

        // Calcul des coefs
        for (qint32 j = -nbPoints; j <= nbPoints; j++)
        {
            delta = pos - floor(pos);

            // Calcul du sinus cardinal
            sincCoef[j + nbPoints] = sinc(M_PI * (static_cast<double>(j) - delta));

            // Application fenêtre
            delta = static_cast<double>(j + nbPoints - delta) / (1 + 2 * nbPoints) * 2048;
            pos1 = static_cast<qint32>(floor(delta) + .5);
            if (pos1 < 0)
                pos1 = 0;
            else if (pos1 > 2047)
                pos1 = 2047;
            pos2 = static_cast<qint32>(ceil (delta) + .5);
            if (pos2 < 0)
                pos2 = 0;
            else if (pos2 > 2047)
                pos2 = 2047;

            sincCoef[j + nbPoints] *= kbdWindow[pos1] * (ceil((delta)) - delta)
                    + kbdWindow[pos2] * (1.f - ceil((delta)) + delta);
        }

        // Valeur
        dataRet[i] = 0;
        for (int j = qMax(0, static_cast<qint32>(pos) - nbPoints);
             j <= qMin(static_cast<qint32>(sizeInit) - 1, static_cast<qint32>(pos) + nbPoints); j++)
            dataRet[i] += sincCoef[j - static_cast<qint32>(pos) + nbPoints] * dataF[j];

        valMax = qMax(valMax, qAbs(dataRet[i]));
    }
    delete [] sincCoef;

    // Limitation si besoin
    if (valMax > 1.0f)
    {
        float coef = 1.0f / valMax;
        for (quint32 i = 0; i < sizeFinal; i++)
            dataRet[i] *= coef;
    }

    // Filtre passe bas après resampling
    dataRet = SampleUtils::bandFilter(dataRet, echFinal, echFinal / 2, 0, -1);
    return dataRet;
}

QVector<float> SampleUtils::bandFilter(QVector<float> vData, double dwSmplRate, double fBas, double fHaut, int ordre)
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
    if (dwSmplRate < 1 || (fHaut <= 0 && fBas <= 0) || 2 * fHaut > dwSmplRate || 2 * fBas > dwSmplRate)
    {
        // Controle des fréquences de coupures (il faut que Fc<Fe/2 )
        return vData;
    }

    quint32 size;

    // Conversion de baData en complexes
    Complex * cpxData;
    cpxData = fromFloatToComplex(vData, size);

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
            for (unsigned long i = 0; i < (size + 1) / 2; i++)
            {
                pos = static_cast<double>(i) / (size - 1);
                fc_sortie_fft[i] *= (pos * dwSmplRate) < fBas;
                fc_sortie_fft[size - 1 - i] *= (pos * dwSmplRate) < fBas;
            }
        }
        else
        {
            for (unsigned long i = 0; i < (size + 1) / 2; i++)
            {
                pos = static_cast<double>(i) / (size - 1);
                d_gain_pb = 1.0 / (1.0 + pow(pos * dwSmplRate / fBas, 2 * ordre));
                fc_sortie_fft[i] *= d_gain_pb;
                fc_sortie_fft[size - 1 - i] *= d_gain_pb;
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
            for (unsigned long i = 0; i < (size + 1) / 2; i++)
            {
                pos = static_cast<double>(i) / (size - 1);
                fc_sortie_fft[i] *= (pos * dwSmplRate) > fHaut;
                fc_sortie_fft[size - 1 - i] *= (pos * dwSmplRate) > fHaut;
            }
        }
        else
        {
            for (unsigned long i = 0; i < (size + 1) / 2; i++)
            {
                pos = static_cast<double>(i) / (size - 1);
                d_gain_ph = 1 - (1.0 / (1.0 + pow((pos * dwSmplRate) / fHaut, 2 * ordre)));
                fc_sortie_fft[i] *= d_gain_ph;
                fc_sortie_fft[size - 1 - i] *= d_gain_ph;
            }
        }
    }
    else
    {
        double pos;

        // Filtre passe bande
        for (unsigned long i = 0; i < (size+1)/2; i++)
        {
            pos = static_cast<double>(i) / (size - 1);
            d_gain_ph = 1 - (1.0 / (1.0 + pow((pos * dwSmplRate) / fHaut, 2 * ordre)));
            d_gain_pb = 1.0 / (1.0 + pow(pos * dwSmplRate / fBas, 2 * ordre));
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
    QVector<float> vRet = fromComplexToFloat(cpxData, vData.size());
    delete [] cpxData;
    return vRet;
}

QVector<float> SampleUtils::cutFilter(QVector<float> vData, quint32 dwSmplRate, QVector<float> dValues, int maxFreq)
{
    // Convert baData in complex
    quint32 size;
    Complex * cpxData = fromFloatToComplex(vData, size);

    // Compute the fft
    Complex * fc_sortie_fft = FFT(cpxData, size);
    delete [] cpxData;

    // Get the maximum module of the FFT
    float moduleMax = 0;
    for (unsigned long i = 0; i < (size + 1) / 2; i++)
    {
        // Left side
        float module = sqrt(fc_sortie_fft[i].imag() * fc_sortie_fft[i].imag() +
                             fc_sortie_fft[i].real() * fc_sortie_fft[i].real());
        moduleMax = qMax(moduleMax, module);

        // Right side
        module = sqrt(fc_sortie_fft[size-1-i].imag() * fc_sortie_fft[size-1-i].imag() +
                fc_sortie_fft[size-1-i].real() * fc_sortie_fft[size-1-i].real());
        moduleMax = qMax(moduleMax, module);
    }

    // Cut the frequencies according to dValues (representing maximum intensities from minFreq to maxFreq)
    int nbValues = dValues.count();
    for (unsigned long i = 0; i < (size + 1) / 2; i++)
    {
        // Current frequency and current module
        float freq = static_cast<float>(dwSmplRate * i) / (size - 1);
        float module1 = sqrt(fc_sortie_fft[i].imag() * fc_sortie_fft[i].imag() +
                              fc_sortie_fft[i].real() * fc_sortie_fft[i].real());
        float module2 = sqrt(fc_sortie_fft[size - 1 - i].imag() * fc_sortie_fft[size - 1 - i].imag() +
                fc_sortie_fft[size - 1 - i].real() * fc_sortie_fft[size - 1 - i].real());

        // Module max
        float limit = moduleMax;
        int index1 = static_cast<int>(freq / maxFreq * dValues.count());
        if (index1 >= nbValues - 1)
            limit *= dValues[nbValues - 1];
        else
        {
            float x1 = static_cast<float>(index1) / nbValues * maxFreq;
            float y1 = dValues[index1];
            float x2 = static_cast<float>(index1 + 1) / nbValues * maxFreq;
            float y2 = dValues[index1 + 1];
            limit *= ((freq - x1) / (x2 - x1)) * (y2 - y1) + y1;
        }

        // Cut the frequency if it's above the limit
        if (module1 > limit)
            fc_sortie_fft[i] *= limit / module1;
        if (module2 > limit)
            fc_sortie_fft[size - 1 - i] *= limit / module2;
    }

    // Calculer l'ifft du signal
    cpxData = IFFT(fc_sortie_fft, size);
    delete [] fc_sortie_fft;

    // Prise en compte du facteur d'echelle
    for (unsigned long i = 0; i < size; i++)
        cpxData[i].real(cpxData[i].real() / size);

    // Retour en QByteArray
    QVector<float> vRet = fromComplexToFloat(cpxData, vData.size());
    delete [] cpxData;
    return vRet;
}

QVector<float> SampleUtils::EQ(QVector<float> vData, quint32 dwSmplRate, QVector<int> eqGains)
{
    quint32 size;

    // Conversion de baData en complexes
    Complex * cpxData;
    cpxData = fromFloatToComplex(vData, size);

    // Calculer la fft du signal
    Complex * fc_sortie_fft = FFT(cpxData, size);
    delete [] cpxData;
    // Filtrage
    double freq;
    double gain;
    for (unsigned long i = 0; i < (size + 1) / 2; i++)
    {
        freq = static_cast<double>(i * dwSmplRate) / (size - 1);
        gain = gainEQ(freq, eqGains);
        fc_sortie_fft[i] *= gain;
        fc_sortie_fft[size - 1 - i] *= gain;
    }

    // Calculer l'ifft du signal
    cpxData = IFFT(fc_sortie_fft, size);
    delete [] fc_sortie_fft;

    // Prise en compte du facteur d'echelle
    for (unsigned long i = 0; i < size; i++)
        cpxData[i].real(cpxData[i].real() / size);

    // Retour en QByteArray
    QVector<float> vRet = fromComplexToFloat(cpxData, vData.size());
    delete [] cpxData;
    return vRet;
}

Complex * SampleUtils::FFT(Complex * x, quint32 N)
{
    Complex* out = new Complex[N];
    Complex* scratch = new Complex[N];
    Complex* twiddles = new Complex [N];
    quint32 k;
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

Complex * SampleUtils::IFFT(Complex * x, quint32 N)
{
    Complex * out = new Complex[N];
    Complex * scratch = new Complex[N];
    Complex * twiddles = new Complex [N];
    quint32 k;
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

QVector<float> SampleUtils::getFourierTransform(QVector<float> input)
{
    quint32 size = 0;
    Complex * cpxData = fromFloatToComplex(input, size);
    Complex * fc_sortie_fft = FFT(cpxData, size);
    delete [] cpxData;
    QVector<float> vectFourier;
    vectFourier.resize(size / 2);
    for (quint32 i = 0; i < size / 2; i++)
    {
        vectFourier[static_cast<int>(i)] =
                static_cast<float>(0.5 * qSqrt(fc_sortie_fft[i].real() * fc_sortie_fft[i].real() +
                                               fc_sortie_fft[i].imag() * fc_sortie_fft[i].imag()));
        vectFourier[static_cast<int>(i)] +=
                static_cast<float>(0.5 * qSqrt(fc_sortie_fft[size-i-1].real() * fc_sortie_fft[size-i-1].real() +
                fc_sortie_fft[size-i-1].imag() * fc_sortie_fft[size-i-1].imag()));
    }
    delete [] fc_sortie_fft;

    return vectFourier;
}

Complex * SampleUtils::fromFloatToComplex(QVector<float> fData, quint32 &size)
{
    // Nombre de données (puissance de 2 la plus proche)
    quint32 nb = static_cast<quint32>(ceil(qLn(fData.size()) / 0.69314718056));
    size = 1;
    for (quint32 i = 0; i < nb; i++)
        size *= 2;

    // Création et remplissage d'un tableau de complexes
    Complex * cpxData = new Complex[size];

    // Remplissage
    for (int i = 0; i < fData.size(); i++)
    {
        cpxData[i].real(static_cast<double>(fData[i]));
        cpxData[i].imag(0);
    }

    // On complète avec des 0
    for (quint32 i = static_cast<quint32>(fData.size()); i < size; i++)
    {
        cpxData[i].real(0);
        cpxData[i].imag(0);
    }

    return cpxData;
}

QVector<float> SampleUtils::fromComplexToFloat(Complex * cpxData, int size)
{
    // Convert to real values
    QVector<float> vRet(size);
    float valMax = 0;
    for (int i = 0; i < size; i++)
    {
        vRet[i] = cpxData[i].real();
        valMax = qMax(valMax, qAbs(vRet[i]));
    }

    // Possible attenuation
    if (valMax > 1.0)
    {
        double att = 1.0f / valMax;
        for (int i = 0; i < size; i++)
            vRet[i] *= att;
    }

    return vRet;
}

QVector<float> SampleUtils::normalize(QVector<float> vData, float dVal, float &db)
{
    // Get the maximum value
    float * data = vData.data();
    float valMax = 0;
    for (int i = 0; i < vData.size(); i++)
        valMax = qMax(valMax, qAbs(data[i]));

    // Compute the amplification
    float mult = dVal / valMax;
    db = 20.0f * log10(mult);

    // Amplify
    for (int i = 0; i < vData.size(); i++)
        data[i] *= mult;

    return vData;
}

QVector<float> SampleUtils::multiply(QVector<float> vData, float dMult, float &db)
{
    // Compute the amplification
    db = 20.0f * log10(dMult);

    // Amplify
    float * data = vData.data();
    for (int i = 0; i < vData.size(); i++)
        data[i] *= dMult;

    return vData;
}

void SampleUtils::removeBlankStep1(QVector<float> vData, quint32 &pos1, quint32 &pos2)
{
    // Thresholds
    const float threshold1 = 0.0000005f;
    const float threshold2 = 0.0000010f;

    // Compute the number of elements to skip
    pos1 = pos2 = 0;
    bool pos1Found = false;
    bool pos2Found = false;
    quint32 size = static_cast<quint32>(vData.size());
    for (quint32 i = 0; i < size; i++)
    {
        float value = vData.at(i);
        if (value < 0)
            value = -value;
        if (!pos1Found && value > threshold1)
        {
            pos1 = i;
            pos1Found = true;
        }
        if (!pos2Found && value > threshold2)
        {
            pos2 = i;
            pos2Found = true;
        }
        if (pos1Found && pos2Found)
            break;
    }
}

QVector<float> SampleUtils::removeBlankStep2(QVector<float> vData, quint32 pos)
{
    // Skip the first points
    if (static_cast<quint32>(vData.size()) > pos + 8)
        vData = vData.mid(static_cast<int>(pos), vData.size() - static_cast<int>(pos));
    return vData;
}

bool SampleUtils::regimePermanent(QVector<float> fData, quint32 dwSmplRate, quint32 &posStart, quint32 &posEnd)
{
    quint32 size = static_cast<quint32>(fData.size());

    // Recherche fine
    regimePermanent(fData, dwSmplRate, posStart, posEnd, 10, 1.05f);
    if (posEnd < size / 2 + posStart)
    {
        // Recherche grossière
        regimePermanent(fData, dwSmplRate, posStart, posEnd, 7, 1.2f);
        if (posEnd < size / 2 + posStart)
        {
            // Recherche très grossière
            regimePermanent(fData, dwSmplRate, posStart, posEnd, 4, 1.35f);
            if (posEnd < size / 2 + posStart)
            {
                // moitié du milieu
                posStart = size / 4;
                posEnd = size * 3 / 4;
                return false;
            }
        }
    }
    return true;
}

QVector<float> SampleUtils::correlation(const float * fData, quint32 size, quint32 dwSmplRate, quint32 fMin, quint32 fMax, quint32 &dMin)
{
    QVector<float> vectCorrel;
    if (size < 10)
        return vectCorrel;

    // Décalage max (fréquence basse)
    quint32 dMax = dwSmplRate / fMin;
    if (dMax >= size / 2)
        dMax = size / 2 - 1;

    // Décalage min (fréquence haute)
    dMin = dwSmplRate / fMax;

    // Calcul de la corrélation
    if (dMax + 1 <= dMin)
        return vectCorrel;
    vectCorrel.resize(static_cast<int>(dMax - dMin + 1));

    double qTmp;
    float fTmp;
    for (quint32 i = dMin; i <= dMax; ++i)
    {
        // Mesure de la ressemblance
        qTmp = 0;
        for (quint32 j = 0; j < size - dMax; j++)
        {
            fTmp = fData[j] - fData[j+i];
            qTmp += static_cast<double>(fTmp * fTmp);
        }
        vectCorrel[static_cast<int>(i - dMin)] = static_cast<float>(qTmp / (size - dMax));
    }

    return vectCorrel;
}

bool SampleUtils::loopStep1(QVector<float> vData, quint32 dwSmplRate, quint32 &loopStart, quint32 &loopEnd, quint32 &loopCrossfadeLength)
{
    // Search area
    quint32 posStart, posEnd;
    if (!regimePermanent(vData, dwSmplRate, posStart, posEnd))
        posEnd = vData.count() - 1;
    if (vData.count() > 40000)
    {
        posStart = 4000;
        if (posEnd + 400 > vData.count())
            posEnd = vData.count() - 400;
    }
    else if (vData.count() > 4000)
    {
        posStart = 400;
        if (posEnd + 400 > vData.count())
            posEnd = vData.count() - 400;
    }
    else
    {
        posStart = 2;
        if (posEnd + 1 > vData.count())
            posEnd = vData.count() - 1;
    }

    // No manual input (previously it was possible to manually adapt the left limit for finding a loop)
    // if (loopStart > posStart)
    //     posStart = loopStart;

    // Find all points crossing 0
    QList<quint32> crossPositions;
    for (quint32 i = posStart; i < posEnd; i++)
        if (vData[i - 1] < 0 && vData[i] >= 0)
            crossPositions << i;
    if (crossPositions.count() < 2)
        return false;

    // Limit the area of the loop start
    quint32 minLoopStart = posStart + 0.25 * (posEnd - posStart);
    if (minLoopStart > dwSmplRate)
        minLoopStart = dwSmplRate;
    int minLoopStartIndex = 0;
    for (int i = 0; i < crossPositions.count(); i++)
    {
        if (crossPositions[i] > minLoopStart)
        {
            minLoopStartIndex = i;
            break;
        }
    }

    // Limit the area of the loop end
    quint32 minLoopEnd = posStart + 0.25 * (posEnd - posStart);
    int minLoopEndIndex = 0;
    for (int i = 0; i < crossPositions.count(); i++)
    {
        if (crossPositions[i] > minLoopEnd)
        {
            minLoopEndIndex = i;
            break;
        }
    }

    // Number of positions per seconds
    float positionDensity = static_cast<float>(crossPositions.count()) / vData.count() * dwSmplRate;

    // Absolute maximum value of the signal
    float max = 0.00045f, tmp;
    for (qint32 i = 0; i < vData.size(); i++)
    {
        tmp = vData[i];
        if (tmp > max)
            max = tmp;
        else if (-tmp > max)
            max = -tmp;
    }

    // Compute the quality of the different possible loops and keep the best, minimizing the error and maximizing the loop length
    int checkNumber = 22 - positionDensity / 150;
    if (checkNumber < 8)
        checkNumber = 8;

    float bestQuality = -1;
    float bestRank = 999999; // Lower is better
    quint32 bestLoopStart = 0;
    quint32 bestLoopEnd = 0;
    float currentQuality, currentRank, lengthScore;
    quint32 currentPosStart, currentPosEnd;

    for (int i = minLoopStartIndex; i <= crossPositions.count() - 2; i++)
    {
        currentPosStart = crossPositions[i];

        for (int j = crossPositions.count() - 1; j >= qMax(minLoopEndIndex, i + 1); j--)
        {
            currentPosEnd = crossPositions[j];
            lengthScore = qMax(3.0f, static_cast<float>(posEnd - posStart) / (currentPosEnd - currentPosStart));
            if (lengthScore > bestRank)
                continue; // No need to go further

            currentQuality = computeLoopQuality(vData, currentPosStart, currentPosEnd, checkNumber, true, max);
            currentRank = 200.0f * currentQuality + lengthScore;
            if (currentRank < bestRank)
            {
                bestQuality = currentQuality;
                bestRank = currentRank;
                bestLoopStart = currentPosStart;
                bestLoopEnd = currentPosEnd;
            }
        }
    }

    // Result
    loopStart = bestLoopStart;
    loopEnd = bestLoopEnd;
    loopCrossfadeLength = (bestQuality * dwSmplRate / (5 + 0.75 * checkNumber)) * (bestQuality * dwSmplRate / (5 + 0.75 * checkNumber));
    if (loopCrossfadeLength > loopStart - posStart)
        loopCrossfadeLength = loopStart - posStart;
    return true;
}

QVector<float> SampleUtils::loopStep2(QVector<float> vData, quint32 loopStart, quint32 loopEnd, quint32 loopCrossfadeLength, bool withTrimEnd)
{
    // Loop with a crossfade
    float * fData = vData.data();
    float dTmp;
    for (quint32 i = 0; i < loopCrossfadeLength; i++)
    {
        dTmp = static_cast<float>(i) / static_cast<float>(loopCrossfadeLength - 1);
        fData[loopEnd - loopCrossfadeLength + i] =
                (1.f - dTmp) * fData[loopEnd - loopCrossfadeLength + i] +
                dTmp * fData[loopStart - loopCrossfadeLength + i];
    }

    if (withTrimEnd)
    {
        // Coupure et ajout de 8 valeurs
        vData.resize(loopEnd + 8);
        for (quint32 i = 0; i < 8; i++)
            vData[loopEnd + i] = fData[loopStart + i];
    }

    return vData;
}

QList<quint32> SampleUtils::findMins(QVector<float> vectData, int maxNb, float minFrac)
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

    // Valeur à ne pas dépasser
    float valMax = maxi - minFrac * (maxi - mini);

    // Recherche des indices de tous les creux
    QMap<quint32, float> mapCreux;
    for (int i = 1; i < vectData.size() - 1; i++)
        if (vectData[i-1] > vectData[i] && vectData[i+1] > vectData[i] && vectData[i] < valMax)
            mapCreux[static_cast<quint32>(i)] = vectData[i];

    // Sélection des plus petits creux
    QList<float> listCreux = mapCreux.values();
    std::sort(listCreux.begin(), listCreux.end());
    QList<quint32> listRet;
    for (int i = 0; i < qMin(maxNb, listCreux.size()); i++)
        listRet << mapCreux.key(listCreux.at(i));

    return listRet;
}

QList<quint32> SampleUtils::findMax(QVector<float> vectData, int maxNb, float minFrac)
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
    std::sort(listPics.begin(), listPics.end());
    QList<quint32> listRet;
    for (int i = listPics.size() - 1; i >= qMax(0, listPics.size() - maxNb); i--)
        listRet << static_cast<quint32>(mapPics.key(listPics.at(i)));

    return listRet;
}

float SampleUtils::max(QVector<float> vData)
{
    const float * data = vData.constData();
    qint32 maxi = data[0];
    for (int i = 1; i < vData.size(); i++)
        if (data[i] > maxi)
            maxi = data[i];
    return maxi;
}

// Reconnaissance de liens R - L dans les noms de samples
int SampleUtils::lastLettersToRemove(QString str1, QString str2)
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

void SampleUtils::FFT_calculate(Complex * x, quint32 N /* must be a power of 2 */,
                                Complex * X, Complex * scratch, Complex * twiddles)
{
    quint32 k, m, n, skip;
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
        skip = N / (2 * n);
        /* each of D and E is of length n, and each element of each D and E is
        separated by 2*skip. The Es begin at E[0] to E[skip - 1] and the Ds
        begin at E[skip] to E[2*skip - 1] */
        Xp = Xstart;
        Xp2 = Xstart + N / 2;
        for (k = 0; k != n; k++)
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

float SampleUtils::mean(QVector<float> vData)
{
    if (vData.size() > 0)
        return sum(vData) / vData.size();
    else
        return 0;
}

float SampleUtils::meanSquare(QVector<float> vData)
{
    return qSqrt(sumSquare(vData)) / vData.size();
}

float SampleUtils::median(QVector<float> vData)
{
    float * arr = vData.data();
    qint32 n = vData.size();
    qint32 low, high;
    qint32 median;
    qint32 middle, ll, hh;
    float qTmp;
    low = 0 ; high = n-1 ; median = (low + high) / 2;
    for (;;)
    {
        if (high <= low) // One element only
            return arr[median];

        if (high == low + 1)
        {  // Two elements only
            if (arr[low] > arr[high])
            {
                qTmp = arr[low];
                arr[low] = arr[high];
                arr[high] = qTmp;
            }
            return arr[median];
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
            do ll++; while (arr[low] > arr[ll]);
            do hh--; while (arr[hh]  > arr[low]);
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

float SampleUtils::sum(QVector<float> vData)
{
    float result = 0;
    for (int i = 0; i < vData.size(); i++)
        result += vData.at(i);
    return result;
}

float SampleUtils::sumSquare(QVector<float> vData)
{
    float result = 0;
    for (int i = 0; i < vData.size(); i++)
        result += vData.at(i) * vData.at(i);
    return result;
}

double SampleUtils::gainEQ(double freq, QVector<int> eqGains)
{
    int x1 = 0;
    int x2 = 1;
    int y1 = 0;
    int y2 = 1;
    if (freq < 32)
    {
        x1 = 32; x2 = 64;
        y1 = qMin(eqGains[0], eqGains[1]); y2 = eqGains[1];
    }
    else if (freq < 64)
    {
        x1 = 32; x2 = 64;
        y1 = eqGains[0]; y2 = eqGains[1];
    }
    else if (freq < 125)
    {
        x1 = 64; x2 = 125;
        y1 = eqGains[1]; y2 = eqGains[2];
    }
    else if (freq < 250)
    {
        x1 = 125; x2 = 250;
        y1 = eqGains[2]; y2 = eqGains[3];
    }
    else if (freq < 500)
    {
        x1 = 250; x2 = 500;
        y1 = eqGains[3]; y2 = eqGains[4];
    }
    else if (freq < 1000)
    {
        x1 = 500; x2 = 1000;
        y1 = eqGains[4]; y2 = eqGains[5];
    }
    else if (freq < 2000)
    {
        x1 = 1000; x2 = 2000;
        y1 = eqGains[5]; y2 = eqGains[6];
    }
    else if (freq < 4000)
    {
        x1 = 2000; x2 = 4000;
        y1 = eqGains[6]; y2 = eqGains[7];
    }
    else if (freq < 8000)
    {
        x1 = 4000; x2 = 8000;
        y1 = eqGains[7]; y2 = eqGains[8];
    }
    else if (freq < 16000)
    {
        x1 = 8000; x2 = 16000;
        y1 = eqGains[8]; y2 = eqGains[9];
    }
    else
    {
        x1 = 8000; x2 = 16000;
        y1 = eqGains[8]; y2 = qMin(eqGains[8], eqGains[9]);
    }
    double a = static_cast<double>(y1 - y2) / (x1 - x2);
    double b = static_cast<double>(y2) - a * x2;

    // Gain en dB
    double val = a * freq + b;

    // Conversion
    return pow(10.0, 0.1 * val);
}

void SampleUtils::regimePermanent(QVector<float> data, quint32 dwSmplRate, quint32 &posStart, quint32 &posEnd, quint32 nbOK, float coef)
{
    // Calcul de la moyenne des valeurs absolues sur une période de 0.05 s à chaque 10ième de seconde
    quint32 sizePeriode = dwSmplRate / 10;
    quint32 len = static_cast<quint32>(data.size());
    if (len < sizePeriode)
    {
        // Take the full length of the sample
        posStart = 0;
        posEnd = (len == 0 ? 0 : len - 1);
        return;
    }
    quint32 nbValeurs = (len - sizePeriode) / (dwSmplRate / 20);
    if (nbValeurs == 0)
    {
        // Take also the full length of the sample
        posStart = 0;
        posEnd = (len == 0 ? 0 : len - 1);
        return;
    }
    QVector<float> tableauMoyennes;
    tableauMoyennes.resize(static_cast<int>(nbValeurs));
    for (quint32 i = 0; i < nbValeurs; i++)
    {
        float valTmp = 0;
        for (quint32 j = 0; j < sizePeriode; j++)
            valTmp += qAbs(data[static_cast<int>((dwSmplRate / 20) * i + j)]);
        data[static_cast<int>(i)] = valTmp / sizePeriode;
    }

    // Calcul de la médiane des valeurs
    float med = median(tableauMoyennes);
    posStart = 0;
    posEnd = nbValeurs - 1;
    quint32 count = 0;
    while (count < nbOK && posStart <= posEnd)
    {
        if (data[static_cast<int>(posStart)] < coef * med && data[static_cast<int>(posStart)] > med / coef)
            count++;
        else
            count = 0;
        posStart++;
    }
    posStart = posStart + 2 - count;
    count = 0;
    while (count < nbOK && posEnd > 0)
    {
        if (data[static_cast<int>(posEnd)] < coef * med && data[static_cast<int>(posEnd)] > med / coef)
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

double SampleUtils::sinc(double x)
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
void SampleUtils::KBDWindow(double* window, int size, double alpha)
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

double SampleUtils::BesselI0(double x)
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

float SampleUtils::computeLoopQuality(QVector<float> vData, quint32 loopStart, quint32 loopEnd, quint32 checkNumber, bool bipolar, float maxValue)
{
    const float * data = vData.constData();
    quint32 length = vData.size();
    if (loopStart < 2 || loopStart >= loopEnd || loopEnd >= length)
        return 0;

    // Compute the difference at the loop points and a bit before
    int n = 1;
    float result = getDiffForLoopQuality(data, loopStart, loopEnd);

    quint32 offset = 3;
    for (quint32 i = 1; i < checkNumber; i++)
    {
        offset += (M_PI_2 /* something irrational */ + i - 1) * i;
        if (loopStart > 3 + offset)
        {
            result += getDiffForLoopQuality(data, loopStart - offset - 1, loopEnd - offset - 1);
            n += 1;
        }

    }

    if (bipolar)
    {
        // Also take into account data following the loop point
        offset = 3;
        for (quint32 i = 1; i < checkNumber; i++)
        {
            offset += (M_PI_2 /* something irrational */ + i - 1) * i;

            if (loopEnd + 3 + offset < vData.size())
            {
                result += getDiffForLoopQuality(data, loopStart + offset - 1, loopEnd + offset - 1);
                n += 1;
            }
        }
    }

    // Maximum value inside the loop
    float max = maxValue;
    if (max < 0)
    {
        max = 0.00045f;
        float tmp;
        for (quint32 i = loopStart; i <= loopEnd; i++)
        {
            tmp = data[i];
            if (tmp > max)
                max = tmp;
            else if (-tmp > max)
                max = -tmp;
        }
    }

    // Differences are relative to the maximum
    return result / (max * n);
}

float SampleUtils::getDiffForLoopQuality(const float * data, quint32 pos1, quint32 pos2)
{
    // Difference on the loop points
    float diff0 = data[pos1] - data[pos2];

    // Difference on the slope
    float diff1 = data[pos1 - 1] - data[pos2 - 1] - diff0;

    // Difference on the slope difference
    float diff2 = data[pos1 - 2] - data[pos2 - 2] - 2 * diff1 + diff0;

    // Combine the values
    return 0.45f * qAbs(diff0) + 0.33f * qAbs(diff1) + 0.22f * qAbs(diff2);
}

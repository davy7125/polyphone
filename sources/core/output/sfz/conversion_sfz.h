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

#ifndef CONVERSION_SFZ_H
#define CONVERSION_SFZ_H

#include <QString>
#include <QList>
#include <QMap>
#include <QTextStream>
#include "qmath.h"
#include "basetypes.h"
#include "sfz/sfzwriter.h"
class SfzParamList;
class SoundfontManager;
class QFile;
class BalanceParameters;
class SfzWriter;

class ConversionSfz : public QObject
{
    Q_OBJECT
    
public:
    ConversionSfz();

    /// Convert a soundfont
    /// dirPath is the root directory and must be created yet (otherwise a suffix will be added)
    QString convert(QString dirPath, EltID idSf2, bool presetPrefix, bool bankDir, bool gmSort);

private:
    SoundfontManager * _sf2;
    QMap<int, QPair<int, QString> > _mapSamples;
    QMap<int, int> _stereoIds;
    QString _dirSamples;
    bool _bankSortEnabled, _gmSortEnabled;
    SfzWriter * _sfzWriter;

    void exportPrst(QString dir, EltID id, bool presetPrefix);
    QString getPathSfz(QString dir, QString name);
    QString getSamplePath(EltID idSmpl, int &sampleChannel, int &linkedSampleId);
    QMap<AttributeType, AttributeValue> getInstSmplParameters(EltID idInstSmpl);

    int findOtherChannel(EltID idSmpl);
    void writeEntete(EltID id);
    void writeGroup(SfzParamList * listeParam, bool isPercKit);
    void writeRegion(SfzParamList * listeParam, QString pathSample, BalanceParameters *balance);
    void writeElement(AttributeType champ, double value);
    void writeModulator(ModulatorData modData);
    bool isIncluded(SfzParamList * paramPrst, EltID idInstSmpl);
    static double dbToPercent(double dB) { return 100. * pow(10, -dB / 20); }
    static int lastLettersToRemove(QString str1, QString str2);
    static QString getDirectoryName(int numPreset);
    static QString getDrumCategory(int numPreset);
};

#endif // CONVERSION_SFZ_H

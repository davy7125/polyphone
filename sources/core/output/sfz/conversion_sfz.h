/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2017 Davy Triponney                                **
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
**  Website/Contact: http://polyphone-soundfonts.com                      **
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

class SoundfontManager;
class QFile;

class ParamListe
{
public:
    ParamListe(SoundfontManager * sf2, EltID id);
    ParamListe(SoundfontManager * sf2, ParamListe * paramPrst, EltID idInst);
    AttributeType getChamp(int num)     { return _listeChamps.at(num); }
    double getValeur(int num)   { return _listeValeurs.at(num); }
    int size()                  { return _listeChamps.size(); }
    int findChamp(AttributeType champ)  { return _listeChamps.indexOf(champ); }

private:
    QList<AttributeType> _listeChamps;
    QList<double> _listeValeurs;
    static int _boucleGlobale;

    double limit(double val, AttributeType champ);
    void load(SoundfontManager *sf2, EltID id);
    void getGlobalValue(SoundfontManager * sf2, EltID id, AttributeType champ);
    void mix(AttributeType champCoarse, AttributeType champFine, int addValue = 0);
    void fusion(AttributeType champ, double value);
    void prepend(AttributeType champ);
    void adaptKeynum2();
    void adaptKeynum2(int minKey, int maxKey, AttributeType champBase, AttributeType champKeynum);
    void adaptLfo(SoundfontManager *sf2, EltID idInstSmpl);
    double getValKeynum(double valBase, int key, double keynum);
};


class ConversionSfz
{
public:
    ConversionSfz(SoundfontManager * sf2);

    // Export en sfz
    void convert(QString dir, QList<EltID> listID, bool presetPrefix, bool bankDir, bool gmSort);

private:
    SoundfontManager * _sf2;
    QMap<int, QString> _mapStereoSamples, _mapMonoSamples;
    QString _dirSamples;
    bool _bankSortEnabled, _gmSortEnabled;

    void exportPrst(QString dir, EltID id, bool presetPrefix);
    QString getPathSfz(QString dir, QString name);
    QString getLink(EltID idSmpl, bool enableStereo);
    void writeEntete(QFile * fichierSfz, EltID id);
    void writeGroup(QFile * fichierSfz, ParamListe * listeParam, bool isPercKit);
    void writeRegion(QFile * fichierSfz, ParamListe * listeParam, QString pathSample, bool ignorePan);
    void writeElement(QTextStream &out, AttributeType champ, double value);
    bool isIncluded(ParamListe * paramPrst, EltID idInstSmpl);
    static double dbToPercent(double dB) { return 100. * pow(10, -dB / 20); }
    static QString escapeStr(QString str);
    static int lastLettersToRemove(QString str1, QString str2);
    static QString getDirectoryName(int numPreset);
    static QString getDrumCategory(int numPreset);
};

#endif // CONVERSION_SFZ_H

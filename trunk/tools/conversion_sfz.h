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

#ifndef CONVERSION_SFZ_H
#define CONVERSION_SFZ_H

#include <QString>
#include <QList>
#include "sf2_types.h"

class Pile_sf2;


class ParamListe
{
public:
    ParamListe(Pile_sf2 * sf2, EltID id);
    ParamListe(Pile_sf2 * sf2, ParamListe * paramPrst, EltID idInst);
    Champ getChamp(int num)     { return _listeChamps.at(num); }
    double getValeur(int num)   { return _listeValeurs.at(num); }
    int size()                  { return _listeChamps.size(); }

private:
    QList<Champ> _listeChamps;
    QList<double> _listeValeurs;

    double getValue(Champ champ, genAmountType amount, bool isPrst);
    double d1200e2(qint32 val) { return exp2((double)val / 1200.); }
    double limit(double val, Champ champ);
    void load(Pile_sf2 *sf2, EltID id);
    void mix(Champ champCoarse, Champ champFine);
    void fusion(Champ champ, double value);
    double getDefaultValue(Champ champ);
    void prepend(Champ champ);
};


class ConversionSfz
{
public:
    ConversionSfz(Pile_sf2 * sf2);

    // Export en sfz, renvoi de l'adresse du dernier instrument créé
    QString convert(QString dir, EltID id);

private:
    Pile_sf2 * _sf2;
    QList<int> _sampleIDs;
    QList<QString> _samplePaths;

    QString exportPrst(QString dir, EltID id);
    QString getPathSfz(QString dir, QString name);
    QString getLink(QString root, EltID idSmpl);
    void writeEntete(QFile * fichierSfz, EltID id);
    void writeGroup(QFile * fichierSfz, ParamListe * listeParam);
    void writeRegion(QFile * fichierSfz, ParamListe * listeParam, QString pathSample);
    void writeElement(QTextStream &out, Champ champ, double value);
    static double dbToPercent(double dB) { return 100. * pow(10, -dB / 20); }
    static QString escapeStr(QString str);
    static int lastLettersToRemove(QString str1, QString str2);
};

#endif // CONVERSION_SFZ_H

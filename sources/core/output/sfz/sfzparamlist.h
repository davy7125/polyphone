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

#ifndef SFZPARAMLIST_H
#define SFZPARAMLIST_H

#include "basetypes.h"
class SoundfontManager;

class SfzParamList
{
public:
    SfzParamList(SoundfontManager * sf2, EltID id);
    SfzParamList(SoundfontManager * sf2, SfzParamList * paramPrst, EltID idInst);
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

#endif // SFZPARAMLIST_H

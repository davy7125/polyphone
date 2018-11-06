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

#ifndef DUPLICATOR_H
#define DUPLICATOR_H

#include "sf2_types.h"

class QWidget;
class Pile_sf2;

class Duplicator
{
public:
    Duplicator(Pile_sf2 * source, Pile_sf2 * destination, QWidget * parent = NULL);
    ~Duplicator() {}

    // Copie idSrc vers idDest si sf2 différent
    // Sinon établit un lien (InstSmpl ou PrstInst)
    // Ou ne fait rien
    void copy(EltID idSource, EltID idDest);

private:
    enum EtatMessage
    {
        DUPLIQUER_TOUT = -1,
        DUPLIQUER,
        REMPLACER,
        REMPLACER_TOUT,
        IGNORER,
        IGNORER_TOUT
    };

    QWidget * _parent;
    Pile_sf2 * _source, * _destination;

    // Correspondances
    QList<EltID> _listCopy, _listPaste;
    EtatMessage _copieSmpl, _copieInst, _copiePrst;
    bool _presetFull;

    // DEPLACEMENT DANS UN MEME SF2 //
    void linkSmpl(EltID idSource, EltID idDest);
    void linkInst(EltID idSource, EltID idDest);
    void copyLink(EltID idSource, EltID idDest);

    // DEPLACEMENT DANS UN AUTRE SF2 //
    void copySmpl(EltID idSource, EltID idDest);
    void copyInst(EltID idSource, EltID idDest);
    void copyPrst(EltID idSource, EltID idDest);

    // Utilitaires
    void copyGen(EltID idSource, EltID idDest);
    void copyMod(EltID idSource, EltID idDest);
    EtatMessage openDialog(QString question);
    void reset(EltID idDest);
};

#endif // DUPLICATOR_H

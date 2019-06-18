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

#ifndef DUPLICATOR_H
#define DUPLICATOR_H

#include "basetypes.h"
#include <QList>
#include <QMap>

class QWidget;
class SoundfontManager;

class Duplicator: public QObject
{
    Q_OBJECT

public:
    Duplicator();
    ~Duplicator() {}

    // Copie idSrc vers idDest si sf2 différent
    // Sinon établit un lien (InstSmpl ou PrstInst)
    // Ou ne fait rien
    void copy(EltID idSource, EltID idDest);

    // Duplicate an element
    void duplicate(EltID id);

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
    SoundfontManager * _sm;

    // Correspondances
    QList<EltID> _listCopy, _listPaste;
    EtatMessage _copieSmpl, _copieInst, _copiePrst;
    bool _presetFull, _displayWarningGlobal;

    // Initial sample, instrument and preset indexes
    QMap<int, QList<int> > _initialSmplIndexes, _initialInstIndexes, _initialPrstIndexes;

    // Link
    void linkSmpl(EltID idSource, EltID idDest);
    void linkInst(EltID idSource, EltID idDest);
    void copyLink(EltID idSource, EltID idDest);
    void copyGlobal(EltID idSource, EltID idDest);

    // Copy
    void copySmpl(EltID idSource, EltID idDest);
    void copyInst(EltID idSource, EltID idDest, bool withSmpl);
    void copyPrst(EltID idSource, EltID idDest, bool withInst);

    // Utils
    void copyGen(EltID idSource, EltID idDest);
    void copyMod(EltID idSource, EltID idDest);
    EtatMessage openDialog(QString question);
    void reset(EltID idDest);
    bool isGlobalEmpty(EltID id);
    QString adaptName(QString nom, EltID idDest);
    static QString getName(QString name, int maxCharacters, int suffixNumber);
};

#endif // DUPLICATOR_H

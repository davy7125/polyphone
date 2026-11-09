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

#ifndef DUPLICATOR_H
#define DUPLICATOR_H

#include "basetypes.h"
#include <QList>
#include <QMap>

class SoundfontManager;

// Copy of elements within a soundfont or toward another one. Without interaction, the elements having the same name as
// existing ones are duplicated and no warnings are shown: the user interface derives this class to ask what to do
// and to show the warnings (InteractiveDuplicator).
class Duplicator: public QObject
{
    Q_OBJECT

public:
    Duplicator();
    virtual ~Duplicator() {}

    // Copy idSrc toward idDest if sf2 different
    // Else add a division (InstSmpl or PrstInst)
    // Else nothing
    EltID copy(EltID idSource, EltID idDest);

    // Duplicate an element
    EltID duplicate(EltID id);

    // No questions asked (automation): elements having the same name are duplicated, a filled global division is
    // not overwritten and no warnings are displayed
    void setNonInteractive();

protected:
    enum EtatMessage
    {
        DUPLIQUER_TOUT = -1,
        DUPLIQUER,
        REMPLACER,
        REMPLACER_TOUT,
        IGNORER,
        IGNORER_TOUT
    };

    /// An element has the same name as an existing one: return what to do with it (and possibly with the next ones)
    virtual EtatMessage askReplacement(QString question) { Q_UNUSED(question) return DUPLIQUER_TOUT; }

    /// Warning: the global division is not copied, the destination having one
    virtual void warnGlobalNotCopied() {}

    /// Warning: no preset is available in the destination
    virtual void warnNoPresetAvailable() {}

private:
    SoundfontManager * _sm;

    // Correspondances
    QList<EltID> _listCopy, _listPaste;
    EtatMessage _copieSmpl, _copieInst, _copiePrst;
    bool _presetFull, _displayWarningGlobal;

    // Initial sample, instrument and preset indexes
    QMap<int, QList<int> > _initialSmplIndexes, _initialInstIndexes, _initialPrstIndexes;

    // Link
    EltID linkSmpl(EltID idSource, EltID idDest);
    EltID linkInst(EltID idSource, EltID idDest);
    EltID copyLink(EltID idSource, EltID idDest);
    void copyGlobal(EltID idSource, EltID idDest);

    // Copy
    EltID copySmpl(EltID idSource, EltID idDest);
    EltID copyInst(EltID idSource, EltID idDest, bool withSmpl);
    EltID copyPrst(EltID idSource, EltID idDest, bool withInst);

    // Utils
    void copyGen(EltID idSource, EltID idDest);
    void copyMod(EltID idSource, EltID idDest);
    void reset(EltID idDest);
    bool isGlobalEmpty(EltID id);
    QString adaptName(QString name, EltID idDest);
    static QString getName(QString name, int maxCharacters, int suffixNumber);
};

#endif // DUPLICATOR_H

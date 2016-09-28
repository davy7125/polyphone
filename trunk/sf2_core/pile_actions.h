/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
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

#ifndef PILE_ACTIONS_H
#define PILE_ACTIONS_H

#include "sf2_types.h"
#include <QList>


class Action
{
public:
    // Catégorie d'action
    typedef enum
    {
        TypeRemoval = 0,
        TypeCreation = 1,
        TypeUpdate = 2,
        TypeChangeFromDefault = 3,
        TypeChangeToDefault = 4,
        TypeNull = 5
    } ActionType;

    // Data
    ActionType typeAction;
    EltID id;
    Champ champ;
    QString qNewValue;
    QString qOldValue;
    Valeur vNewValue;
    Valeur vOldValue;
    QByteArray baNewValue;
    QByteArray baOldValue;

    /// Decrement the index
    /// Return true if the action must be deleted
    bool decrement(EltID id);
};


class ActionManager
{
public:
    ActionManager() : _numEdition(0) {}
    ~ActionManager()
    {
        while (!_redoActions.isEmpty())
            delete _redoActions.takeFirst();
    }

    /// Prepare a new action set
    void newActionSet();

    /// Add an action in the last action set
    void add(Action *action);


    void cleanActions();

    /// Get the last edition of an sf2
    int getEdition(int indexSf2);

    /// Perform an undo, get a list of actions to process
    QList<Action *> undo();

    /// Perform a redo, get a list of actions to process
    QList<Action *> redo();

    /// Number of possible "undo"
    int countUndo() { return _undoActions.count(); }
    bool isUndoable() { return countUndo() > 0; }

    /// Number of possible "redo"
    int countRedo() { return _redoActions.count(); }
    bool isRedoable() { return countRedo() > 0; }

    /// Get actions associated to an undo
    QList<Action *> getUndoActions(int pos) { return _undoActions[pos]->_actions; }

    /// Get actions associated to a redo
    QList<Action *> getRedoActions(int pos) { return _redoActions[pos]->_actions; }

    /// Delete an undo
    void deleteUndo(int pos);

    /// Delete a redo
    void deleteRedo(int pos);

    /// Adjust the ids
    void decrement(EltID id);

private:
    // Action set contains a set of action and the edition number of the different sf2 when they were edited
    class ActionSet
    {
    public:
        struct Edition
        {
            int indexSf2;
            double numEdition;
        };

        ~ActionSet()
        {
            while (!_actions.isEmpty())
                delete _actions.takeFirst();
        }

        /// Get the edition of an sf2
        int getEdition(int indexSf2);

        /// An sf2 has been definitely removed
        void removeSf2(int indexSf2);

        QList<Action *> _actions;
        QList<Edition> _editions;
    };

    QList<ActionSet *> _redoActions;
    QList<ActionSet *> _undoActions;
    int _numEdition;
};

#endif // PILE_ACTIONS_H

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

#include "actionset.h"
#include "action.h"

const int ActionSet::UNDO_NUMBER = 50;

ActionSet::ActionSet() :
    _latestEdition(0)
{

}

ActionSet::~ActionSet()
{
    // Delete all actions in the redo
    while (!_redoActions.empty())
    {
        QPair<int, QList<Action *> > item = _redoActions.takeFirst();
        while (!item.second.empty())
            delete item.second.takeFirst();
    }

    // Delete all actions in the undo
    while (!_undoActions.empty())
    {
        QPair<int, QList<Action *> > item = _undoActions.takeFirst();
        while (!item.second.empty())
            delete item.second.takeFirst();
    }
}

void ActionSet::addActions(QList<Action *> actions)
{
    if (actions.empty())
        return; // Nothing to do

    // First remove all redo that are now invalidated
    this->clearRedo();

    // Increment the edition and store it along with the action list
    _latestEdition++;
    _undoActions.insert(0, QPair<int, QList<Action *> >(_latestEdition, actions));

    // Finally, clean the undo so that they are not unlimited
    this->cleanUndo();
}

int ActionSet::getCurrentEdition()
{
    if (_undoActions.empty())
        return 0; // Base edition number
    return _undoActions.first().first;
}


bool ActionSet::isUndoable()
{
    return !_undoActions.empty();
}

bool ActionSet::isRedoable()
{
    return !_redoActions.empty();
}

QList<Action *> ActionSet::undo()
{
    if (_undoActions.empty())
        return QList<Action *>();

    // An undo become a redo
    _redoActions.insert(0, _undoActions.takeFirst());

    // Return the first redo
    return _redoActions.first().second;
}

QList<Action *> ActionSet::redo()
{
    if (_redoActions.empty())
        return QList<Action *>();

    // A redo become an undo
    _undoActions.insert(0, _redoActions.takeFirst());

    // Return the first undo
    return _undoActions.first().second;
}

void ActionSet::clearRedo()
{
    while (!_redoActions.empty())
    {
        QPair<int, QList<Action *> > item = _redoActions.takeLast(); // Reverse order
        while (!item.second.empty())
        {
            Action * actionToDelete = item.second.takeFirst(); // Normal order

            // Definitely delete an element that has been created and then deleted
            if (actionToDelete->typeAction == Action::TypeCreation)
                emit(dropId(actionToDelete->id));

            delete actionToDelete;
        }
    }
}

void ActionSet::cleanUndo()
{
    while (_undoActions.count() > UNDO_NUMBER)
    {
        QPair<int, QList<Action *> > item = _undoActions.takeLast(); // Reverse order
        while (!item.second.empty())
        {
            Action * actionToDelete = item.second.takeLast(); // Reverse order

            // Definitely delete an element that has been deleted a long time ago
            if (actionToDelete->typeAction == Action::TypeRemoval)
                emit(dropId(actionToDelete->id));

            delete actionToDelete;
        }
    }
}

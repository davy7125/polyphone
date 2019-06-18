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

#include "actionmanager.h"
#include "actionset.h"
#include "action.h"
#include <QMap>

ActionManager::ActionManager() {}

ActionManager::~ActionManager()
{
    while (!_currentActions.isEmpty())
        delete _currentActions.takeFirst();
    QList<int> keys = _actionSets.keys();
    foreach (int key, keys)
        delete _actionSets.take(key);
}

void ActionManager::add(Action *action)
{
    _mutex.lock();
    _currentActions.insert(0, action);
    _mutex.unlock();
}

QList<int> ActionManager::commitActionSet()
{
    if (_currentActions.empty())
        return QList<int>(); // Nothing to do

    // Separate the actions per soundfont
    QMap<int, QList<Action *> > actionsPerSoundfont;
    while (!_currentActions.isEmpty())
    {
        Action * action = _currentActions.takeFirst();
        actionsPerSoundfont[action->id.indexSf2] << action;
    }

    // Add the new sets of actions
    foreach (int sf2Index, actionsPerSoundfont.keys())
    {
        if (!_actionSets.contains(sf2Index))
        {
            _actionSets[sf2Index] = new ActionSet();
            connect(_actionSets[sf2Index], SIGNAL(dropId(EltID)), this, SIGNAL(dropId(EltID)));
        }
        _actionSets[sf2Index]->addActions(actionsPerSoundfont[sf2Index]);
    }

    return actionsPerSoundfont.keys();
}

void ActionManager::clearCurrentActionSet()
{
    while (!_currentActions.isEmpty())
        delete _currentActions.takeFirst();
}

int ActionManager::getEdition(int sf2Index)
{
    if (!_actionSets.contains(sf2Index))
        return 0;
    return _actionSets[sf2Index]->getCurrentEdition();
}

bool ActionManager::isUndoable(int sf2Index)
{
    if (!_actionSets.contains(sf2Index))
        return false;
    return _actionSets[sf2Index]->isUndoable();
}

bool ActionManager::isRedoable(int sf2Index)
{
    if (!_actionSets.contains(sf2Index))
        return false;
    return _actionSets[sf2Index]->isRedoable();
}

QList<Action *> ActionManager::undo(int sf2Index)
{
    commitActionSet();
    if (!_actionSets.contains(sf2Index))
        return QList<Action *>();
    return _actionSets[sf2Index]->undo();
}

QList<Action *> ActionManager::redo(int sf2Index)
{
    commitActionSet();
    if (!_actionSets.contains(sf2Index))
        return QList<Action *>();
    return _actionSets[sf2Index]->redo();
}

void ActionManager::dropSoundfont(int sf2Index)
{
    if (_actionSets.contains(sf2Index))
        delete _actionSets.take(sf2Index);
}

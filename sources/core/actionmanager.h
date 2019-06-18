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

#ifndef PILE_ACTIONS_H
#define PILE_ACTIONS_H

#include <QList>
#include <QMap>
#include <QObject>
#include <QMutex>
#include "basetypes.h"
class ActionSet;
class Action;

class ActionManager: public QObject
{
    Q_OBJECT

public:
    ActionManager();
    ~ActionManager();

    /// Add an action in the current action set
    void add(Action *action);

    /// Commit the current action set, get the list of the edited soundfonts
    QList<int> commitActionSet();

    /// Clear the current action set
    void clearCurrentActionSet();

    /// Get the current action list
    QList<Action *> getCurrentActions() { return _currentActions; }

    /// Get the current edition of an sf2
    int getEdition(int sf2Index);

    /// Return true if at least one "undo" is available for a soundfont
    bool isUndoable(int sf2Index);

    /// Return true if at least one "redo" is available for a soundfont
    bool isRedoable(int sf2Index);

    /// Perform an undo, get a list of actions to process
    QList<Action *> undo(int sf2Index);

    /// Perform a redo, get a list of actions to process
    QList<Action *> redo(int sf2Index);

    /// When a soundfont is closed, free all actions associated to it
    void dropSoundfont(int sf2Index);

signals:
    /// Emitted when an ID must be definitely removed
    void dropId(EltID id);

private:
    // Current list of actions that are occurring
    QList<Action *> _currentActions;

    // Redo / undo and latest edition per soundfont
    QMap<int, ActionSet *> _actionSets;

    // For protecting the insertion of action (if concurrent updates)
    QMutex _mutex;
};

#endif // PILE_ACTIONS_H

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

#ifndef ACTIONSET_H
#define ACTIONSET_H

#include <QList>
#include <QObject>
#include "basetypes.h"
class Action;

class ActionSet: public QObject
{
    Q_OBJECT

public:
    ActionSet();
    ~ActionSet();

    /// Add a new set of actions as a new undo
    void addActions(QList<Action *> actions);

    /// Current edition corresponding to the latest undo
    int getCurrentEdition();

    /// True if at least one undo is available
    bool isUndoable();

    /// True if at least one redo is available
    bool isRedoable();

    /// Perform an undo and get the list of actions to compute
    QList<Action *> undo();

    /// Perform a redo and get the list of actions to compute
    QList<Action *> redo();

signals:
    void dropId(EltID id);

private:
    void clearRedo();
    void cleanUndo();

    static const int UNDO_NUMBER;

    QList<QPair<int, QList<Action *> > > _redoActions;
    QList<QPair<int, QList<Action *> > > _undoActions;
    int _latestEdition;
};

#endif // ACTIONSET_H

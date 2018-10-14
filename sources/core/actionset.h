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

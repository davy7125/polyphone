#ifndef ACTION_H
#define ACTION_H

#include "sf2_types.h"

class Action
{
public:
    Action();

    // Type of action
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
};

#endif // ACTION_H

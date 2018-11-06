#ifndef ACTION_H
#define ACTION_H

#include "basetypes.h"

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
    AttributeType champ;
    QString qNewValue;
    QString qOldValue;
    AttributeValue vNewValue;
    AttributeValue vOldValue;
    QByteArray baNewValue;
    QByteArray baOldValue;
};

#endif // ACTION_H

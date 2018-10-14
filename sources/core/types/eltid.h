#ifndef ELTID_H
#define ELTID_H

#include <QMetaType>
#include <QString>

typedef enum
{
    elementUnknown = -1,
    elementSf2 = 0,
    elementSmpl = 1,
    elementInst = 2,
    elementPrst = 3,
    elementInstSmpl = 4,
    elementPrstInst = 5,
    elementRootSmpl = 6,
    elementRootInst = 7,
    elementRootPrst = 8,
    elementInstMod = 9,
    elementPrstMod = 10,
    elementInstSmplMod = 11,
    elementPrstInstMod = 12,
    elementInstGen = 13,
    elementPrstGen = 14,
    elementInstSmplGen = 15,
    elementPrstInstGen = 16
} ElementType;

class EltID
{
public:
    /// Constructors
    EltID(ElementType elementType, int indexSf2 = -1, int indexElt = -1, int indexElt2 = -1, int indexMod = -1);
    EltID(EltID * other);
    EltID();

    /// Parent of the ID, possibly including elementRootSmpl / elementRootInst / elementRootPrst (elementSf2 otherwise)
    EltID parent(bool includeRoot = false);

    /// Operator for comparison
    bool operator !=(const EltID &other);
    bool operator==(const EltID &other);

    /// Textual description of the ID (for debug purposes)
    QString toString();

    /// Public access to the id keys
    ElementType typeElement;
    int indexSf2;
    int indexElt;
    int indexElt2;
    int indexMod;
};

Q_DECLARE_METATYPE(EltID)

#endif // ELTID_H

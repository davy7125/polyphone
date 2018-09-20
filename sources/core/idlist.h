#ifndef IDLIST_H
#define IDLIST_H

#include <QList>
#include <QMap>
#include "sf2_types.h"

class IdList : public QList<EltID>
{
public:
    // Constructors
    IdList() : QList<EltID>() {}
    IdList(EltID id) : QList<EltID>() { this->append(id); }

    // Call this function if an element has been added or removed after one of the methods below is used
    void resetComputedLists();

    // Get all possible ids of a specific type based on the current selection
    IdList getSelectedIds(ElementType type);

    // Get the first id of a specific type. Can return unknown if nothing is found
    EltID getFirstId(ElementType type);

    // Return true if the selection is related to only one smpl / inst / prst / sf2
    bool isElementUnique(ElementType type);

    // Return true if the selection is only related to smpl / inst / prst / sf2
    bool areAllWithType(ElementType type);

    // For instruments and presets only, return true if all elements comprises divisions (InstSmpl or PrstInst)
    bool areAllWithDivisions(ElementType type);

    // For instrument and preset only, return true if all their divisions have a range specified
    bool areAllWithRange(ElementType type);

private:
    QMap<ElementType, IdList > _computedLists;
};

#endif // IDLIST_H

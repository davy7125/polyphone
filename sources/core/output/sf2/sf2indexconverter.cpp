#include "sf2indexconverter.h"
#include "soundfontmanager.h"

Sf2IndexConverter::Sf2IndexConverter(EltID id)
{
    _indexes = SoundfontManager::getInstance()->getSiblings(id);
}

int Sf2IndexConverter::getIndexOf(int index, bool isModDestOper)
{
    int correspondingIndex = 0;

    if (isModDestOper)
    {
        if (index < 32768) // Same destination
            correspondingIndex = index;
        else if (_indexes.contains(index - 32768)) // Adapt the destination mod
            correspondingIndex = 32768 + _indexes.indexOf(index - 32768);
    }
    else if (_indexes.contains(index))
        correspondingIndex = _indexes.indexOf(index);

    return correspondingIndex;
}

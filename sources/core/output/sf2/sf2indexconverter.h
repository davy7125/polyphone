#ifndef SF2INDEXCONVERTER_H
#define SF2INDEXCONVERTER_H

#include "basetypes.h"

class Sf2IndexConverter
{
public:
    Sf2IndexConverter(EltID id);
    int getIndexOf(int index, bool isModDestOper);

private:
    QList<int> _indexes;
};

#endif // SF2INDEXCONVERTER_H

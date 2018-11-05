#ifndef SF2SDTAPART_H
#define SF2SDTAPART_H

#include "sf2basetypes.h"
class QDataStream;

class Sf2SdtaPart
{
public:
    Sf2SdtaPart();

    bool _isValid;

    char _LIST[4]; // Should be "LIST"
    quint32Reversed _sdtaSize; // Size of the section sdta
    char _sdta[4]; // Should be "sdta"
    int _startSmplOffset; // Relative position of the first sample in the section "smpl" (absolute is 20 + infoSize + this value)
    int _startSm24Offset; // Same for the section "sm24"
};

// Extension methods for QDataStream to serialize / deserialize
QDataStream & operator >> (QDataStream &in, Sf2SdtaPart &sdta);
//QDataStream & operator << (QDataStream &out, const Sf2SdtaPart &sdta);

#endif // SF2SDTAPART_H

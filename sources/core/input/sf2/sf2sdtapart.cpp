#include "sf2sdtapart.h"
#include <QDataStream>

Sf2SdtaPart::Sf2SdtaPart() :
    _isValid(false),
    _sdtaSize(0),
    _startSmplOffset(0),
    _startSm24Offset(0)
{

}

QDataStream & operator >> (QDataStream &in, Sf2SdtaPart &sdta)
{
    // 4 char, should be "LIST"
    if (in.readRawData(sdta._LIST, 4) != 4)
        return in;

    // Size of the section "sdta"
    in >> sdta._sdtaSize;

    // 4 char, should be "sdta"
    if (in.readRawData(sdta._sdta, 4) != 4)
        return in;

    // Next block: can be directly the samples or smpl
    QString block;
    {
        char buffer[4];
        if (in.readRawData(buffer, 4) != 4)
            return in;
        block = QString::fromLatin1(buffer, 4);
    }
    if (block == "smpl")
    {
        // Size of the block "smpl"
        quint32Reversed smplSize;
        in >> smplSize;

        // Keep the position of "smpl" and skip the block
        sdta._startSmplOffset = 20;
        in.skipRawData(smplSize.value);

        // Block sm24?
        {
            char buffer[4];
            if (in.readRawData(buffer, 4) != 4)
                return in;
            block = QString::fromLatin1(buffer, 4);
        }
        if (block == "sm24")
        {
            // Size of the block "smpl"
            quint32Reversed sm24Size;
            in >> sm24Size;

            // Keep the position of "sm24" and skip the block
            sdta._startSm24Offset = 20 + smplSize.value + 8;
            in.skipRawData(sm24Size.value);
        }
        else
        {
            // No "sm24" block, we go back
            QIODevice * fi = in.device();
            fi->seek(fi->pos() - 4);

            // No "sm24" position
            sdta._startSm24Offset = 0;
        }
    }
    else
    {
        // Memorize the positions and skip the part
        sdta._startSmplOffset = 12;
        sdta._startSm24Offset = 0;
        in.skipRawData(sdta._sdtaSize.value - 8);
    }

    sdta._isValid = true;
    return in;
}


QDataStream & operator << (QDataStream &out, const Sf2SdtaPart &sdta)
{
    return out;
}

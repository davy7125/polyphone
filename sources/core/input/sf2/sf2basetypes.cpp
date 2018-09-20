#include "sf2basetypes.h"
#include <QDataStream>

QDataStream & operator >> (QDataStream &in, quint32Reversed &val)
{
    quint8 b0, b1, b2, b3;
    in >> b0 >> b1 >> b2 >> b3;
    val.value = b3 << 24 | b2 << 16 | b1 << 8 | b0;
    return in;
}

QDataStream & operator >> (QDataStream &in, quint16Reversed &val)
{
    quint8 b0, b1;
    in >> b0 >> b1;
    val.value = b1 << 8 | b0;
    return in;
}

QDataStream & operator >> (QDataStream &in, qint16Reversed &val)
{
    quint8 b0, b1;
    in >> b0 >> b1;
    val.value = ((short) b1) << 8 | b0;
    return in;
}

QDataStream & operator >> (QDataStream &in, SFModulator &mod)
{
    quint8 b0, b1;
    in >> b0 >> b1;
    mod.Type = (int) b1 >> 2;
    mod.P = bool((b1 >> 1) & 1);
    mod.D = bool(b1 & 1);
    mod.CC = bool(b0 >> 7);
    mod.Index = quint16(b0 & 0x7F);
    return in;
}

#ifndef SF2BASETYPE_H
#define SF2BASETYPE_H

#include "qglobal.h"
#include "sf2_types.h"
class QDataStream;

class quint32Reversed
{
public:
    quint32Reversed(quint32 val = 0) : value(val) {}
    quint32 value;
};
QDataStream & operator >> (QDataStream &in, quint32Reversed &val);

class quint16Reversed
{
public:
    quint16Reversed(quint16 val = 0) : value(val) {}
    quint16 value;
};
QDataStream & operator >> (QDataStream &in, quint16Reversed &val);

class qint16Reversed
{
public:
    qint16Reversed(qint16 val = 0) : value(val) {}
    qint16 value;
};
QDataStream & operator >> (QDataStream &in, qint16Reversed &val);

QDataStream & operator >> (QDataStream &in, SFModulator &mod);

#endif // SF2BASETYPE_H

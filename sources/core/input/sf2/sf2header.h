#ifndef SF2HEADER_H
#define SF2HEADER_H

#include "sf2_types.h"
#include "sf2basetypes.h"
#include <QMap>
class QDataStream;

class Sf2Header
{
public:
    Sf2Header();

    QString getInfo(QString key);
    sfVersionTag getVersion(QString key);

    bool _isValid;

    char _RIFF[4]; // Should be "RIFF"
    quint32Reversed _size; // Full size of the file - 8
    char _sfbk[4]; // Should be "sfbk"
    char _LIST[4]; // Should be "LIST"
    quint32Reversed _infoSize; // Size of the section INFO
    char _INFO[4]; // Should be "INFO"
    QMap<QString, QString> _infos;
    QMap<QString, sfVersionTag> _versions;
};

// Extension methods for QDataStream to serialize / deserialize
QDataStream & operator >> (QDataStream &in, Sf2Header &header);
QDataStream & operator << (QDataStream &out, const Sf2Header &header);

#endif // SF2HEADER_H

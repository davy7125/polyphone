#include "sf2header.h"
#include <QDataStream>

Sf2Header::Sf2Header() :
    _isValid(false),
    _size(0),
    _infoSize(0)
{

}

QString Sf2Header::getInfo(QString key)
{
    if (_infos.contains(key))
        return _infos[key];
    return "";
}

sfVersionTag Sf2Header::getVersion(QString key)
{
    if (_versions.contains(key))
        return _versions[key];
    sfVersionTag version;
    version.wMajor = 0;
    version.wMinor = 0;
    return version;
}

QDataStream & operator >> (QDataStream &in, Sf2Header &header)
{
    // 4 char, should be "RIFF"
    if (in.readRawData(header._RIFF, 4) != 4)
        return in;

    // Full size - 8
    in >> header._size;

    // 4 char, should be "sfbk"
    if (in.readRawData(header._sfbk, 4) != 4)
        return in;

    // 4 char, should be "LIST"
    if (in.readRawData(header._LIST, 4) != 4)
        return in;

    // Size of the section "INFO"
    in >> header._infoSize;

    // 4 char, should be "INFO"
    if (in.readRawData(header._INFO, 4) != 4)
        return in;

    uint pos = 4;
    while (pos < header._infoSize.value)
    {
        // Bloc name
        char blocName[4];
        if (in.readRawData(blocName, 4) != 4)
            return in;
        QString bloc = QString::fromLatin1(blocName, 4);

        // Number of bytes to read
        quint32Reversed size;
        in >> size;
        if (size.value == 0 || size.value > 1000)
            return in;

        if (bloc == "ifil" || bloc == "iver")
        {
            // Read a version
            quint16Reversed valTmp;
            in >> valTmp;
            header._versions[bloc].wMajor = valTmp.value;
            in >> valTmp;
            header._versions[bloc].wMinor = valTmp.value;
        }
        else
        {
            // Read a text
            int length = size.value;
            QByteArray buffer(length, Qt::Uninitialized);
            if (in.readRawData(buffer.data(), length) != length)
                return in;
            header._infos[bloc] = QString::fromLatin1(buffer);
        }

        pos += 8 + size.value;
    }

    header._isValid = true;
    return in;
}

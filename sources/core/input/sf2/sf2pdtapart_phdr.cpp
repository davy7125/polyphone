#include "sf2pdtapart_phdr.h"
#include <QDataStream>

Sf2PdtaPart_phdr::Sf2PdtaPart_phdr() :
    _isValid(false),
    _preset(0),
    _bank(0),
    _pBagIndex(0),
    _library(0),
    _genre(0),
    _morphology(0)
{

}

QDataStream & operator >> (QDataStream &in, Sf2PdtaPart_phdr &phdr)
{
    // Name
    int length = 20;
    QByteArray buffer(length, Qt::Uninitialized);
    if (in.readRawData(buffer.data(), length) != length)
        return in;
    phdr._name = QString::fromLatin1(buffer);

    // Preset, bank
    in >> phdr._preset;
    in >> phdr._bank;

    // Min index for pBag
    in >> phdr._pBagIndex;

    // Characteristics
    in >> phdr._library;
    in >> phdr._genre;
    in >> phdr._morphology;

    phdr._isValid = true;
    return in;
}

QDataStream & operator << (QDataStream &out, const Sf2PdtaPart_phdr &phdr)
{
    return out;
}

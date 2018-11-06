#include "soundfonts.h"
#include "soundfont.h"

Soundfonts::Soundfonts() :
    _soundfontCounter(0)
{

}

Soundfonts::~Soundfonts()
{
    QList<int> keys = _soundfonts.keys();
    foreach (int key, keys)
        delete _soundfonts.take(key);
}

int Soundfonts::addSoundfont()
{
    _soundfonts[_soundfontCounter] = new Soundfont(EltID(elementSf2, _soundfontCounter, -1, -1, -1));
    return _soundfontCounter++;
}

Soundfont * Soundfonts::getSoundfont(int index)
{
    if (_soundfonts.contains(index))
        return _soundfonts[index];
    return nullptr;
}

bool Soundfonts::deleteSoundfont(int index)
{
    if (_soundfonts.contains(index))
    {
        delete _soundfonts.take(index);
        return true;
    }
    return false;
}

int Soundfonts::indexOf(Soundfont * soundfont)
{
    return _soundfonts.values().indexOf(soundfont);
}

QAbstractItemModel * Soundfonts::getModel(int indexSf2)
{
    if (_soundfonts.contains(indexSf2))
        return _soundfonts[indexSf2]->getModel();
    return nullptr;
}

#include "keymapper.h"

int KeyMapper::KEY_NUMBER = 49;
int KeyMapper::ROOT_KEY = 36;

KeyMapper::KeyMapper() :
    _octave(0)
{
    _listCombinaisons.resize(KEY_NUMBER);
}

void KeyMapper::addCombinaisonKey(int key, QKeySequence combinaison)
{
    if (key >= ROOT_KEY && key < KEY_NUMBER + ROOT_KEY)
        _listCombinaisons[key-ROOT_KEY].append(combinaison);
}

void KeyMapper::setCombinaisonKey(int key, int numCombinaison, QString sequence)
{
    key -= ROOT_KEY;
    if (key >= 0 && key < KEY_NUMBER)
    {
        if (numCombinaison >= 0 && numCombinaison < _listCombinaisons.at(key).size())
        {
            _listCombinaisons[key][numCombinaison] = QKeySequence(sequence);
        }
    }
}

void KeyMapper::setOctave(int numOctave)
{
    _octave = numOctave;
}

int KeyMapper::getKey(QKeySequence combinaison)
{
    int index = -1;
    for (int i = 0; i < _listCombinaisons.size(); i++)
    {
        for (int j = 0; j < _listCombinaisons.at(i).size(); j++)
        {
            if (_listCombinaisons.at(i).at(j) == combinaison)
                index = i;
        }
    }
    if (index != -1)
    {
        index += _octave * 12 + ROOT_KEY;
        if (index < 0)
            index = 0;
        else if (index > 127)
            index = 127;
    }
    return index;
}

QString KeyMapper::getSequence(int key, int numCombinaison)
{
    QString strRet = "";
    key -= ROOT_KEY;
    if (key >= 0 && key < KEY_NUMBER)
    {
        if (numCombinaison >= 0 && numCombinaison < _listCombinaisons.at(key).size())
            strRet = _listCombinaisons.at(key).at(numCombinaison).toString(QKeySequence::NativeText);
    }
    return strRet;
}

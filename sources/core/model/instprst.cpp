#include "instprst.h"
#include "soundfont.h"

InstPrst::InstPrst(Soundfont * soundfont, TreeItem * parent, EltID id) : TreeItem(id, parent),
    _soundfont(soundfont),
    _globalDivision(new Division(NULL, NULL, EltID())),
    _divisionCounter(0)
{
}

InstPrst::~InstPrst()
{
    QList<int> keys = _divisions.keys();
    foreach (int key, keys)
        delete _divisions.take(key);
    delete _globalDivision;
}

int InstPrst::addDivision()
{
    EltID childId = this->getId();
    childId.typeElement = (this->getId().typeElement == elementPrst ? elementPrstInst : elementInstSmpl);
    childId.indexElt2 = _divisionCounter;

    _divisions[_divisionCounter] = new Division(this, this, childId);
    _divisions[_divisionCounter]->notifyCreation();
    return _divisionCounter++;
}

Division * InstPrst::getDivision(int index)
{
    if (_divisions.contains(index))
        return _divisions[index];
    return NULL;
}

bool InstPrst::deleteDivision(int index)
{
    if (_divisions.contains(index))
    {
        _divisions[index]->notifyDeletion();
        delete _divisions.take(index);
        return true;
    }
    return false;
}

int InstPrst::indexOfId(int id)
{
    return _divisions.keys().indexOf(id);
}

int InstPrst::childCount() const
{
    return _divisions.count();
}

TreeItem * InstPrst::child(int row)
{
    return _divisions.values()[row];
}

QString InstPrst::display()
{
    if (_extraFields.contains(champ_wBank) && _extraFields.contains(champ_wPreset))
        return QString("%0:%1 %2").arg(_extraFields[champ_wBank], 3, 10, QChar('0')).arg(_extraFields[champ_wPreset], 3, 10, QChar('0')).arg(_name);
    return _name;
}

int InstPrst::row()
{
    // Preset?
    if (this->getId().typeElement == elementPrst)
        return _soundfont->indexOfPreset(this);
    return _soundfont->indexOfInstrument(this);
}

void InstPrst::setName(QString name)
{
    _name = name;
    notifyRename();
}

QString InstPrst::getName()
{
    return _name;
}

void InstPrst::setExtraField(Champ champ, int value)
{
    _extraFields[champ] = value;
    if (champ == champ_wPreset || champ == champ_wBank)
        notifyRename();
}

int InstPrst::getExtraField(Champ champ)
{
    return _extraFields.contains(champ) ? _extraFields[champ] : 0;
}

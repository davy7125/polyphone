#include "division.h"
#include "modulator.h"
#include "instprst.h"
#include "soundfont.h"
#include "smpl.h"

Division::Division(InstPrst * instPrst, TreeItem * parent, EltID id) : TreeItem(id, parent),
    _instPrst(instPrst),
    _modCounter(0)
{}

Division::~Division()
{
    QList<int> modKeys = _modulators.keys();
    foreach (int modKey, modKeys)
        delete _modulators.take(modKey);
}

bool Division::isSet(Champ champ)
{
    return _parameters.contains(champ);
}

Valeur Division::getGen(Champ champ)
{
    Valeur value;
    if (_parameters.contains(champ))
        value.genValue = _parameters[champ];
    else
        value.genValue.wAmount = 0;
    return value;
}

void Division::setGen(Champ champ, Valeur value)
{
    _parameters[champ] = value.genValue;
}

void Division::resetGen(Champ champ)
{
    _parameters.remove(champ);
}

int Division::addMod()
{
    _modulators[_modCounter] = new Modulator(_modCounter);
    return _modCounter++;
}

Modulator * Division::getMod(int index)
{
    if (_modulators.contains(index))
        return _modulators[index];
    return NULL;
}

bool Division::deleteMod(int index)
{
    if (_modulators.contains(index))
    {
        delete _modulators.take(index);
        return true;
    }
    return false;
}

int Division::childCount() const
{
    return 0;
}

TreeItem * Division::child(int row)
{
    Q_UNUSED(row)
    return NULL;
}

QString Division::display()
{
    // Take the name of the corresponding element
    ElementType type = this->getId().typeElement;
    if (type == elementInstSmpl && isSet(champ_sampleID))
    {
        Smpl * smpl = _instPrst->soundfont()->getSample(getGen(champ_sampleID).wValue);
        if (smpl != NULL)
            return smpl->display();
    }
    if (type == elementPrstInst && isSet(champ_instrument))
    {
        InstPrst * inst = _instPrst->soundfont()->getInstrument(getGen(champ_instrument).wValue);
        if (inst != NULL)
            return inst->display();
    }

    return "...";
}

int Division::row()
{
    return _instPrst->indexOfId(_id.indexElt);
}

int Division::indexOfId(int id)
{
    Q_UNUSED(id)
    return -1;
}
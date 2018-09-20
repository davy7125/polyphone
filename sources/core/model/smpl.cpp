#include "smpl.h"
#include "soundfont.h"

Smpl::Smpl(Soundfont * soundfont, TreeItem *parent, EltID id) : TreeItem(id, parent),
    _soundfont(soundfont)
{

}

int Smpl::childCount() const
{
    return 0;
}

TreeItem * Smpl::child(int row)
{
    Q_UNUSED(row)
    return NULL;
}

QString Smpl::display()
{
    return _name;
}

int Smpl::row()
{
    return _soundfont->indexOfSample(this);
}

void Smpl::setName(QString name)
{
    _name = name;
    notifyRename();
}

int Smpl::indexOfId(int id)
{
    Q_UNUSED(id)
    return -1;
}

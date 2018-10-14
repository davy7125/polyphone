#ifndef SMPL_H
#define SMPL_H

#include "basetypes.h"
#include "sound.h"
#include "treeitem.h"
class Soundfont;

class Smpl: public TreeItem
{
public:
    Smpl(Soundfont * soundfont, TreeItem * parent, EltID id);
    virtual ~Smpl() {}

    void setName(QString name);
    QString getName() { return _name; }

    Sound _sound;
    quint16 _wSampleLink;
    SFSampleLink _sfSampleType;

    // TreeItem implementation
    int childCount() const override;
    TreeItem * child(int row) override;
    QString display() override;
    int row() override;
    int indexOfId(int id) override;

private:
    Soundfont * _soundfont;
    QString _name;
};

#endif // SMPL_H

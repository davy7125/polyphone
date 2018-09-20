#ifndef DIVISION_H
#define DIVISION_H

#include "sf2_types.h"
#include <QMap>
#include "treeitem.h"
class Modulator;
class InstPrst;

class Division: public TreeItem
{
public:
    Division(InstPrst * instPrst, TreeItem * parent, EltID id);
    virtual ~Division();

    // Operations on parameters
    bool isSet(Champ champ);
    void setGen(Champ champ, Valeur value);
    void resetGen(Champ champ);
    Valeur getGen(Champ champ);
    const QMap<Champ, genAmountType> getGens() { return _parameters; }

    // Operations on modulators
    int addMod();
    Modulator * getMod(int index);
    const QMap<int, Modulator *> getMods() { return _modulators; }
    bool deleteMod(int index);

    // TreeItem implementation
    int childCount() const override;
    TreeItem * child(int row) override;
    QString display() override;
    int row() override;
    int indexOfId(int id) override;

private:
    InstPrst * _instPrst;
    QMap<int, Modulator *> _modulators;
    QMap<Champ, genAmountType> _parameters;
    int _modCounter;
};

#endif // DIVISION_H

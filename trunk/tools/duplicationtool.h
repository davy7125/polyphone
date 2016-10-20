#ifndef DUPLICATIONTOOL_H
#define DUPLICATIONTOOL_H

#include "pile_sf2.h"

class DuplicationTool : QObject
{
public:
    /// Initialize the duplication tool with an instrument or a preset
    DuplicationTool(Pile_sf2 * sf2, EltID id);

    /// Duplicate each division by key
    void duplicateByKey();

    /// Duplicate each division by velocity
    void duplicateByVelocity(QVector<QPair<int, int> > velocities);

private:
    /// Duplicate a division
    void duplicateByKey(QPair<int, int> keyRange, QPair<int, int> velRange, EltID id);

    /// Duplicate a series of division sharing the same key range
    void duplicateByVelocity(QMap<QPair<int, int>, QList<EltID> > elts, QVector<QPair<int, int> > velocities);

    /// Copy the attributes of idFrom to idTo
    void duplicateGenMod(EltID idFrom, EltID idTo);

    Pile_sf2 * _sf2;
    EltID _id;
    QMap<QPair<int, int>, QMap<QPair<int, int>, QList<EltID> > > _elts;
    bool _isInst;

    static bool lessThan(QPair<int, int> elt1, QPair<int, int> elt2);
};

#endif // DUPLICATIONTOOL_H

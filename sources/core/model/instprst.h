#ifndef INSTPRST_H
#define INSTPRST_H

#include <QMap>
#include "basetypes.h"
#include "treeitem.h"
#include "division.h"
class Soundfont;

class InstPrst: public TreeItem // Common class for inst and prst
{
public:
    InstPrst(Soundfont * soundfont, TreeItem * parent, EltID id);
    virtual ~InstPrst();

    // Link to the soundfont
    Soundfont * soundfont() { return _soundfont; }

    // Division operations
    int addDivision();
    Division * getGlobalDivision() { return _globalDivision; }
    Division * getDivision(int index);
    const QMap<int, Division *> & getDivisions() { return _divisions; }
    bool deleteDivision(int index);
    int indexOfId(int id);

    // Name, extra fields
    void setName(QString name);
    QString getName();
    void setExtraField(AttributeType champ, int value);
    int getExtraField(AttributeType champ);

    // TreeItem implementation
    int childCount() const override;
    TreeItem * child(int row) override;
    QString display() override;
    int row() override;

private:
    Soundfont * _soundfont;
    QMap<int, Division *> _divisions;
    Division * _globalDivision;
    bool _hidden;
    int _divisionCounter;
    QString _name;
    QMap<AttributeType, int> _extraFields; // Used for presets only
};

#endif // INSTPRST_H

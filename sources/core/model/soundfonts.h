#ifndef MODELSOUNDFONTS_H
#define MODELSOUNDFONTS_H

#include <QMap>
class QAbstractItemModel;
class Soundfont;

class Soundfonts
{
public:
    Soundfonts();
    ~Soundfonts();

    int addSoundfont();
    Soundfont * getSoundfont(int index);
    const QMap<int, Soundfont *> getSoundfonts() { return _soundfonts; }
    bool deleteSoundfont(int index);
    int indexOf(Soundfont * soundfont);

    // Get a tree model associated to a soundfont
    QAbstractItemModel *getModel(int indexSf2);

private:
    int _soundfontCounter;
    QMap<int, Soundfont *> _soundfonts;
};

#endif // MODELSOUNDFONTS_H

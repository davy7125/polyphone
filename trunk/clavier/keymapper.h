#ifndef KEYMAPPER_H
#define KEYMAPPER_H

#include <QVector>
#include <QList>
#include <QKeySequence>

class KeyMapper
{
public:
    KeyMapper();
    void addCombinaisonKey(int key, QKeySequence combinaison);
    void setCombinaisonKey(int key, int numCombinaison, QString sequence);
    void setOctave(int numOctave);
    int getKey(QKeySequence combinaison);
    QString getSequence(int key, int numCombinaison);
    int getRootKey()    { return ROOT_KEY; }
    int getKeyCount()   { return KEY_NUMBER; }

private:
    QVector<QList<QKeySequence> > _listCombinaisons;
    int _octave;
    static int KEY_NUMBER;
    static int ROOT_KEY;
};

#endif // KEYMAPPER_H

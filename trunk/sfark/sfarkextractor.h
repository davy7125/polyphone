#ifndef SFARKEXTRACTOR_H
#define SFARKEXTRACTOR_H

#include <QString>
class Pile_sf2;

class SfArkExtractor
{
public:
    SfArkExtractor(Pile_sf2 * sf2);
    void extract(QString fileName);

private:
    Pile_sf2 * _sf2;
};



#endif // SFARKEXTRACTOR_H

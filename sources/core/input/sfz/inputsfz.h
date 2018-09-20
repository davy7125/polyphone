#ifndef INPUTSFZ_H
#define INPUTSFZ_H

#include "abstractinput.h"
class SoundfontManager;

class InputSfz : public AbstractInput
{
public:
    InputSfz();

protected slots:
    void processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int &sf2Index) override;
};

#endif // INPUTSFZ_H

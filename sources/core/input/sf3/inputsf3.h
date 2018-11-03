#ifndef INPUTSF3_H
#define INPUTSF3_H

#include "abstractinput.h"
class SoundfontManager;

class InputSf3 : public AbstractInput
{
public:
    InputSf3();

protected slots:
    void processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int &sf2Index) override;
};

#endif // INPUTSF3_H

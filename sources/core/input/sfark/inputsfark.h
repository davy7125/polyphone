#ifndef INPUTSFARK_H
#define INPUTSFARK_H

#include "abstractinput.h"

class InputSfArk : public AbstractInput
{
public:
    InputSfArk();

protected slots:
    void processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int &sf2Index) override;
};

#endif // INPUTSFARK_H

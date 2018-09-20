#ifndef INPUTEMPTY_H
#define INPUTEMPTY_H

#include "abstractinput.h"
class SoundfontManager;

class InputEmpty : public AbstractInput
{
public:
    InputEmpty();

protected slots:
    void processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int &sf2Index) override;
};

#endif // INPUTEMPTY_H

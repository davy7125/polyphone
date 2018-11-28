#ifndef INPUTSFARK_H
#define INPUTSFARK_H

#include "abstractinput.h"

class InputSfArk : public AbstractInput
{
    Q_OBJECT
    
public:
    InputSfArk();

protected slots:
    void processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int &sf2Index, QString &tempFilePath) override;
};

#endif // INPUTSFARK_H

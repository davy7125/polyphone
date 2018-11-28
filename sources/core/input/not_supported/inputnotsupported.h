#ifndef INPUTNOTSUPPORTED_H
#define INPUTNOTSUPPORTED_H

#include "abstractinput.h"

class InputNotSupported : public AbstractInput
{
    Q_OBJECT
    
public:
    InputNotSupported();

protected slots:
    void processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int &sf2Index, QString &tempFilePath) override;
};

#endif // INPUTNOTSUPPORTED_H

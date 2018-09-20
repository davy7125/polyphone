#ifndef INPUTSF2_H
#define INPUTSF2_H

#include "abstractinput.h"
class SoundfontManager;
class Sf2Header;
class Sf2SdtaPart;
class Sf2PdtaPart;

class InputSf2 : public AbstractInput
{
public:
    InputSf2();

protected slots:
    void processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int &sf2Index) override;

private:
    void parse(QDataStream &stream, bool &success, QString &error, int &sf2Index);
    void fillSf2(Sf2Header &header, Sf2SdtaPart &sdtaPart, Sf2PdtaPart &pdtaPart, bool &success, QString &error, int &sf2Index);

    SoundfontManager * _sm;
    QString _filename;
};

#endif // INPUTSF2_H

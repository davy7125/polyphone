#ifndef OUTPUTDUMMY_H
#define OUTPUTDUMMY_H

#include "abstractoutput.h"
class SoundfontManager;

class OutputDummy : public AbstractOutput
{
public:
    OutputDummy();

protected slots:
    void processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int sf2Index, QMap<QString, QVariant> & options) override;
};

#endif // OUTPUTDUMMY_H

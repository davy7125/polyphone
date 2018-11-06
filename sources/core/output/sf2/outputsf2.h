#ifndef OUTPUTSF2_H
#define OUTPUTSF2_H

#include "abstractoutput.h"
class SoundfontManager;

class OutputSf2 : public AbstractOutput
{
public:
    OutputSf2();

protected slots:
    void processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int sf2Index, QMap<QString, QVariant> & options) override;
};

#endif // OUTPUTSF2_H

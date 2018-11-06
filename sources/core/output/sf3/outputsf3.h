#ifndef OUTPUTSF3_H
#define OUTPUTSF3_H

#include "abstractoutput.h"
class SoundfontManager;

class OutputSf3 : public AbstractOutput
{
public:
    OutputSf3();

protected slots:
    void processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int sf2Index, QMap<QString, QVariant> & options) override;
};

#endif // OUTPUTSF3_H
